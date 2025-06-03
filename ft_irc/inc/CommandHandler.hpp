/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 16:51:08 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/02 15:04:11 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "Client.hpp"
#include "Server.hpp"

class Server; // forward declaration

namespace CommandHandler {
    void handleCommand(Server &server, Client &client, const std::string &rawMessage);
    void handleNick(Server &server, Client &client, const std::vector<std::string> &args);
    void handleUser(Server &server, Client &client, const std::vector<std::string> &args);
    void handleJoin(Server& server, Client& client, const std::vector<std::string>& args);
    void handlePrivmsg(Server& server, Client& sender, const std::vector<std::string>& args);
    void handlePass(Server& server, Client& client, const std::vector<std::string>& args);
    bool notDuplicateNickname(Server &server, const std::string &nickname);
    bool notDuplicateUsername(Server &server, const std::string &username);
}