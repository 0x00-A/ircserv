#include "Server.hpp"

void Server::reply(Client &client, string const& msg)
{

    // string response = msg + "\r\n";
    // send(client.getSockfd(), response.c_str(), response.length(), 0);

    client.sdBuf().push(msg + "\r\n");
    _pollfds[getIndexOfClient(client) + 1].events |= POLLOUT;
}
