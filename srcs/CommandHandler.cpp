/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luiberna <luiberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 16:52:41 by luiberna          #+#    #+#             */
/*   Updated: 2025/07/01 20:26:07 by luiberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CommandHandler.hpp"

static std::vector<std::string> split(const std::string& str)
{
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    for (size_t i = 0; i < str.length(); ++i)
    {
        char c = str[i];
        if (c == '"')
        {
            inQuotes = !inQuotes;
            if (!inQuotes && !token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (c == ' ' && !inQuotes)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else
            token += c;
    }
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}

/*
ERROS ENCONTRADOS
- Quando um user sai do server, não dá para dar reconnect*
- Quando connectamos através o irss o cliente aparentemente conecta, mas depois diz que não está conectado
além disto se tentarmos connectar várias vezes o server cria vários clientes através do mesmo terminal, que não fazem nada.
*/

void CommandHandler::handleCommand(Server& server, Client& client, const std::string& rawMessage)
{
    std::string clean = rawMessage;
    clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\n'), clean.end());

    std::cout << "Raw command received: [" << clean << "]" << "\n";

    std::vector<std::string> args = split(clean);
    if (args.empty())
        return;
    
    std::string cmd = args[0];

    if (cmd == "CAP" || cmd == "PING" || cmd == "PONG")
        return;
    if (cmd == "PASS")
    {
        handlePass(server, client, args);
        return;
    }

    // If password is required and not given yet, reject further input
    if (client.getServerNeedsPass() && !client.getHasGivenPass())
    {
        std::string error = ":irc.42.local 464 * :Password required\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        server.closeEvent(client.getFd());
        return;
    }

    // Not yet registered, allow NICK/USER
    if (!client.isRegistered())
    {
        if (cmd == "NICK")
            handleNick(server, client, args);
        else if (cmd == "USER")
            handleUser(server, client, args);
        else
        {
            std::string error = ":irc.42.local 451 * :You have not registered\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }
        std::cout << "nick " << client.getNickname() << " user " << client.getUsername() << "\n";
        std::cout << "passed: " << client.getHasGivenPass() << "\n"; 

        client.tryRegister(); // If all pieces are there, this will set isRegistered to true
        return;
    }

    // Post-registration commands
    if (client.isRegistered())
    {   
        // para VOTE e POLL user /raw POLL ou /raw VOTE
        if(cmd == "VOTE")
            handleVote(server, client, args);
        else if(cmd == "POLL")
            handlePoll(server, client, args);
        else if(cmd == "NOTICE")
            handleNotice(server, client, args);
        else if(cmd == "KILL")
            handleKill(server, client, args);
        else if(cmd == "INVITE")
            handleInvite(server, client, args);
        else if(cmd == "MODE")
            handleMode(server, client, args);
        else if(cmd == "KICK")
            handleKick(server, client, args);
        else if(cmd == "TOPIC")
            handleTopic(server, client, args);
        else if (cmd == "JOIN")
            handleJoin(server, client, args);
        else if (cmd == "PRIVMSG")
            handlePrivmsg(server, client, args);
        else if (cmd == "QUIT")
            handleQuit(server, client, args);
        else if (cmd == "PART")
            handlePart(server, client, args);
        else
            {
                std::string error = ":irc.42.local 421 " + client.getNickname() + " " + cmd + " :Unknown command\r\n";
                send(client.getFd(), error.c_str(), error.length(), 0);
                std::cerr << "Unknown command: " << cmd << "\n";
            }
    }
}

void CommandHandler::handleNotice(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " NOTICE :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string target = args[1];
    std::string message = args[2];
    for (size_t i = 3; i < args.size(); ++i)
        message += " " + args[i];
    
    if (target[0] == '#')
    {
        if (!server.hasChannel(target))
        {
            std::string error = ":irc.42.local 403 " + client.getNickname() + " " + target + " :No such channel\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }
        Channel& channel = server.getOrCreateChannel(target, &client);
        const std::vector<Client*>& members = channel.getClients();
        for (size_t i = 0; i < members.size(); ++i)
        {
            std::string noticeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost NOTICE " +
                                    target + " :" + message + "\r\n";
            send(members[i]->getFd(), noticeMsg.c_str(), noticeMsg.length(), 0);
        }
    }
    else // Private notice
    {
        Client* targetClient = server.getClientByNick(target);
        if (!targetClient)
        {
            std::string error = ":irc.42.local 401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }
        std::string noticeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost NOTICE " +
                                        targetClient->getNickname() + " :" + message + "\r\n";
        send(targetClient->getFd(), noticeMsg.c_str(), noticeMsg.length(), 0);
    }
}

void CommandHandler::handleKill(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " KILL :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string targetNick = args[1];
    Client *targetClient = server.getClientByNick(targetNick);

    if (!targetClient)
    {
        std::string error = ":irc.42.local 401 " + client.getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Only allow operators to kill other clients
    if (!client.isOperator())
    {
        std::string error = ":irc.42.local 481 " + client.getNickname() + " :Permission Denied - You're not an IRC operator\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Notify the target client and close their connection
    std::string killMessage = ":irc.42.local KILL " + targetNick + " :Killed by " + client.getNickname() + "\r\n";
    send(targetClient->getFd(), killMessage.c_str(), killMessage.length(), 0);
    server.closeEvent(targetClient->getFd());
}

void CommandHandler::handleMode(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string chanName = args[1];
    if (chanName[0] != '#')
        chanName = "#" + chanName;

    if (!server.hasChannel(chanName))
    {
        std::string error = ":irc.42.local 403 " + client.getNickname() + " " + chanName + " :No such channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    Channel& channel = server.getOrCreateChannel(chanName, &client);

    // Check if the client is a channel operator
    if (!channel.isOperatorInChannel(&client))
    {
        std::string error = ":irc.42.local 482 " + client.getNickname() + " " + chanName + " :You're not channel operator\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // If only channel name, show current modes
    if (args.size() == 2)
    {
        std::string modesReply = ":irc.42.local 324 " + client.getNickname() + " " + chanName + " :" +
                                (channel.isInviteOnly() ? "+i " : "-i ") + (channel.isTopicRestricted() ? "+t " : "-t ") +
                                    (channel.hasKey() ? "+k " : "-k ") + (channel.hasUserLimit() ? "+l " : "-l ") +
                                        channel.getChannelKey() + "\r\n";
        send(client.getFd(), modesReply.c_str(), modesReply.length(), 0);
        return;
    }

    // Track changes
    std::string modeChanges;
    std::string modeArgs;
    for (size_t i = 2; i < args.size(); ++i)
    {
        if (args[i] == "+i")
        {
            if (!channel.isInviteOnly())
            {
                channel.setInviteOnly(true);
                modeChanges += "+i ";
            }
        }
        else if (args[i] == "-i")
        {
            if (channel.isInviteOnly())
            {
                channel.setInviteOnly(false);
                modeChanges += "-i ";
            }
        }
        else if (args[i] == "+t")
        {
            if (!channel.isTopicRestricted())
            {
                channel.setTopicRestricted(true);
                modeChanges += "+t ";
            }
        }
        else if (args[i] == "-t")
        {
            if (channel.isTopicRestricted())
            {
                channel.setTopicRestricted(false);
                modeChanges += "-t ";
            }
        }
        else if (args[i].size() >= 2 && args[i].substr(0, 2) == "+k")
        {
            if (args.size() > i + 1)
            {
                if (!channel.hasKey() || channel.getChannelKey() != args[i + 1])
                {
                    channel.setHasKey(true);
                    channel.setChannelKey(args[i + 1]);
                    modeChanges += "+k ";
                    modeArgs += args[i + 1] + " ";
                }
                ++i;
            }
            else
            {
                std::string error = ":irc.42.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
                send(client.getFd(), error.c_str(), error.length(), 0);
                return;
            }
        }
        else if (args[i].size() >= 2 && args[i].substr(0, 2) == "-k")
        {
            if (channel.hasKey())
            {
                channel.setHasKey(false);
                channel.setChannelKey("");
                modeChanges += "-k ";
            }
        }
        else if (args[i].size() >= 2 && args[i].substr(0, 2) == "+l")
        {
            if (args.size() > i + 1)
            {
                std::istringstream iss(args[i + 1]);
                size_t limit = 0;
                iss >> limit;
                if (!channel.hasUserLimit() || channel.getUserLimit() != limit)
                {
                    channel.setUserLimit(limit);
                    modeChanges += "+l ";
                    std::ostringstream oss;
                    oss << limit;
                    modeArgs += oss.str() + " ";
                }
                ++i;
            }
            else
            {
                std::string error = ":irc.42.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
                send(client.getFd(), error.c_str(), error.length(), 0);
                return;
            }
        }
        else if (args[i].size() >= 2 && args[i].substr(0, 2) == "-l")
        {
            if (channel.hasUserLimit())
            {
                channel.setUserLimit(0);
                channel.setUserLimit(false);
                modeChanges += "-l ";
            }
        }
        else if (args[i].size() >= 2 && args[i].substr(0, 2) == "+o")
        {
            // +o requires a nick argument
            if (args.size() > i + 1)
            {
                std::string targetNick = args[i + 1];
                Client* target = server.getClientByNick(targetNick);
                if (target && channel.hasClient(target))
                {
                    if (!channel.isOperatorInChannel(target))
                    {
                        channel.makeOperator(target);
                        modeChanges += "+o ";
                        modeArgs += targetNick + " ";
                    }
                }
                else
                {
                    std::string error = ":irc.42.local 401 " + client.getNickname() + " " + targetNick + " :No such nick/channel\r\n";
                    send(client.getFd(), error.c_str(), error.length(), 0);
                }
                ++i;
            }
            else
            {
                std::string error = ":irc.42.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
                send(client.getFd(), error.c_str(), error.length(), 0);
                return;
            }
        }
        else if (args[i].size() >= 2 && args[i].substr(0, 2) == "-o")
        {
            // -o requires a nick argument
            if (args.size() > i + 1)
            {
                std::string targetNick = args[i + 1];
                Client* target = server.getClientByNick(targetNick);
                if (target && channel.hasClient(target))
                {
                    if (channel.isOperatorInChannel(target))
                    {
                        channel.removeOperator(target);
                        modeChanges += "-o ";
                        modeArgs += targetNick + " ";
                    }
                }
                else
                {
                    std::string error = ":irc.42.local 401 " + client.getNickname() + " " + targetNick + " :No such nick/channel\r\n";
                    send(client.getFd(), error.c_str(), error.length(), 0);
                }
                ++i;
            }
            else
            {
                std::string error = ":irc.42.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
                send(client.getFd(), error.c_str(), error.length(), 0);
                return;
            }
        }
        else
        {
            std::string error = ":irc.42.local 472 " + client.getNickname() + " " + args[i] + " :Unknown mode\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }
    }

    // Only send mode change if something changed
    if (!modeChanges.empty())
    {
        std::string modeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + chanName + " " + modeChanges + modeArgs + "\r\n";
        const std::vector<Client*>& members = channel.getClients();
        for (size_t i = 0; i < members.size(); ++i)
            send(members[i]->getFd(), modeMsg.c_str(), modeMsg.length(), 0);
    }
}

void CommandHandler::handleInvite(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " INVITE :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string targetNick = args[1];
    std::string chanName = args[2];
    if (chanName[0] != '#')
        chanName = "#" + chanName;

    if (!server.hasChannel(chanName))
    {
        std::string error = ":irc.42.local 403 " + client.getNickname() + " " + chanName + " :No such channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    Channel& channel = server.getOrCreateChannel(chanName, &client);

    // Check if the client is a channel operator
    if (!channel.isOperatorInChannel(&client))
    {
        std::string error = ":irc.42.local 482 " + client.getNickname() + " " + chanName + " :You're not channel operator\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    Client *targetClient = server.getClientByNick(targetNick);
    if (!targetClient)
    {
        std::string error = ":irc.42.local 401 " + client.getNickname() + " " + targetNick + " :No such nick\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Send invite message to the target client
    std::string inviteMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost INVITE " + targetNick + " :" + chanName + "\r\n";
    send(targetClient->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);

    // Notify the inviter
    std::string notifyMsg = ":irc.42.local 341 " + client.getNickname() + " " + targetNick + " :" + chanName + "\r\n";
    send(client.getFd(), notifyMsg.c_str(), notifyMsg.length(), 0);
    // Add the invite to the specific channel -> to the invites channel on the target client
    targetClient->addInvite(chanName);
}

void CommandHandler::handlePart(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " PART :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string chanName = args[1];
    if (chanName[0] != '#')
        chanName = "#" + chanName;

    Channel& channel = server.getOrCreateChannel(chanName, &client);
    
    // Remove client from the channel
    channel.removeClient(&client);
    channel.incrementHowMany(channel.getHowMany() - 1);
    std::string emptyChannel = "";
    client.setChannel(emptyChannel); // Clear the client's channel
    // Notify the client
    std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART :" + chanName + "\r\n";
    send(client.getFd(), partMsg.c_str(), partMsg.length(), 0);

    // Notify other clients in the channel
    const std::vector<Client*>& members = channel.getClients();
    for (size_t i = 0; i < members.size(); ++i)
        if (members[i]->getFd() != client.getFd())
            send(members[i]->getFd(), partMsg.c_str(), partMsg.length(), 0);

    // If the channel is empty, remove it
    if (channel.isEmpty())
        server.removeChannel(chanName);
}

void CommandHandler::handleQuit(Server& server, Client& client, const std::vector<std::string>& args)
{
    std::string quitMessage = "Client has disconnected";
    if (args.size() > 1)
    {
        quitMessage = args[1];
        for (size_t i = 2; i < args.size(); ++i)
            quitMessage += " " + args[i];
    }

    std::cout << "QUIT received: " << quitMessage << "\n";

    std::string quitMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost QUIT :" + quitMessage + "\r\n";

    // Notify all channels the client is part of and remove client from them
    std::map<std::string, Channel> channels = server.getChannels();

    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel& channel = it->second;
        if (channel.hasClient(&client))
        {
            std::vector<Client*> members = channel.getClients();
            for (size_t i = 0; i < members.size(); ++i)
            {
                if (members[i]->getFd() != client.getFd())
                {
                    send(members[i]->getFd(), quitMsg.c_str(), quitMsg.length(), 0);
                }
            }
            channel.removeClient(&client);
        }
    }

    // Remove client from server client list
    // Assuming you have a method like this:
    //server.closeEvent(client.getFd());
    
    // Close the client's socket
    server.closeEvent(client.getFd());
}



void CommandHandler::handlePass(Server& server, Client& client, const std::vector<std::string>& args) 
{   
    if (args.size() < 2)
    {
        std::string error = ":irc.42.local 461 * PASS :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::cout << "PASS received: " << args[1] << "\n";

    if (args[1] != server.getPassword())
    {
        std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
        std::string error = ":irc.42.local 464 " + nick + " :Password incorrect\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        server.closeEvent(client.getFd());
        return;
    }

    client.setHasGivenPass(true);
}

bool CommandHandler::notDuplicateNickname(Server& server, const std::string& nickname)
{
    const std::vector<Client *> &clients = server.getClients();
    for (size_t i = 0; i < clients.size(); ++i)
        if (clients[i]->getNickname() == nickname)
            // Do NOT send error to the existing client or close their connection!
            return false;
    return true;
}

bool CommandHandler::notDuplicateUsername(Server& server, const std::string& username)
{
    const std::vector<Client *>& clients = server.getClients();
    for (size_t i = 0; i < clients.size(); ++i)
        if (clients[i]->getUsername() == username)
            // Do NOT send error to the existing client or close their connection!
            return false;
    return true;
}


void CommandHandler::handleNick(Server& server, Client& client, const std::vector<std::string>& args)
{   
    if ((args.size() != 2 || args[1].empty()) && !client.getUsername().empty())
    {
        std::string error = ":irc.42.local 431 * :No nickname given\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Check for duplicate nickname
    if (!notDuplicateNickname(server, args[1]))
    {
        std::string error = ":irc.42.local 433 * " + args[1] + " :Nickname is already in use\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Set the client's nickname if valid
    client.setNickname(args[1]);
    std::cout << "NICK received: " << args[1] << "\n";
}

void CommandHandler::handleUser(Server& server, Client& client, const std::vector<std::string>& args)
{   
     if (args.size() < 2 || args[1].empty())
    {
        std::string error = ":irc.42.local 431 * :No username given\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Check for duplicate username
    if (!notDuplicateUsername(server, args[1]))
    {
        std::string error = ":irc.42.local 434 * " + args[1] + " :Username is already in use\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Set the client's username if valid
    client.setUsername(args[1]);
    std::cout << "USER received: " << args[1] << "\r\n";
}

void CommandHandler::handleJoin(Server& server, Client& client, const std::vector<std::string>& args)
{   
    if (args.size() < 2)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!client.isRegistered())
    {
        std::string error = ":irc.42.local 451 " + client.getNickname() + " JOIN :You have not registered\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string chanName = args[1];
    if (chanName[0] != '#')
        chanName = "#" + chanName;

    Channel& channel = server.getOrCreateChannel(chanName, &client);
    std::cout << "Currently in channel :   " << channel.getHowMany() << std::endl;
    std::vector<Client *> clients = channel.getClients();

    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->getUsername() == client.getUsername()) {
            std::string error = ":irc.42.local 443 " + client.getNickname() + " " + chanName + " :Already in channel\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }
    }
    if (channel.hasUserLimit() && (channel.getHowMany() == channel.getUserLimit()))
    {
        std::string error = ":irc.42.local 477 " + client.getNickname() + " " + chanName + " :Cannot join channel user limit reached (+l)\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    if (channel.hasKey() && args.size() < 3)
    {
        std::string error = ":irc.42.local 475 " + client.getNickname() + " " + chanName + " :Cannot join channel (+k)\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    if (channel.hasKey() && args[2] != channel.getChannelKey())
    {
        std::string error = ":irc.42.local 475 " + client.getNickname() + " " + chanName + " :Cannot join channel password incorrect. (+k)\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    if (channel.isInviteOnly() && !channel.isOperatorInChannel(&client))
    {
        // Check if the client has been invited
        if (std::find(client.getInvites().begin(), client.getInvites().end(), chanName) == client.getInvites().end())
        {
            std::string error = ":irc.42.local 473 " + client.getNickname() + " " + chanName + " :Cannot join channel (+i)\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }
    }
    // Add client to the channel
    channel.addClient(&client);
    client.setChannel(chanName);
    channel.incrementHowMany(channel.getHowMany() + 1);
    // Send JOIN message to the client
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + chanName + "\r\n";
    send(client.getFd(), joinMsg.c_str(), joinMsg.length(), 0);

    // manda o notice de topic vazio se nao houver topic
    if(channel.getTopic().empty())
    {
        std::string topic = ":irc.42.local 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n";
        send(client.getFd(), topic.c_str(), topic.length(), 0);
    }

    // Send NAMES list (353) with @ for operators
    std::string namesReply = ":irc.42.local 353 " + client.getNickname() + " = " + chanName + " :";
    const std::vector<Client*>& members = channel.getClients();
    for (size_t i = 0; i < members.size(); ++i)
    {
        if (channel.isOperatorInChannel(members[i]))
            namesReply += "@";
        namesReply += members[i]->getNickname();
        if (i != members.size() - 1)
            namesReply += " ";
    }
    namesReply += "\r\n";
    send(client.getFd(), namesReply.c_str(), namesReply.length(), 0);

    // End of NAMES list (366)
    std::string endNames = ":irc.42.local 366 " + client.getNickname() + " " + chanName + " :End of /NAMES list.\r\n";
    send(client.getFd(), endNames.c_str(), endNames.length(), 0);

    // Broadcast JOIN to other clients in the channel
    for (size_t i = 0; i < members.size(); ++i)
        if (members[i]->getFd() != client.getFd())
            send(members[i]->getFd(), joinMsg.c_str(), joinMsg.length(), 0);

    // Send MODE message to all clients in the channel if the joining client is now an operator
    if (channel.isOperatorInChannel(&client))
    {
        std::string modeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + chanName + " +o " + client.getNickname() + "\r\n";
        for (size_t i = 0; i < members.size(); ++i)
            send(members[i]->getFd(), modeMsg.c_str(), modeMsg.length(), 0);
    }
}

void CommandHandler::handlePrivmsg(Server& server, Client& sender, const std::vector<std::string>& args)
{    
    if(!sender.isRegistered())
    {
        std::string error = ":irc.42.local 451 " + sender.getNickname() + " PRIVMSG :You have not registered\r\n";
        send(sender.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    if (args.size() < 3)
    {
        std::string error = ":irc.42.local 461 " + sender.getNickname() + " PRIVMSG :Not enough parameters\r\n";
        send(sender.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    std::string target = args[1];
    std::string message;

    // Concatenate todas as partes da mensagem
    for (size_t i = 2; i < args.size(); ++i)
    {
        message += args[i];
        if (i != args.size() - 1)
            message += " ";
    }

    // Remove os dois pontos no início da mensagem, se existirem
    if (message[0] == ':')
        message = message.substr(1);
    
    // Verifica se o destinatário é um canal ou um cliente
    std::string fullMsg = ":" + sender.getNickname() + "!" + sender.getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#')
    {
        /// Mensagem para um canal
        Channel& chan = server.getOrCreateChannel(target, &sender);
        chan.normalizeChannelName(); 
        // Normaliza o nome do canal
        const std::vector<Client*>& members = chan.getClients();
        for (size_t i = 0; i < members.size(); ++i)
            if (members[i]->getFd() != sender.getFd())
                send(members[i]->getFd(), fullMsg.c_str(), fullMsg.length(), 0);
    }
    else
    {
        // DM para a shorty
        const std::vector<Client *>& clients = server.getClients();
        for (size_t i = 0; i < clients.size(); ++i)
            if (clients[i]->getNickname() == target)
            {
                send(clients[i]->getFd(), fullMsg.c_str(), fullMsg.length(), 0);
                return;
            }
        // Se não encontrar o destinatário, envia um erro
        std::string error = ":irc.42.local 401 " + sender.getNickname() + " " + target + " :No such nick/channel\r\n";
        // Envia o erro para o remetente
        send(sender.getFd(), error.c_str(), error.length(), 0);
    }
}

void CommandHandler::handleKick(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " KICK :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string chanName = args[1];
    if (chanName[0] != '#')
        chanName = "#" + chanName;

    if (!server.hasChannel(chanName))
    {
        std::string error = ":irc.42.local 403 " + client.getNickname() + " " + chanName + " :No such channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    Channel& channel = server.getOrCreateChannel(chanName, &client);

    // Check if client is in the channel
    if (!channel.hasClient(&client))
    {
        std::string error = ":irc.42.local 442 " + client.getNickname() + " " + chanName + " :You're not on that channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Check if the client is a channel operator
    if (!channel.isOperatorInChannel(&client))
    {
        std::string error = ":irc.42.local 482 " + client.getNickname() + " " + chanName + " :You're not channel operator\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string targetNick = args[2];
    Client *targetClient = server.getClientByNick(targetNick);

    if (!targetClient)
    {
        std::string error = ":irc.42.local 401 " + client.getNickname() + " " + targetNick + " :No such nick\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!channel.hasClient(targetClient))
    {
        std::string error = ":irc.42.local 441 " + client.getNickname() + " " + targetNick + " " + chanName + " :User not on channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Optional kick reason
    std::string reason = client.getNickname(); // default reason is kicker's nick
    if (args.size() > 3)
    {
        reason = args[3];
        for (size_t i = 4; i < args.size(); ++i)
            reason += " " + args[i];
        if (reason[0] == ':')
            reason = reason.substr(1);
    }

    std::string kickMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost KICK " + chanName + " " + targetNick + " :" + reason + "\r\n";

    // Send KICK to all members before removing
    const std::vector<Client *>& members = channel.getClients();
    for (size_t i = 0; i < members.size(); ++i)
        send(members[i]->getFd(), kickMsg.c_str(), kickMsg.length(), 0);

    // Remove client from channel
    channel.removeClient(targetClient);
    channel.incrementHowMany(channel.getHowMany() - 1);
    targetClient->removeInvite(chanName);

    // If channel is empty, delete it
    if (channel.isEmpty())
        server.removeChannel(chanName);
}

void CommandHandler::handleTopic(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string chanName = args[1];
    if (chanName[0] != '#')
        chanName = "#" + chanName;

    if (!server.hasChannel(chanName))
    {
        std::string error = ":irc.42.local 403 " + client.getNickname() + " " + chanName + " :No such channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    Channel& channel = server.getOrCreateChannel(chanName, &client);

    // Check if client is in the channel
    if (!channel.hasClient(&client))
    {
        std::string error = ":irc.42.local 442 " + client.getNickname() + " " + chanName + " :You're not on that channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Check if the client is a channel operator
    if (!channel.isOperatorInChannel(&client) && channel.isTopicRestricted())
    {
        std::string error = ":irc.42.local 482 " + client.getNickname() + " " + chanName + " :You're not channel operator\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // If no topic is provided, return the current topic
    if (args.size() == 2)
    {
        std::string topicReply = ":irc.42.local 332 " + client.getNickname() + " " + chanName + " :" + channel.getTopic() + "\r\n";
        send(client.getFd(), topicReply.c_str(), topicReply.length(), 0);
        return;
    }

    // Set the new topic
    std::string newTopic;
    for (size_t i = 2; i < args.size(); ++i)
    {
        newTopic += args[i];
        if (i != args.size() - 1)
            newTopic += " ";
    }
    if (newTopic[0] == ':')
        newTopic = newTopic.substr(1);
    channel.setTopic(newTopic);
    std::string topicMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost TOPIC " + chanName + " :" + newTopic + "\r\n";
    // Notify all clients in the channel
    const std::vector<Client *>& members = channel.getClients();
    for (size_t i = 0; i < members.size(); ++i)
        send(members[i]->getFd(), topicMsg.c_str(), topicMsg.length(), 0);
    // Send the topic to the client who set it
    std::string topicReply = ":irc.42.local 332 " + client.getNickname() + " " + chanName + " :" + newTopic + "\r\n";
    send(client.getFd(), topicReply.c_str(), topicReply.length(), 0);
    std::string topicEnd = ":irc.42.local 333 " + client.getNickname() + " " + chanName + " " + client.getNickname() + " :Topic set by " + client.getNickname() + "\r\n";
    send(client.getFd(), topicEnd.c_str(), topicEnd.length(), 0);
}   
