/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/29 23:41:13 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBuilder.hpp"

// private constructor
RequestBuilder::RequestBuilder(void) {}

RequestBuilder::RequestBuilder(Server* server, int connection):
	_fd(connection), _ready(false), _server(server), _bytes_readed(0) {
	_request = new Request(connection, server);  // deletar fora porque vamos devolver
	_buffer = new char[server->getBufferSize()];
	memset(_buffer, 0, server->getBufferSize());
	_parser = RequestParser(_request);
}

RequestBuilder::~RequestBuilder(void) {
	delete[] _buffer;
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
			printf(GREY "socket %d hung up\n" "RESET", this->_fd);
		} else
			log.strerror("recv", error);

		close(this->_fd);
		return false;
	}

	_parser.break_data(_buffer, nbytes);
	memset(_buffer, 0, nbytes);
	return true;
}

void RequestBuilder::parse(void) {
	if (_parser.step() == RequestParser::METHOD) {
		_parser.first_line();
	}
	if (_parser.step() == RequestParser::HEADER) {
		_parser.header();
	}
	if (_parser.error()) {
		_ready = true;
		_error = PARSE_ERROR; // pode ter outros
		_error_str = "parser error"; // alterar isso aqui
	}
	if (_parser.step() == RequestParser::END)
		_ready = true;
}

Request* RequestBuilder::build(void) {
	t_string_map parse_result = _parser.get_result();
	Logger log;

	if (_parser.error())
		_request->setError(true);
	this->_ready = false;
	return _request;
}

bool RequestBuilder::is_ready(void) const {
	return this->_ready;
}
