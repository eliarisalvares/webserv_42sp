/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:14:50 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 11:59:41 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.hpp"


namespace http {

std::set<std::string> _fill_methods(void) {
	std::string methods_names[] = {
		"GET",
		"POST",
		"DELETE",
		// "PUT",
		// "PATCH",
		// "HEAD",
		// "CONNECT",
		// "OPTIONS",
		// "TRACE",
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

// headers that needs a validation - não sei se isso vai ser útil
std::set<std::string> _fill_headers(void) {
	std::string headers_names[] = {
		"Host", // obrigatório, se não tiver retorna 400 - Bad Request
		"Content-Length",  // número
		"Content-Type",  // ?
	};
	std::set<std::string> headers (headers_names, headers_names + 3);
	return headers;
}

}

const char* http::InvalidErrorCode::what() const throw() {
	return "InvalidRequest: invalid error status code";
}

http::InvalidRequest::InvalidRequest(http::HttpStatus error) {
	if (error < http::BAD_REQUEST)
		throw http::InvalidErrorCode();
	_error = error;
}

const char* http::InvalidRequest::what() const throw() {
	switch (_error) {
    case http::BAD_REQUEST:
      return "400 Bad request";
    case http::METHOD_NOT_ALLOWED:
      return "405 Method not allowed";
    case http::LENGTH_REQUIRED:
      return "411 Length required";
    case http::URI_TOO_LONG:
      return "414 Request URI too long";
    case http::HTTP_VERSION_NOT_SUPPORTED:
      return "505 HTTP version not supported";
    default:
      return "Unknown error";
  }
}

http::HttpStatus http::InvalidRequest::get_error_code(void) const {
	return _error;
}
