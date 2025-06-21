/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:47:17 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/21 18:18:42 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <algorithm>
#include "Client.hpp"

class Channel {
private:
    std::string _name;
    std::vector<Client*> _clients;
    std::vector<Client*> _operators; // List of channel operators

public:
    Channel();
    Channel(const std::string& name, Client *client);
    void normalizeChannelName();
    bool hasClient(Client* client) const {
        return std::find(_clients.begin(), _clients.end(), client) != _clients.end();
    }
    void printChannelInfo() const;
    void addClient(Client* client);
    std::vector<Client*> getClients() const;
    const std::string& getName() const;
    void removeClient(Client *client);
    void makeOperator(Client *client);
    void removeOperator(Client *client);
    bool isOperatorInChannel(Client *client) const {
        return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
    }
    bool isEmpty() const;
};

