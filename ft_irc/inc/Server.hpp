/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:35:23 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/18 13:07:34 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "Irc.hpp"

class Client;

class Server
{
    private:
        int _port;
        std::string _password; // server password
        int _socketFd; // listening socket
        static bool _signal; // signal handler (ctrl + c, ctrl + /)
        std::vector<Client> _clients; // clients vector
        std::map<std::string, Channel> _channels; // channels map
   
    public:
        Server();
        ~Server();
        
        /* ACTION FUNCTIONS */
        void initServer(int port, std::string password);
        void createSocket();
        
        /* GETTERS AND SETTERS */
        void setPort(int port);
        void setSocketFd(int socketFd);
        int getPort() const;
        int getSocketFd() const;
        int getSignal() const;
        void setPassword(const std::string& password);
        std::string getPassword() const;
        void printChannelInfo() const;
        static void signalHandler(int signum);
        void closeAll();
        void closeEvent(int fd);
        void acceptClient();
        void receiveData(int fd);
        void removeClientFromAllChannels(Client* client);
        Client * getClientByFd(int fd) const;

        int epollFd; // epoll instance

        /*luiberna*/
        Channel& getOrCreateChannel(const std::string& name);
        const std::vector<Client>& getClients() const;
};
