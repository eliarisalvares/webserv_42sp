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
		if (input[index].substr() == "}" || input[index].substr(0, 7) == "location ")
			break ;
	}
	return (port);
}

// Sets the maximum body size for client requests.
// Megabytes, M, and Kilobytes, K, are the accepted units.
int	getBodySizeConf(std::vector<std::string> input, int index) {
	int			bodySize;
	std::string	bodySizeString;
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
			log.debug("client_max_body_size setted from .conf file.");
		}
		if (input[index].substr() == "}" || input[index].substr(0, 7) == "location ")
			break ;
	}
	return (bodySize);
}

std::string	getRootConf(std::vector<std::string> input, int index) {
	std::string	root;
	Logger		log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 5) == "root ") {
			//check if name makes sense? idk
			root = input[i].substr(5);
			log.debug("Root setted from .conf file.");
		}
		if (input[index].substr() == "}" || input[index].substr(0, 7) == "location ")
			break ;
	}
	return (root);
}

std::vector<std::string>	getCGIConf(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;
	Logger						log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 4) == "cgi ") {
			name = input[i].substr(4);
			serverName = ftstring::split(name, ' ');
			if (serverName.size() != 2)
				throw CGIMissconfigurationException();
			else if (serverName[0] != ".py" || serverName[1] != "python3")
				throw CGINotSupportedException();
			log.debug("CGI setted from .conf file.");
		}
		if (input[index].substr() == "}" || input[index].substr(0, 7) == "location ")
			break ;
	}
	return (serverName);
}

std::vector<std::string>	getNameConf(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;
	Logger						log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "server_name ") {
			name = input[i].substr(7);
			serverName = ftstring::split(name, ' ');
			log.debug("Server name setted from .conf file.");
		}
		if (input[index].substr() == "}" || input[index].substr(0, 7) == "location ")
			break ;
	}
	return (serverName);
}

t_location	getLocConf(std::vector<std::string> input, int index) {
	//deal with location :')
	t_location					location;
	Logger						log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "location ") {
			//do stuff
		}
		if (input[index].substr() == "}")
			break ;
	}
	return (location);
}

std::set<std::string>	getMethConf(std::vector<std::string> input, int index) {
	std::set<std::string>		methods;
	std::vector<std::string>	words;
	Logger					log;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 16) == "allowed_methods ") {
			//check if name makes sense? idk
			words = ftstring::split(input[i].substr(16), ' ');
			if (words.size() <= 0 || words.size() >= 4)
				throw InvalidNbrMethodsException();
			for (size_t j = 0; j < words.size(); j++) {
				if (words[j] == "GET" || words[j] == "POST" || words[j] == "DELETE")
					methods.insert(words[j]);
				else
					throw InvalidMethodsException();
			}
			log.debug("Allowed Methods setted from .conf file.");
		}
		if (input[index].substr() == "}" || input[index].substr(0, 7) == "location ")
			break ;
	}
	return (methods);
}

const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Invalid Port found.");
}

const char* PortNeedsSudoExeption::what() const throw() {
	return ("Sudo required to run.");
}

const char* CGIMissconfigurationException::what() const throw() {
	return ("Wrong number of arguments in CGI.");
}

const char* CGINotSupportedException::what() const throw() {
	return ("CGI mode not supported by webserv.");
}

const char* InvalidNbrMethodsException::what() const throw() {
	return ("Wrong values for Allowed Methods.");
}

const char* InvalidMethodsException::what() const throw() {
	return ("Invalid Allowed Method.");
}
