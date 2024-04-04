#include "IRCbot.hpp"

void IRCbot::connectToWeatherServer(void)
{
	struct sockaddr_in serv_addr;

	if ( (_weatherSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::perror("socket");
		close(ircSock);
		exit(1);
	}

	uint32_t ipInt = inet_addr(_weatherServIP.c_str());
	if (ipInt == INADDR_NONE)
	{
		std::perror("inet_addr");
		close(_weatherSock);
		throw (string("Invalid IP address for weather server"));
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);
	serv_addr.sin_addr.s_addr = ipInt;

	if (connect(_weatherSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::perror("connect");
		close(_weatherSock);
		throw (string("Connection to weather server Failed"));
	}
}

string IRCbot::getWeatherInfo(string& location)
{
    char buffer[1024] = {0};

	(void)location;
    std::string request = 
        "GET /data/2.5/weather?q=" + location + "&APPID=851bb946327ee4bdc5230fe57cd6439f HTTP/1.1\r\n"
        "Host: " + _weatherServIP + "\r\n"
        "Connection: close\r\n\r\n";

    write(_weatherSock, request.c_str(), request.size());
    read(_weatherSock, buffer, sizeof(buffer)-1);

	return (buffer);
}

void IRCbot::sendWeatherInfo(string& client_nick, string& location)
{
	connectToWeatherServer();

	if (location.empty())
	{
		sendReply("PRIVMSG " + client_nick + " :No city provided");
		return;
	}
	string	response = getWeatherInfo(location);
	if (response.find("404") != string::npos)
	{
		sendReply("PRIVMSG " + client_nick + " :City not found");
		return;
	}
	std::string res, reply;
    reply = "Weather for " + parseInfo("\"name\":\"", "\"", response) + ", " + \
			parseInfo("\"country\":\"", "\"", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Coordinates: Longitude " + parseInfo("\"lon\":", "\"", response) + \
			" Latitude " + parseInfo("\"lat\":", "}", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Weather Main: " + parseInfo("\"main\":\"", "\"", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Weather Description: " + parseInfo("\"description\":\"", "\"", response);
	sendReply("PRIVMSG " + client_nick + " :" + reply);
	double d = std::strtod(parseInfo("\"temp\":", ",", response).c_str(), NULL);
	d -= 273.15;
	std::stringstream ss;
	ss << d;
    reply = "- Temperature: " + ss.str() + " Celsius";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Humidity: " + parseInfo("\"humidity\":", ",", response) + "%";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
    reply = "- Wind Speed: " + parseInfo("\"speed\":", ",", response) + " m/s";
	sendReply("PRIVMSG " + client_nick + " :" + reply);
	close(_weatherSock);
}

