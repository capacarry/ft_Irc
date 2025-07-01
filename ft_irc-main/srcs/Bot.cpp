/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:28:17 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/07/01 12:25:20 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CommandHandler.hpp"

/*
Como o POLL vai funcionar é: Se um Operator mandar POLL benfica sporting "melhor clube".
O ultimo vai ser o titulo da POLL e as opções vao ser sporting benfica. Vai aparecer "New Poll created "Quem é melhor?
Sporting. Benfica
Use VOTE [option] to vote
Se todos os clientes do channel votarem ou se o Operator user POLL "melhor clube" a POLL fecha e os resultados são demonstrados.
Pode ser vitória do máximo de votos ou empate.
As POLL options vão ser um mapa para podermos incrementar  por 1 cada opção de cada vez que um cliente vota.
Cada cliente só pode votar uma vez, até que a POLL feche e cada channel só pode ter uma POLL aberta
*/

void CommandHandler::handlePoll(Server& server, Client& client, const std::vector<std::string>& args) 
{   
    std::string chanName = client.getChannel();
    Channel& channel = server.getOrCreateChannel(chanName, &client);
      // If a poll exists, close it and return
    if (channel.hasPoll()) 
    {
        channel.closeAndResetPoll();
        return;
    }
    if (args.size() < 3) 
    {
        std::string usage = "Usage: POLL <title> <option1> <option2> ...\r\n";
        send(client.getFd(), usage.c_str(), usage.length(), 0);
        return;
    }
    // Check if client is in a channel
   
    if (chanName.empty()) 
    {
        std::string error = ":irc.42.local 442 " + client.getNickname() + " :You're not on a channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    // Check operator status first
    if (!client.isOperator()) 
    {
        std::string error = ":irc.42.local 481 " + client.getNickname() + " " + chanName + " :Permission Denied - You're not an IRC operator\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    // Parse title and options
    // Parse in quotes
    std::string title = args[1];
    std::cout << "Title: " << title << "\n";
    std::vector<std::string> options;
    for (size_t i = 2; i < args.size(); ++i)
        options.push_back(args[i]);

    // Create the poll
    channel.createPoll(title, options);

    std::string botNick = "Bot";
    std::string prefix = ":" + botNick + "!bot@localhost PRIVMSG " + channel.getName() + " :";
    std::string pollMsg = prefix + "Poll created: " + title + "\r\n";
    std::string optionsMessage = prefix + "Options: ";
    for (size_t i = 0; i < options.size(); ++i)
    {
        optionsMessage += options[i];
        if (i != options.size() - 1)
            optionsMessage += " ";
    }
    optionsMessage += "\r\n";
    std::string notifyMessage = prefix + "Use VOTE <option> to vote.\r\n";
    
    // Send to all channel members
    const std::vector<Client*>& members = channel.getClients();
    for (size_t i = 0; i < members.size(); ++i)
    {
        send(members[i]->getFd(), pollMsg.c_str(), pollMsg.length(), 0);
        send(members[i]->getFd(), optionsMessage.c_str(), optionsMessage.length(), 0);
        send(members[i]->getFd(), notifyMessage.c_str(), notifyMessage.length(), 0);
    }
}

void CommandHandler::createPoll(Server& server, const std::string& title, const std::vector<std::string>& options, const std::string& channelName)
{
    Channel &channel = server.getOrCreateChannel(channelName, NULL);
    channel.createPoll(title, options);
}

void CommandHandler::handleVote(Server& server, Client& client, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::string error = ":irc.42.local 400 " + client.getNickname() + " " + client.getChannel() + " :VOTE usage: /VOTE <OPTION> \r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    
    Channel& channel = server.getOrCreateChannel(client.getChannel(), NULL);
    if (!channel.hasPoll())
    {
        std::string error = ":irc.42.local 400 " + client.getNickname() + " " + client.getChannel() + " :VOTE: No POLL is open on this channel\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    std::string option = args[1];
    channel.vote(client, option);
}
