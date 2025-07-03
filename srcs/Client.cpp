/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-cas <gude-cas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:43:30 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/25 12:09:23 by gude-cas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client() : _fd(-1), _ip(""), _nickname(""), _username(""), _hasGivenPass(false),
                    _isRegistered(false), _serverNeedsPass(false), _isoperator(false)
{
    std::cout << GRE << B << "Client created" << R << "\n";
}

void Client::tryRegister() 
{
    if (!_nickname.empty() && !_username.empty() && !_serverNeedsPass && !_hasGivenPass)
    {
        std::cout << RED << B << "Client " << _nickname << " has not given password but server does not require it" << R << "\n";
        std::cout << GRE << B << "Client " << _nickname << " registered successfully" << R << "\n";
        std::string welcome = ":irc.42.local 001 " + _nickname + " :Welcome to the Internet Relay Network " + _nickname + "\r\n";
        send(_fd, welcome.c_str(), welcome.size(), 0);
        return;
    }
    if (!_nickname.empty() && (_serverNeedsPass || _hasGivenPass == true) && !_username.empty() && !_isRegistered)
    {
        _isRegistered = true;
        std::cout << GRE << B << "Client " << _nickname << " registered successfully" << R << "\n";
        std::string welcome = ":irc.42.local 001 " + _nickname + " :Welcome to the Internet Relay Network " + _nickname + "\r\n";
        send(_fd, welcome.c_str(), welcome.size(), 0);
    }   
}
