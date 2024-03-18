#include "Server.hpp"

void Server::reply(Client &client, string const& msg)
{

    string response = msg + "\r\n";

    client.sdBuf().push(response);
    _pollfds[getIndexOfClient(client) + 1].events |= POLLOUT;
}
