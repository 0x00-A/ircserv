/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:03:26 by rel-isma          #+#    #+#             */
/*   Updated: 2024/03/07 21:05:27 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server() {}

Server::Server(int port, std::string password) : _port(port), _passwd(password)
{
    this->commandMap["PASS"] = &Server::pass;
    this->commandMap["USER"] = &Server::user;
    this->commandMap["NICK"] = &Server::nick;
}

Server::~Server()
{
}

void Server::serverStart()
{
    char buffer[512];
    this->_servfd = socket(AF_INET, SOCK_STREAM, 0);
    int a = 1;
    setsockopt(this->_servfd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a));
    if (this->_servfd == -1)
    {
        std::cerr << "Error: socket creation failed" << std::endl;
        return;
    }

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(this->_port);
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_servfd, (struct sockaddr *)&this->serverAddress, sizeof(this->serverAddress)) == -1)
    {
        std::cerr << "Error: bind failed" << std::endl;
        close(this->_servfd);
        return;
    }

    if (listen(this->_servfd, 5) == -1)
    {
        std::cerr << "Error: listen failed" << std::endl;
        close(this->_servfd);
        return;
    }

    std::cout << "Server started on port " << this->_port << std::endl;

    struct sockaddr_in clientAddress;
    int clientSocket = accept(this->_servfd, nullptr, nullptr);

    if (clientSocket == -1)
    {
        std::cerr << "Error: accept failed" << std::endl;
        close(this->_servfd);
        return;
    }

    std::cout << ">>>>>>>> Client connected <<<<<<<<<" << std::endl;

    Client client;
    client.setClientSocket(clientSocket);
    client.setClientAddress(clientAddress);
    size_t len;
    while ((len = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)))
    {
        if (len < 0)
            exit(1);
        buffer[len] = '\0';
        if (parseCommandClient(buffer, client))
        {
            // for (size_t i = 0; i < this->serverParamiters.size(); i++)
            // {
            //     std::cout << "Paramiter" << i << ": >>|" << this->serverParamiters[i] << "|<<" << std::endl;
            // }
            std::map<std::string, void (Server::*)(Client &)>::iterator it = this->commandMap.find(this->serverParamiters[0]);
            if (it != this->commandMap.end())
            {
                // if not connected check this for connection
                (this->*it->second)(client);
            }
            else
            {
                std::cerr << "Error: invalid command" << std::endl;
            }
        }
        memset(buffer, 0, sizeof(buffer));
        this->command.clear();
        this->serverParamiters.clear();
    }
    close(clientSocket);
}

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