/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:34:56 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/18 12:35:56 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/Irc.hpp"
#include "inc/Server.hpp"
#include "inc/Client.hpp"
#include "inc/Utils.hpp"

int main(int argc, char **argv)
{   
    Server server;
    std::cout << "---- SERVER ----" << std::endl;
    if (argc != 3)
    {   
        std::cerr << RED << B << "Error: ";
        std::cerr << "Usage: <port> <pass>" << R << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        signal(SIGINT, Server::signalHandler); // (ctrl + c)
        signal(SIGQUIT, Server::signalHandler); // (ctrl + \)
        server.initServer(convToInt(argv[1]), argv[2]); // initialize server with port and password
    }
    catch (const std::exception &e)
    {   
        server.closeAll();
        std::cerr << RED << B << "Error: " << e.what() << R << std::endl;
    }
    server.closeAll();
    std::cout << GRE << B << "Server closed!" << R << std::endl;
    return EXIT_SUCCESS;
}