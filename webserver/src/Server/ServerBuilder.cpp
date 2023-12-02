#include "ServerBuilder.hpp"

// 		if (input[i].substr(0, 12) == "error_pages ") {
// 			server["error_pages"] = input[i].substr(12);
// 			log.debug("Error pages setted.");
// 		}
// 		if (input[i].substr(0, 16) == "allowed_methods ") {
// 			server["allowed_methods"] = input[i].substr(16);
// 			log.debug("Methods allowed setted.");
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
	int		port;
	int		extraBrackets = 0;
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

// Sets the maximum body size for client requests.
// Megabytes, M, and Kilobytes, K, are the accepted units.
int	getBodySizeConf(std::vector<std::string> input, int index) {
	int			bodySize;
	std::string	bodySizeString;
	int			extraBrackets = 0;
	Logger		log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 21) == "client_max_body_size ") {
			bodySizeString = input[i].substr(21);
			//read size, get M or K -> 1M is the limit
			//check string for the letter -> give error if letter is different than M or K
			//if bigger than 1M -> give error
			// 1K is the minimal size
			//range from 1K to 1000K -> pass this info ahead
			bodySize = ftstring::strtoi(bodySizeString);
			log.debug("client_max_body_size successfully setted from .conf file.");
		}
		if (input[index].substr(0, 7) == "location ")
			extraBrackets++;
		if (input[index].substr() == "}")
			extraBrackets--;
		if (input[index].substr() == "}" && !extraBrackets)
			break ;
	}
	return (bodySize);
}

std::string	getRootConf(std::vector<std::string> input, int index) {
	std::string	root;
	int			extraBrackets = 0;
	Logger		log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 5) == "root ") {
			//check if name makes sense? idk
			root = input[i].substr(5);
			log.debug("Root setted.");
		}
		if (input[index].substr(0, 7) == "location ")
			extraBrackets++;
		if (input[index].substr() == "}")
			extraBrackets--;
		if (input[index].substr() == "}" && !extraBrackets)
			break ;
	}
	return (root);
}

std::vector<std::string>	getNameConf(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;
	int							extraBrackets = 0;
	Logger						log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "server_name ") {
			name = input[i].substr(7);
			serverName = ftstring::split(name, ' ');
			log.debug("Server name successfully setted from .conf file.");
		}
		if (input[index].substr(0, 7) == "location ")
			extraBrackets++;
		if (input[index].substr() == "}")
			extraBrackets--;
		if (input[index].substr() == "}" && !extraBrackets)
			break ;
	}
	return (serverName);
}

const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Invalid Port found");
}

const char* PortNeedsSudoExeption::what() const throw() {
	return ("Sudo required to run");
}
