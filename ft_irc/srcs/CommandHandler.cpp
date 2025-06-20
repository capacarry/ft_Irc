/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 16:52:41 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/20 16:26:32 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/CommandHandler.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

static std::vector<std::string> split(const std::string &str) {
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    
    while (iss >> token)
        tokens.push_back(token);
    return tokens;
}

/*ERROS ENCONTRADOS

- Quando um usr sai do server, não dá para dar reconnect*
- Quando connectamos através o irss o cliente aparentemente conecta, mas depois diz que não está conectado
além disto se tentarmos connectar várias vezes o server cria vários clientes a través do mesmo terminal, que não fazem nada.
*/

void CommandHandler::handleCommand(Server &server, Client &client, const std::string &rawMessage) {
    std::string clean = rawMessage;
    clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\n'), clean.end());

    std::cout << "Raw command received: [" << clean << "]" << std::endl;

    std::vector<std::string> args = split(clean);
    if (args.empty())
        return;

    std::string cmd = args[0];

    // Always allow these
    if (cmd == "CAP" || cmd == "PING" || cmd == "PONG") {
        return;
    }

    // Handle PASS before anything else
    if (cmd == "PASS") {
        handlePass(server, client, args);
        return;
    }

    // If password is required and not given yet, reject further input
    if (client.getServerNeedsPass() && !client.getHasGivenPass()) {
        std::string error = ":irc.42.local 464 * :Password required\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        server.closeEvent(client.getFd());
        return;
    }

    // Not yet registered, allow NICK/USER
    if (!client.isRegistered()) {
        if (cmd == "NICK") {
            handleNick(server, client, args);
        } else if (cmd == "USER") {
            handleUser(server, client, args);
        } else {
            std::string error = ":irc.42.local 451 * :You have not registered\r\n";
            send(client.getFd(), error.c_str(), error.length(), 0);
            return;
        }

        client.tryRegister(); // If all pieces are there, this will set isRegistered = true
        return;
    }

    // Post-registration commands
    if (client.isRegistered()) {
        if (cmd == "JOIN") {
            handleJoin(server, client, args);
        } else if (cmd == "PRIVMSG") {
            handlePrivmsg(server, client, args);
        } else if (cmd == "QUIT") {
            // handleQuit(server, client, args);
        } else {
            std::cerr << "Unknown command: " << cmd << std::endl;
        }
    }
}


void CommandHandler::handlePass(Server &server, Client &client, const std::vector<std::string> &args) 
{   
    if (args.size() < 2) {
        std::string error = ":irc.42.local 461 * PASS :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::cout << "PASS received: " << args[1] << std::endl;

    if (args[1] != server.getPassword()) {
        std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
        std::string error = ":irc.42.local 464 " + nick + " :Password incorrect\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        server.closeEvent(client.getFd());
        return;
    }

    client.setHasGivenPass(true);
}


bool CommandHandler::notDuplicateNickname(Server &server, const std::string &nickname) {
    const std::vector<Client> &clients = server.getClients();
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].getNickname() == nickname) {
            std::string error = ":irc.42.local 433 " + nickname + " :Nickname is already in use\r\n";
            send(clients[i].getFd(), error.c_str(), error.length(), 0);
            //remove the client from the server's client list
            server.closeEvent(clients[i].getFd());
            return false;
        }
    }
    return true;
}

bool CommandHandler::notDuplicateUsername(Server &server, const std::string &username) {
    const std::vector<Client> &clients = server.getClients();
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].getUsername() == username) {
            std::string error = ":irc.42.local 433 " + username + " :Username is already in use\r\n";
            send(clients[i].getFd(), error.c_str(), error.length(), 0);
            server.closeEvent(clients[i].getFd());
            return false;
        }
    }
    return true;
}


void CommandHandler::handleNick(Server &server, Client &client, const std::vector<std::string> &args) {
    if (args.size() >= 2 && !args[1].empty() && notDuplicateNickname(server, args[1]))
        client.setNickname(args[1]);
}

void CommandHandler::handleUser(Server &server, Client &client, const std::vector<std::string> &args) {
    if (args.size() >= 5 && !args[1].empty() && notDuplicateUsername(server, args[1]))
        client.setUsername(args[1]); //Tentar guardar o username
}

void CommandHandler::handleJoin(Server& server, Client& client, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::string error = ":irc.42.local 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    if (!client.isRegistered()) {
        std::string error = ":irc.42.local 451 " + client.getNickname() + " JOIN :You have not registered\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::string chanName = args[1];
    if (chanName[0] != '#')
        chanName = "#" + chanName;

    Channel& channel = server.getOrCreateChannel(chanName);

    // Add client to the channel
    channel.addClient(&client);

    // Send JOIN message to the client
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + chanName + "\r\n";
    send(client.getFd(), joinMsg.c_str(), joinMsg.length(), 0);

    // Send TOPIC (332)
    std::string topic = ":irc.42.local 332 " + client.getNickname() + " " + chanName + " :No topic is set\r\n";
    send(client.getFd(), topic.c_str(), topic.length(), 0);

    // Send NAMES list (353)
    std::string namesReply = ":irc.42.local 353 " + client.getNickname() + " = " + chanName + " :";
    const std::vector<Client*>& members = channel.getClients();
    for (size_t i = 0; i < members.size(); ++i) {
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
    for (size_t i = 0; i < members.size(); ++i) {
        if (members[i]->getFd() != client.getFd()) {
            send(members[i]->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
        }
    }
}


void CommandHandler::handlePrivmsg(Server& server, Client& sender, const std::vector<std::string>& args) {
    
    if(!sender.isRegistered()) {
        std::string error = ":irc.42.local 451 " + sender.getNickname() + " PRIVMSG :You have not registered\r\n";
        send(sender.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    if (args.size() < 3) {
        std::string error = ":irc.42.local 461 " + sender.getNickname() + " PRIVMSG :Not enough parameters\r\n";
        send(sender.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    std::string target = args[1];
    std::string message;

    // Concatena todas as partes da mensagem
    for (size_t i = 2; i < args.size(); ++i) {
        message += args[i];
        if (i != args.size() - 1)
            message += " ";
    }

    // Remove os dois pontos no início da mensagem, se existirem
    if (message[0] == ':')
        message = message.substr(1);
    
    // Verifica se o destinatário é um canal ou um cliente
    std::string fullMsg = ":" + sender.getNickname() + "!" + sender.getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#') {
        /// Mensagem para um canal
        Channel& chan = server.getOrCreateChannel(target);
        chan.normalizeChannelName(); 
        // Normaliza o nome do canal
        const std::vector<Client*>& members = chan.getClients();
        for (size_t i = 0; i < members.size(); ++i) {
            if (members[i]->getFd() != sender.getFd())
                send(members[i]->getFd(), fullMsg.c_str(), fullMsg.length(), 0);
        }
    }
    else {
        // DM para a shorty
        const std::vector<Client>& clients = server.getClients();
        for (size_t i = 0; i < clients.size(); ++i) {
            if (clients[i].getNickname() == target) {
                send(clients[i].getFd(), fullMsg.c_str(), fullMsg.length(), 0);
                return;
            }
        }
        // Se não encontrar o destinatário, envia um erro
        std::string error = ":irc.42.local 401 " + sender.getNickname() + " " + target + " :No such nick/channel\r\n";
        // Envia o erro para o remetente
        send(sender.getFd(), error.c_str(), error.length(), 0);
    }
}
