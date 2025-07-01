/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-cas <gude-cas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 15:22:43 by gcapa-pe          #+#    #+#             */
/*   Updated: 2025/06/25 21:15:01 by gude-cas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Irc.hpp"

int convToInt(std::string port)
{
    int nb;
    
    std::istringstream(port) >> nb;
    return nb;
}

std::string ft_quote_trim(const std::string& str)
{
    std::string trimmed = str;
    if (trimmed.size() >= 2 && trimmed[0] == '"' && trimmed[trimmed.size() - 1] == '"')
    {
        trimmed.erase(0, 1); // remove first quote
        trimmed.erase(trimmed.size() - 1, 1); // remove last quote
    }
    return trimmed;
}
