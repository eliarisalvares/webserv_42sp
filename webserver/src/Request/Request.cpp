/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:30:54 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/08 11:10:37 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void):
	_server(NULL),
	_fd(0),
	_error(false),
	_method(http::GET),
	_status_code(http::OK),
	_path("content/"),
	_uri("/"),
	_location(NULL),
	_host(""),
	// _content_type(),
	_content_length(0),
	_is_chuncked(false) { }

Request::Request(int fd, Server* server):
	_server(server),
	_fd(fd),
	_error(false),
	_method(http::GET),
	_status_code(http::OK),
	_path("content/"),
	_uri("/"),
	_location(NULL),
	_host(""),
	// _content_type(),
	_content_length(0),
	_is_chuncked(false) { }

Request::~Request(void) { }

Request::Request(Request const& copy) {
	*this = copy;
}

Request const& Request::operator=(Request const & copy) {
	if (this != &copy) {
		this->_server = copy.server();
		this->_method = copy.method();
		this->_fd = copy.fd();
		this->_status_code = copy.status_code();
		this->_path = copy.path();
		this->_uri = copy.uri();
		this->_host = copy.host();
		this->_location = copy.location();
		this->_error = copy.has_error();
		this->_is_chuncked = copy.is_chuncked();
		this->_content_length = copy.content_length();
		// this->_content_type = copy.content_type();
	}
	return *this;
}


/********************************** GETTERS **********************************/

Server* Request::server(void) const {
	return this->_server;
}

http::RequestMethod Request::method(void) const {
	return this->_method;
}

int Request::fd(void) const {
	(void)_server;
	return this->_fd;
}

http::HttpStatus Request::status_code(void) const {
	return this->_status_code;
}

std::string Request::path(void) const {
	Logger::info("path", this->_path);
	return this->_path;
}

std::string Request::uri(void) const {
	Logger::info("uri", this->_uri);
	return this->_uri;
}

std::string Request::host(void) const {
	return this->_host;
}

t_location* Request::location(void) const {
	return this->_location;
}

bool Request::has_error(void) const {
	return this->_error;
}

bool Request::is_chuncked(void) const {
	return this->_is_chuncked;
}

// http::ContentType Request::content_type(void) const {
// 	return this->_content_type;
// }

size_t Request::content_length(void) const {
	return this->_content_length;
}


/********************************** SETTERS **********************************/

void Request::setMethod(http::RequestMethod method) {
	this->_method = method;
}

void Request::setStatusCode(http::HttpStatus status) {
	this->_status_code = status;
}

void Request::setUri(std::string const uri) {
	this->_uri = uri;
}

void Request::setPath(std::string const path) {
	this->_path = path;
}

void Request::setLocation(t_location* location) {
	this->_location = location;
}

void Request::setHost(std::string const host) {
	this->_host = host;
}


void Request::setError(bool has_error) {
	this->_error = has_error;
}

void Request::setChuncked(bool is_chuncked) {
	this->_is_chuncked = is_chuncked;
}

// void Request::setContentType(http::ContentType type) {
// 	this->_content_type = type;
// }

void Request::setContentLength(size_t length) {
	this->_content_length = length;
}

