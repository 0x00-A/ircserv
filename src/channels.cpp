#include "Server.hpp"

void Server::addChannel(string channelName, Client &client)
{
    // ERR_CHANNELISFULL
    //                     "<channel> :Cannot join channel (+l)" 
    // if ()
    // {

    // }
    Channel newChannel(channelName);
    this->_channels.push_back(newChannel);
    this->_channels.back().setUserAsOperator(client.getNick());
}

void Server::joinChannel(Client &client, string channelName)
{
    string response;

    for (size_t i = 0; i < this->_channels.size(); i++)
    {
        if (this->_channels[i].getName() == channelName)
        {
            // if (_channels[i].hasUserLimit())
            // {
            //     // if (_channels[i].getUserLimit() < _channels[i].)
            //     // {
            //     //     // ERR_CHANNELISFULL
            //     //     // "<channel> :Cannot join channel (+l)"
            //     //     // ":" + nick + " 471 " + channelName + " :Cannot join channel (+l)\r\n"
            //     //     // response = ":ft_irc.1337.ma "


            //     // }
            // }
            this->_channels[i].joinUser(client.getNick());
            return ;
        }
    }
    addChannel(channelName, client);
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
