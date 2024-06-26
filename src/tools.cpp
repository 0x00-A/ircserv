#include "ircserv.hpp"

bool	isdigitstring(const string& str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
		{
			return (false);
		}
	}
	return (true);
}

struct tm *getCurrentTime() 
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return timeinfo;
}

string itos(int num)
{
    stringstream ss;
    ss << num;
    return ss.str();
}

void to_upper(string& str) 
{
	for (size_t i = 0; i < str.size() ; i++)
	{
        str[i] = static_cast<char>(std::toupper(str[i]));
	}
}
