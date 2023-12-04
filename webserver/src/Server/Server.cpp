/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 13:31:39 by feralves          #+#    #+#             */
/*   Updated: 2023/12/04 14:28:48 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void) { }

Server::Server(int port): _port(port) {
	setBasics();
	configSocket(port);
	_location_root.clear();
	_location_root.insert(std::pair<std::string, std::string>(LOCATION, ROOT));
	_location_root.insert(std::pair<std::string, std::string>(CGI_LOCATION, "content/cgi"));
}

Server::Server(std::vector<std::string> input, size_t index) {
	setBasics();
	setPort(input, index);
	int x = 0;
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 12) == "server_name ")
			setName(obtainName(input, i));
		if (input[i].substr(0, 21) == "client_max_body_size ")
			setBodySize(obtainBodySize(input, i));
		if (input[i].substr(0, 5) == "root ")
			setRoot(obtainRoot(input, i));
		if (input[i].substr(0, 9) == "location ") {
			x++;
			addLocation(obtainLoc(input, i));
			while (input[i].substr() != "}")
				i++;
		}
		if (input[i].substr(0, 4) == "cgi ")
			setCGI(obtainCGI(input, i));
		if (input[i].substr(0, 16) == "allowed_methods ") {
			_allowed_methods.clear();
			setMethods(obtainMethod(input, i));
		}
		if (input[i].substr(0, 11) == "error_page ")
			addErrorPages(obtainErrorPages(input, i));
		if (input[i].substr(0, 6) == "index ")
			setIndex(obtainIndex(input, i));

		//_bufferSize;
		// _uploadPath;
	}
	configSocket(_port);
	_location_root.clear();
	_location_root.insert(std::pair<std::string, std::string>(LOCATION, ROOT));
	_location_root.insert(std::pair<std::string, std::string>(CGI_LOCATION, "content/cgi"));
}

Server::~Server(void) {
	Logger::debug("Server cleaned", _port);
}

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
	std::set<std::string>		index;
	t_location					location;

	serverName.push_back(SERVER_NAME);
	index.insert("index.html");
	location = initLocation();
	setBufferSize(BUFFSIZE);
	setBodySize(CLIENT_MAX_BODY_SIZE);
	setRoot(ROOT);
	setCGI(ftstring::split(".py python3", ' '));
	addErrorPages(std::pair<int, std::string>(404, "404.html"));
	_locations.push_back(location);
	setUpPath("/content/upload/"); //idk
	setMethods(http::methods);
	setIndex(index);
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

