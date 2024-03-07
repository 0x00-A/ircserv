/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 07:50:00 by rel-isma          #+#    #+#             */
/*   Updated: 2024/03/07 21:05:27 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::pass(Client &client)
{
    if (client.checkConnect())
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ALREADYREGISTRED) + " " + \
            client.getNick()  + " :You may not reregister";
        reply(client, response);
        return ;
    }
    if (this->serverParamiters.size() < 2)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick()  + " " + this->serverParamiters[0] + " :Not enough parameters";
        reply(client, response);
        return;
    }
    if (this->serverParamiters[1] == this->_passwd)
    {
        client.setHasPassed(true);
    }
    else
    {
        // ERR_PASSWDMISMATCH
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_PASSWDMISMATCH) + " " + \
            client.getNick()  + " " + this->serverParamiters[1] + " :Password incorrect";
        reply(client, response);
    }
}

void Server::nick(Client &client)
{
    // i wnat check here
    if (client.getHasPassed() == false)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :dir b3da pass";
        reply(client, response);
        return;
    }
    if (this->serverParamiters.size() < 2)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NONICKNAMEGIVEN) + " " + \
            client.getNick()  + " :No nickname given";
        reply(client, response);
        return;
    }
    if (client.checkNick(this->serverParamiters[1]) == false)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :Erroneus nickname";
        reply(client, response);
        return;
    }
    if (checkAlreadyNick(this->serverParamiters[1]) == false)
    {
        // this command not work now you moust add how find all client for check nick (add clinets in vector);
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NICKNAMEINUSE) + " " + \
            client.getNick()  + " :Nickname is already in use";

    }
    if (!client.getNick().empty())
    {
        if (client.getHasUsedNick() == true)
        {
            std::string response = ":" + client.getNick() + "!@ " + this->serverParamiters[0] + " :" +  this->serverParamiters[1];
            reply(client, response);
        }
        client.setNick(this->serverParamiters[1]);
        client.setHasUsedNick(true);
    }
}

void Server::user(Client &client)
{
    if (client.getHasPassed() == false)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_ERRONEUSNICKNAME) + " " + \
            client.getNick()  + " :dir b3da pass";
        reply(client, response);
        return;
    }
    if (this->serverParamiters.size() < 5)
    {
        std::string response = ":ft_irc.1337.ma " + std::to_string(ERR_NEEDMOREPARAMS) + " " + \
            client.getNick()  + " " + this->serverParamiters[0] + " :Not enough parameters";
        reply(client, response);
        return;
    }
    if (!client.getUsername().empty())
    {
        client.setHasUsedUser(true);
        client.setUsername(this->serverParamiters[1]);
    }
}
