#include "ServerBuilder.hpp"

int	getPort(std::string line) {
	std::string	info;
	int			port;

	info = line.substr(7);

	std::istringstream	sPort(info) ;
	sPort >> port;
	return port;
}

void	getServerInfo(std::vector<std::string> input, int index) {
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr() == "}")
			break ;
		if (input[i].substr(0, 7) == "listen ") {
			int port = getPort(input[i]);
			if (!port)
				throw PortNotFoundErrorExeption();
		}
	}
}

const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Wrong port input");
}
