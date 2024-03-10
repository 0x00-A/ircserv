#include "Channel.hpp"

Channel::Channel(string channelName) : name(channelName) {}

Channel::~Channel() {}

void Channel::join(Client &client)
{
    clients.push_back(&client);
    cout << client.getNick() << " joined channel " << name << endl;
}

void Channel::leave(Client &client)
{
    std::vector<Client *>::iterator it = std::find(clients.begin(), clients.end(), &client);
    if (it != clients.end())
    {
        clients.erase(it);
        cout << client.getNick() << " left channel " << name << endl;
    }
}

void Channel::broadcastMessage(Client &sender, string message)
{
    cout << "[" << name << "] " << sender.getNick() << ": " << message << endl;
}

void Channel::listClients()
{
    cout << "Users in channel " << name << ": ";

    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        cout << (*it)->getNick() << " ";
    }
    cout << endl;
}

string Channel::getName()
{
    return name;
}

std::vector<Client *> &Channel::getClients()
{
    return clients;
}
