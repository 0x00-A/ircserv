/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:57:09 by rel-isma          #+#    #+#             */
/*   Updated: 2024/03/05 14:57:55 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include "client.hpp"

class Channel
{
    private:
        std::string name;
        std::vector<Client *> clients;

    public:
        Channel(std::string channelName);
        ~Channel();

        // Channel operations
        void join(Client &client);
        void leave(Client &client);
        void broadcastMessage(Client &sender, std::string message);
        void listClients();

        // Getters
        std::string getName();
        std::vector<Client *> &getClients();
};
