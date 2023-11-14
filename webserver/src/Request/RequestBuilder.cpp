/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/14 12:09:26 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBuilder.hpp"

// private constructor
RequestBuilder::RequestBuilder(void) {}

RequestBuilder::RequestBuilder(Server* server, int connection):
	_fd(connection), _ready(false), _server(server) {
	_buffer = new char[server->getBufferSize()];
	memset(_buffer, 0, strlen(_buffer));
	_parser = RequestParser();
}

RequestBuilder::~RequestBuilder(void) {
	delete _buffer;
}

RequestBuilder::RequestBuilder(RequestBuilder const& copy) {
	*this = copy;
}

// private operator
RequestBuilder& RequestBuilder::operator=(RequestBuilder const& copy) {
	if (this != &copy) {
		(void)copy;
	}
	return *this;
}

bool RequestBuilder::read(void) {
	Logger log;
	int nbytes;
	int error;

	log.debug("reading data and saving it...");
	nbytes = recv(_fd, _buffer, _server->getBufferSize(), 0);
	error = errno;

	if (nbytes <= 0) {
		std::cout << "error: " << errno << std::endl;
		if (nbytes == 0) {
			log.warning("client connection closed:");
			printf("GREY" "socket %d hung up\n" "RESET", this->_fd);
		} else
			log.strerror("recv", error);

		close(this->_fd);
		return false;
	}
	for (int i = 0; i < nbytes; ++i)
		_requestData.push_back(buf[i]);

	// _parser.break_data(_buffer, nbytes);
	memset(_buffer, 0, nbytes);
	return true;
}

void RequestBuilder::parse(void) {
	std::vector<char>::iterator it, end = _requestData.end();
	std::cout << GREY << "Received: ";
	for(it = _requestData.begin(); it != end; ++it) {
		std::cout << *it;
		if (*it == CR)
			std::cout << "\nCR here";
		if (*it == LF)
			std::cout << "LF here";
	}
	std::cout << RESET << std::endl;
}

Request* RequestBuilder::build(void) {
	Logger log;
	this->parse();
	Request *req = new Request(this->_fd);
	this->_ready = false;
	return req;
}

bool RequestBuilder::is_ready(void) const {
	return this->_ready;
}
