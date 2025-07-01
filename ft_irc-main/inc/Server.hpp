/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:35:23 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/07/01 16:48:50 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Client;

class Server
{
    private:
        int _port;
        std::string _password;
        int _socketFd;
        static bool _signal;
        std::vector<Client*> _clients;
        std::map<std::string, Channel> _channels;
   
    public:
        int epollFd;
        Server();
        ~Server();
        void initServer(int port, std::string password);
        void createSocket();
        void setPort(int port) {_port = port;}
        void setSocketFd(int socketFd) {_socketFd = socketFd;}
        int getPort() const {return _port;}
        int getSocketFd() const {return _socketFd;}
        int getSignal() const {return _signal;}
        void setPassword(const std::string& password) {_password = password;}
        std::string getPassword() const {return _password;}
        void printChannelInfo() const;
        static void signalHandler(int signum);
        void closeAll();
        void closeEvent(int fd);
        void acceptClient();
        void receiveData(int fd);
        void removeClientFromAllChannels(Client* client);
        Client* getClientByFd(int fd) const;
        Client* getClientByNick(const std::string& nick) const;
        std::map<std::string, Channel> getChannels() const {return _channels;}
        bool hasChannel(const std::string& name) const {return _channels.find(name) != _channels.end();}
        Channel& getChannel(const std::string& name);
        Channel& getOrCreateChannel(const std::string& name, Client* client);
        void removeChannel(const std::string& name);
        void notifyChannelMembers(const std::string& channelName, const std::string& message);
        const std::vector<Client*>& getClients() const {return _clients;}
};
