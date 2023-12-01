/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 16:43:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/30 21:02:48 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

RequestParser::RequestParser(void):
	_idx(0), _step(INIT) {
	_request = new Request();
	_data.clear();
	// _result.clear();
}

RequestParser::RequestParser(Request* request):
	_idx(0), _step(INIT), _request(request) {
	_data.clear();
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
	_data.clear();
	// _result.clear();
}

// void RequestParser::break_data(char* buffer, size_t bytes_read) {
// 	for (size_t i = 0; i < bytes_read; ++i)
// 		_data.push_back(buffer[i]);

// 	if (DEBUG) {
// 		std::cout << GREY << "Received/remaining data:\n";
// 		_print_data();
// 	}
// }

// void RequestParser::_print_data(void) {
// 	std::vector<char>::iterator it, end = _data.end();

// 	if (DEBUG) {
// 		for(it = _data.begin(); it != end; ++it) {
// 			if (*it == CR)
// 				std::cout << " CR";
// 			else if (*it == LF)
// 				std::cout << "LF" << *it;
// 			else
// 				std::cout << *it;
// 		}
// 		std::cout << RESET << std::endl;
// 	}
// }

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
		if (_method.size() == 0) {
			_step = ERROR; // acho que vai poder tirar isso
			throw http::InvalidRequest(http::BAD_REQUEST);
		}
		_request->setMethod(GET); // ver se vou fazer isso aqui e lógica
		_step = URI;
		log.debug(_method);
	}
	else {
		_step = ERROR; // acho que vai poder tirar isso
		throw http::InvalidRequest(http::BAD_REQUEST);
	}
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
	else {
		_step = ERROR; // acho que vai poder tirar isso
		throw http::InvalidRequest(http::BAD_REQUEST);
	}
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
		_step = VERSION;
		log.debug(_protocol);
	} // checkar _protocol == "HTTP"
	else {
		_step = ERROR; // acho que vai poder tirar isso
		throw http::InvalidRequest(http::BAD_REQUEST);
	}
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
	else {
		_step = ERROR; // acho que vai poder tirar isso
		throw http::InvalidRequest(http::BAD_REQUEST);
	}
}

// Each part of a HTTP request is separated by a new line
// Note: Technically they should be \r\n but you are strongly encouraged to also accept \n as a newline
// Initial lines and headers should end in CRLF, though you should gracefully handle lines ending in just LF.
// Even though header lines should end with CRLF, someone might use a single LF instead. Accept either CRLF or LF.
// lidando apenas com CRLF por hora, verificar no RFC
void RequestParser::check_crlf(char c) {
	if (c != LF) {
		_step = ERROR; // acho que vai poder tirar isso
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
			_step = END; //
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
		// salvar o valor
		_field_name.clear();
	}
}

// Any number of spaces or tabs may be between the ":" and the value
void RequestParser::_parse_field_value(char c) {
	if (c == SP)
		return ;
	if (c == CR) {
		log.debug(_field_value);
		_step = CR_HEADER;
		// salvar o valor
		_field_value.clear();
	}
	else
		_field_value.push_back(c);
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

// The most common use of POST, by far, is to submit HTML form data to CGI scripts. In this case, the Content-Type: header is usually application/x-www-form-urlencoded, and the Content-Length: header gives the length of the URL-encoded form data
// Ex:
// POST /path/script.cgi HTTP/1.0
// From: frog@jmarshall.com
// User-Agent: HTTPTool/1.0
// Content-Type: application/x-www-form-urlencoded
// Content-Length: 32

// home=Cosby&favorite+flavor=flies

// You can use a POST request to send whatever data you want, not just form submissions. Just make sure the sender and the receiving program agree on the format.


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
