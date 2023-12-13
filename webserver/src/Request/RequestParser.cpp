/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 21:21:48 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/12 18:16:13 by sguilher         ###   ########.fr       */
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
	_multipart_type = http::NONE;
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
	_multipart_type = http::NONE;
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
	}
	return *this;
}

RequestParser::~RequestParser(void) {
	_body.clear();
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
	Logger::debug("Checking request...");

	_print_headers();
	_check_host();
	_check_content_length();
	_check_transfer_encoding();
	_set_content_type();
	_headers.clear();

	_check_uri();
	_check_method();
	if (_request->method() == http::POST)
		_check_post_headers();
	Logger::debug("End checking request");
}
