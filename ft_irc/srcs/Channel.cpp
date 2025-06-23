/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:45:36 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/23 15:33:50 by gcapa-pe         ###   ########.fr       */
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
    setHasPoll(false);
    setHasKey(false);
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
    _operators.push_back(client);
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


void Channel::createPoll(const std::string& title, const std::map<std::string, int>& options) {
    if (_hasPoll) {
        std::cout << RED << B << "A poll already exists in channel " << _name << R << std::endl;
        return;
    }
    
    _pollOptions = options;
    _hasPoll = true;
    
    std::cout << GRE << B << "Poll created in channel " << _name << " with title: " << title << R << std::endl;
}

size_t Channel::getVotedClientsCount() const {
    size_t count = 0;
    for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->hasVoted()) {
            ++count;
        }
    }
    return count;
}

void Channel::createPoll(const std::string& title, const std::vector<std::string>& options) {
    if (_hasPoll) {
        std::cout << RED << B << "A poll already exists in channel " << _name << R << std::endl;
        return;
    }

    _pollOptions.clear();
    for (std::vector<std::string>::const_iterator it = options.begin(); it != options.end(); ++it) {
        _pollOptions[*it] = 0; // Initialize vote count to 0
    }
    _hasPoll = true;

    std::cout << GRE << B << "Poll created in channel " << _name << " with title: " << title << R << std::endl;
}


void Channel::closeAndResetPoll() {
    // Find who won the poll or if there's a tie
    // Send who won to all clients in the channel
    if (!_hasPoll) {
        std::cout << RED << B << "No poll exists in channel " << _name << R << std::endl;
        return;
    }
    resetVotedClientsCount();
    std::vector<std::string> winners;
    int maxVotes = -1;
    for (std::map<std::string, int>::const_iterator it = _pollOptions.begin(); it != _pollOptions.end(); ++it) {
        if (it->second > maxVotes) {
            maxVotes = it->second;
            winners.clear();
            winners.push_back(it->first);
        } else if (it->second == maxVotes && maxVotes > 0) {
            winners.push_back(it->first);
        }
    }
    std::string resultMsg;
    if (maxVotes > 0 && !winners.empty()) {
        resultMsg = ":irc.42.local 307 " + _name + " :Poll results: Winner(s): ";
        for (size_t i = 0; i < winners.size(); ++i) {
            resultMsg += winners[i];
            if (i + 1 < winners.size())
                resultMsg += " and ";
        }
        resultMsg += "\r\n";
    } else {
        resultMsg = ":irc.42.local 307 " + _name + " :Poll closed with no votes cast.\r\n";
    }
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        send((*it)->getFd(), resultMsg.c_str(), resultMsg.length(), 0);
    }
    _hasPoll = false;
    _pollOptions.clear(); // Reset the voting status of all clients
    std::cout << GRE << B << "Poll in channel " << _name << " has been closed and reset." << R << std::endl;
}

void Channel::vote(Client& client, const std::string& option) {
    if (!_hasPoll) {
        std::string msg = "*[POLL:] No poll exists in channel " + _name + "\r\n";
        send(client.getFd(), msg.c_str(), msg.length(), 0);
        std::cout << RED << B << "No poll exists in channel " << _name << R << std::endl;
        return;
    }

    std::map<std::string, int>::iterator it = _pollOptions.find(option);
    if (it == _pollOptions.end()) {
        std::string msg = "*[POLL:] Invalid option: " + option + "\r\n";
        send(client.getFd(), msg.c_str(), msg.length(), 0);
        std::cout << RED << B << "Invalid option: " << option << R << std::endl;
        return;
    }

    // Only allow voting once
    if (!client.hasVoted()) {
        client.setHasVoted(true);
        it->second++; // Increment the vote count for the option
        std::string msg = "*[POLL:] You voted for option: " + option + "\r\n";
        send(client.getFd(), msg.c_str(), msg.length(), 0);
        std::cout << GRE << B << client.getNickname() << " voted for option: " << option 
                  << " in channel " << _name << R << std::endl;

        // Broadcast to all clients in the channel
        std::string voteMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost VOTE " + _name + " :" + option + "\r\n";
        for (std::vector<Client*>::iterator jt = _clients.begin(); jt != _clients.end(); ++jt) {
            send((*jt)->getFd(), voteMsg.c_str(), voteMsg.length(), 0);
        }

        // Check if all clients have voted
        size_t voted = 0;
        for (std::vector<Client*>::iterator jt = _clients.begin(); jt != _clients.end(); ++jt) {
            if ((*jt)->hasVoted())
                ++voted;
        }
        if (voted == _clients.size()) {
            closeAndResetPoll();
        }
    } else {
        std::string msg = "*[POLL:] You have already voted in this poll.\r\n";
        send(client.getFd(), msg.c_str(), msg.length(), 0);
        std::cout << RED << B << client.getNickname() << " has already voted in channel " << _name << R << std::endl;
    }
}