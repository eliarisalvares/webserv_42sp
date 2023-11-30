/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/30 11:36:06 by sguilher         ###   ########.fr       */
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
	int error;

	log.debug("reading data and saving it...");
	_bytes_readed = recv(_fd, _buffer, _server->getBufferSize(), 0);
	error = errno;

	if (_bytes_readed <= 0) {
		std::cout << "error: " << errno << std::endl;  // remover, deixei apenas pra nos auxiliar
		if (_bytes_readed == 0) {
			log.warning("client connection closed:");
			printf(GREY "socket %d hung up\n" "RESET", this->_fd);
		} else
			log.strerror("recv", error);

		close(this->_fd);
		return false;
	}

	return true;
}

void RequestBuilder::parse(void) {
	size_t i = 0;

	while (i < _bytes_readed && !_ready) {
		try {
			switch (_parser.step())
			{
				case RequestParser::INIT:
				case RequestParser::METHOD:
					_parser.method(_buffer[i]);
					break;
				case RequestParser::URI:
					_parser.uri(_buffer[i]);
					break;
				case RequestParser::PROTOCOL:
					_parser.protocol(_buffer[i]);
					break;
				case RequestParser::VERSION:
					_parser.version(_buffer[i]);
					break;
				case RequestParser::CR_FIRST_LINE:
					_parser.check_crlf(_buffer[i]);
					break;
				case RequestParser::HEADER:
					_parser.header();
					break;
				case RequestParser::BODY:
				case RequestParser::END:
					_ready = true;
					break;
				case RequestParser::ERROR:
					_request->setError(true);
					_ready = true;
					break;
				default:
					break;
			}
		} catch (http::InvalidRequest& e) {
			log.warning(e.what());
			_request->setError(true);
			_request->setStatusCode(e.get_error_code());
			_ready = true;
		} catch (std::exception& e) {
			log.error("error on request parsing: ");
			log.error(e.what());
		}
		i++;
	}
	memset(_buffer, 0, _bytes_readed);
}

Request* RequestBuilder::build(void) {
	t_string_map parse_result = _parser.get_result();
	Logger log;

	// if (_parser.error())
	// 	_request->setError(true);
	this->_ready = false;
	return _request;
}

bool RequestBuilder::is_ready(void) const {
	return this->_ready;
}
