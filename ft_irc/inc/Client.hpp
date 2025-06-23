/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:36:03 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/23 15:38:05 by gcapa-pe         ###   ########.fr       */
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
        bool _has_voted;
        std::string _channel;
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
        void setChannel(std::string &channel) { _channel = channel; }
        std::string getChannel() const { return _channel; }
        void setHasVoted(bool hasVoted) { _has_voted = hasVoted; }
        bool hasVoted() const { return _has_voted; }
        const std::vector<std::string>& getInvites() const { return _invites; }
        void sendMessage(const std::string &message) const {
            send(_fd, message.c_str(), message.length(), 0);
        }
        void removeInvite(const std::string &channelName) {
            _invites.erase(std::remove(_invites.begin(), _invites.end(), channelName), _invites.end());
        }
};
