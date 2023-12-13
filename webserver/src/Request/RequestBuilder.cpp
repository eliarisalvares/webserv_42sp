/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 23:00:04 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/12 18:08:43 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBuilder.hpp"

RequestBuilder::RequestBuilder(void):
	_fd(0), _ready(false), _server(NULL), _request(NULL), _bytes_readed(0),
	_buffer(NULL) {
	}

RequestBuilder::RequestBuilder(Server* server, int connection):
	_fd(connection), _ready(false), _server(server), _bytes_readed(0) {
	_request = new Request(connection, server);
	_buffer = new char[server->getBufferSize()];
	memset(_buffer, 0, server->getBufferSize());
	_parser = RequestParser(_request);
}

RequestBuilder::~RequestBuilder(void) {
	Logger::debug("Deleting RequestBuilder");
	delete[] _buffer;
	if (_request)
		delete _request;
}

RequestBuilder::RequestBuilder(RequestBuilder const& copy) {
	*this = copy;
}

RequestBuilder& RequestBuilder::operator=(RequestBuilder const& copy) {
	if (this != &copy) {
		_fd = copy.getServerFd();
		_ready = copy.is_ready();
		_server = copy.getServer();
		if (_request && _request != copy.getRequest())
			delete _request;
		_request = copy.getRequest();
		_bytes_readed = copy.bytes_readed();
		if (_buffer && _buffer != copy.getBuffer())
			delete[] _buffer;
		_buffer = copy.getBuffer();
		_parser = copy.getParser();
	}
	return *this;
}

bool RequestBuilder::read(void) {
	int error;

	_bytes_readed = recv(_fd, _buffer, _server->getBufferSize(), 0);
	error = errno;

	if (_bytes_readed <= 0) {
		if (_bytes_readed == 0) {
			Logger::warning_no_lf("client connection is closed: ");
			printf(GREY "socket %d hung up\n" RESET, this->_fd);
		} else
			Logger::strerror("recv", error);
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
					if(!_parser.getRequest() || !_request)
						throw utils::GeneralException(
							utils::REQUEST_DELETED_ON_BUILDER_OR_PARSER
						);
					_parser.init(c);
					break;
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
					_parser.check_request();
					break;
				case RequestParser::BODY:
					_parser.body(c);
					break;
				case RequestParser::BODY_LENGTH_END:
					_parser.end_body(c);
					break;
				case RequestParser::CR_BODY:
					_parser.check_crlf(c);
					break;
				case RequestParser::CHUNK_SIZE:
				case RequestParser::CHUNK_PARAMETERS:
				case RequestParser::CHUNK_DATA:
				case RequestParser::CHUNK_DATA_END:
				case RequestParser::CHUNK_END:
					_parser.body(c);
					break;
				case RequestParser::CR_CHUNK_SIZE:
				case RequestParser::CR_CHUNK_DATA:
				case RequestParser::CR_CHUNK_END:
				case RequestParser::SECOND_CR_CHUNK_END:
					_parser.check_crlf(c);
					break;
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
	if (_parser.step() == RequestParser::BODY_LENGTH_END) {
		_parser.setStep(RequestParser::END);
		_ready = true;
	}
	memset(_buffer, 0, _bytes_readed);
}

Request* RequestBuilder::build(void) {
	if (!_request->has_error())
		Logger::info("Request parsed and created successfully.");
	if (_request->method() == http::POST && !_request->has_error())
		_parser.parse_body();
	this->_ready = false;
	return _request;
}

bool RequestBuilder::is_ready(void) const {
	return this->_ready;
}

void RequestBuilder::_setRequestError(http::InvalidRequest& e) {
	Logger::warning(e.what());
	_request->setStatusCode(e.get_error_code());
	_request->setError(true);
	_parser.setStep(RequestParser::END);
}

void RequestBuilder::_setRequestError(std::exception& e) {
	Logger::error("error on request parsing: ");
	Logger::error(e.what());
	_request->setStatusCode(http::INTERNAL_SERVER_ERROR);
	_request->setError(true);
	_parser.setStep(RequestParser::END);
}

int	RequestBuilder::getServerFd(void) const {
	return this->_fd;
}

Server*	RequestBuilder::getServer(void) const {
	return this->_server;
}

Request* RequestBuilder::getRequest(void) const {
	return this->_request;
}

size_t	RequestBuilder::bytes_readed(void) const {
	return this->_bytes_readed;
}

char*	RequestBuilder::getBuffer(void) const {
	return this->_buffer;
}

RequestParser	RequestBuilder::getParser(void) const {
	return this->_parser;
}

