/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_body.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 00:24:26 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/08 00:27:05 by sguilher         ###   ########.fr       */
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
	// _print_body();
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
	if (_chunk_bytes_readed < _chunk_size) {
		_body.push_back(c);
		++_chunk_bytes_readed;
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
				<< BLUE << "Received body of size " << _body_bytes_readed
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
