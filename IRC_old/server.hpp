/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 12:20:19 by rel-isma          #+#    #+#             */
/*   Updated: 2024/03/07 21:09:19 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "client.hpp"
#include <vector>
#include <sstream>
#include <map>
#include "channel.hpp"

class client;
class Channel;

class Server
{
    private:
        int _port; //
        int _servfd; //
        std::string _passwd; //

        struct sockaddr_in serverAddress; //
        std::vector<std::string> serverParamiters;
        std::map<std::string, void (Server::*)(Client&)> commandMap;
        std::vector<Channel> channels; 
    public:
        Server(void);
        Server(int port, std::string password);
        ~Server();

        // start server
        void serverStart(void);
        // parser functions
        bool parseCommandClient(char *buffer, Client& client);
        void handleCommand(std::string& commad);

        // check nick clients
        bool checkAlreadyNick(std::string &nick);

        // command member functions
        void pass(Client& client);
        void user(Client& client);
        void nick(Client& client);

        // channel member functions

        void createChannel(std::string channelName);
        void joinChannel(Client& client, std::string channelName);
        void leaveChannel(Client& client, std::string channelName);
        // void listChannels();

        // send messg
		void 	reply(Client &client, std::string const& reply);
};