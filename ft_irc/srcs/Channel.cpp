/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:45:36 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/18 12:33:59 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel.hpp"

void Channel::printChannelInfo() const 
{
    std::cout << BLU << B << "Channel Name: " << _name << R << std::endl;
    std::cout << BLU << B << "Clients in channel: " << _clients.size() << R << std::endl;
    for (size_t i = 0; i < _clients.size(); ++i) {
        std::cout << BLU << B << "Client " << i + 1 << ": " 
                  << _clients[i]->getNickname() 
                  << " (" << _clients[i]->getIp() << ")" 
                  << R << std::endl;
    }
}

void Channel::removeClient(Client *client) 
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (*it == client) {
            _clients.erase(it);
            return;
        }
    }
}


void Channel::normalizeChannelName() 
{
    std::string norm = getName();
    // Always start with '#'
    if (norm.empty() || norm[0] != '#')
        norm = "#" + norm;
    // Lowercase all letters
    for (size_t i = 0; i < norm.size(); ++i)
        norm[i] = std::tolower(norm[i]);
    _name = norm;
}

Channel::Channel() {
    std::cout << GRE << B << "Channel created" << R << std::endl;
}

Channel::Channel(const std::string& name): _name(name) {
    std::cout << GRE << B << "Channel created with name: " << _name << R << std::endl;
}

void Channel::addClient(Client* client) {
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (*it == client)
            return;
    }
    _clients.push_back(client);
}

std::vector<Client*> Channel::getClients() const {
    return _clients;
}

const std::string& Channel::getName() const {
    return _name;
}
