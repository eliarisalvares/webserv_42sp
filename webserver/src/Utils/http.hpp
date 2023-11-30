/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 21:13:58 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/30 10:33:38 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

# include <set>
# include <string>

namespace http {
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

	// all methods in HTTP
	std::set<std::string>				_fill_methods(void);
	static std::set<std::string> const	methods = http::_fill_methods();

	// headers that needs a validation - não sei se isso vai ser útil
	std::set<std::string>				_fill_headers(void);
	static std::set<std::string> const	headers = http::_fill_headers();
}

#endif


