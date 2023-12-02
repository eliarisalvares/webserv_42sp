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


// int										_bufferSize;
// int										_client_max_body_size;
// int										_port;  // único item obrigatório no arquivo
// int										_socket;
// std::string								_cgi_location;
// std::string								_root;  // geral do server; cada location vai poder ter um root diferente
// std::string								_uploadPath;
// std::set<std::string>					_allowed_methods;
// std::set<std::string>					_index; //autoindex
// std::vector<t_location>					_locations;
// std::vector<std::string>				_server_name;
// std::map<int, std::string>				_error_pages;