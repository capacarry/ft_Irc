/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:50:54 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/07/01 14:53:01 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/CommandHandler.hpp"

bool Server::_signal = false; 

Server::Server():_socketFd(-1)
{
    std::cout << GRE << B << "Server created" << R << std::endl;
}

Server::~Server()
{
    std::cout << RED << B << "Server destroyed" << R << std::endl;
}
 
void Server::initServer(int port, std::string password)
{
    setPort(port); // standard irc port
    createSocket(); // create listening socket
    
    setPassword(password); // set server password
    std::cout << GRE << "Server <" << getSocketFd() << "> Connected" << WHI << std::endl;
    std::cout << "\033[5mWaiting to accept a connection...\033[25m" << std::flush;

    const int maxEvents = 64; // max events to handle
    struct epoll_event events[maxEvents]; // events array
    
    while (Server::getSignal() == false)
    {
        int n_events = epoll_wait(epollFd, events, maxEvents, -1); // wait for events
        if (n_events == -1 && Server::getSignal() == false)
            throw std::runtime_error("Error waiting for events");
        for (int i = 0; i < n_events; i++)
        {
            int eventFd = events[i].data.fd;

            if (events[i].events & EPOLLIN)
            {
                if (eventFd == getSocketFd())
                {   
                    std::cout << "\r\033[2K" << BLU << " [ New connection accepted ]\n" << R;
                    acceptClient(); 
                }
                else
                {   
                    std::cout << BLU << "[ Data received from client ]\n" << R;
                    receiveData(eventFd);
                }
            }
            else if (events[i].events & (EPOLLERR | EPOLLHUP))
                closeEvent(eventFd);
        }
    }
    closeAll(); // close all sockets and clients
}

void Server::removeChannel(const std::string& name)
{
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if (it != _channels.end())
    {
        if (it->second.isEmpty()) // if channel is empty, remove it
        {
            _channels.erase(it);
            std::cout << YEL << B << "Channel <" << name << "> removed\n" << R;
        }
    }
    else
        std::cout << RED << B << "Channel <" << name << "> not found\n" << R;
}

void Server::printChannelInfo() const
{
    std::cout << BLU << B << "Channels Information:\n" << R;
    std::map<std::string, Channel>::const_iterator it = _channels.begin();
    for (; it != _channels.end(); ++it)
    {
        const Channel& channel = it->second;
        channel.printChannelInfo();
    }
}

void Server::removeClientFromAllChannels(Client* client) 
{
    for (std::map<std::string, Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
        it->second.removeClient(client);
}

void Server::closeEvent(int fd)
{   
    Client* client = getClientByFd(fd);
    if (client == NULL)
    {
        std::cerr << RED << "Client with fd <" << fd << "> not found!" << R << std::endl;
        return;
    }
    removeClientFromAllChannels(client); // remove client from all channels
    /* remove client fd from the epoll instance */
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd,  NULL) == -1)
        std::cerr << "Failed to remove fd <" << fd << "> from epoll: " << strerror(errno) << "\n";

    close(fd); // close the client socket

    /* remove from the clients vector */
    for (size_t i = 0; i < _clients.size(); ++i) 
    {
        if (_clients[i]->getFd() == fd)
        {   
            delete _clients[i]; 
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
}

/* close all sockets and clients, and remove them from poll */
void Server::closeAll()
{
    size_t i = 0;
    /* close all client sockets and remove clients from vector */
    while (i < _clients.size())
    {   
        /* remove client from epoll instance */
        epoll_ctl(epollFd, EPOLL_CTL_DEL, _clients[i]->getFd(), NULL);
        std::cout << YEL << B << "Closing client socket: " << _clients[i]->getFd() << R << "\n";
        close(_clients[i]->getFd()); // close client socket
        delete _clients[i]; 
        _clients.erase(_clients.begin() + i); // remove client from vector
        i++;
    }
    /* close server socket */
    if (getSocketFd() != -1)
    {   
        /* remove server socket from epoll instance */
        epoll_ctl(epollFd, EPOLL_CTL_DEL, getSocketFd(), NULL);
        std::cout << YEL << B << "Closing server socket: " << getSocketFd() <<  R << "\n";
        close(getSocketFd());
    }
}

void Server::createSocket()
{   
    struct sockaddr_in serverAddr;
    struct epoll_event event; // event structure for epoll (ex: if a client is connected)
    epollFd = epoll_create1(0); // create epoll instance (create the manager who will control the events)
    
    if (epollFd == -1)
        throw std::runtime_error("Error creating epoll instance");

    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // any local address
    serverAddr.sin_port = htons(getPort()); // port number
    
    /* AF_INET = IPV4 (ip family), SOCK_STREAM = TCP (protocol) */
    setSocketFd(socket(AF_INET, SOCK_STREAM, 0));
    if (getSocketFd() == -1)
            throw std::runtime_error("Error creating socket");

    /* socket protections and setting */
    /* 
        SOL_SOCKET = as opções são para a socket em si
        F_SETFL = dar set das fd flags
        O_NONBLOCK = não bloquear operações como read() e accept()
        SOMAXCONN = quantas conexoes podem estar na fila
    */
    int opt = 1;
    if (setsockopt(getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("Error setting SO_REUSEADDR on socket");   
    if (fcntl(getSocketFd(), F_SETFL, O_NONBLOCK) == -1) // set socket to non-blocking mode
        throw std::runtime_error("Error setting socket to non-blocking mode");
    if (bind(getSocketFd(), (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) // bind socket to server adress
        throw std::runtime_error("Error binding socket");
    if (listen(getSocketFd(), SOMAXCONN) == -1) // listen for incoming connections
        throw std::runtime_error("Error listening on socket");
    if (epollFd == -1)
        throw std::runtime_error("Error creating epoll instance");
    
    event.events = EPOLLIN; // event type (input)
    event.data.fd = getSocketFd(); // file descriptor of the socket

    /* epoll_ctl = add, modify or delete a file descriptor from the epoll instance */
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, getSocketFd(), &event) == -1) // add socket to epoll instance
        throw std::runtime_error("Error adding socket to epoll instance");
}

void Server::signalHandler(int signum)
{
    (void)signum; // unused parameter
    std::cout << YEL << B << "Signal received!\n" << R;
    _signal = true; // set signal to true
}

void Server::acceptClient()
{   
    Client* new_cli = new Client(); // create a new client instance
    struct sockaddr_in cli_add; // client adress details(ip e port)
    socklen_t len = sizeof(cli_add);
    
    /* automaticamente criamos um novo client (socket) */
    int new_cli_fd = accept(Server::getSocketFd(), (sockaddr *)&cli_add, &len);
    if (new_cli_fd == -1)
    {
        std::cerr << RED << "accept() failed\n" << R;
        return;
    }
    /* O_NONBLOCKING significa que o programa nao vai ficar a espera da socket no caso de correr mal */
    if (fcntl(new_cli_fd,F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << RED << "fcntl() failed\n" << R;
        return;
    }

    struct epoll_event event; // new epoll event

    event.events = EPOLLIN; // definimos este evento como (read data)
    event.data.fd = new_cli_fd; // atribuimos a socket do cliente a este evento

    /*Aqui pegamos no noss epollFD (o nosso manager) e adicionamos o new_cli_fd á instancia gerida por este */
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, new_cli_fd, &event) == -1)
    {
        std::cout << "epoll_ctl() failed to add client socket\n";
        return;
    }

    /* adicionamos as informacoes do cliente */
    new_cli->setFd(new_cli_fd);
    new_cli->setIp(inet_ntoa((cli_add.sin_addr)));
    if(getPassword().empty())
        new_cli->setServerNeedsPass(false);
    else
        new_cli->setServerNeedsPass(true);
    /* por fim, inserir no vetor */
    _clients.push_back(new_cli);
    std::cout << GRE << "Client <" << new_cli_fd << "> Connected\n" << WHI;
}

void Server::receiveData(int fd)
{   
    char buff[1024]; // buffer to store received data
    memset(buff, 0,sizeof(buff));
    ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0); // receber a data em bytes

    if (bytes == 0)
    {
        std::cout << RED << "CLIENT <" << fd << " > Disconnected\n" << R;
        closeEvent(fd);
    }
    else if (bytes < 0)
    {
        /* error occurred during recv */
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            /* non-blocking mode: no data available, but connection is still open */
            std::cout << YEL << "CLIENT <" << fd << " > No data available (connection still open)\n" << R;
        }
        else
        {
            /* other errors */
            std::cerr << RED << "Error receiving data from CLIENT <" << fd << ">: " << strerror(errno) << R << "\n";
            closeEvent(fd);
        }
    }
    else
    {   
        /* print da data */
        buff[bytes] = '\0';
        /* pass data to debug log */
        std::ofstream logFile("debug.log", std::ios::app); // open in append mode
        if (logFile.is_open())
        {
            logFile << "Client <" << fd << ">: " << buff << "\n";
            logFile.close();
        }
        else
            std::cerr << RED << "Failed to open debug.log\n" << R;
        for (size_t i = 0; i < _clients.size(); ++i)
        {
            if (_clients[i]->getFd() == fd)
            {
                std::string input(buff);
                std::istringstream stream(input);
                std::string line;
                while (std::getline(stream, line))
                    CommandHandler::handleCommand(*this, *_clients[i], line);
                break;
            }
        }
    }
}

Client* Server::getClientByNick(const std::string& nick) const
{
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i]->getNickname() == nick)
            return const_cast<Client*>(_clients[i]); // return non-const pointer
    return NULL; // client not found
}

void Server::notifyChannelMembers(const std::string& channelName, const std::string& message)
{
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end())
    {
        Channel& channel = it->second;
        const std::vector<Client*>& clients = channel.getClients();
        for (size_t i = 0; i < clients.size(); ++i)
            clients[i]->sendMessage(message); // send message to each client in the channel
    }
    else
        std::cerr << RED << "Channel <" << channelName << "> not found!\n" << R;
}

Channel& Server::getChannel(const std::string& name)
{
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second; // return existing channel
    throw std::runtime_error("Channel not found: " + name); // throw exception if channel not found
}

Channel& Server::getOrCreateChannel(const std::string& name, Client* client)
{
    if (_channels.find(name) == _channels.end())
        _channels[name] = Channel(name, client); // constructor with name
    return _channels[name];
}

Client* Server::getClientByFd(int fd) const 
{
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i]->getFd() == fd)
            return const_cast<Client*>(_clients[i]); // return non-const pointer
    return NULL; // client not found
}
