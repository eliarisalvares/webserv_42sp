/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 00:24:26 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/10 01:30:58 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

// Body

//  Request messages are never close-delimited because they are always explicitly
//  framed by length or transfer coding, with the absence of both implying the
//   request ends immediately after the header section

// A server MAY reject a request that contains a message body but not a
// Content-Length by responding with 411 (Length Required).

// If an HTTP message includes a body, there are usually header lines in the message that describe the body, eg (ver se são obrigatórios):
// Content-Type: header gives the MIME-type of the data in the body, such as text/html or image/gif.
// Content-Length: header gives the number of bytes in the body.

// A HTTP request is terminated by two newlines
// Note: Technically they should be 4 bytes: \r\n\r\n but you are strongly encouraged to also accept 2 byte terminator: \n\n.

// If a valid Content-Length header field is present without Transfer-Encoding,
// its decimal value defines the expected message body length in octets. If the
// sender closes the connection or the recipient times out before the indicated
// number of octets are received, the recipient MUST consider the message to be
// incomplete and close the connection.

void RequestParser::body(char c) {
	if (_is_chunked)
		return _body_chunked(c);
	if (_body_bytes_readed < _content_length) {
		_body.push_back(c);
		++_body_bytes_readed;
		if (_body_bytes_readed == _content_length) {
			_step = BODY_LENGTH_END;
			_print_body();
		}
	}
}

void RequestParser::end_body(char c) {
	if (c == CR)
		_step = CR_BODY;
	else if (c == LF) {
		Logger::debug("LF end Body");
		_step = END;
	}
	else
		_invalid_request("Body larger than specified", http::CONTENT_TOO_LARGE);
}

// RFC 9112 - 6.3.4
// If a Transfer-Encoding header field is present in a request and the chunked
// transfer coding is not the final encoding (0), the message body length cannot
// be determined reliably; the server MUST respond with the 400 (Bad Request)
//  status code and then close the connection.

// depois de ler cada chunck, devolve uma resposta com status code = 202 pra
// indicar pro cliente que foi aceito
// quando finalizar, retorna 200 ou 201 (CREATED)

// Example:
// 4␍␊            (chunk size is four octets)
// Wiki           (four octets of data)
// ␍␊             (end of chunk)

// 7␍␊            (chunk size is seven octets)
// pedia i        (seven octets of data)
// ␍␊             (end of chunk)

// B␍␊            (chunk size is eleven octets)
// n ␍␊chunks.    (eleven octets of data)
// ␍␊             (end of chunk)

// 0␍␊            (chunk size is zero octets, no more chunks)
// ␍␊             (end of final chunk with zero data octets)

// Result string: "Wikipedia in ␍␊chunks."
// Result print:
// "Wikipedia in
// chunks."

void RequestParser::_body_chunked(char c) {
	switch (_step) {
		case CHUNK_SIZE:
			_parse_chunk_size(c);
			break;
		case CHUNK_PARAMETERS:
			// por hora não vou fazer nada com os parametros
			if (c == CR)
				_step = CR_CHUNK_SIZE;
			if (c == LF)
				_step = CHUNK_DATA;
			Logger::debug("chunk parameter: ", c);
			break;
		case CHUNK_DATA:
			_parse_chunk_data(c);
			break;
		case CHUNK_DATA_END:
			if (c == CR)
				_step = CR_CHUNK_DATA;
			else if (c == LF)
				_step = CHUNK_SIZE;
			else // verificar o tipo de erro
				_invalid_request(
					"chunk data larger than specified", http::CONTENT_TOO_LARGE
				);
			break;
		case CHUNK_END:
			if (c == CR)
				_step = SECOND_CR_CHUNK_END;
			else if (c == LF)
				_step = END;
			else // verificar o tipo de erro
				_invalid_request(
					"chunk data larger than specified", http::CONTENT_TOO_LARGE
				);
			break;

		default:
			break;
	}
	_print_body();
}

void RequestParser::_parse_chunk_size(char c) {
	if (c == CR || c == LF || c == SEMICOLON) {
		if (c == CR)
			_step = CR_CHUNK_SIZE;
		else if (c == LF)
			_step = CHUNK_DATA;
		else if (c == SEMICOLON)
			_step = CHUNK_PARAMETERS;
		_chunk_size = utils::xtod(_chunk_size_str);
		_chunk_bytes_readed = 0;
		Logger::debug("chunk size hex:", _chunk_size_str);
		Logger::debug("chunk size decimal:", _chunk_size);
		if (_chunk_size == 0) {
			if (c == CR)
				_step = CR_CHUNK_END;
			if (c == LF) {
				Logger::debug("first LF chunk end");
				_step = CHUNK_END;
			}
		}
		_chunk_size_str.clear();
	}
	else if (!std::isxdigit(c))
		_bad_request("Invalid character on chunk size");
	else
		_chunk_size_str.push_back(c);
}

void RequestParser::_parse_chunk_data(char c) {
	if (_body_bytes_readed == _max_body_size)
		_invalid_request(
			"Data transfered bigger than max body size",
			http::CONTENT_TOO_LARGE
		);
	if (_chunk_bytes_readed < _chunk_size) {
		_body.push_back(c);
		++_chunk_bytes_readed;
		++_body_bytes_readed;
		if (_chunk_bytes_readed == _chunk_size) {
			_step = CHUNK_DATA_END;
			_print_body();
		}
	}
}

void RequestParser::_print_body(void) {
	std::vector<char>::iterator it, end = _body.end();

	if (DEBUG) {
		std::cout << GREY
				<< "------------------------------------------------------\n"
				<< BLUE << "Body of size " << _body.size()
				<< ":\n" << GREY;
		for(it = _body.begin(); it != end; ++it) {
			// if (*it == CR)
			// 	std::cout << " CR";
			// else if (*it == LF)
			// 	std::cout << "LF" << *it;
			// else
			std::cout << *it;
		}
		std::cout
				<< "\n------------------------------------------------------\n"
				<< RESET << std::endl;
	}
}

void RequestParser::parse_body(void) {
	if (_media_type == http::MULTIPART_FORM_DATA)
		_parse_multipart();
	else if (_media_type == http::FORM_URLENCODED)
		_parse_form_data();
}

void RequestParser::_parse_form_data(void) {
	FormData step = FIELD_NAME;
	std::string hexadecimal;
	bool is_hexadecimal = false;

	_body_iterator_first = _body.begin();
	_body_iterator_end = _body.end();
	_body_iterator = _body_iterator_first;
	_field_name.clear();
	_field_value.clear();
	hexadecimal.clear();
	if (DEBUG)
		Logger::info("Parsing form url encoded data:");
	while (_body_iterator != _body_iterator_end) {
		switch (step)
		{
		case FIELD_NAME:
			if (*_body_iterator == EQUAL)
				step = FIELD_VALUE;
			else if (!http::is_uri_char(*_body_iterator))
				_bad_request("Field name in POST with invalid character");
			else if (is_hexadecimal) {
				if (hexadecimal.size() == 1) {
					hexadecimal.push_back(*_body_iterator);
					_field_name.push_back(utils::xtod(hexadecimal));
					is_hexadecimal = false;
					hexadecimal.clear();
				}
				else
					hexadecimal.push_back(*_body_iterator);
			}
			else if (*_body_iterator == PERCENT)
				is_hexadecimal = true;
			else
				_field_name.push_back(*_body_iterator);
			break;
		case FIELD_VALUE:
			if (*_body_iterator == AMPERSEND) {
				step = FIELD_NAME;
				_request->addPostData(_field_name, _field_value);
				_field_name.clear();
				_field_value.clear();
			}
			else if (!http::is_uri_char(*_body_iterator))
				_bad_request("Field value in POST with invalida character");
			else if (is_hexadecimal) {
				if (hexadecimal.size() == 1) {
					hexadecimal.push_back(*_body_iterator);
					_field_value.push_back(utils::xtod(hexadecimal));
					is_hexadecimal = false;
					hexadecimal.clear();
				}
				else
					hexadecimal.push_back(*_body_iterator);
			}
			else if (*_body_iterator == PERCENT)
				is_hexadecimal = true;
			else
				_field_value.push_back(*_body_iterator);
			break;

		default:
			break;
		}
		++_body_iterator;
	}
	_request->addPostData(_field_name, _field_value);
	_request->printPostData();
	_field_name.clear();
	_field_value.clear();
}

void RequestParser::_parse_multipart(void) {
	// ParseBody parse_body_step = INITIAL_BOUNDARY;

	_check_boundary_delimiter(0);
	_check_boundary();
	_check_content_disposition();
	_check_data_content_type();
	_check_multipart_crfl();
	_separate_data();
	if (_body.size()) {
		_request->setHasImage(true);
		_request->setImage(&_body);
		_request->setImageType("png");
	}
	// _check_multipart_crfl();
	// _check_boundary_delimiter();
	// _check_boundary();


	// switch (parse_body_step)
	// {
	// case INITIAL_BOUNDARY:
	// 	/* code */
	// 	break;
	// case BOUNDARY:
	// cas CO

	// default:
	// 	break;
	// }
	// INITIAL_BOUNDARY
	// BOUNDARY
	// CONTENT_DISPOSITION
	// CONTENT_TYPE
	// CRLF
	// BODY
	// CRLF
}

void RequestParser::_check_boundary_delimiter(size_t pos) {
	_body_iterator_first = _body.begin();
	_body_iterator_end = _body.end();
	_body_iterator = _body_iterator_first;
	Logger::debug("Boundary delimiter:");
	for (pos = 0; pos < 2; ++pos) {
		// passar para uma string
		std::cout << GREY << *_body_iterator;
		++_body_iterator;
	}
	std::cout << RESET << "\n";
	// checkar string
	_body.erase(_body_iterator_first, _body_iterator);
}

void RequestParser::_check_boundary(void) {
	_body_iterator_first = _body.begin();
	_body_iterator_end = _body.end();
	_body_iterator = _body_iterator_first;
	Logger::debug("Boundary:");
	while (
		_body_iterator != _body_iterator_end &&
		*_body_iterator != CR &&
		*_body_iterator != LF
	) {
		// passar para uma string
		std::cout << GREY << *_body_iterator;
		++_body_iterator;
	}
	std::cout << RESET << "\n";
	// check string
	if (*_body_iterator == CR) {
		++_body_iterator;
	}
	if (*_body_iterator == LF) {
		++_body_iterator;
	}
	_body.erase(_body_iterator_first, _body_iterator);
}

void RequestParser::_check_content_disposition(void) {
	_body_iterator_first = _body.begin();
	_body_iterator_end = _body.end();
	_body_iterator = _body_iterator_first;
	Logger::debug("Content-Disposition:");
	while (
		_body_iterator != _body_iterator_end &&
		*_body_iterator != CR &&
		*_body_iterator != LF
	) {
		// passar para uma string
		std::cout << GREY << *_body_iterator;
		++_body_iterator;
	}
	std::cout << RESET << "\n";
	// check string
	if (*_body_iterator == CR)
		++_body_iterator;
	if (*_body_iterator == LF)
		++_body_iterator;
	_body.erase(_body_iterator_first, _body_iterator);
}

void RequestParser::_check_data_content_type(void) {
	_body_iterator_first = _body.begin();
	if (*_body_iterator_first == CR)
		return ;
	_body_iterator_end = _body.end();
	_body_iterator = _body_iterator_first;
	Logger::debug("Content-Type:");
	while (
		_body_iterator != _body_iterator_end &&
		*_body_iterator != CR &&
		*_body_iterator != LF
	) {
		// passar para uma string
		std::cout << GREY << *_body_iterator;
		++_body_iterator;
	}
	std::cout << RESET << "\n";
	// check string
	if (*_body_iterator == CR)
		++_body_iterator;
	if (*_body_iterator == LF)
		++_body_iterator;
	_body.erase(_body_iterator_first, _body_iterator);
}

void RequestParser::_check_multipart_crfl(void) {
	_body_iterator_first = _body.begin();
	_body_iterator = _body_iterator_first;

	if (*_body_iterator == CR)
		++_body_iterator;
	if (*_body_iterator == LF)
		++_body_iterator;
	_body.erase(_body_iterator_first, _body_iterator);
}

void RequestParser::_separate_data(void) {
	// bool end = false;
	// bool dash = false;

	// size_t body_size = _body.size();
	// for (int i = body_size; i > body_size )
	_body_iterator_end = _body.end();
	_body_iterator_first = _body_iterator_end - 8 - _boundary.size();
	_body.erase(_body_iterator_first, _body_iterator_end);
	Logger::warning("Image data:");
	_print_body();
	// while (
	// 	_body_iterator != _body_iterator_end
	// 	&& (*_body_iterator != CR || *_body_iterator != LF)
	// ) {
	// 	// passar para uma string
	// 	std::cout << GREY << *_body_iterator;
	// 	++_body_iterator;
	// } // PNG
	// if (*_body_iterator == CR)
	// 	++_body_iterator;
	// if (*_body_iterator == LF)
	// 	++_body_iterator;
	// while (
	// 	_body_iterator != _body_iterator_end
	// 	&& (*_body_iterator != CR || *_body_iterator != LF)
	// ) {
	// 	if (*_body_iterator == DASH) {
	// 		Logger::error("Found dash");
	// 		if (!dash)
	// 			dash = true;
	// 		else
	// 			end = _check_end_boundary(_body_iterator - 1);
	// 		if (end)
	// 			break;
	// 		else
	// 			dash = true;
	// 	}
	// 	// passar para uma string
	// 	std::cout << GREY << *_body_iterator;
	// 	++_body_iterator;
	// }
	// std::cout << RESET << "\n";
	// check string
	// if (*_body_iterator == CR)
	// 	++_body_iterator;
	// if (*_body_iterator == LF)
	// 	++_body_iterator;
	// _body.erase(_body_iterator_first, _body_iterator);
}

bool RequestParser::_check_end_boundary(std::vector<char>::iterator initial) {
	Logger::error("end boundary");
	_body_iterator_end = _body.end();
	_body.erase(initial, initial + 2 + _boundary.size());
	return true;
}



// --------------------------f069bd9492f6146e
// ------------------------f069bd9492f6146e

// 2 DASHS - BOUNDARY - CRFL
// BODY_HEADERS:
// Content-Disposition: form-data; params (name="" pelo menos...) - CRLF
// Content-Type (opcional) - CRLF
// CRLF
// content ... CRFL
// 2 DASHS - BOUNDARY - CRFL

// --------------------------62774971962833fd
// Content-Disposition: form-data; name="file"; filename="txt.txt"
// Content-Type: text/plain

// Hello World
// --------------------------62774971962833fd--
