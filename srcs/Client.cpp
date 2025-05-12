/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:43:30 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/12 15:51:14 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client()
{
    std::cout << GRE << BOLD << "Client created" << RESET << std::endl;
}

Client::~Client()
{
    std::cout << RED << BOLD << "Client destroyed" << RESET << std::endl;
}

/*----------------------------------------------------------------------------*/

/*GETTERS AND SETTERS*/
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