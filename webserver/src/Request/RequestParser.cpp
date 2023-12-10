/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 21:21:48 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/10 14:56:13 by sguilher         ###   ########.fr       */
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
	_has_content_type = false;
	_boundary_size = 0;
	_media_type = http::NONE;
	_multipart_step = INITIAL_BOUNDARY;
	_multipart_content_crfl = false;
}

RequestParser::RequestParser(Request* request): _step(INIT), _request(request) {
	_has_content_length = false;
	_is_chunked = false;
	_content_length = 0;
	_max_body_size = size_t(request->server()->getBodySize());
	_body_bytes_readed = 0;
	_chunk_size = 0;
	_chunk_bytes_readed = 0;
	_has_content_type = false;
	_boundary_size = 0;
	_media_type = http::NONE;
	_multipart_step = INITIAL_BOUNDARY;
	_multipart_content_crfl = false;
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
		_has_content_type = copy.has_content_type();
		_content_type = copy.getContentType();
		_media_type_str = copy.getMediaTypeStr();
		_boundary = copy.getBoundary();
		_boundary_size = _boundary.size();
		_media_type = copy.getMediaType();
		_multipart_step = copy.getMultipartStep();
		// copy _multipart_tmp
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
	_has_content_type = false;
	_content_type.clear();
	_media_type_str.clear();
	_boundary.clear();
	_boundary_size = 0;
	_media_type = http::NONE;
	_multipart_step = INITIAL_BOUNDARY;
	_multipart_tmp.clear();
	_multipart_content_crfl = false;

	_step = METHOD;
	method(c);
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

void RequestParser::check_request(void) {
	Logger::debug("Checking request");
	// first check headers and get necessary values
	_print_headers();
	_check_host();
	_check_content_length();
	_check_transfer_encoding();
	_set_content_type();
	_headers.clear();

	// check if the path exists and set it; get location
	_check_uri();
	// verify if method is allowed in specified location:
	_check_method();
	if (_request->method() == http::POST)
		_check_post_headers();
	Logger::debug("End checking request");
}


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
