/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:14:50 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/04 22:09:09 by sguilher         ###   ########.fr       */
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

// headers that needs a validation / singletons
std::set<std::string> _fill_content_types(void) {
	std::string _content_types[] = {
		"text/html",
		"application/x-www-form-urlencoded",
		"application/json",
	};
	std::set<std::string> content_types (_content_types, _content_types + 2);
	return content_types;
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
    case METHOD_NOT_ALLOWED:
      return "405 Method not allowed";
    case LENGTH_REQUIRED:
      return "411 Length required";
    case URI_TOO_LONG:
      return "414 Request URI too long";
    case CONTENT_TOO_LARGE:
      return "501 Content Too Large";
    case HTTP_VERSION_NOT_SUPPORTED:
      return "505 HTTP version not supported";
    default:
      return "Unknown error";
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

