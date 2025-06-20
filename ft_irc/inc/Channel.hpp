/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:47:17 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/20 16:43:08 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include "Client.hpp"

class Channel {
private:
    std::string _name;
    std::vector<Client*> _clients;

public:
    Channel();
    Channel(const std::string& name);

    void normalizeChannelName();
    void printChannelInfo() const;
    void addClient(Client* client);
    std::vector<Client*> getClients() const;
    const std::string& getName() const;
    void removeClient(Client *client);
    bool isEmpty() const;
};

