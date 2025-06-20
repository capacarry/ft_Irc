/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:43:30 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/20 16:02:39 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

/*----------------------------------------------------------------------------*/
/* CONSTRUCTOR 🚧 */
Client::Client()
    : _fd(-1),
      _ip(""),
      _nickname(""),
      _username(""),
      _hasGivenPass(false),
      _isRegistered(false),
      _serverNeedsPass(false)
{
    std::cout << GRE << B << "Client created" << R << std::endl;
}

/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/* GETTERS AND SETTERS */
void Client::setFd(int fd)
{
    _fd = fd;
}

void Client::setIp(std::string ip)
{
    _ip = ip;
}

int Client::getFd() const
{
    return _fd;
}

std::string Client::getIp() const
{
    return _ip;
}

/*----------------------------------------------------------------------------*/


/*luiberna*/


void Client::setNickname(std::string nickname)
{   
    _nickname = nickname;
}

void Client::setUsername(std::string username)
{
    _username = username;
}
std::string Client::getNickname() const
{
    return _nickname;
}

std::string Client::getUsername() const
{
    return _username;
}

bool Client::isRegistered() const
{
    return _isRegistered;
}

void Client::tryRegister() {
    if(!_nickname.empty() && !_username.empty() && !_serverNeedsPass && !_hasGivenPass) {
        std::cout << RED << B << "Client " << _nickname << " has not given password but server does not require it" << R << std::endl;
        std::cout << GRE << B << "Client " << _nickname << " registered successfully" << R << std::endl;
        std::string welcome = ":irc.42.local 001 " + _nickname + " :Welcome to the Internet Relay Network " + _nickname + "\r\n";
        send(_fd, welcome.c_str(), welcome.size(), 0);
        return;
    }
    if (!_nickname.empty() && (_serverNeedsPass || _hasGivenPass == true) && !_username.empty() && !_isRegistered) {
        _isRegistered = true;
        std::cout << GRE << B << "Client " << _nickname << " registered successfully" << R << std::endl;
        std::string welcome = ":irc.42.local 001 " + _nickname + " :Welcome to the Internet Relay Network " + _nickname + "\r\n";
        send(_fd, welcome.c_str(), welcome.size(), 0);
    }
    
}

void Client::setHasGivenPass(bool hasGivenPass)
{
    _hasGivenPass = hasGivenPass;
}