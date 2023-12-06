#include "server_builder.hpp"
#include <dirent.h>

t_location	initLocation(void) {
	t_location		location;

	location.allowed_methods = http::methods;
	location.location = LOCATION;
	location.root = "content";
	location.index.insert("content/index.html");
	location.permit.autoindex = false;
	location.permit.directory_listing = false;
	location.permit.has_redir = false;
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
			throw BufferSizeInvalidException();
		Logger::debug("Buffer Size setted", bufferSize);
	}
	return (bufferSize);
}

std::string	obtainRoot(std::vector<std::string> input, int index) {
	std::string					root;
	std::vector<std::string>	words;
	DIR *dr;

	if (input[index].substr(0, 5) == "root ") {
		words = ftstring::split(input[index].substr(5), ' ');
		root = "content" + words[0];
		dr = opendir(root.data());
		if (dr == NULL) {
			throw utils::GeneralException(utils::INVALID_DIRECTORY);
		}
		Logger::debug("Root setted", root);
	}
	return (root);
}

bool	obtainCGI(std::vector<std::string> input, int index) {
	std::string					name;
	std::vector<std::string>	cgiName;
	bool						valid = true;

	if (input[index].substr(0, 4) == "cgi ") {
		name = input[index].substr(4);
		cgiName = ftstring::split(name, ' ');
		if (cgiName.size() != 2) {
			valid = false;
			throw CGIWrongArgumentException();
		}
		else if (cgiName[0] != ".py" || cgiName[1] != "python3") {
			valid = false;
			throw CGIInvalidException();
		}
		Logger::debug("CGI setted from .conf file");
	}
	return (valid);
}

bool	obtainAutoIndex(std::vector<std::string> input, int index) {
	std::vector<std::string>	autoindex;
	bool						valid = false;

	if (input[index].substr(0, 10) == "autoindex ") {
		autoindex = ftstring::split(input[index].substr(10), ' ');
		if (autoindex.size() != 1)
			throw AutoIndexWrongArgumentException();
		if (autoindex[0] == "true")
			valid = true;
		else if (autoindex[0] == "false")
			valid = false;
		else
			throw AutoIndexInvalidException();
		Logger::debug("Autoindex setted as ", autoindex[0]);
	}
	return (valid);
}

bool	obtainDirList(std::vector<std::string> input, int index) {
	std::vector<std::string>	dirList;
	bool						valid = false;

	if (input[index].substr(0, 18) == "directory_listing ") {
		dirList = ftstring::split(input[index].substr(18), ' ');
		if (dirList.size() != 1)
			throw DirListWrongArgumentException();
		if (dirList[0] == "true")
			valid = true;
		else if (dirList[0] == "false")
			valid = false;
		else
			throw DirListInvalidException();
		Logger::debug("Directory Listing setted as ", dirList[0]);
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

std::set<std::string>	obtainMethod(std::vector<std::string> input, int index) {
	std::set<std::string>		methods;
	std::vector<std::string>	words;

	if (input[index].substr(0, 16) == "allowed_methods ") {
		words = ftstring::split(input[index].substr(16), ' ');
		if (words.size() <= 0 || words.size() >= 4)
			throw MethodsWrongArgumentException();
		for (size_t j = 0; j < words.size(); j++) {
			if (words[j] == "GET" || words[j] == "POST" || words[j] == "DELETE")
				methods.insert(words[j]);
			else
				throw utils::GeneralException(utils::INVALID_METHOD);
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
			throw ErrPagesWrongArgumentException();
		it = words.begin();
		if (!(std::find(codes.begin(), codes.end(), *it) != codes.end()))
			throw ErrPagesInvalidException();
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
	std::string					page;

	if (input[index].substr(0, 6) == "index ") {
		words = ftstring::split(input[index].substr(16), ' ');
		for (size_t j = 0; j < words.size(); j++) {
			page = "content/" + words[j];
			if (!checkFileWorks(page))
				throw InvalidFileException();
			value.insert(page);
		}
		Logger::debug("Index setted", input[index].substr(16));
	}
	return (value);
}

t_location	obtainLoc(std::vector<std::string> input, int index) {
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
		if (input[i].substr(0, 10) == "autoindex ")
			location.permit.autoindex = obtainAutoIndex(input, i);
		if (input[i].substr(0, 18) == "directory_listing ")
			location.permit.directory_listing = obtainDirList(input, i);
		if (input[i].substr(0, 4) == "cgi ")
			location.cgi = obtainCGI(input, i);
		if (input[i].substr(0, 11) == "error_page ") {
			std::pair<int, std::string> paired = obtainErrorPages(input, i);
			if (location.error_pages.find(paired.first) != location.error_pages.end())
				location.error_pages[paired.first] = paired.second;
			else
				location.error_pages.insert(paired);
		}
		//redirect
	}
	Logger::debug("Location saved", location.location);
	return (location);
}

//-------------------------------EXCEPTIONS-------------------------//
const char* PortNotFoundErrorExeption::what() const throw() {
	return ("Invalid Port found.");
}

const char* PortNeedsSudoExeption::what() const throw() {
	return ("Sudo required to run.");
}

const char* CGIWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for CGI.");
}

const char* CGIInvalidException::what() const throw() {
	return ("CGI mode not supported by webserv.");
}

const char* MethodsWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for Allowed Methods.");
}

const char* TooLargeException::what() const throw() {
	return ("Size of client body too large.");
}

const char* BufferSizeInvalidException::what() const throw() {
	return ("Not a valid argument for Buffer size.");
}

const char* LocationNotOpenedException::what() const throw() {
	return ("Location open bracket '{' not found.");
}

const char* ErrPagesWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments to set Error Pages.");
}

const char* ErrPagesInvalidException::what() const throw() {
	return ("Not a valid argument for Error Pages.");
}

const char* InvalidFileException::what() const throw() {
	return ("Invalid File");
}

const char* AutoIndexWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for autoindex.");
}

const char* AutoIndexInvalidException::what() const throw() {
	return ("Not a valid argument for autoindex.");
}

const char* DirListWrongArgumentException::what() const throw() {
	return ("Wrong number of arguments for directory_listing.");
}

const char* DirListInvalidException::what() const throw() {
	return ("Not a valid argument for directory_listing.");
}
