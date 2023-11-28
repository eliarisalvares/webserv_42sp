#include "Server.hpp"

int	Server::getSocket(void) const {
	return (this->_socket);
}

int	Server::getBufferSize(void) const {
	return this->_bufferSize;
}


int	Server::getPort(void) const {
	return this->_port;
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
