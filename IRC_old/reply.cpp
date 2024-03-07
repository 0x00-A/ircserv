/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 11:51:52 by rel-isma          #+#    #+#             */
/*   Updated: 2024/03/07 14:00:14 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::reply(Client &client, std::string const& reply)
{

    std::string response = reply + "\r\n";
    send(client.getClientSocket(), response.c_str(), response.length(), 0);
}