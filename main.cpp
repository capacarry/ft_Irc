/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcapa-pe <gcapa-pe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:34:56 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/13 15:18:38 by gcapa-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/Server.hpp"
#include "inc/Client.hpp"
#include "inc/Ft_irc.hpp"

int convToInt(std::string port)
{
    int nb;
    
    std::istringstream(port) >> nb;
    return(nb);
}

int main(int argc, char **argv)
{   
    (void)argv; //unused parameter
    Server server;
    std::cout << "---- SERVER ----" << std::endl;
    if(argc != 3)
    {   
        std::cerr << RED << BOLD << "Error: ";
        std::cerr << "Usage: <ip> <pass>" << RESET << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        signal(SIGINT, Server::signalHandler); // (ctrl + c)
        signal(SIGQUIT, Server::signalHandler); // (ctrl + \)
        server.initServer(convToInt(argv[1]));
    }
    catch(const std::exception &e)
    {   
        server.closeAll();
        std::cerr << RED << BOLD << "Error: " << e.what() << RESET << std::endl;
    }
    std::cout << GRE << BOLD << "Server closed!" << RESET << std::endl;
    return EXIT_SUCCESS;
}