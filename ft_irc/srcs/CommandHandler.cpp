/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 16:52:41 by luiberna          #+#    #+#             */
/*   Updated: 2025/06/03 15:52:32 by gcapa-pe         ###   ########.fr       */
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
    (void)server;

    std::string clean = rawMessage;
    clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\n'), clean.end());

    std::cout << "Raw command received: [" << clean << "]" << std::endl;

    std::vector<std::string> args = split(clean);
    if (args.empty())
        return;

    std::string cmd = args[0];

   
    if(server.getPassword().empty()) 
    {
        client.setServerNeedsPass(false);
    } 
    else 
    {
        client.setServerNeedsPass(true);
    }
         
if (!client.isRegistered()) 
{       
    if (cmd == "PASS" && !client.getHasGivenPass() && client.getServerNeedsPass())
    {
        handlePass(server, client, args);
       if (!client.getHasGivenPass()) 
       {
            if (cmd != "PASS") 
            {
                std::string error = ":irc.42.local 464 * :Password required\r\n";
                send(client.getFd(), error.c_str(), error.length(), 0);
                server.closeEvent(client.getFd());
                return;
            }
        }
    }
    if (cmd == "NICK" && client.getNickname().empty())
        handleNick(server, client, args);
    else if (cmd == "USER" && client.getUsername().empty()) 
        handleUser(server, client, args); 
    else 
    {
        std::string error = ":irc.42.local 451 " + client.getNickname() + " :You have not registered\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
    }
    client.tryRegister();
}

if(client.isRegistered()) 
{
    if (cmd == "JOIN") {
        handleJoin(server, client, args);
    } else if (cmd == "PRIVMSG") {
        handlePrivmsg(server, client, args);
    } else if (cmd == "QUIT") {
        // TODO: implement handleQuit(server, client, args);
    } else if (cmd == "CAP" || cmd == "PING" || cmd == "PONG") {
        // Optionally handle these, or just ignore
    } else {
        std::cerr << "Unknown command: " << cmd << std::endl;
    }
}
   
}

void CommandHandler::handlePass(Server &server, Client &client, const std::vector<std::string> &args) 
{   
    //check if the pass the client sent is the same as the server password
    std::cout << "pass: " << args[1] << std::endl; 
    if(args[1] != server.getPassword()) {
        std::string error = ":irc.42.local 464 " + client.getNickname() + " :Password incorrect\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        server.closeEvent(client.getFd());
        return;
    }
    else
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
    // O cliente precisa de dar join a um canal com um nome se não não faz nada
    
    
    if (args.size() < 2)
        return;

    if( !client.isRegistered()) {
        std::string error = ":irc.42.local 451 " + client.getNickname() + " JOIN :You have not registered\r\n";
        send(client.getFd(), error.c_str(), error.length(), 0);
        return;
    }
    std::string chanName;
    // Pega o nome do canal e tenta criar o canal
    if(args[1][0] == '#')
        chanName = args[1];
    else
        chanName = "#" + args[1]; // Adiciona o # se não estiver presente

    // Se o canal existe ou não o servidor cria ou pega o canal
    Channel& channel = server.getOrCreateChannel(chanName);

    //Adiciona o cliente ao canal e a sua lista interna
    channel.addClient(&client);

    // Envia a mensagem de join para o cliente
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + chanName + "\r\n";
    send(client.getFd(), joinMsg.c_str(), joinMsg.length(), 0);

    // Cria as mensagens para o canal (332 codigo de topico, 353 topico de nomes, 366 fim de lista de nomes)
    std::string topic = ":irc.42.local 332 " + client.getNickname() + " " + chanName + " :No topic is set\r\n";
    std::string names = ":irc.42.local 353 " + client.getNickname() + " = " + chanName + " :@" + client.getNickname() + "\r\n";
    std::string endNames = ":irc.42.local 366 " + client.getNickname() + " " + chanName + " :End of /NAMES list.\r\n";

    // Envia o topico e os nomes para o cliente
    send(client.getFd(), topic.c_str(), topic.length(), 0);
    send(client.getFd(), names.c_str(), names.length(), 0);
    send(client.getFd(), endNames.c_str(), endNames.length(), 0);
     //server.printChannelInfo(); 
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
