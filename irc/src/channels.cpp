#include "Server.hpp"

void Server::createChannel(std::string channelName)
{
    Channel newChannel(channelName);
    this->channels.push_back(newChannel);
}

void Server::joinChannel(Client &client, std::string channelName)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].getName() == channelName)
        {
            (void)client;
            // this->channels[i].addClient(client);
            return;
        }
    }
    std::cerr << "Error: channel not found" << std::endl;
}

void Server::leaveChannel(Client &client, std::string channelName)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].getName() == channelName)
        {
            (void)client;
            // this->channels[i].removeClient(client);
            return;
        }
    }
    std::cerr << "Error: channel not found" << std::endl;
}

bool Server::checkAlreadyNick(std::string &nick)
{
    (void)nick;
    // after add clients check nick name all client
    return true;
}