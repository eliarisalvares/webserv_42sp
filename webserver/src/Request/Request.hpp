/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:26:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/02 12:06:09 by feralves         ###   ########.fr       */
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

class Request {
public:
	Request(int fd, Server* server);
	Request(void);
	~Request(void);
	Request(Request const& copy);
	Request const& operator=(Request const& copy);

	// getters
	Server*					server(void) const;
	http::RequestMethod			method(void) const;
	int						fd(void) const;
	http::HttpStatus			status_code(void) const;
	std::string				uri(void) const;
	bool					has_error(void) const;
	bool					is_chuncked(void) const;
	http::ContentType		content_type(void) const;
	size_t					content_length(void) const;

	// setters
	void					setMethod(http::RequestMethod method);
	void					setStatusCode(http::HttpStatus status);
	void					setUri(std::string const uri);
	void					setError(bool has_error);
	void					setChuncked(bool is_chuncked);
	void					setContentType(http::ContentType type);
	void					setContentLength(size_t length);

private:

	Server*					_server;
	int						_fd;
	bool					_error;
	http::RequestMethod		_method;
	http::HttpStatus		_status_code;
	std::string				_uri;
	bool					_is_chuncked;

	// headers data
	std::string				_host;
	http::ContentType		_content_type;
	size_t					_content_length;

};

#endif
