/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:13:58 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/07 19:42:17 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

# include <map>
# include <set>
# include <string>
# include <iostream>
# include <exception>

# include "utils.hpp"
# include "Logger.hpp"

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
		LENGTH_REQUIRED = 411,
		CONTENT_TOO_LARGE = 413,
		URI_TOO_LONG = 414,
		UNSUPPORTED_MEDIA_TYPE = 415,
		// 422 Unprocessable Content - erro de código em python (cgi gera automático?)
		INTERNAL_SERVER_ERROR = 500,
		NOT_IMPLEMENTED = 501,
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

	// headers that needs a validation / singletons
	std::set<std::string>				_fill_content_types(void);
	static std::set<std::string> const	special_headers = http::_fill_content_types();

	bool is_uri_char(char c);
	bool uri_path_end(char c);

	// http exceptions
	class InvalidRequest;
	class InvalidErrorCode;

	void	bad_request(std::string const description);
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


