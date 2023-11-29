/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:43:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 12:08:46 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

std::string const RequestParser::_right_protocol = "HTTP";
// int const RequestParser::_right_version = 1;

RequestParser::RequestParser(void):
	_idx(0), _step(INIT) {
	_request = new Request();
	_field_name.clear();
	_field_value.clear();
	_last_header.clear();
	_headers.clear();
	_body.clear();
	// _result.clear();
}

RequestParser::RequestParser(Request* request):
	_idx(0), _step(INIT), _request(request) {
	_field_name.clear();
	_field_value.clear();
	_last_header.clear();
	_headers.clear();
	_body.clear();
	// _result.clear();
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
	_body.clear();
	// _result.clear();
}

RequestParser::Steps RequestParser::step(void) {
	return this->_step;
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
	_step = METHOD;
	if (c >= 'A' && c <= 'Z')
		_method.push_back(c);
	else if (c == SP) {
		if (_method.size() == 0) // " GET..." - iniciando com espaço
			throw http::InvalidRequest(http::BAD_REQUEST);
		_step = URI;
		log.debug(_method);
	}
	else // caracteres diferentes de letras maiúsculas
		throw http::InvalidRequest(http::BAD_REQUEST);
}

// URI enconding (tb serve pra POST) - não entendi se posso receber https
// URI normalization:
// normalizing the scheme and host to lowercase
// normalizing the port to remove any leading zeros.
// If port is elided from the URI, the default port for that scheme is used
// o host pode ser um IP, que pode estar em ipv4 ou ipv6...
// It is RECOMMENDED that all senders and recipients support, at a minimum, URIs with lengths of 8000 octets in protocol elements.
void RequestParser::uri(char c) {
	if (c != SP) // make first validation
		_uri.push_back(c);
	else if (c == SP) {
		log.debug(_uri);
		_step = PROTOCOL;
	}
	else
		throw http::InvalidRequest(http::BAD_REQUEST);
}

// The HTTP version always takes the form "HTTP/x.x", uppercase
void RequestParser::protocol(char c) {
	static int n = 0;
	// static std::string protocol; // testar se dá pra usar

	if (c != '/') {
		_protocol.push_back(c);
		n++;
	}
	else if (c == '/') {
		if (_protocol.compare(_right_protocol) != 0)
			throw http::InvalidRequest(http::NOT_FOUND); // erro que o nginx retorna
			// throw http::InvalidRequest(http::BAD_REQUEST); // eu acho mais adequado
		_step = VERSION;
		log.debug(_protocol);
	} // checkar _protocol == "HTTP"
	else
		throw http::InvalidRequest(http::BAD_REQUEST);
}

// se número - verificar se é versão suportada
// outro caracter - Bad Request
void RequestParser::version(char c) {
	// if (c != SP && ((c >= '0' && c <= '1') || c == '.')) // checkar para número
	if (c != CR)
		_version.push_back(c);
	else if (c == CR) {
		// if _version != "1.1"
		_step = CR_FIRST_LINE;
		log.debug(_version);
	}
	else
		throw http::InvalidRequest(http::BAD_REQUEST);
}

// Each part of a HTTP request is separated by a new line
// Note: Technically they should be \r\n but you are strongly encouraged to also accept \n as a newline
// Initial lines and headers should end in CRLF, though you should gracefully handle lines ending in just LF.
// Even though header lines should end with CRLF, someone might use a single LF instead. Accept either CRLF or LF.
// lidando apenas com CRLF por hora, verificar no RFC
void RequestParser::check_crlf(char c) {
	if (c != LF) {
		throw http::InvalidRequest(http::BAD_REQUEST);
	}
	else {
		switch (_step)
		{
		case CR_FIRST_LINE:
			log.debug("CRLF first line");
			_step = HEADER;
			break;
		case CR_HEADER:
			log.debug("CRLF header");
			_step = HEADER;
			break;
		case SECOND_CR_HEADER:
			log.debug("CRLF end header");
			_step = END; // mudar para BODY?
			// por hora:
			// BODY para testar body;
			// END para retornar a resposta sem ficar travando
			break;
		case CR_BODY:
			log.debug("CRLF body");
			_step = BODY_NEW_LINE;
			break;
		case SECOND_CR_BODY:
			log.debug("CRLF end body");
			_step = END;
			break;
		default:
			break;
		}
	}
}

void RequestParser::check_first_line(void) {
	// method
	// uri
	// version
}

// headers
// servers should treat headers as an unordered set
// one line per header, of the form "Header-Name: value", ending with CRLF
// you should handle LF correctly
// The header name is not case-sensitive (though the value may be) - Header
// name can be either title-case or lowercase or mixed, all are valid
// HTTP 1.1 defines 46 headers, and one (Host:) is required in requests.
void RequestParser::header(char c) {
	if (_step == HEADER) {
		if (c == CR) {
			_step = SECOND_CR_HEADER;
			return ;
		}
		else
			_step = HEADER_NAME;
	}
	if (_step == HEADER_NAME)
		_parse_field_name(c);
	else if (_step == HEADER_VALUE)
		_parse_field_value(c);
}

// Field name:
// The requested field name. It MUST conform to the field-name syntax defined
// in Section 5.1, and it SHOULD be restricted to just letters, digits,
// and hyphen ('-') characters, with the first character being a letter.

// nos headers parece considerar um espaço no começo do nome do header como parte dele
// interpreta " Host" e não "Host" (dá erro porque considera que o header não está incluso)
// Header lines beginning with space or tab are actually part of the previous header line, folded into multiple lines for easy reading.
// the following two headers are equivalent:

// Header1: some-long-value-1a, some-long-value-1b
// HEADER1:    some-long-value-1a,
//             some-long-value-1b

void RequestParser::_parse_field_name(char c) {
	if (c != COLON) // make first validation
		_field_name.push_back(c);
	else {
		log.debug(_field_name);
		_step = HEADER_VALUE;
	}
	// checkar espaços -> significa que é continuação do header anterior
	// se for o primeiro, guarda com espaço

	// if (c == CR)
	// erro - não, ele ignora, a não ser que seja um header que tem validação
}

// Any number of spaces or tabs may be between the ":" and the value
void RequestParser::_parse_field_value(char c) {
	if (c == SP || c == TAB)
		return ;
	if (c == CR) {
		log.debug(_field_value);
		_step = CR_HEADER;
		// _last_header = _field_name; // verificar se faz deep copy...
		_add_header();
	}
	else
		_field_value.push_back(c);
}

static unsigned char c_tolower(unsigned char c) { return std::tolower(c); }
void RequestParser::_add_header(void) {
	std::map<std::string, std::vector<std::string> >::iterator it;

	std::transform(
		_field_name.begin(),
		_field_name.end(),
		_field_name.begin(),
		&c_tolower
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
	else // tem o header
		it->second.push_back(_field_value); // assumindo que ele já estará limpo de espaços e tabs no início
	_field_value.clear();
	_field_name.clear();
}

void RequestParser::check_headers(void) {
	// check se tem body -> passa _step = BODY (ver onde entra)

	// 1 - verificar se tem o header obrigatório host
	std::map<std::string, std::vector<std::string> >::iterator it;

	it = _headers.find("host");
	if (it == _headers.end()) {
		throw http::InvalidRequest(http::BAD_REQUEST);
	}

	// 2 - check de alguns headers: verificar se os valores dos headers estão compatíveis
	_print_headers();

	// 3 - verificar se o método é permitido:
	try {
		// esse check vai ter que levar em conta também o location
		_request->setMethod(http::str_to_enum_method(_method));
	} catch(const utils::GeneralException& e) {
		throw http::InvalidRequest(http::METHOD_NOT_ALLOWED);
	} catch(const std::exception& e) {
		log.error("error on request parsing: ");
		log.error(e.what());
		_request->setError(true);
		_request->setStatusCode(http::INTERNAL_SERVER_ERROR);
		_step = END;
	}

	// ver onde a verificação da uri entra
	// uri
	// not found here
	_request->setUri(_uri);

	// version
}

void RequestParser::_print_headers(void) {
	if (DEBUG) {
		std::map<std::string, std::vector<std::string> >::iterator it, end;
		std::vector<std::string>::iterator v_it, v_end;
		int i;

		end = _headers.end();
		std::cout << GREY
				<< "------------------------------------------------------\n"
				<< BLUE << "Headers saved:\n" << GREY;
		for (it = _headers.begin(); it != end; ++it) {
			i = 1;
			std::cout << it->first << ": ";
			v_end = it->second.end();
			for (v_it = it->second.begin(); v_it != v_end; ++v_it)
				std::cout << i << ". " << *v_it << " | ";
			std::cout << std::endl;
		}
		std::cout << "------------------------------------------------------\n";
		std::cout << RESET << std::endl;
	}
}

// t_string_map	RequestParser::get_result(void) const {
// 	return _result;  // tem problema? não vou alterar, só consultar...
// }

// Body
// If an HTTP message includes a body, there are usually header lines in the message that describe the body, eg (ver se são obrigatórios):
// Content-Type: header gives the MIME-type of the data in the body, such as text/html or image/gif.
// Content-Length: header gives the number of bytes in the body.

// A HTTP request is terminated by two newlines
// Note: Technically they should be 4 bytes: \r\n\r\n but you are strongly encouraged to also accept 2 byte terminator: \n\n.

void RequestParser::body(char c) {
	if (_step == BODY_NEW_LINE) {
		if (c == CR) {
			_step = SECOND_CR_BODY;
			if (DEBUG) {
				std::cout << GREY << "Received body:\n";
				_print_body();
			}
			return ;
		}
		else
			_step = BODY;
	}
	if (_step == BODY) {
		if (c == CR)
			_step = CR_BODY;
		else
			_body.push_back(c);
	}
	// if (_step == HEADER_NAME)
	// 	_parse_field_name(c);
	// else if (_step == HEADER_VALUE)
	// 	_parse_field_value(c);
}

void RequestParser::_print_body(void) {
	std::vector<char>::iterator it, end = _body.end();

	if (DEBUG) {
		for(it = _body.begin(); it != end; ++it) {
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

// void RequestParser::break_data(char* buffer, size_t bytes_read) {
// 	for (size_t i = 0; i < bytes_read; ++i)
// 		_body.push_back(buffer[i]);

// 	if (DEBUG) {
// 		std::cout << GREY << "Received/remaining data:\n";
// 		_print_body();
// 	}
// }
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
		// ++it;
		if (it != _data_it && *it == '/') {
			++it;
			for (; it != _data_it; ++it)
				version.push_back(*it);
			log.debug(version);
		}
		_step = HEADER;
		_data.erase(_data.begin(), _data_it + 2);
		_result.insert(t_string_pair("method", method));
		_result.insert(t_string_pair("uri", uri));
		_result.insert(t_string_pair("protocol", protocol));
		_result.insert(t_string_pair("version", version));
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

// Field name:
// The requested field name. It MUST conform to the field-name syntax defined
// in Section 5.1, and it SHOULD be restricted to just letters, digits,
// and hyphen ('-') characters, with the first character being a letter.
void RequestParser::header(void) {
	std::vector<char>::iterator it;
	std::string header_name;
	std::string header_value;

	log.debug("parsing header");
	while (_found_EOL()) {
		log.debug("Found EOL");
		for (it = _data.begin(); it != _data_it && *it != COLON; ++it)
			header_name.push_back(*it);
		log.debug(header_name);
		if (*it == COLON)
			++it;
		while (*it == SP)
			++it;
		for (; it != _data_it; ++it)
			header_value.push_back(*it);
		log.debug(header_value);
		_result.insert(t_string_pair(header_name, header_value));
		it += 2;
		_data.erase(_data.begin(), it);
		header_name.clear();
		header_value.clear();
		if (*it == CR && *(it + 1) == LF) {
			log.info("end of headers - remaining data:");
			_print_data();
			_step = END;  ///////////////////////
		}  // double CRFL
	}
}

t_string_map	RequestParser::get_result(void) const {
	return _result;  // tem problema? não vou alterar, só consultar...
}

// general:
// Each part of a HTTP request is separated by a new line
// Note: Technically they should be \r\n but you are strongly encouraged to also accept \n as a newline
// Parts of request line is separated by a space character. Technically there should be only one space though I've seen badly malformed requests that send multiple spaces. Browsers will never send more than one space

// Even though header lines should end with CRLF, someone might use a single LF instead. Accept either CRLF or LF.
// The three fields in the initial message line should be separated by a single space, but might instead use several spaces, or tabs. Accept any number of spaces or tabs between these fields.

// headers
// one line per header, of the form "Header-Name: value", ending with CRLF
// you should handle LF correctly
// The header name is not case-sensitive (though the value may be) - Header
// name can be either title-case or lowercase or mixed, all are valid
// Any number of spaces or tabs may be between the ":" and the value
// Header lines beginning with space or tab are actually part of the previous header line, folded into multiple lines for easy reading.
// the following two headers are equivalent:

// Header1: some-long-value-1a, some-long-value-1b
// HEADER1:    some-long-value-1a,
//             some-long-value-1b

// HTTP 1.1 defines 46 headers, and one (Host:) is required in requests.

// If an HTTP message includes a body, there are usually header lines in the message that describe the body, eg (ver se são obrigatórios):
// Content-Type: header gives the MIME-type of the data in the body, such as text/html or image/gif.
// Content-Length: header gives the number of bytes in the body.

// A HTTP request is terminated by two newlines
// Note: Technically they should be 4 bytes: \r\n\r\n but you are strongly encouraged to also accept 2 byte terminator: \n\n.

// The most common use of POST, by far, is to submit HTML form data to CGI scripts. In this case, the Content-Type: header is usually application/x-www-form-urlencoded, and the Content-Length: header gives the length of the URL-encoded form data
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
// You can use GET to send small amounts of data to the server. The key is to understand just what the request URI is: It's not necessarily a file name, it's a string that identifies a data resource on the server. That may be a file name, but it may also be, for example, a specific query to a specific database. The result of that query may not live in a file, but it's a data resource all the same, identified by the search engine and the query data that together produce it.

// So, to send data to a CGI script using a GET request, include that data after the question mark in the URL (read about URL syntax for more details). For example,

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


// GET não pode ter body -> retornar erro

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
