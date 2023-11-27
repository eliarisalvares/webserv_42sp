/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:43:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/27 12:57:11 by feralves         ###   ########.fr       */
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
	std::cout << GREY << "Received: ";
	for(it = _data.begin(); it != end; ++it) {
		std::cout << *it;
		if (*it == CR)
			std::cout << "\nCR here";
		if (*it == LF)
			std::cout << "LF here";
	}
	std::cout << RESET << std::endl;
}

RequestParser::e_steps RequestParser::step(void) {
	return this->_step;
}

RequestParser::e_parser_error RequestParser::error(void) {
	return this->_error;
}

void RequestParser::first_line(void) {
	std::string method;
	std::string uri;
	std::string protocol;
	std::string version;
	std::vector<char>::iterator it;

	if (_found_EOL()) {
		log.debug("Found EOL");
		for (it = _data.begin(); it != _data_it && *it != SP; ++it)
			method.push_back(*it);
		log.debug(method);
		for (it = _data.begin(); it != _data_it && *it != SP; ++it)
			uri.push_back(*it);
		log.debug(uri);
		for (it = _data.begin(); it != _data_it && *it != SP; ++it)
			protocol.push_back(*it);
		log.debug(protocol);
		for (it = _data.begin(); it != _data_it; ++it)
			version.push_back(*it);
		log.debug(version);
	}
}

bool RequestParser::_found_EOL(void) {
	std::vector<char>::iterator end = _data.end();
	(void)_idx;

	for(_data_it = _data.begin(); _data_it != end; ++_data_it) {
		if (*_data_it == CR) {
			log.debug("\nCR here");
			break;
		}
		if (*_data_it == LF) {
			log.debug("\nLF here");
			_error = LF_WITHOUT_CR;
			_step = END;
			return false;
		}
	}
	if (_data_it == end)
		return false;
	if (*_data_it == CR) {
		if (*(_data_it + 1) == LF) {
			_step = CRLF;
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
	return false;
}

