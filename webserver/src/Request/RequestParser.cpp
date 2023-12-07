/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 21:21:48 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/07 13:18:47 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

std::string const RequestParser::_right_protocol = "HTTP";

RequestParser::RequestParser(void): _step(INIT) {
	_request = NULL;
	_has_content_length = false;
	_is_chunked = false;
	_content_length = 0;
	_max_body_size = 0;
	_body_bytes_readed = 0;
	_chunk_size = 0;
	_chunk_bytes_readed = 0;
}

RequestParser::RequestParser(Request* request): _step(INIT), _request(request) {
	_has_content_length = false;
	_is_chunked = false;
	_content_length = 0;
	_max_body_size = size_t(request->server()->getBodySize());
	_body_bytes_readed = 0;
	_chunk_size = 0;
	_chunk_bytes_readed = 0;
}

RequestParser::RequestParser(RequestParser const& copy) {
	*this = copy;
}

RequestParser& RequestParser::operator=(RequestParser const& copy) {
	if (this != &copy) {
		_step = copy.step();
		if (_request && _request != copy.getRequest())
			delete _request;
		_request = copy.getRequest();
		_method = copy.getMethod();
		_uri = copy.getUri();
		_protocol = copy.getProtocol();
		_version = copy.getVersion();
		_field_name = copy.getFieldName();
		_field_value = copy.getFieldValue();
		_last_header = copy.getLastHeader();
		_headers = copy.getHeaders();
		_has_content_length = copy.has_content_length();
		_is_chunked = copy.is_chunked();
		_content_length = copy.content_length();
		_max_body_size = copy.max_body_size();
		_body_bytes_readed = copy.body_bytes_readed();
		_chunk_size = copy.chunk_size();
		_chunk_bytes_readed = copy.chunk_bytes_readed();
		_chunk_size_str = copy.getChunkSizeStr();
		_body = getBody();
	}
	return *this;
}

RequestParser::~RequestParser(void) {
	_body.clear();
	// _result.clear();
}

void RequestParser::setStep(Step s) {
	this->_step = s;
}

void RequestParser::init(char c) {
	_method.clear();
	_uri.clear();
	_protocol.clear();
	_version.clear();
	_field_name.clear();
	_field_value.clear();
	_last_header.clear();
	_headers.clear();
	_chunk_size_str.clear();
	_body.clear();
	_has_content_length = false;
	_is_chunked = false;
	_content_length = 0;
	_body_bytes_readed = 0;
	_chunk_size = 0;
	_chunk_bytes_readed = 0;

	_step = METHOD;
	method(c);
}

// general:
// Parts of request line is separated by a space character.
// Technically there should be only one space though I've seen badly malformed requests that send multiple spaces.
// Browsers will never send more than one space
// nginx aceita vários espaços e nós tb estamos aceitando
// não aceita espaço no início da primeira linha (não é letra maiúscula)

// The three fields in the initial message line should be separated by a single space, but might instead use several spaces, or tabs. Accept any number of spaces or tabs between these fields.

// Method names are always uppercase
// The method token is case-sensitive
// When a request method is received that is unrecognized or not implemented by an origin
// server, the origin server SHOULD respond with the 501 (Not Implemented) status code.
// When a request method is received that is known by an origin
// server but not allowed for the target resource, the origin server
// SHOULD respond with the 405 (Method Not Allowed) status code.
void RequestParser::method(char c) {
	if (utils::is_ualpha(c))
		_method.push_back(c);
	else if (c == SP) {
		if (_method.size() == 0)
			_bad_request("invalid method: method token begins with space");
		_step = URI;
		Logger::debug("method", _method);
	}
	else if (c == HTAB)
		_bad_request("invalid method/uri separator: horizontal tab");
	else if (c == CR || c == LF)
		_bad_request("missing data on request first line");
	else if (utils::is_lalpha(c))
		_bad_request("method characters must be upper case");
	else
		_bad_request("invalid character at request first line");
}

// URI enconding (tb serve pra POST) - não entendi se posso receber https
// URI normalization:
// normalizing the scheme and host to lowercase
// normalizing the port to remove any leading zeros.
// If port is elided from the URI, the default port for that scheme is used
// o host pode ser um IP, que pode estar em ipv4 ou ipv6...
// It is RECOMMENDED that all senders and recipients support, at a minimum, URIs with lengths of 8000 octets in protocol elements.
void RequestParser::uri(char c) {
	static bool init_uri = true;

	if (c == SP && init_uri)
		return ;
	else if (c == CR || c == LF)
		_bad_request("missing data on request first line");
	if (c != SP) { // make first validation
		if (init_uri) {
			if (c == HTAB)
				_bad_request("invalid method/uri separator: horizontal tab");
			init_uri = false;
		}
		_uri.push_back(c);
	}
	else if (c == SP) {
		Logger::debug("uri", _uri);
		_step = PROTOCOL;
		init_uri = true;
	}
	else
		_invalid_request("invalid uri", _uri, http::BAD_REQUEST);
}

// The HTTP version always takes the form "HTTP/x.x", uppercase
void RequestParser::protocol(char c) {
	static bool init_protocol = true;
	// static std::string protocol; // testar se dá pra usar

	if (c == SP && init_protocol)
		return ;
	if (c != SLASH && c != CR && c != LF && c != SP) {
		if (init_protocol)
			init_protocol = false;
		_protocol.push_back(c);
	}
	else if (c == SLASH) {
		if (_protocol.compare(_right_protocol) != 0)
			_invalid_request("invalid protocol", _protocol, http::BAD_REQUEST);
			// throw http::InvalidRequest(http::NOT_FOUND); // nginx
		_step = VERSION;
		Logger::debug("protocol", _protocol);
		init_protocol = true;
	}
	else if (c == SP)
		_bad_request("invalid space in HTTP protocol");
	else if (_protocol.compare(_right_protocol) == 0)
		_bad_request("missing HTTP version");
	else
		_invalid_request("invalid protocol", _protocol, http::BAD_REQUEST);
}

// RFC 9110 - item 2.5. Protocol Version:
// HTTP's version number consists of two decimal digits separated by a "."
// (period or decimal point)
// version format: <major-version>.<minor-version>

// RFC 9110 - item 6.2. Control Data:
// A server can send a 505 (HTTP Version Not Supported) response if it wishes,
// for any reason, to refuse service of the client's major protocol version.
// if major-version != 1: refuse -> 505

// RFC 9110 - item 6.2. Control Data:
// A recipient that receives a message with a major version number that it
// implements and a minor version number higher than what it implements SHOULD
// process the message as if it were in the highest minor version within that
// major version to which the recipient is conformant
// temos que aceitar 1.2, 1.3, 1.11
// não precisamos aceitar 1.0?

void RequestParser::version(char c) {
	int size;

	if (c == SP && !_version.empty())
		return ;
	if (std::isdigit(c) || (c == POINT && !_version.empty()))
		_version.push_back(c);
	else if (c == CR || c == LF) {
		size = _version.size();
		if (size == 3) {
			if (_version[0] == '1' && _version[1] == POINT && _version[2] == '1') {
				Logger::debug("version", _version);
				if (c == CR)
					_step = CR_FIRST_LINE;
				if (c == LF) {
					Logger::debug("LF first line");
					_step = HEADER;
				}
			}
			else
				_invalid_request(
					HTTP_VERSION, _version, http::HTTP_VERSION_NOT_SUPPORTED
				);
		}
		else if (
			size == 0
			|| (size == 1 && _version[0] == '1')
			|| (size == 2 && _version[0] == '1' && _version[1] == POINT)
		)
			_invalid_request(HTTP_VERSION, _version, http::BAD_REQUEST);
		else
			_invalid_request(HTTP_VERSION, _version, http::HTTP_VERSION_NOT_SUPPORTED);
	}
	else
		_invalid_request(HTTP_VERSION, http::BAD_REQUEST);
}

// Each part of a HTTP request is separated by a new line
// Note: Technically they should be \r\n but you are strongly encouraged to also accept \n as a newline
// Initial lines and headers should end in CRLF, though you should gracefully handle lines ending in just LF.
// Even though header lines should end with CRLF, someone might use a single LF instead. Accept either CRLF or LF.
// lidando apenas com CRLF por hora, verificar no RFC
void RequestParser::check_crlf(char c) {
	if (c != LF)
		_bad_request("CR without LF");
	else {
		switch (_step)
		{
		case CR_FIRST_LINE:
			Logger::debug("CRLF first line");
			_step = HEADER;
			break;
		case CR_HEADER:
			Logger::debug("CRLF header");
			_step = HEADER;
			break;
		case SECOND_CR_HEADER:
			Logger::debug("CRLF end header");
			_step = END;
			break;
		case CR_BODY:
			Logger::debug("CRLF end body");
			_step = END;
			break;
		case CR_CHUNK_SIZE:
			Logger::debug("CRLF chunk size");
			_step = CHUNK_DATA;
			break;
		case CR_CHUNK_DATA:
			Logger::debug("CRLF chunk data");
			_step = CHUNK_SIZE;
			break;
		case CR_CHUNK_END:
			Logger::debug("first CRLF chunk end");
			_step = CHUNK_END;
			break;
		case SECOND_CR_CHUNK_END:
			Logger::debug("second CRLF chunk end");
			_step = END;
			break;
		default:
			break;
		}
	}
}

// headers
// servers should treat headers as an unordered set
// one line per header, of the form "Header-Name: value", ending with CRLF
// you should handle LF correctly
// HTTP 1.1 defines 46 headers, and one (Host:) is required in requests.

// A server MUST NOT apply a request to the target resource until it receives the
// entire request header section, since later header field lines might include
// conditionals, authentication credentials, or deliberately misleading duplicate
// header fields that could impact request processing
void RequestParser::header(char c) {
	if (_step == HEADER) {
		if (c == CR) {
			_step = SECOND_CR_HEADER;
			return ;
		}
		else if (c == LF) {
			Logger::debug("LF end header");
			_step = END;
			return ;
		}
		else if (c == SP || c == HTAB) {
			_field_name = _last_header;
			_step = HEADER_VALUE_INIT;
		}
		else
			_step = HEADER_NAME;
	}
	switch (_step) {
		case HEADER_NAME:
			_parse_field_name(c);
			break;
		case HEADER_VALUE_INIT:
			if (c == SP || c == HTAB)
				return ;
			_step = HEADER_VALUE;
			_parse_field_value(c);
			break;
		case HEADER_VALUE:
			_parse_field_value(c);
			break;
		default:
			break;
	}
}

// Field name:
// The requested field name. It MUST conform to the field-name syntax defined
// in Section 5.1, and it SHOULD be restricted to just letters, digits,
// and hyphen ('-') characters, with the first character being a letter.
// nginx não segue essas regras, aceita qualquer coisa, google tb...

// The header name is not case-sensitive (though the value may be) - Header
// name can be either title-case or lowercase or mixed, all are valid

// nos headers parece considerar um espaço no começo do nome do header como parte dele
// interpreta " Host" e não "Host" (dá erro porque considera que o header não está incluso)
// Header lines beginning with space or tab are actually part of the previous header line, folded into multiple lines for easy reading.
// the following two headers are equivalent:

// Header1: some-long-value-1a, some-long-value-1b
// HEADER1:    some-long-value-1a,
//             some-long-value-1b

void RequestParser::_parse_field_name(char c) {
	if (c != COLON && !utils::is_ctl(c) && c != SP)
		_field_name.push_back(c);
	else if (c == COLON) {
		Logger::debug("key", _field_name);
		_step = HEADER_VALUE_INIT;
	}
	else if (c == CR) {
		// Logger::warning("field name without value: " + _field_name);
		_step = CR_HEADER;
		_add_header();
	}
	else
		_invalid_request("field name character", c, http::BAD_REQUEST);
}

// Any number of spaces or tabs may be between the ":" and the value
void RequestParser::_parse_field_value(char c) {
	// if (c == SP || c == HTAB) // só pode ser no começo
	// 	return ;
	if (c == CR || c == LF) {
		Logger::debug("value", _field_value);
		if (c == CR)
			_step = CR_HEADER;
		else if (c == LF) {
			Logger::debug("LF header");
			_step = HEADER;
		}
		_add_header();
	}
	else if (utils::is_ctl(c) && c != HTAB)
		_bad_request("request parser: control character on field value");
	else
		_field_value.push_back(c);
}

void RequestParser::_add_header(void) {
	t_header_iterator it;

	std::transform(
		_field_name.begin(),
		_field_name.end(),
		_field_name.begin(),
		&utils::c_tolower
	);
	_last_header = _field_name; // verificar se faz deep copy...
	it = _headers.find(_field_name);
	if (it == _headers.end()) {
		// não tem o header
		std::vector<std::string> v;

		v.clear();
		v.push_back(_field_value);
		_headers.insert(std::make_pair(_field_name, v));
	}
	else // tem o header - se a linha começou com espaço/tab; se não dá erro!!
		it->second.push_back(_field_value); // assumindo que ele já estará limpo de espaços e tabs no início e fim
	_field_value.clear();
	_field_name.clear();
}

void RequestParser::check_request(void) {
	Logger::debug("Checking request");
	// first check headers and get necessary values
	_print_headers();
	_check_host();
	_check_content_length();
	_check_transfer_encoding();
	_headers.clear();

	// check if the path exists and set it; get location
	_check_uri();
	// verify if method is allowed in specified location:
	_check_method();
	if (_request->method() == http::POST)
		_check_post_headers();
	Logger::debug("End checking request");
}

// header Host is mandatory and singleton
void RequestParser::_check_host(void) {
	t_header_iterator it;

	it = _headers.find("host");
	if (it == _headers.end())
		_bad_request("'Host' header not found");
	if (it->second.size() > 1 || it->second[0].size() == 0)
		_bad_request("'Host' header");
	_request->setHost(it->second[0]);
}

// header Content-Length has only numbers, is singleton and has a maximum size
void RequestParser::_check_content_length(void) {
	t_header_iterator it_header;

	it_header = _headers.find("content-length");
	if (it_header == _headers.end())
		return ;
	if (it_header->second.size() > 1)
		_bad_request("'Content-Length' header");
	_has_content_length = true;

	std::string content_lenght = it_header->second[0];
	std::string::iterator it, end = content_lenght.end();
	for (it = content_lenght.begin(); it != end; ++it) {
		if (!std::isdigit(*it))
			_invalid_request(
				"'Content-Length' header invalid character",
				*it,
				http::BAD_REQUEST
			);
	}

	std::stringstream ss(content_lenght);
	ss >> _content_length;
	if (_content_length > _max_body_size)
		_invalid_request(
			"Content-Lenght bigger than max body size",
			http::CONTENT_TOO_LARGE
		);
	_request->setContentLength(_content_length);
}

void RequestParser::_check_transfer_encoding(void) {
	t_header_iterator it_header;

	it_header = _headers.find("transfer-encoding");
	if (it_header == _headers.end())
		return ;
	if (it_header->second.size() > 1)
		_bad_request("'Transfer-Encoding' header");
	if (it_header->second[0].compare("chunked") != 0)
		_invalid_request("Transfer-Encoding type", http::NOT_IMPLEMENTED);
	_is_chunked = true;
	_request->setChuncked(true);
	Logger::debug("found Transfer-Encoding: chunked");
}

void RequestParser::_check_uri(void) {
	// check if the path exists and set it; get location
	// uri
	// not found here
	_request->setUri(_uri);
}

void RequestParser::_check_method(void) {
	// verify if method is allowed in specified location:
	try {
		// esse check vai ter que levar em conta o location
		_request->setMethod(http::str_to_enum_method(_method));
	} catch(const utils::GeneralException& e) {
		throw http::InvalidRequest(http::METHOD_NOT_ALLOWED);
	} catch(const std::exception& e) {
		throw e;
	}
}

void RequestParser::_check_post_headers(void) {
	if (!_is_chunked && (!_has_content_length || !_content_length))
		_invalid_request(
			"POST without 'Transfer-Encoding' or 'Content-Length' headers",
			http::LENGTH_REQUIRED
		);
	if (_is_chunked && _has_content_length && _content_length)
		_bad_request(
			"chunked data and content-length both setted"
		);
	_step = BODY;
	if (_is_chunked)
		_step = CHUNK_SIZE;
}

void RequestParser::_print_headers(void) {
	if (DEBUG) {
		t_header_iterator it, end;
		std::vector<std::string>::iterator v_it, v_end;
		int i;

		end = _headers.end();
		std::cout << GREY
				<< "------------------------------------------------------\n"
				<< BLUE << "Headers saved:\n" << GREY;
		for (it = _headers.begin(); it != end; ++it) {
			i = 0;
			std::cout << it->first << ": ";
			v_end = it->second.end();
			for (v_it = it->second.begin(); v_it != v_end; ++v_it) {
				++i;
				if (i == 1)
					std::cout << *v_it;
				else
					std::cout << "," << *v_it;
			}
			std::cout << std::endl;
		}
		std::cout << "------------------------------------------------------\n"
				<< RESET;
	}
}

// t_string_map	RequestParser::get_result(void) const {
// 	return _result;  // tem problema? não vou alterar, só consultar...
// }

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

void RequestParser::_bad_request(std::string const description) {
	http::bad_request(REQPARSER_ERROR + description);
}

void  RequestParser::_invalid_request(
	std::string const description,
	std::string const value,
	http::HttpStatus error_code
) {
	Logger::warning(REQPARSER_ERROR + description, value);
	throw http::InvalidRequest(error_code);
}

void  RequestParser::_invalid_request(
	std::string const description,
	char const c,
	http::HttpStatus error_code
) {
	Logger::warning_no_lf(REQPARSER_ERROR + description);
	std::cout << GREY << c << std::endl;
	throw http::InvalidRequest(error_code);
}

void  RequestParser::_invalid_request(
	std::string const description,
	http::HttpStatus error_code
) {
	Logger::warning(REQPARSER_ERROR + description);
	throw http::InvalidRequest(error_code);
}

// void RequestParser::break_data(char* buffer, size_t bytes_read) {
// 	for (size_t i = 0; i < bytes_read; ++i)
// 		_body.push_back(buffer[i]);

// 	if (DEBUG) {
// 		std::cout << GREY << "Received/remaining data:\n";
// 		_print_body();
// 	}
// }

// The most common use of POST, by far, is to submit HTML form data to CGI scripts.
// In this case, the Content-Type: header is usually application/x-www-form-urlencoded,
// and the Content-Length: header gives the length of the URL-encoded form data
// Ex:
// POST /path/script.cgi HTTP/1.0
// From: frog@jmarshall.com
// User-Agent: HTTPTool/1.0
// Content-Type: application/x-www-form-urlencoded
// Content-Length: 32

// home=Cosby&favorite+flavor=flies


// You can use a POST request to send whatever data you want, not just form submissions.
// Just make sure the sender and the receiving program agree on the format.


// URL encoding
// HTML form data is usually URL-encoded to package it in a GET or POST submission. In a nutshell, here's how you URL-encode the name-value pairs of the form data:

// Convert all "unsafe" characters in the names and values to "%xx", where "xx" is the ascii value of the character, in hex. "Unsafe" characters include =, &, %, +, non-printable characters, and any others you want to encode-- there's no danger in encoding too many characters. For simplicity, you might encode all non-alphanumeric characters.
// Change all spaces to plusses.
// String the names and values together with = and &, like
// name1=value1&name2=value2&name3=value3
// This string is your message body for POST submissions, or the query string for GET submissions.
// For example, if a form has a field called "name" that's set to "Lucy", and a field called "neighbors" that's set to "Fred & Ethel", the URL-encoded form data would be
// name=Lucy&neighbors=Fred+%26+Ethel
// with a length of 34.

// Using GET to Submit Query or Form Data
// You can use GET to send small amounts of data to the server. The key is to
// understand just what the request URI is: It's not necessarily a file name,
// it's a string that identifies a data resource on the server. That may be a file
// name, but it may also be, for example, a specific query to a specific database.
// The result of that query may not live in a file, but it's a data resource all
// the same, identified by the search engine and the query data that together produce it.

// So, to send data to a CGI script using a GET request, include that data after
// the question mark in the URL (read about URL syntax for more details). For example:

// GET /path/script.cgi?field1=value1&field2=value2 HTTP/1.0


// CGI response
// Sending an Existing File Back as a Response
// If your HTML response is always the same, or if you want to respond with one of several existing files, you may find the "Location:" response header useful. Use it to redirect the browser to another URL.

// By way of example, if your CGI script prints the following line to STDOUT:

// Location: response.html
// followed by a blank line, then the remote browser will retrieve response.html and treat it as the response from your CGI script. You can redirect the browser to either a relative or absolute URL.
// In this situation, do not print the "Content-type:" response header.


// "Transfer-Encoding: chunked" header. All HTTP 1.1 clients must be able to receive chunked messages
// Servers aren't required to generate chunked messages; they just have to be able to receive them.

// POST ->

// chunked:
// Transfer-Encoding: chunked
// Content-Length: 0 (se tiver, deve ser 0 para evitar ambiguidade)


// 3. Chunk Format (RFC 7230, Section 4.1):
// Format: Each chunk starts with the size of the chunk in hexadecimal followed
// by a CRLF (carriage return and line feed) sequence, then the chunk data, and
// another CRLF.
// Example:
// http
// Copy code
// POST /example HTTP/1.1
// Host: example.com
// Transfer-Encoding: chunked

// 4
// data
// 5
// chunk
// 0
// Explanation: In this example, two chunks of data are sent. The size of the
// first chunk is 4, followed by the chunk data ("data"). The size of the second
// chunk is 5, followed by the chunk data ("chunk"). A chunk with size 0
// indicates the end of the message.
// 4. Trailer Headers (Optional):
// Header Format: Additional headers may be sent in trailers after the final
// chunk. Trailers are optional and can include headers like Content-MD5 or others.


// @app.route('/example', methods=['GET'])
// def handle_request():
//     user_input = request.headers.get('User-Input')

//     # Validate and sanitize user input
//     if '\r' in user_input or '\n' in user_input:
//         return "Invalid input", 400
