#include "Server.hpp"

// void	Server::setPort(int port) {
// 	this->_port = port;
// }

// void	Server::setSocket(int port) {
// 	this->_socket = port;
// }

void	Server::setBufferSize(int size) {
	this->_bufferSize = size;
}

// void	Server::setBodySize(int size) {
// 	this->_client_max_body_size = size;
// }

// void	Server::setRoot(std::string root) {
// 	this->_root = root;
// }

// void	Server::setCGILocation(std::string cgi) {
// 	this->_cgi_location = cgi;
// }

// void	Server::setErrorPages(std::map<int, std::string> errorPages) {
// 	this->_error_pages = errorPages;
// }

// typedef unsigned int uint_t;

// typedef struct	s_location
// {
// 	std::string                          location;
// 	std::vector<std::string>             allowed_methods;
// 	std::vector<std::string>             http_methods;
// 	std::pair<unsigned int, std::string> http_redirection;
// 	std::string                          root;
// 	std::string                          directory_listing;
// 	std::string                          response_is_dir;
// 	std::set<std::string>                cgi_pass;
// 	std::set<std::string>                required_cookie;
// 	std::vector<t_cookie>                set_cookie;
// }				t_location;

// int									_port;  // obrigatório
// 		int										_socket;
// 		int										_bufferSize;
// 		int										_client_max_body_size;
// 		std::string								_root;  // geral do server; cada location vai poder ter um root diferente
// 		std::string								_cgi_location;
// 		std::set<std::string>					_index;
// 		std::vector<std::string>				_server_name;
// 		std::vector<std::string>				_allowed_methods;
// 		std::vector<location_t>					_locations;
// 		std::map<std::string, std::string>		_location_root;  // inclui o par pro cgi
// 		std::map<int, std::string>				_error_pages;
// 		std::string								_uploadPath;