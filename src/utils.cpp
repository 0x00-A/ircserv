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
    // oss << std::setw(3) << std::setfill('0') << num;
    ss << num;
    return ss.str();
}
