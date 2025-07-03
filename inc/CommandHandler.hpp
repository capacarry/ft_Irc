/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-cas <gude-cas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 16:51:08 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/25 11:24:36 by gude-cas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Utils.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Server;

namespace CommandHandler 
{
    void handleCommand(Server& server, Client& client, const std::string& rawMessage);
    void handleNick(Server& server, Client& client, const std::vector<std::string>& args);
    void handleUser(Server& server, Client& client, const std::vector<std::string>& args);
    void handleJoin(Server& server, Client& client, const std::vector<std::string>& args);
    void handlePrivmsg(Server& server, Client& sender, const std::vector<std::string>& args);
    void handlePass(Server& server, Client& client, const std::vector<std::string>& args);
    void handleQuit(Server& server, Client& client, const std::vector<std::string>& args);
    void handlePart(Server& server, Client& client, const std::vector<std::string>& args);
    void handleKick(Server& server, Client& client, const std::vector<std::string>& args);
    void handleTopic(Server& server, Client& client, const std::vector<std::string>& args);
    void handleInvite(Server& server, Client& client, const std::vector<std::string>& args);
    void handleMode(Server& server, Client& client, const std::vector<std::string>& args);
    void handleKill(Server& server, Client& client, const std::vector<std::string>& args);
    void handleNotice(Server& server, Client& client, const std::vector<std::string>& args);
    void handlePoll(Server& server, Client& client, const std::vector<std::string>& args);
    void createPoll(Server& server, const std::string& title, const std::vector<std::string>& options, const std::string& channelName);
    void handleVote(Server& server, Client& client, const std::vector<std::string>& args);
    bool notDuplicateNickname(Server& server, const std::string& nickname);
    bool notDuplicateUsername(Server& server, const std::string& username);
}
