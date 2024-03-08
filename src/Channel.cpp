#include "Channel.hpp"

Channel::Channel(std::string channelName) : name(channelName) {}

Channel::~Channel() {}

void Channel::join(Client &client)
{
    clients.push_back(&client);
    std::cout << client.getNick() << " joined channel " << name << std::endl;
}

void Channel::leave(Client &client)
{
    std::vector<Client *>::iterator it = std::find(clients.begin(), clients.end(), &client);
    if (it != clients.end())
    {
        clients.erase(it);
        std::cout << client.getNick() << " left channel " << name << std::endl;
    }
}

void Channel::broadcastMessage(Client &sender, std::string message)
{
    std::cout << "[" << name << "] " << sender.getNick() << ": " << message << std::endl;
}

void Channel::listClients()
{
    std::cout << "Users in channel " << name << ": ";

    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        std::cout << (*it)->getNick() << " ";
    }
    std::cout << std::endl;
}

std::string Channel::getName()
{
    return name;
}

std::vector<Client *> &Channel::getClients()
{
    return clients;
}
