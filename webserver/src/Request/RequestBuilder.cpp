/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/03 21:51:39 by sguilher         ###   ########.fr       */
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
	int error;

	log.debug("reading data and saving it...");
	_bytes_readed = recv(_fd, _buffer, _server->getBufferSize(), 0);
	error = errno;

	if (_bytes_readed <= 0) {
		std::cout << ORANGE << "error: " << errno << std::endl;  // remover, deixei apenas pra nos auxiliar
		if (_bytes_readed == 0) {
			log.warning_no_lf("client connection closed: ");
			printf(GREY "socket %d hung up\n" RESET, this->_fd); // podemos usar a printf (por ser cpp)?
		} else
			log.strerror("recv", error);

		close(this->_fd);
		return false;
	}

	return true;
}

void RequestBuilder::parse(void) {
	size_t i = 0;
	char c;

	while (i < _bytes_readed && !_ready) {
		c = _buffer[i];
		try {
			switch (_parser.step())
			{
				case RequestParser::INIT:
				case RequestParser::METHOD:
					_parser.method(c);
					break;
				case RequestParser::URI:
					_parser.uri(c);
					break;
				case RequestParser::PROTOCOL:
					_parser.protocol(c);
					break;
				case RequestParser::VERSION:
					_parser.version(c);
					break;
				case RequestParser::CR_FIRST_LINE:
					_parser.check_crlf(c);
					break;
				case RequestParser::HEADER:
				case RequestParser::HEADER_NAME:
				case RequestParser::HEADER_VALUE_INIT:
				case RequestParser::HEADER_VALUE:
					_parser.header(c);
					break;
				case RequestParser::CR_HEADER:
					_parser.check_crlf(c);
					break;
				case RequestParser::SECOND_CR_HEADER:
					_parser.check_crlf(c);
					_parser.check_headers();
					break;
				case RequestParser::BODY:
					_parser.body(c);
					break;
				case RequestParser::BODY_NEW_LINE:
					_parser.body(c);
					break;
				case RequestParser::CR_BODY:
					_parser.check_crlf(c);
					break;
				case RequestParser::SECOND_CR_BODY:
					_parser.check_crlf(c);
					// validação extra do body aqui
					break;
				// case RequestParser::ERROR:
				// 	_request->setError(true);
				// 	_ready = true;
				// 	break;
				default:
					break;
			}
		} catch (http::InvalidRequest& e) {
			_setRequestError(e);
		} catch (std::exception& e) {
			_setRequestError(e);
		}
		if (_parser.step() == RequestParser::END)
			_ready = true;
		i++;
	}
	memset(_buffer, 0, _bytes_readed);
}

Request* RequestBuilder::build(void) {
	// t_string_map parse_result = _parser.get_result();
	Logger log;

	this->_ready = false;
	return _request;
}

bool RequestBuilder::is_ready(void) const {
	return this->_ready;
}

void RequestBuilder::_setRequestError(http::InvalidRequest& e) {
	log.warning(e.what());
	_request->setStatusCode(e.get_error_code());
	_request->setError(true);
	_parser.setStep(RequestParser::END);
}

void RequestBuilder::_setRequestError(std::exception& e) {
	log.error("error on request parsing: ");
	log.error(e.what());
	_request->setStatusCode(http::INTERNAL_SERVER_ERROR);
	_request->setError(true);
	_parser.setStep(RequestParser::END);
}
