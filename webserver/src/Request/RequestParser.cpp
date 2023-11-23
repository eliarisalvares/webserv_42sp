/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:43:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/27 16:37:04 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

RequestParser::RequestParser(void):
	_idx(0), _step(INIT), _error(NONE) {
	_data.clear();
	_result.clear();
}

RequestParser::RequestParser(RequestParser const& copy) {
	*this = copy;
}

RequestParser& RequestParser::operator=(RequestParser const& copy) {
	if (this != &copy) {
		(void)copy;
	}
	return *this;
}

RequestParser::~RequestParser(void) {
	_data.clear();
	_result.clear();
}

void RequestParser::break_data(char* buffer, size_t bytes_read) {
	for (size_t i = 0; i < bytes_read; ++i)
		_data.push_back(buffer[i]);

	std::vector<char>::iterator it, end = _data.end();
	if (DEBUG) {
		std::cout << GREY << "Received/remaining data:\n";
		for(it = _data.begin(); it != end; ++it) {
			if (*it == CR)
				std::cout << " CR";
			else if (*it == LF)
				std::cout << "LF" << *it;
			else
				std::cout << *it;
		}
		std::cout << RESET << std::endl;
	}
}

RequestParser::e_steps RequestParser::step(void) {
	return this->_step;
}

RequestParser::e_parser_error RequestParser::error(void) {
	return this->_error;
}



//Initial lines and headers should end in CRLF, though you should gracefully handle lines ending in just LF.
// Method names are always uppercase
// URI enconding (tb serve pra POST) - não entendi se posso receber https
// URI normalization:
// normalizing the scheme and host to lowercase
// normalizing the port to remove any leading zeros.
// If port is elided from the URI, the default port for that scheme is used
// o host pode ser um IP, que pode estar em ipv4 ou ipv6...
// It is RECOMMENDED that all senders and recipients support, at a minimum, URIs with lengths of 8000 octets in protocol elements.
// The HTTP version always takes the form "HTTP/x.x", uppercase
void RequestParser::first_line(void) {
	std::string method;
	std::string uri;
	std::string protocol;
	std::string version;
	std::vector<char>::iterator it;

	// separar os parsers para evitar receber um monte de dado inválido sem CRLF
	log.debug("first_line");
	if (_found_EOL()) {
		log.debug("Found EOL");
		for (it = _data.begin(); it != _data_it && *it != SP; ++it)
			method.push_back(*it);
		log.debug(method);
		while (*it == SP)
			++it;
		for (; it != _data_it && *it != SP; ++it)
			uri.push_back(*it);
		log.debug(uri);
		while (*it == SP)
			++it;
		for (; it != _data_it && *it != '/'; ++it)
			protocol.push_back(*it);
		log.debug(protocol);
		++it;
		for (; it != _data_it; ++it)
			version.push_back(*it);
		log.debug(version);
		_step = HEADER;
		_data.erase(_data.begin(), _data_it + 2);
		_result.insert(t_result_pair("method", method));
		_result.insert(t_result_pair("uri", uri));
		_result.insert(t_result_pair("protocol", protocol));
		_result.insert(t_result_pair("version", version));
	}
}

bool RequestParser::_found_EOL(void) {
	std::vector<char>::iterator end = _data.end();

	for(_data_it = _data.begin(); _data_it != end; ++_data_it) {
		if (*_data_it == CR) {
			// log.debug("found CR");
			break;
		}
		if (*_data_it == LF) {
			// log.debug("found LF without CR");
			// talvez o CR não seja necessário; checar na RFC se pode aceitar só o \n
			_error = LF_WITHOUT_CR;
			_step = END;
			return false;
		}
	}
	if (_data_it == end)
		return false;
	if (*_data_it == CR) {
		if (*(_data_it + 1) == LF) {
			// log.debug("found LF");
			// _step = CRLF;
			return true;
		}
		else if (_data_it + 1 == end)
			return false;
		else {
			_error = CR_WITHOUT_LF;
			_step = END;
			return false;
		}
	}
	(void)_idx;
	return false;
}

