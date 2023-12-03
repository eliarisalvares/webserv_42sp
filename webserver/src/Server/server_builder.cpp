#include "server_builder.hpp"

t_location	initLocation(void) {
	t_location	location;

	location.allowed_methods = http::methods;
	location.location = LOCATION;
	location.root = "/content";
	return (location);
}

int	obtainPort(std::vector<std::string> input, int index) {
	int		port;
	Logger	log;

	if (input[index].substr(0, 7) == "listen ") {
		port = ftstring::strtoi(input[index].substr(7));
		if (port < 1 || port > 65535)
			throw PortNotFoundErrorExeption();
		if (port < 1024)
			throw PortNeedsSudoExeption();
		log.debug("Port successfully setted from .conf file.");
	}
	return (port);
}

// Sets the maximum body size for client requests.
// Megabytes, M, and Kilobytes, K, are the accepted units.
int	obtainBodySize(std::vector<std::string> input, int index) {
	int			bodySize;
	std::string	bodySizeString;
	Logger		log;

	if (input[index].substr(0, 21) == "client_max_body_size ") {
		bodySizeString = input[index].substr(21);
		//read size, obtain M or K -> 1M is the limit
		//check string for the letter -> give error if letter is different than M or K
		//if bigger than 1M -> give error
		// 1K is the minimal size
		//range from 1K to 1000K -> pass this info ahead
		bodySize = ftstring::strtoi(bodySizeString);
		log.debug("client_max_body_size setted from .conf file.");
	}
	return (bodySize);
}

std::string	obtainRoot(std::vector<std::string> input, int index) {
	std::string	root;
	Logger		log;

	if (input[index].substr(0, 5) == "root ") {
		root = input[index].substr(5);
		//check if root makes sense/exists
		log.debug("Root setted from .conf file.");
	}
	return (root);
}

std::vector<std::string>	obtainCGI(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;
	Logger						log;

	if (input[index].substr(0, 4) == "cgi ") {
		name = input[index].substr(4);
		serverName = ftstring::split(name, ' ');
		if (serverName.size() != 2)
			throw CGIMissconfigurationException();
		else if (serverName[0] != ".py" || serverName[1] != "python3")
			throw CGINotSupportedException();
		log.debug("CGI setted from .conf file.");
	}
	return (serverName);
}

std::vector<std::string>	obtainName(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;
	Logger						log;

	if (input[index].substr(0, 7) == "server_name ") {
		name = input[index].substr(7);
		serverName = ftstring::split(name, ' ');
		log.debug("Server name setted from .conf file.");
	}
	return (serverName);
}

t_location	obtainLoc(std::vector<std::string> input, int index) {
	//deal with location :')
	t_location					location;
	Logger						log;
	std::vector<std::string>	locName;


	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr(0, 9) == "location ") {
			locName = ftstring::split(input[i].substr(9), ' ');
			location.location = locName[0];
			//do stuff
		}
		if (input[i] == "}")
			break ;
	}
	log.debug("Location saved", location.location);
	return (location);
}

std::set<std::string>	obtainMethod(std::vector<std::string> input, int index) {
	std::set<std::string>		methods;
	std::vector<std::string>	words;
	Logger						log;

	if (input[index].substr(0, 16) == "allowed_methods ") {
		words = ftstring::split(input[index].substr(16), ' ');
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
	return (methods);
}

std::pair<int, std::string>	obtainErrorPages(std::vector<std::string> input, int index) {
	std::pair<int, std::string>	paired;
	std::vector<std::string>	words;
	int							nbr;
	Logger						log;

	if (input[index].substr(0, 11) == "error_page ") {
		words = ftstring::split(input[index].substr(11), ' ');
		if (words.size() != 2)
			throw PortNeedsSudoExeption();
		nbr = ftstring::strtoi(words[0]); //verificar se o numero de erro é valido
		//verificar arquivo (words[1]);
		paired = std::make_pair(nbr, words[1]);
		log.debug("Error page setted from .conf file.");
	}
	return (paired);
}

std::set<std::string>	obtainIndex(std::vector<std::string> input, int index) {
	std::set<std::string>		value;
	std::vector<std::string>	words;
	Logger						log;

	if (input[index].substr(0, 6) == "index ") {
		words = ftstring::split(input[index].substr(16), ' ');
		for (size_t j = 0; j < words.size(); j++) {
			value.insert(words[j]);
		}
		log.debug("Index setted from .conf file.");
	}
	return (value);
}

//-------------------------------EXCEPTIONS-------------------------//
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
