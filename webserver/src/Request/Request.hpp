/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:26:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/09 23:27:43 by sguilher         ###   ########.fr       */
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
	http::RequestMethod		method(void) const;
	int						fd(void) const;
	http::HttpStatus		status_code(void) const;
	std::string				path(void) const;
	std::string				uri(void) const;
	std::string				host(void) const;
	t_location*				location(void) const;
	bool					has_error(void) const;
	bool					is_chuncked(void) const;
	// http::MediaType		content_type(void) const;
	size_t					content_length(void) const;
	std::vector<char>*		getImageData(void) const;
	std::string				getImageType(void) const;

	// setters
	void					setMethod(http::RequestMethod method);
	void					setStatusCode(http::HttpStatus status);
	void					setPath(std::string const uri);
	void					setUri(std::string const uri);
	void					setLocation(t_location* location);
	void					setHost(std::string const host);
	void					setError(bool has_error);
	void					setChuncked(bool is_chuncked);
	// void					setContentType(http::MediaType type);
	void					setContentLength(size_t length);
	void					setImageType(std::string const& type);
	void					setImage(std::vector<char>* image);

private:

	Server*					_server;
	int						_fd;
	bool					_error;
	http::RequestMethod		_method;
	http::HttpStatus		_status_code;
	std::string				_path;
	std::string				_uri;
	t_location*				_location;
	std::string				_image_type;
	std::vector<char>*		_image;

	// headers data
	std::string				_host;
	// http::MediaType			_media_type;
	size_t					_content_length;
	bool					_is_chuncked;

};

#endif
