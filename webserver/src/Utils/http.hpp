/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:13:58 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 11:51:25 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

# include <map>
# include <set>
# include <string>
# include <exception>

# include "utils.hpp"

namespace http {
	enum RequestMethod {
		GET,
		POST,
		DELETE,
	};

	enum HttpStatus {
		CONTINUE = 100,
		OK = 200,
		CREATED = 201,
		ACCEPTED = 202,
		NO_CONTENT = 204,
		MOVED_PERMANENTLY = 301,
		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		METHOD_NOT_ALLOWED = 405,
		NOT_ACCEPTABLE = 406,
		REQUEST_TIMEOUT = 408,
		CONFLICT = 409,  // checar se vamos usar
		LENGTH_REQUIRED = 411,  // checar se vamos usar
		CONTENT_TOO_LARGE = 413,
		URI_TOO_LONG = 414,
		UNSUPPORTED_MEDIA_TYPE = 415,
		// 422 Unprocessable Content - erro de código em python (cgi gera automático?)
		INTERNAL_SERVER_ERROR = 500,
		HTTP_VERSION_NOT_SUPPORTED = 505,
	};

	enum ContentType {

	};

	// http methods in webserv
	std::set<std::string>				_fill_methods(void);
	static std::set<std::string> const	methods = http::_fill_methods();
	std::map<std::string, RequestMethod>				_fill_map_method_str_to_enum(void);
	static std::map<std::string, RequestMethod> const	map_str_to_enum_method = http::_fill_map_method_str_to_enum();
	std::map<RequestMethod, std::string>				_fill_map_method_enum_to_str(void);
	static std::map<RequestMethod, std::string> const	map_enum_to_str_method = http::_fill_map_method_enum_to_str();
	// makes transformation easier
	RequestMethod	str_to_enum_method(std::string method);
	std::string		enum_to_str_method(RequestMethod method);

	// headers that needs a validation - não sei se isso vai ser útil
	std::set<std::string>				_fill_headers(void);
	static std::set<std::string> const	headers = http::_fill_headers();

	class InvalidRequest;
	class InvalidErrorCode;
}

class http::InvalidRequest: public std::exception {
	http::HttpStatus _error;

	public:
		InvalidRequest(http::HttpStatus error);
		const char* what() const throw();
		http::HttpStatus get_error_code(void) const;
};

class http::InvalidErrorCode: public std::exception {
	public:
		const char* what() const throw();
};

#endif


