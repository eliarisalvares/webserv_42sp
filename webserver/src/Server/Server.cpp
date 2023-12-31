/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 13:31:39 by feralves          #+#    #+#             */
/*   Updated: 2023/12/12 19:53:37 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void) { }

Server::Server(int port): _port(port) {
	setBasics();
	if (!configSocket(port))
		throw PortAlreadyInUseException();
}

Server::Server(std::vector<std::string> input, size_t index) {
	int		extraBrackets = 0;
	bool	gotIndex = false;

	setBasics();
	setPort(input, index);
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 12) == "server_name ")
			setName(obtainName(input, i));
		if (input[i].substr(0, 5) == "root ")
			setRoot(obtainRoot(input, i));
		if (input[i].substr(0, 16) == "allowed_methods ") {
			_allowed_methods.clear();
			setMethods(obtainMethod(input, i));
		}
		if (input[i].substr(0, 9) == "location " || input[i].substr() == "}")
			break ;
	}
	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 21) == "client_max_body_size ")
			setBodySize(obtainBodySize(input, i));
		if (input[i].substr(0, 9) == "location ") {
			extraBrackets++;
			addLocation(obtainLoc(input, i));
			while (input[i].substr() != "}")
				i++;
		}
		if (input[i].substr(0, 4) == "cgi ")
			setCGI(obtainCGI(input, i));
		if (input[i].substr(0, 11) == "error_page ")
			addErrorPages(obtainErrorPages(input, i));
		if (input[i].substr(0, 6) == "index ") {
			gotIndex = true;
			setIndex(obtainIndex(input, i, getRoot()));
		}
		if (input[i].substr(0, 12) == "buffer_size ")
			setBufferSize(obtainBufferSize(input, i));
		if (input[i].substr(0, 10) == "autoindex ")
			_permit.autoindex = obtainAutoIndex(input, i);
		if (input[i].substr(0, 9) == "redirect ")
			setRedirect(obtainRedirect(input, i));
		if (input[i].substr() == "}" && extraBrackets == 0)
			break ;
		else if (input[i].substr() == "}")
			extraBrackets--;
	}
	if (!gotIndex)
		setIndex(getRootIndex(getRoot()));
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
	t_permissions				permit;

	permit.autoindex = false;
	permit.has_redir = false;
	permit.redirExternal = false;
	serverName.push_back(SERVER_NAME);
	index.insert("content/index.html");
	setBufferSize(BUFFSIZE);
	setBodySize(CLIENT_MAX_BODY_SIZE);
	setRoot(ROOT);
	setCGI(true);
	_error_pages.clear();
	_locations.clear();
	_permit = permit;
	setUpPath(DEFAULT_UPLOAD);
	setMethods(http::methods);
	setIndex(index);
	setName(serverName);
}

bool	Server::configSocket(int port) {
	struct sockaddr_in address;

	this->_socket = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	int yes = 1;

	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
		perror("setsockopt");
		return false;
	}
	if (bind(this->_socket, (struct sockaddr*)&address, sizeof(address)) == -1) {
		perror("bind");
		return false;
	}

	listen(this->_socket, 500);
	return true;
}

