/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:26:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/30 02:16:29 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Request Class
*/

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>

# include "Server.hpp"
# include "http.hpp"

typedef enum requestMethod {
	GET,
	POST,
	DELETE,
	NOT_ALLOWED, // 405
	NOT_IMPLEMENTED, // 501
	INVALID_METHOD, // 400
}			requestMethod;

class Request {
public:
	Request(int fd, Server* server);
	Request(void);
	~Request(void);
	Request(Request const& copy);
	Request const& operator=(Request const& copy);

	// getters
	Server*					server(void) const;
	requestMethod			method(void) const;
	int						fd(void) const;
	http::e_status			status_code(void) const;
	std::string				uri(void) const;
	bool					has_error(void) const;
	bool					is_chuncked(void) const;
	http::e_content_type	content_type(void) const;
	size_t					content_length(void) const;

	// setters
	void					setMethod(requestMethod method);
	void					setStatusCode(http::e_status status);
	void					setUri(std::string const uri);
	void					setError(bool has_error);
	void					setChuncked(bool is_chuncked);
	void					setContentType(http::e_content_type type);
	void					setContentLength(size_t length);

private:

	Server*					_server;
	int						_fd;
	bool					_error;
	requestMethod			_method;
	http::e_status			_status_code;
	std::string				_uri;
	bool					_is_chuncked;

	// headers data
	std::string				_host;
	http::e_content_type	_content_type;
	size_t					_content_length;

};

#endif
