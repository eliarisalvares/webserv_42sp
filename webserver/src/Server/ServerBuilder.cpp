#include "ServerBuilder.hpp"

int	getPort(std::string line) {
	std::string	info;
	int			port;

	info = line.substr(7);

	std::istringstream	sPort(info) ;
	sPort >> port;
	return port;
}

std::string	getPortInfo(int port) {
	std::ostringstream	info;

	info << port;
	std::string	i_as_string(info.str());
	return (i_as_string);
}

void	getServerInfo(std::vector<std::string> input, int index) {
	t_infoServer	server;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr() == "}")
			break ;
		if (input[i].substr(0, 7) == "listen ") {
			int			port = getPort(input[i]);
			if (!port)
				throw PortNotFoundErrorExeption();
			if (port < 1024)
				throw PortNeedsSudoExeption();
			server["listen"] = getPortInfo(port);
		}
	}
}

const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Wrong port input");
}

const char* PortNeedsSudoExeption::what() const throw() {
	return ("Sudo required to run");
}