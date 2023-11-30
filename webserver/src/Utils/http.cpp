/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:14:50 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/30 11:15:12 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.hpp"


namespace http {

std::set<std::string> _fill_methods(void) {
	std::string methods_names[] = {
		"GET",
		"POST",
		"PUT",
		"PATCH",
		"DELETE",
		"HEAD",
		"CONNECT",
		"OPTIONS",
		"TRACE",
	};
	std::set<std::string> methods (methods_names, methods_names + 9);
	return methods;
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
