
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

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

#endif  // CHANNEL_HPP