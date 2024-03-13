
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

class Channel
{
    private:
        string name;
        string admin;
        bool    key;
        std::vector<Client *> clients;

    public:
        Channel(string channelName);
        ~Channel();

        // Channel operations
        void join(Client &client);
        void leave(Client &client);
        void broadcastMessage(Client &sender, string message);
        void listClients();

        // Getters
        string getName();
        std::vector<Client *> &getClients();
};

#endif  // CHANNEL_HPP