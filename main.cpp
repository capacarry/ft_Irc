/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luiberna <luiberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:34:56 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/07/01 19:49:25 by luiberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/Irc.hpp"
#include "inc/Server.hpp"
#include "inc/Client.hpp"
#include "inc/Utils.hpp"

int main(int argc, char **argv)
{   
    Server server;
    std::cout << "---- SERVER ----\n";
    if (argc != 3)
    {   
        std::cerr << RED << B << "Error: ";
        std::cerr << "Usage: <port> <pass>\n" << R;
        return EXIT_FAILURE;
    }
    try
    {
        signal(SIGINT, Server::signalHandler); // (ctrl + c)
        signal(SIGQUIT, Server::signalHandler); // (ctrl + \)
        server.initServer(convToInt(argv[1]), argv[2]); // initialize server with port and password
    }
    catch (const std::exception& e)
    {   
        server.closeAll();
        std::cerr << RED << B << "Error: " << e.what() << R << "\n";
    }
    server.closeAll();
    std::cout << GRE << B << "Server closed!" << R << "\n";
    return EXIT_SUCCESS;
}
