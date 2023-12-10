/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:30:54 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/10 01:32:40 by sguilher         ###   ########.fr       */
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
	_content_length(0),
	_has_image(false) {
		_post_data.clear();
	}


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
	_content_length(0),
	_has_image(false) {
		_post_data.clear();
	}

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
		this->_content_length = copy.content_length();
		this->_has_image = copy.has_image();
		this->_image = copy.image();
		this->_image_type = copy.image_type();
	}
	return *this;
}

void Request::printPostData(void) const {
	std::map<std::string, std::string>::const_iterator it, end = _post_data.end();

	if (DEBUG)
		Logger::info("Post data");
	for (it = _post_data.begin(); it != end; ++it)
		Logger::debug(it->first, it->second);
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

size_t Request::content_length(void) const {
	return this->_content_length;
}

bool Request::has_image(void) const {
	return this->_has_image;
}

std::vector<char>* Request::image(void) const {
	return this->_image;
}

std::string Request::image_type(void) const {
	return this->_image_type;
}

std::map<std::string, std::string> Request::post_data(void) const {
	return this->_post_data;
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

void Request::setContentLength(size_t length) {
	this->_content_length = length;
}


void Request::setHasImage(bool has_image) {
	this->_has_image = has_image;
}

void Request::setImageType(std::string const& type) {
	this->_image_type = type;
}

void Request::setImage(std::vector<char>* image) {
	this->_image = image;
}

void Request::addPostData(std::string const& name, std::string const& value) {
	std::map<std::string, std::string>::iterator it;

	it = _post_data.find(name);
	if (it == _post_data.end())
		_post_data.insert(std::make_pair(name, value));
	else
		_post_data[name] = value;
}
