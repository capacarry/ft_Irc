/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-cas <gude-cas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 13:36:22 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/25 11:24:18 by gude-cas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define WHI "\e[0;37m"
#define RED "\e[1;31m"
#define GRE "\e[1;32m"
#define BLU "\e[1;34m"
#define YEL "\e[1;33m"
#define B   "\e[1m"
#define R   "\e[0m"

#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <iostream>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/epoll.h>
