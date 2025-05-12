/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:34:56 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/12 16:48:10 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/Server.hpp"
#include "inc/Client.hpp"
#include "inc/Ft_irc.hpp"

int main(int argc, char **argv)
{   
    (void)argv; //unused parameter
    Server server;
    if(argc != 2)
    {   
        std::cerr << RED << BOLD << "Error: ";
        std::cerr << "Usage: <ip> <port>" << RESET << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        signal(SIGINT, Server::signalHandler); // (ctrl + c)
        signal(SIGQUIT, Server::signalHandler); // (ctrl + \)
        server.initServer();
    }
    catch(const std::exception &e)
    {   
        server.closeAll();
        std::cerr << RED << BOLD << "Error: " << e.what() << RESET << std::endl;
    }
    std::cout << GRE << BOLD << "Server closed!" << RESET << std::endl;
    return EXIT_SUCCESS;
}