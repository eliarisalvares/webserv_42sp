#include "ftstring.hpp"

namespace ftstring {

std::string	trimWhitespace(const std::string& line)
{
	std::string trimmed = line;

	size_t start = trimmed.find_first_not_of(" \t");
	if (start != std::string::npos) {
		trimmed = trimmed.substr(start);
	}

	size_t end = trimmed.find_last_not_of(" \t");
	if (end != std::string::npos) {
		trimmed = trimmed.substr(0, end + 1);
	}
	return trimmed;
}

int	strtoi(std::string const &str)
{
	int			port;

	std::istringstream	sPort(str) ;
	sPort >> port;
	return port;
}

std::string	itostr(int nbr) {
	std::ostringstream	info;

	info << nbr;
	std::string	i_as_string(info.str());
	return i_as_string;
}

std::vector<std::string> split(std::string const &str, char delimiter)
{
	std::string					tmp;
	std::stringstream			ss(str);
	std::vector<std::string>	substrings;

	while (std::getline(ss, tmp, delimiter))
		substrings.push_back(tmp);
	return (substrings);
}

} //end namespace ftstring