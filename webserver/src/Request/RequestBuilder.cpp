/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/09 03:14:27 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBuilder.hpp"

// private constructor
RequestBuilder::RequestBuilder(void) {}

RequestBuilder::RequestBuilder(Server& server, int connection):
	_server(&server), _fd(connection), _ready(false) {  // change _ready
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


bool RequestBuilder::addRequestData(void) {
	char buf[(this->_server)->getBufferSize()];

	int nbytes = recv(this->_fd, buf, sizeof buf, 0);
	if (nbytes <= 0) {
		// Got error or connection closed by client
		if (nbytes == 0) {
			// Connection closed
			printf("pollserver: socket %d hung up\n", this->_fd);
		} else {
			perror("recv");
		}

		close(this->_fd); // Bye!

		return false;

		// raise error to close and remove RequestBuilder etc
		// this->_pfds.erase(this->_pfds.begin() + i); // Remove an index from the set
	}
	return true;

	// add request data in RequestBuilder
	// quebrar em char
	// análise parcial - status - error, response
	_requestData.push_back(buf);
	memset(&buf, 0, sizeof(buf)); // isso faz limpar o que foi salvo??
}

Request* RequestBuilder::build() {
	Logger log;
	this->parse();
	Request *req = new Request(this->_fd);
	log.debug("Request pointer:");
	std::cout << req << std::endl;
	return req;
}
  // params: Server, fd da conexão


void RequestBuilder::parse() {}


bool RequestBuilder::is_ready(void) const {
	return this->_ready;
}
