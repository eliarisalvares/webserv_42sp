#include "server_builder.hpp"

t_location	initLocation(void) {
	t_location	location;

	location.allowed_methods = http::methods;
	location.location = LOCATION;
	location.root = "/content";
	location.index.insert("/content/index.html");
	return (location);
}

int	obtainPort(std::vector<std::string> input, int index) {
	int		port;

	if (input[index].substr(0, 7) == "listen ") {
		port = ftstring::strtoi(input[index].substr(7));
		if (port < 1 || port > 65535)
			throw PortNotFoundErrorExeption();
		if (port < 1024)
			throw PortNeedsSudoExeption();
		Logger::debug("Port setted", port);
	}
	return (port);
}

int	obtainBodySize(std::vector<std::string> input, int index) {
	int							bodySize;
	std::string					bodySizeString;
	std::vector<std::string>	words;

	if (input[index].substr(0, 21) == "client_max_body_size ") {
		words = ftstring::split(input[index].substr(21), ' ');
		bodySizeString = words[0];
		bodySize = ftstring::strtoi(bodySizeString);
		switch (bodySizeString.at(bodySizeString.length() - 1))
		{
		case 'M':
			bodySize *= 1048576;
			break;
		case 'K':
			bodySize *= 1024;
			break;
		default:
			break;
		}
		if (bodySize > 1048576)
			throw TooLargeException();
		Logger::debug("client_max_body_size setted", bodySize);
	}
	return (bodySize);
}

int	obtainBufferSize(std::vector<std::string> input, int index) {
	int							bufferSize;

	if (input[index].substr(0, 12) == "buffer_size ") {
		bufferSize = ftstring::strtoi(input[index].substr(12));
		if (bufferSize < 1 || bufferSize > 1024)
			throw InvalidBufferSize();
		Logger::debug("Buffer Size setted", bufferSize);
	}
	return (bufferSize);
}

std::string	obtainRoot(std::vector<std::string> input, int index) {
	std::string					root;
	std::vector<std::string>	words;

	if (input[index].substr(0, 5) == "root ") {
		words = ftstring::split(input[index].substr(5), ' ');
		root = "/content" + words[0];
		//check if root makes sense/exists
		Logger::debug("Root setted", root);
	}
	return (root);
}

bool	obtainCGI(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;
	bool						valid = true;

	if (input[index].substr(0, 4) == "cgi ") {
		name = input[index].substr(4);
		serverName = ftstring::split(name, ' ');
		if (serverName.size() != 2) {
			valid = false;
			throw CGIMissconfigurationException();
		}
		else if (serverName[0] != ".py" || serverName[1] != "python3") {
			valid = false;
			throw CGINotSupportedException();
		}
		Logger::debug("CGI setted from .conf file");
	}
	return (valid);
}

std::vector<std::string>	obtainName(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	serverName;

	if (input[index].substr(0, 7) == "server_name ") {
		name = input[index].substr(7);
		serverName = ftstring::split(name, ' ');
		Logger::debug("Server name setted", name);
	}
	return (serverName);
}

t_location	obtainLoc(std::vector<std::string> input, int index) {
	//deal with location :')
	t_location					location;
	std::vector<std::string>	locName;

	location = initLocation();
	Logger::debug("Init location parsing", location.location);
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr(0, 9) == "location ") {
			locName = ftstring::split(input[i].substr(9), ' ');
			location.location = locName[0];
			if (locName[1] != "{")
				throw LocationNotOpenedException();
		}
		if (input[i] == "}")
			break ;
		if (input[i].substr(0, 5) == "root ")
			location.root = obtainRoot(input, i);
		if (input[i].substr(0, 16) == "allowed_methods ")
			location.allowed_methods = obtainMethod(input, i);
		if (input[i].substr(0, 6) == "index ")
			location.index = obtainIndex(input, i);
		// faltam todas as infos referentes a response ->http methods, cgi, redirecionamento, permissões, ?
	}
	Logger::debug("Location saved", location.location);
	return (location);
}

std::set<std::string>	obtainMethod(std::vector<std::string> input, int index) {
	std::set<std::string>		methods;
	std::vector<std::string>	words;

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
		Logger::debug("Allowed Methods setted from .conf file");
	}
	return (methods);
}

std::pair<int, std::string>	obtainErrorPages(std::vector<std::string> input, int index) {
	std::pair<int, std::string>	paired;
	std::vector<std::string>	words;
	std::string					page;
	int							nbr;
	char const		*values[] = {"400", "401", "402", "403", "404", "405", "406", "407", "408", "409", "410",
								"411", "412", "413", "414", "415", "416", "417", "418", "421", "422", "423",
								"424", "425", "426", "428", "429", "431", "451", "500", "501", "502", "503",
								"504", "505", "506", "507", "508", "510", "511"};
	std::vector<std::string> const codes(values, (values + sizeof(values) / sizeof(values[0])));
	std::vector<std::string>::const_iterator it;

	if (input[index].substr(0, 11) == "error_page ") {
		words = ftstring::split(input[index].substr(11), ' ');
		if (words.size() != 2)
			throw WrongNbrErrorException();
		it = words.begin();
		if (!(std::find(codes.begin(), codes.end(), *it) != codes.end()))
			throw InvalidErrorNbrException();
		nbr = ftstring::strtoi(words[0]);
		page = "content/error_pages/" + words[1];
		if (!checkFileWorks(page))
			throw InvalidFileException();
		paired = std::make_pair(nbr, page);
		Logger::debug("Error page setted, value", nbr);
	}
	return (paired);
}

std::set<std::string>	obtainIndex(std::vector<std::string> input, int index) {
	std::set<std::string>		value;
	std::vector<std::string>	words;

	if (input[index].substr(0, 6) == "index ") {
		words = ftstring::split(input[index].substr(16), ' ');
		for (size_t j = 0; j < words.size(); j++) {
			value.insert("/content/" + words[j]);
			//verificar se arquivo existe e tem permissão
		}
		Logger::debug("Index setted", input[index].substr(16));
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

const char* TooLargeException::what() const throw() {
	return ("Size of client body too large.");
}

const char* InvalidBufferSize::what() const throw() {
	return ("Value for Buffer invalid.");
}

const char* LocationNotOpenedException::what() const throw() {
	return ("Location open bracket '{' not found.");
}

const char* WrongNbrErrorException::what() const throw() {
	return ("Wrong number of arguments to set Error Pages.");
}

const char* InvalidErrorNbrException::what() const throw() {
	return ("Value of Error Page does not exist.");
}

const char* InvalidFileException::what() const throw() {
	return ("Invalid File");
}
