/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 13:31:39 by feralves          #+#    #+#             */
/*   Updated: 2023/12/02 15:06:27 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void) { }

Server::Server(int port): _port(port) {
	configSocket(port);
	setBasics();
	_location_root.clear();
	_location_root.insert(std::pair<std::string, std::string>(LOCATION, ROOT));
	_location_root.insert(std::pair<std::string, std::string>(CGI_LOCATION, "content/cgi"));
}

Server::Server(std::vector<std::string> input, size_t index) {
	setBasics();
	_server_name.push_back(SERVER_NAME);
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "listen ") {
			_port = getPortConf(input, index);
			setSocket(_port);
		}
		if (input[i].substr(0, 12) == "server_name ")
			setName(getNameConf(input, index));
		if (input[i].substr(0, 21) == "client_max_body_size ")
			setBodySize(getBodySizeConf(input, index));
		if (input[i].substr(0, 5) == "root ")
			_root = getRootConf(input, index);
		if (input[i].substr(0, 9) == "location ") {
			while (input[i].substr() != "}")
				i++;
			_locations.push_back(getLocConf(input, index));
		}
		//_bufferSize;
		// _cgi_location;
		// _root;  // geral do server; cada location vai poder ter um root diferente
		// _uploadPath;
		// _allowed_methods; //std::set<std::string> _fill_methods(void)
		// _index; //autoindex
		// _locations;
		// _error_pages;
	}
	configSocket(_port);
	_location_root.clear();
	_location_root.insert(std::pair<std::string, std::string>(LOCATION, ROOT));
	_location_root.insert(std::pair<std::string, std::string>(CGI_LOCATION, "content/cgi"));
}

Server::~Server(void) { }

Server::Server(Server const& copy) { (void)copy; }

Server& Server::operator=(Server const & copy) {
	if (this != &copy) {
		configSocket(copy.getSocket());
		setBufferSize(copy.getBufferSize());
	}
	return *this;
}

void	Server::setBasics() {
	std::vector<std::string>	serverName;
	std::map<int, std::string>	errors;

	serverName.push_back(SERVER_NAME);
	errors.empty();
	errors.insert(std::pair<int, std::string>(404, "404.html"));
	setBufferSize(BUFFSIZE);
	setBodySize(CLIENT_MAX_BODY_SIZE);
	setRoot(LOCATION);
	setCGILocation(".cgi");
	setErrorPages(errors);
	setUpPath(ROOT);
	// setMethods(http::_fill_methods);
	// setIndex(std::set<std::string> index);
	setName(serverName);
}

// vamos usar para cada server do arquivo de config
void	Server::configSocket(int port) {
	struct sockaddr_in address;  // Struct para o endereço do servidor

	// Cria o socket do servidor, AF_INET para IPv4, SOCK_STREAM para TCP, 0 para o protocolo
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);

	// Configura o endereço do servidor e a porta, AF_INET para IPv4, INADDR_ANY para o endereço do host, htons para a porta
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	int yes = 1;

	// lose the pesky "Address already in use" error message
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
		perror("setsockopt");
		exit(1);
	}
	// Associa o socket do servidor ao endereço e à porta especificados
	if (bind(this->_socket, (struct sockaddr*)&address, sizeof(address)) == -1) {
		perror("bind");
	}

	// Coloca o socket do servidor em modo de escuta, com um limite de 500 conexões pendentes (isso temos que ver, esse número os meninos usaram no projeto deles,
	// mas não sei se é o ideal, acho que a gente tem que ver isso, meu sistema mostra 4096)
	listen(this->_socket, 500);
}

