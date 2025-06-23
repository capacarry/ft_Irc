/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 14:47:17 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/23 15:33:57 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <algorithm>
#include "Client.hpp"

class Channel {
private:
    std::string _name;
    std::string _topic;
    bool _inviteOnly;
    bool _topicRestricted;
    bool _hasKey;
    std::string _channelKey;
    bool _hasLimit;
    size_t userLimit;
    bool _hasPoll;
    std::vector<Client*> _clients;
    std::vector<Client*> _operators;
    std::map<std::string,int>  _pollOptions;
    
    
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
    void setTopic(const std::string& topic) { _topic = topic; }
    std::string getTopic() const { return _topic; }
    bool isEmpty() const;
    bool isInviteOnly() const { return _inviteOnly; }
    void setInviteOnly(bool inviteOnly) { _inviteOnly = inviteOnly; }
    bool isTopicRestricted() const { return _topicRestricted; }
    void setTopicRestricted(bool topicRestricted) { _topicRestricted = topicRestricted; }
    bool hasKey() const { return _hasKey; }
    void setHasKey(bool hasKey) { _hasKey = hasKey; }
    std::string getChannelKey() const { return _channelKey; }
    void setChannelKey(const std::string& key) { _channelKey = key; }
    bool hasUserLimit() const { return _hasLimit; }
    void setUserLimit(size_t limit) { userLimit = limit; _hasLimit = true; }
    size_t getUserLimit() const { return userLimit; }
    void createPoll(const std::string& title, const std::map<std::string, int>& options);
    bool hasPoll() const { return _hasPoll; }
    void setHasPoll(bool hasPoll) { _hasPoll = hasPoll; }
    void setPollOptions(const std::map<std::string, int>& options) { _pollOptions = options; }
    size_t getVotedClientsCount() const;
    std::map<std::string, int> getPollOptions() const { return _pollOptions; }
    void createPoll(const std::string& title, const std::vector<std::string>& options);
    void closeAndResetPoll();
    void vote(Client& client, const std::string& option);
    void resetVotedClientsCount() {
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        (*it)->setHasVoted(false);
    }
    }
};

