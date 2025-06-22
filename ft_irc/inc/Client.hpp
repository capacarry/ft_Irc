/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:36:03 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/22 17:24:39 by gcapa-pe         ###   ########.fr       */
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

        /*luiberna*/
        std::string _nickname; // client nickname
        std::string _username; // client username
        bool _hasGivenPass; // client has given password
        bool _isRegistered; // client registration status
        bool _serverNeedsPass; // server needs password
        bool _isoperator; // client is operator
        std::vector<std::string> _invites;
    
    public:
        Client();
        
        /* GETTERS AND SETTERS */
        void setFd(int fd);
        void setIp(std::string ip);
        int getFd() const;
        std::string getIp() const;
        void setHasGivenPass(bool hasGivenPass);
        bool getHasGivenPass() const {return _hasGivenPass; };

        /*luiberna*/
        void setServerNeedsPass(bool serverNeedsPass) { _serverNeedsPass = serverNeedsPass; }
        bool getServerNeedsPass() const { return _serverNeedsPass; }
        void setNickname(std::string nickname);
        void setUsername(std::string username);
        std::string getNickname() const;
        std::string getUsername() const;
        bool isRegistered() const;
        void tryRegister();
        void setIsOperator(bool isOperator) { _isoperator = isOperator; }
        bool isOperator() const { return _isoperator; }
        void addInvite(const std::string &channelName) {
            _invites.push_back(channelName);
        }
        const std::vector<std::string>& getInvites() const { return _invites; }
};
