/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:35:23 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/12 17:02:00 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Ft_irc.hpp"

class Client;

class Server
{
    private:
        int _port;
        int _socketFd; //listening socket
        static bool _signal; //signal handler (ctrl + d) etc...
        std::vector<Client> _clients; //clients vector
   
    public:
        Server();
        ~Server();
        
        /*action functions*/
        void initServer();
        void createSocket();
        
        /*getters and setters*/
        void setPort(int port);
        void setSocketFd(int socketFd);
        int getPort() const;
        int getSocketFd() const;
        int getSignal() const;

        static void signalHandler(int signum);
        void closeAll();
        void closeEvent(int fd);
        void acceptClient();
        void parseData(int fd);

        int epollFd; //epoll instance
}; 