/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-isma <rel-isma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 12:59:19 by rel-isma          #+#    #+#             */
/*   Updated: 2024/02/29 20:49:54 by rel-isma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int main(int argc, char **argv)
{
    int port;

    if (argc != 3)
    {
        std::cout << "Usage: ./server <port> <password>" << std::endl;
        return 1;
    }
    port = std::atoi(argv[1]);
    std::string password(argv[2]);
    Server server(port, password);
    server.serverStart();
    return 0;
}