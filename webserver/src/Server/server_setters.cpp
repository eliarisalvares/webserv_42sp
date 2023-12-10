/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_setters.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/10 11:17:34 by feralves          #+#    #+#             */
/*   Updated: 2023/12/10 11:17:35 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::setPort(std::vector<std::string> input, int index) {
	int	extraBrackets = 0;

	for (size_t i = index; i < input.size(); i++) {
		if (input[i].substr() == "server {")
			i++ ;
		if (input[i].substr(0, 7) == "listen ") {
			_port = obtainPort(input, i);
			if (!configSocket(_port))
				throw PortAlreadyInUseException();
		}
		if (input[i].substr(0, 9) == "location ")
			extraBrackets++;
		if (input[i].substr() == "}" && extraBrackets == 0)
			break ;
		else if (input[i].substr() == "}")
			extraBrackets--;
	}
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

void	Server::setRedirect(std::string redirect) {
	this->_permit.has_redir = true;
	this->_redirect = redirect;
	if (redirect.substr(0, 1) != "/")
		this->_permit.redirExternal = true;
}

void	Server::setCGI(bool boolean) {
	this->_cgi = boolean;
}

void	Server::addErrorPages(std::pair<int, std::string> paired) {
	if (_error_pages.find(paired.first) != _error_pages.end()) {
		_error_pages[paired.first] = paired.second;
	}
	else
		_error_pages.insert(paired);
}

void	Server::addLocation(t_location location) {
	if (_locations.size() == 0)
		_locations.push_back(location);
	else {
		for (size_t i = 0; i < _locations.size(); i++) {
			if (_locations[i].location == location.location)
				_locations[i] = location;
			else
				_locations.push_back(location);
		}
	}
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
