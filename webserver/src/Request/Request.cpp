/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:30:54 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/27 23:16:38 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void):
	_fd(0),
	_error(false),
	_method(GET),
	_status_code(http::OK),
	_uri("/"),
	_is_chuncked(false),
	_host(""),
	_content_type(),
	_content_length(0) { }

Request::Request(int fd, Server* server):
	_server(server),
	_fd(fd),
	_error(false),
	_method(GET),
	_status_code(http::OK),
	_uri("/"),
	_is_chuncked(false),
	_host(""),
	_content_type(),
	_content_length(0) { }

Request::~Request(void) { }

Request::Request(Request const& copy) {
	*this = copy;
}

Request const& Request::operator=(Request const & copy) {
	if (this != &copy) {
		this->_method = copy.method();
		// this->server = copy.getServer();
	}
	return *this;
}

/********************************** GETTERS **********************************/
Server* Request::server(void) const {
	return this->_server;
}

requestMethod Request::method(void) const {
	return this->_method;
}

int Request::fd(void) const {
	(void)_server;
	return this->_fd;
}

http::e_status Request::status_code(void) const {
	return this->_status_code;
}

std::string Request::uri(void) const {
	return this->_uri;
}

bool Request::has_error(void) const {
	return this->_error;
}

bool Request::is_chuncked(void) const {
	return this->_is_chuncked;
}

http::e_content_type Request::content_type(void) const {
	return this->_content_type;
}

size_t Request::content_length(void) const {
	return this->_content_length;
}

/********************************** SETTERS **********************************/
void Request::setMethod(requestMethod method) {
	this->_method = method;
}

void Request::setStatusCode(http::e_status status) {
	this->_status_code = status;
}

void Request::setUri(std::string const uri) {
	this->_uri = uri;
}

void Request::setError(bool has_error) {
	this->_error = has_error;
}

void Request::setChuncked(bool is_chuncked) {
	this->_is_chuncked = is_chuncked;
}

void Request::setContentType(http::e_content_type type) {
	this->_content_type = type;
}

void Request::setContentLength(size_t length) {
	this->_content_length = length;
}

