/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:36:03 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/12 16:51:14 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Ft_irc.hpp"

class Server; //forward declaration

class Client
{
    private:
        int _fd; //client socket
        std::string _ip; //client ip
    
    public:
        Client();
        ~Client();
        
        /*getters and setters*/
        void setFd(int fd);
        void setIp(std::string ip);
        int getFd() const;
        std::string getIp() const;
};