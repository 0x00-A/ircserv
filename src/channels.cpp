#include "Server.hpp"

void Server::addChannel(string channelName)
{
    Channel newChannel(channelName);
    this->_channels.push_back(newChannel);
}

void Server::joinChannel(Client &client, string channelName)
{
    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getName() == channelName)
        {
            (void)client;
            this->_channels[i].joinUser(client.getNick());
            return;
        }
    }
    cerr << "Error: channel not found" << endl;
}

// void Server::leaveChannel(Client &client, string channelName)
// {
//     for (size_t i = 0; i < this->_channels.size(); i++)
//     {
//         if (this->_channels[i].getName() == channelName)
//         {
//             (void)client;
//             // this->channels[i].removeClient(client);
//             return;
//         }
//     }
//     cerr << "Error: channel not found" << endl;
// }
