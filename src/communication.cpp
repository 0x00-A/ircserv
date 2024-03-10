#include "Server.hpp"

void Server::reply(Client &client, string const& reply)
{

    string response = reply + "\r\n";
    send(client.getSockfd(), response.c_str(), response.length(), 0);
}
