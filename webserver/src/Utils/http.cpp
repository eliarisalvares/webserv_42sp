/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:14:50 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/09 15:47:49 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.hpp"


namespace http {

// methods implemented in webserv
std::set<std::string> _fill_methods(void) {
	std::string methods_names[] = {
		"GET",
		"POST",
		"DELETE",
	};
	std::set<std::string> methods (methods_names, methods_names + 3);
	return methods;
}

std::map<std::string, RequestMethod> _fill_map_method_str_to_enum(void) {
	std::map<std::string, RequestMethod> str_to_enum;

	str_to_enum.insert(std::make_pair("GET", GET));
	str_to_enum.insert(std::make_pair("POST", POST));
	str_to_enum.insert(std::make_pair("DELETE", DELETE));
	return str_to_enum;
}

std::map<RequestMethod, std::string> _fill_map_method_enum_to_str(void) {
	std::map<RequestMethod, std::string> enum_to_str;

	enum_to_str.insert(std::make_pair(GET, "GET"));
	enum_to_str.insert(std::make_pair(POST, "POST"));
	enum_to_str.insert(std::make_pair(DELETE, "DELETE"));
	return enum_to_str;
}

RequestMethod str_to_enum_method(std::string method) {
	std::map<std::string, RequestMethod>::const_iterator it;

	it = map_str_to_enum_method.find(method);
	if (it == map_str_to_enum_method.end())
		throw utils::GeneralException(utils::INVALID_METHOD);
	return it->second;
}

std::string enum_to_str_method(RequestMethod method) {
	std::map<RequestMethod, std::string>::const_iterator it;

	it = map_enum_to_str_method.find(method);
	if (it == map_enum_to_str_method.end())
		throw utils::GeneralException(utils::INVALID_METHOD);
	return it->second;
}

std::set<std::string> _fill_media_types(void) {
	std::string _media_types[] = {
		"text/plain",
		"text/html",
		"text/css",
		"application/json",
		"application/javascript",
		"application/xml",
		"image/png",
		"image/jpeg",
		"image/gif",
		"application/x-www-form-urlencoded",
		"multipart/form-data"
	};
	std::set<std::string> media_types (_media_types, _media_types + 11);
	return media_types;
}

std::map<std::string, ContentType> _fill_map_media_type_str_to_enum(void) {
	std::map<std::string, ContentType> str_to_enum;

	str_to_enum.insert(std::make_pair("text/plain", TEXT_PLAIN));
	str_to_enum.insert(std::make_pair("image/png", IMAGE_PNG));
	str_to_enum.insert(std::make_pair("image/jpeg", IMAGE_JPEG));
	str_to_enum.insert(std::make_pair("image/gif", IMAGE_GIF));
	str_to_enum.insert(std::make_pair("application/x-www-form-urlencoded", FORM_URLENCODED));
	str_to_enum.insert(std::make_pair("multipart/form-data", MULTIPART_FORM_DATA));
	return str_to_enum;
}

std::map<ContentType, std::string> _fill_map_media_type_enum_to_str(void) {
	std::map<ContentType, std::string> enum_to_str;

	enum_to_str.insert(std::make_pair(TEXT_PLAIN, "text/plain"));
	enum_to_str.insert(std::make_pair(IMAGE_PNG, "image/png"));
	enum_to_str.insert(std::make_pair(IMAGE_JPEG, "image/jpeg"));
	enum_to_str.insert(std::make_pair(IMAGE_GIF, "image/gif"));
	enum_to_str.insert(std::make_pair(FORM_URLENCODED, "application/x-www-form-urlencoded"));
	enum_to_str.insert(std::make_pair(MULTIPART_FORM_DATA, "multipart/form-data"));
	return enum_to_str;
}

ContentType str_to_enum_media_type(std::string media_type) {
	std::map<std::string, ContentType>::const_iterator it;

	it = map_str_to_enum_media_type.find(media_type);
	if (it == map_str_to_enum_media_type.end()) {
		Logger::warning("request parser error: invalid media type", media_type);
		throw InvalidRequest(UNSUPPORTED_MEDIA_TYPE);
	}
	return it->second;
}

std::string enum_to_str_media_type(ContentType media_type) {
	std::map<ContentType, std::string>::const_iterator it;

	it = map_enum_to_str_media_type.find(media_type);
	if (it == map_enum_to_str_media_type.end())
		throw utils::GeneralException(utils::UNSUPPORTED_MEDIA_TYPE);
	return it->second;
}

bool is_uri_char(char c) {
	if (std::isalnum(c)) // unreserved characters
		return true;
	switch (c) {
		// unreserved characters
		case '-':
		case '.':
		case '_':
		case '~':
			return true;
		// reserved characters - general delimiters
		case ':':
		case '/':
		case '?':
		case '#':
		case '[':
		case ']':
		case '@':
			return true;
		// reserved characters - sub delimiters
		case '!':
		case '$':
		case '&':
		case '\'':
		case '(':
		case ')':
		case '*':
		case '+':
		case ',':
		case ';':
		case '=':
			return true;
		// conversion of special characters
		case '%':
			return true;
	default:
		return false;
	}
}

bool uri_path_end(char c) {
	return (c == '?' || c == '#');
}

bool is_path_to_file(std::string const& path) {
	return (path.find('.') != std::string::npos);
}

const char* InvalidErrorCode::what() const throw() {
	return "InvalidRequest: invalid error status code";
}

InvalidRequest::InvalidRequest(HttpStatus error) {
	if (error < BAD_REQUEST)
		throw InvalidErrorCode();
	_error = error;
}

const char* InvalidRequest::what() const throw() {
	switch (_error) {
		case BAD_REQUEST:
			return "400 Bad request";
		case UNAUTHORIZED:
			return "401 Unauthorized";
		case FORBIDDEN:
			return "403 Forbidden";
		case NOT_FOUND:
			return "404 Not found";
		case METHOD_NOT_ALLOWED:
			return "405 Method not allowed";
		case NOT_ACCEPTABLE:
			return "406 Not Acceptable";
		case REQUEST_TIMEOUT:
			return "408 Request Timeout";
		case CONFLICT:
			return "409 Conflict";
		case LENGTH_REQUIRED:
			return "411 Length required";
		case CONTENT_TOO_LARGE:
			return "413 Content Too Large";
		case URI_TOO_LONG:
			return "414 URI too long";
		case UNSUPPORTED_MEDIA_TYPE:
			return "415 Unsupported Media Type";
		case UNPROCESSABLE_CONTENT:
			return "422 Unprocessable Content";
		case INTERNAL_SERVER_ERROR:
			return "500 Internal Server Error";
		case NOT_IMPLEMENTED:
			return "501 Not Implemented";
		case HTTP_VERSION_NOT_SUPPORTED:
			return "505 HTTP Version Not Supported";
		default:
			return (
				std::string("Unknown error code: ")
				+ ftstring::itostr(int(_error))
			).c_str();
  }
}

HttpStatus InvalidRequest::get_error_code(void) const {
	return _error;
}

void bad_request(std::string const description) {
	Logger::warning(description);
	throw InvalidRequest(BAD_REQUEST);
}

} // end namespace http

