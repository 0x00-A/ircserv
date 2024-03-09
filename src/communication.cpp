#include "Server.hpp"

void Server::reply(Client &client, std::string const& reply)
{

    std::string response = reply + "\r\n";
    // send(client.getSockfd(), response.c_str(), response.length(), 0);

    client.sdBuf().push(response);
    _pollfds[getIndexOfClient(client) + 1].events |= POLLOUT;
}
