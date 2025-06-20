/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luiberna <luiberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:36:22 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/29 17:10:43 by luiberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define WHI "\e[0;37m" //-> for white color
#define RED "\e[1;31m" //-> for red color
#define GRE "\e[1;32m" //-> for green color
#define BLU "\e[1;34m" //-> for blue color
#define YEL "\e[1;33m" //-> for yellow color
#define B   "\e[1m" //-> for bold text
#define R   "\e[0m" //-> for reset color

#include <vector>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <map>

// #include <string>
// #include <errno.h>
// #include <stdexcept>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>