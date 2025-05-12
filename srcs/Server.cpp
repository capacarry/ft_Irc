/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:50:54 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/12 17:07:54 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

bool Server::_signal = false; 

/*CONSTRUCTOR AND DESTRUCTOR*/
Server::Server():_socketFd(-1)
{    //set signal to false
   
    std::cout << GRE << BOLD << "Server created" << RESET << std::endl;
}

Server::~Server()
{
    std::cout << RED << BOLD << "Server destroyed" << RESET << std::endl;
}



/*----------------------------------------------------------------------------*/
/*ACTION FUNCTIONS*/
void Server::initServer()
{
    setPort(6667); //standart irc port
    createSocket(); // create listening socket

    std::cout << GRE << "Server <" << getSocketFd() << "> Connected" << WHI << std::endl;
    std::cout << "Waiting to accept a connection...\n";

    const int maxEvents = 64; //max events to handle
    struct epoll_event events[maxEvents]; //events array
    
    while(Server::getSignal() == false)
    {
        int n_events = epoll_wait(epollFd, events, maxEvents, -1); //wait for events
        if(n_events == -1 && Server::getSignal() == false) //if error and signal not received
            throw std::runtime_error("Error waiting for events");
        for(int i = 0; i < n_events; i++)
        {
            int eventFd = events[i].data.fd; //get event file descriptor

            if(events[i].events & EPOLLIN) //if event is input
            {
                if(eventFd == getSocketFd())
                {   
                    std::cout << GRE << BOLD << "New connection accepted" << RESET << std::endl;
                    //acceptClient(); //accept new client
                }
                else
                {   
                    std::cout << GRE << BOLD << "Data received from client" << RESET << std::endl;
                    //parseData(eventFd); //parse data from client
                }
            }
            else if(events[i].events & (EPOLLERR | EPOLLHUP))
            {
                closeEvent(eventFd);
            }
        }
    }
    closeAll(); //close all sockets and clients
}

void Server::closeEvent(int fd)
{
    // Remove the client FD from the epoll instance
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd,  NULL) == -1)
    {
        std::cerr << "Failed to remove fd " << fd << " from epoll: " << strerror(errno) << std::endl;
    }

    // Close the client socket
    close(fd);

    // Remove from the clients vector
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i].getFd() == fd)
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
}


/*remove all all sockets and clients and remove from poll*/
void Server::closeAll()
{
    size_t i = 0;
    //close all client sockets and remove clients from vector
    while(i < _clients.size())
    {   
        //remove client from epoll instance
        epoll_ctl(epollFd, EPOLL_CTL_DEL, _clients[i].getFd(), NULL);
        std::cout << YEL << BOLD << "Closing client socket: " << _clients[i].getFd() << RESET << std::endl;
        close(_clients[i].getFd()); //close client socket
        _clients.erase(_clients.begin() + i); //remove client from vector
        i++;
    }
    //close server socket
    if(getSocketFd() != -1)
    {   
        //remove server socket from epoll instance
        epoll_ctl(epollFd, EPOLL_CTL_DEL, getSocketFd(), NULL);
        std::cout << YEL << BOLD << "Closing server socket: " << getSocketFd() <<  RESET << std::endl;
        close(getSocketFd());
    }
}

void Server::createSocket()
{   
    struct sockaddr_in serverAddr;
    struct epoll_event event; // event structure for epoll (ex: if a client is connected)
    epollFd = epoll_create1(0); // create epoll instance( create the manager who will control the events)
    
    serverAddr.sin_family = AF_INET; //IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; //any local address
    serverAddr.sin_port = htons(getPort()); //port number
    
    /*AF_INET = IPV4 (ip family), SOCK_STREAM = TCP (protocolo)*/
    setSocketFd(socket(AF_INET, SOCK_STREAM, 0));
    if (getSocketFd() == -1)
            throw std::runtime_error("Error creating socket");

    /*socket protections and setting*/
    /* SOL_SOCKET = as opções são para A SOCKET EM SI! 
        F_SETFL = dar set das fd flags
        O_NONBLOCK = não bloquear operações como read() e accept()
        SOMAXCONN = quantas conexoes podem estar na fila*/
    int opt = 1;
    if (setsockopt(getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("Error setting SO_REUSEADDR on socket");   
    if(fcntl(getSocketFd(), F_SETFL, O_NONBLOCK) == -1) //set socket to non-blocking mode
        throw std::runtime_error("Error setting socket to non-blocking mode");
    if (bind(getSocketFd(), (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) //bind socket to address of server
        throw std::runtime_error("Error binding socket");
    if (listen(getSocketFd(), SOMAXCONN) == -1) //listen for incoming connections
        throw std::runtime_error("Error listening on socket");
    if(epollFd == -1)
        throw std::runtime_error("Error creating epoll instance");
    
    event.events = EPOLLIN; //event type (input)
    event.data.fd = getSocketFd(); //file descriptor of the socket

    /*epoll_ctl = add, modify or delete a file descriptor from the epoll instance*/
    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, getSocketFd(), &event) == -1) //add socket to epoll instance
        throw std::runtime_error("Error adding socket to epoll instance");
}

void Server::signalHandler(int signum)
{
    (void)signum; //unused parameter
    std::cout << YEL << BOLD << "Signal received!" << RESET << std::endl;
    _signal = true; //set signal to true
}




/*----------------------------------------------------------------------------------------------*/
/*GETTERS AND SETTERS*/
void Server::setPort(int port)
{
    _port = port;
}

int Server::getPort() const
{
    return _port;
}

void Server::setSocketFd(int socketFd)
{
    _socketFd = socketFd;
}

int Server::getSocketFd() const
{
    return _socketFd;
}

int Server::getSignal() const
{
    return _signal;
}