/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:45:36 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/22 17:46:28 by gcapa-pe         ###   ########.fr       */
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

Channel::Channel(const std::string& name , Client *client): _name(name) {
    setInviteOnly(false);
    std::cout << GRE << B << "Channel created with name: " << _name << R << std::endl;
    makeOperator(client);
}

void Channel::addClient(Client* client) {
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (*it == client)
            return;
    }
    _clients.push_back(client);
}

bool Channel::isEmpty() const {
    if( _clients.empty() ) {
        std::cout << RED << B << "Channel " << _name << " is empty" << R << std::endl;
        return true;
    }
    else
        return false;
    
}

void Channel::makeOperator(Client *client) {
    client->setIsOperator(true); //makes the client an operator in the channel
    std::cout << GRE << B << "Client " << client->getNickname() << " is now an operator in general" << _name << R << std::endl;
    _operators.push_back(client); //adds the client to the list of operators
    // Notify all clients in the channel that the client has been made an operator
   // std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost MODE " + _name + " +o " + client->getNickname() + "\r\n";
    // for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    //         send((*it)->getFd(), msg.c_str(), msg.length(), 0);
    //     }
}

void Channel::removeOperator(Client *client) {
    client->setIsOperator(false); //removes the operator status from the client
    std::cout << RED << B << "Client " << client->getNickname() << " is no longer an operator in channel " << _name << R << std::endl;
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
        if (*it == client) {
            _operators.erase(it);
            return;
        }
    }
    // Notify all clients in the channel that the client has been removed as an operator
    std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost MODE " + _name + " -o " + client->getNickname() + "\r\n";
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (*it != client) {
            send((*it)->getFd(), msg.c_str(), msg.length(), 0);
        }
    }
}


std::vector<Client*> Channel::getClients() const {
    return _clients;
}

const std::string& Channel::getName() const {
    return _name;
}
