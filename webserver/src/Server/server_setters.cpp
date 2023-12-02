#include "Server.hpp"

void	Server::setPort(std::vector<std::string> input, int index) {
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "listen ") {
			_port = obtainPort(input, i);
			setSocket(_port);
		}
	}
}

void	Server::setSocket(int port) {
	this->_socket = port;
}

void	Server::setBufferSize(int size) {
	this->_bufferSize = size;
}

void	Server::setBodySize(int size) {
	this->_client_max_body_size = size;
}

void	Server::setRoot(std::string root) {
	this->_root = root;
}

void	Server::setCGI(std::vector<std::string> cgi) {
	this->_cgi = cgi;
}

void	Server::addErrorPages(std::pair<int, std::string> paired) {
	if (_error_pages.find(paired.first) != _error_pages.end()) {
		_error_pages[paired.first] = paired.second;
	}
	else
		_error_pages.insert(paired);
}

void	Server::setErrorPages(std::map<int, std::string> errorPages) {
	this->_error_pages = errorPages;
}

void	Server::setUpPath(std::string path) {
	this->_uploadPath =  path;
}

void	Server::setMethods(std::set<std::string> methods) {
	this->_allowed_methods = methods;
}

void	Server::setIndex(std::set<std::string> index) {
	this->_index = index;
}

void	Server::setName(std::vector<std::string> name) {
	this->_server_name = name;
}
