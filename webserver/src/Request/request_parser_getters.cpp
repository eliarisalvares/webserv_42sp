/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser_getters.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/07 09:20:25 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/09 16:56:04 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "RequestParser.hpp"

RequestParser::Step RequestParser::step(void) const {
	return this->_step;
}

Request* RequestParser::getRequest(void) const {
	return this->_request;
}

std::string RequestParser::getMethod(void) const {
	return this->_method;
}

std::string RequestParser::getUri(void) const {
	return this->_uri;
}

std::string RequestParser::getProtocol(void) const {
	return this->_protocol;
}

std::string RequestParser::getVersion(void) const {
	return this->_version;
}

std::string RequestParser::getFieldName(void) const {
	return this->_field_name;
}

std::string RequestParser::getFieldValue(void) const {
	return this->_field_value;
}

std::string RequestParser::getLastHeader(void) const {
	return this->_last_header;
}

std::string RequestParser::getContentType(void) const {
	return this->_content_type;
}

std::string RequestParser::getMediaTypeStr(void) const {
	return this->_media_type_str;
}

std::string RequestParser::getBoundary(void) const {
	return this->_boundary;
}

http::ContentType RequestParser::getMediaType(void) const {
	return this->_media_type;
}

t_header_map RequestParser::getHeaders(void) const {
	return this->_headers;
}

bool RequestParser::has_content_length(void) const {
	return this->_has_content_length;
}

bool RequestParser::has_content_type(void) const {
	return this->_has_content_type;
}

bool RequestParser::is_chunked(void) const {
	return this->_is_chunked;
}

size_t RequestParser::content_length(void) const {
	return this->_content_length;
}

size_t RequestParser::max_body_size(void) const {
	return this->_max_body_size;
}

size_t RequestParser::body_bytes_readed(void) const {
	return this->_body_bytes_readed;
}

size_t RequestParser::chunk_bytes_readed(void) const {
	return this->_chunk_bytes_readed;
}

size_t RequestParser::chunk_size(void) const {
	return this->_chunk_size;
}

std::string RequestParser::getChunkSizeStr(void) const {
	return this->_chunk_size_str;
}

std::vector<char> RequestParser::getBody(void) const {
	return this->_body;
}

