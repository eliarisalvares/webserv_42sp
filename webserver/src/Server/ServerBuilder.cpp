#include "ServerBuilder.hpp"

t_infoServer	getServerInfo(std::vector<std::string> input, int index) {
	t_infoServer	server;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "listen ") {
			int			port = ftstring::strtoi(input[i].substr(7));
			if (port < 1 || port > 65535)
				throw PortNotFoundErrorExeption();
			if (port < 1024)
				throw PortNeedsSudoExeption();
			server["listen"] = ftstring::itostr(port);
		}
		if (input[i].substr(0, 12) == "server_name ") {
			server["server_name"] = input[i].substr(12);
		}
		if (input[i].substr() == "}")
			break ;
	}
	// std::cout << "Listen on port: " << server["listen"] << std::endl;
	return server;
}

const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Invalid Port found");
}

const char* PortNeedsSudoExeption::what() const throw() {
	return ("Sudo required to run");
}