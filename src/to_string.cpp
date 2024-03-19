# include <sstream>
# include <string>

std::string	to_string(int n)
{
	std::stringstream ss;
	ss << n;
	return (ss.str());
}
