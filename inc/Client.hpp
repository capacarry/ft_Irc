/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-cas <gude-cas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:36:03 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/25 12:08:20 by gude-cas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Irc.hpp"

class Server;

class Client
{
    private:
        int _fd;
        std::string _ip;
        std::string _nickname;
        std::string _username;
        bool _hasGivenPass;
        bool _isRegistered;
        bool _serverNeedsPass;
        bool _isoperator;
        bool _has_voted;
        std::string _channel;
        std::vector<std::string> _invites;
    
    public:
        Client();
        void setFd(int fd) {_fd = fd;}
        void setIp(std::string ip) {_ip = ip;}
        int getFd() const {return _fd;}
        std::string getIp() const {return _ip;}
        void setHasGivenPass(bool hasGivenPass) {_hasGivenPass = hasGivenPass;}
        bool getHasGivenPass() const {return _hasGivenPass;};
        void setServerNeedsPass(bool serverNeedsPass) {_serverNeedsPass = serverNeedsPass;}
        bool getServerNeedsPass() const {return _serverNeedsPass;}
        void setNickname(std::string nickname) {_nickname = nickname;}
        void setUsername(std::string username) {_username = username;}
        std::string getNickname() const {return _nickname;}
        std::string getUsername() const {return _username;}
        bool isRegistered() const {return _isRegistered;}
        void tryRegister();
        void setIsOperator(bool isOperator) {_isoperator = isOperator;}
        bool isOperator() const {return _isoperator;}
        void addInvite(const std::string& channelName) {
            _invites.push_back(channelName);}
        void setChannel(std::string& channel) {_channel = channel;}
        std::string getChannel() const {return _channel;}
        void setHasVoted(bool hasVoted) {_has_voted = hasVoted;}
        bool hasVoted() const {return _has_voted;}
        const std::vector<std::string>& getInvites() const {return _invites;}
        void sendMessage(const std::string& message) const {
            send(_fd, message.c_str(), message.length(), 0);}
        void removeInvite(const std::string& channelName) {
            _invites.erase(std::remove(_invites.begin(), _invites.end(), channelName), _invites.end());}
};
