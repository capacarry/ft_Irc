/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luiberna <luiberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 15:22:43 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/05/29 17:11:09 by luiberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Irc.hpp"

int convToInt(std::string port)
{
    int nb;
    
    std::istringstream(port) >> nb;
    return nb;
}
