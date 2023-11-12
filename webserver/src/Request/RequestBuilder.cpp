/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/12 13:31:13 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBuilder.hpp"

// private constructor
RequestBuilder::RequestBuilder(void) {}

RequestBuilder::RequestBuilder(Server* server, int connection):
	_fd(connection), _ready(false), _server(server){
	_parser = RequestParser();
	_requestData.clear();
}

RequestBuilder::~RequestBuilder(void) {}

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
	char buf[(this->_server)->getBufferSize()];  // isso pode ser despendioso?
	Logger log;
	int nbytes;

	log.debug("reading data and saving it...");
	nbytes = recv(this->_fd, buf, sizeof buf, 0);

	if (nbytes <= 0) {
		if (nbytes == 0) {
			log.warning("client connection closed:");
			printf("GREY" "socket %d hung up\n" "RESET", this->_fd);
		} else
			log.perror("recv");

		close(this->_fd);
		return false;
	}

	for (int i = 0; i < nbytes; ++i)
		_requestData.push_back(buf[i]);

	memset(&buf, 0, sizeof(buf));
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
