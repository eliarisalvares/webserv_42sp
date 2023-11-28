#include "ServerBuilder.hpp"

int	functionLocationDeal(t_infoServer *server, std::vector<std::string> input, int index) {
	//idk what to do yet
	Logger	log;

	log.debug("Dealing with location info.");
	(void)server;
	(void)input;
	return index;
}

// t_infoServer	getServerInfo(std::vector<std::string> input, int index) {
// 	t_infoServer	server;
// 	Logger	log;
// 	int	extraBrackets = 0;

// 	for (size_t i = index; i < input.size(); i++) {
// 		if (input[i].substr() == "server {")
// 			i++ ;
// 		if (input[i].substr(0, 12) == "server_name ") {
// 			server["server_name"] = input[i].substr(12);
// 			log.debug("Server name setted.");
// 		}
// 		if (input[i].substr(0, 12) == "error_pages ") {
// 			server["error_pages"] = input[i].substr(12);
// 			log.debug("Error pages setted.");
// 		}
// 		if (input[i].substr(0, 5) == "root ") {
// 			server["root"] = input[i].substr(5);
// 			log.debug("Root setted.");
// 		}
// 		if (input[i].substr(0, 16) == "allowed_methods ") {
// 			server["allowed_methods"] = input[i].substr(16);
// 			log.debug("Methods allowed setted.");
// 		}
// 		if (input[i].substr(0, 21) == "client_max_body_size ") {
// 			server["client_max_body_size"] = input[i].substr(21);
// 			log.debug("Client max body size setted.");
// 		}
// 		if (input[i].substr(0, 9) == "location ") {
// 			functionLocationDeal(&server, input, i);
// 			while (input[i].substr() != "}")
// 				i++;
// 		}
// 		if (input[i].substr() == "}")
// 			break ;
// 	}
// 	return server;
// }

int	getPortConf(std::vector<std::string> input, int index) {
	int	port;
	int	extraBrackets = 0;
	Logger	log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "listen ") {
			port = ftstring::strtoi(input[i].substr(7));
			if (port < 1 || port > 65535)
				throw PortNotFoundErrorExeption();
			if (port < 1024)
				throw PortNeedsSudoExeption();
			log.debug("Port successfully setted from .conf file.");
		}
		if (input[index].substr(0, 7) == "location ")
			extraBrackets++;
		if (input[index].substr() == "}")
			extraBrackets--;
		if (input[index].substr() == "}" && !extraBrackets)
			break ;
	}
	return (port);
}

std::string	getNameConf(std::vector<std::string> input, int index) {
	std::string	name;
	int	extraBrackets = 0;
	Logger	log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "server_name ") {
			//do stuff
			name = input[i].substr(7);
		}
		if (input[index].substr(0, 7) == "location ")
			extraBrackets++;
		if (input[index].substr() == "}")
			extraBrackets--;
		if (input[index].substr() == "}" && !extraBrackets)
			break ;
	}
	return (name);
}

const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Invalid Port found");
}

const char* PortNeedsSudoExeption::what() const throw() {
	return ("Sudo required to run");
}
