#include "Server.hpp"

int	Server::getSocket(void) const {
	return this->_socket;
}

int	Server::getBufferSize(void) const {
	return this->_bufferSize;
}

int	Server::getPort(void) const {
	return this->_port;
}

int	Server::getBodySize(void) const {
	return this->_client_max_body_size;
}

std::string	Server::getCGILoc(void) const {
	return this->_cgi_location;
}

std::string	Server::getRoot(void) const {
	return this->_root;
}

std::string	Server::getUpPath(void) const {
	return this->_uploadPath;
}

std::set<std::string>	Server::getMethods(void) const {
	return this->_allowed_methods;
}

std::set<std::string>	Server::getIndex(void) const {
	return this->_index;
}

std::vector<t_location>	Server::getLocations(void) {
	return this->_locations;
}

t_location	Server::getLocations(int index) {
	return this->_locations[index];
}

std::vector<std::string>	Server::getName(void) {
	return this->_server_name;
}

std::string	Server::getName(int index) {
	return this->_server_name[index];
}

std::map<int, std::string>	Server::getErrorPages(void) const {
	return this->_error_pages;
}


std::string	Server::getServerName(void) {
	return (SERVER_NAME);
}

std::string	Server::getCurrentPort(void) {
	return (SERVER_PORT);
}

std::string	Server::getAllowedMethods(void) {
	return (METHODS);
}
