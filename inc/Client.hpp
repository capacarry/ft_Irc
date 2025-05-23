/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:36:03 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/14 15:17:29 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Irc.hpp"

class Server;

class Client
{
    private:
        int _fd; // client socket
        std::string _ip; // client ip
    
    public:
        Client();
        
        /* GETTERS AND SETTERS */
        void setFd(int fd);
        void setIp(std::string ip);
        int getFd() const;
        std::string getIp() const;
};
