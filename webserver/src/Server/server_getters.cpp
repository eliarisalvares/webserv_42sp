/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_getters.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/10 11:17:39 by feralves          #+#    #+#             */
/*   Updated: 2023/12/10 11:17:40 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

bool	Server::getCGI(void) const {
	return this->_cgi;
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

std::vector<t_location>*	Server::getLocations(void) {
	return &(this->_locations);
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

std::string	Server::getErrorPages(int value) const {
	return this->_error_pages.at(value);
}

bool Server::getAutoindex(void) const {
	return this->_permit.autoindex;
}

std::string	Server::getCurrentPort(void) const {
	std::string		port;

	port = ftstring::itostr(_port);
	return port;
}
