/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:26:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/12/12 16:59:52 by sguilher         ###   ########.fr       */
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

	/* getters */
	Server*					server(void) const;
	http::RequestMethod		method(void) const;
	int						fd(void) const;
	http::HttpStatus		status_code(void) const;
	std::string				path(void) const;
	std::string				uri(void) const;
	std::string				host(void) const;
	t_location*				location(void) const;
	bool					has_error(void) const;
	size_t					content_length(void) const;
	http::MediaType			media_type(void) const;
	std::string				text(void) const;
	bool					has_image(void) const;
	std::vector<char>*		image(void) const;
	std::string				image_type(void) const;
	std::map<std::string, std::string> post_data(void) const;

	/* setters */
	void					setMethod(http::RequestMethod method);
	void					setStatusCode(http::HttpStatus status);
	void					setPath(std::string const uri);
	void					setUri(std::string const uri);
	void					setLocation(t_location* location);
	void					setHost(std::string const host);
	void					setError(bool has_error);
	void					setContentLength(size_t length);
	void					setMediaType(http::MediaType type);
	void					setText(std::string const& text);
	void					setHasImage(bool has_image);
	void					setImageType(std::string const& type);
	void					setImage(std::vector<char>* image);
	void					addPostData(
								std::string const& name, std::string const& value
							);

	void					printPostData(void) const;

private:

	Server*					_server;
	int						_fd;
	bool					_error;
	http::RequestMethod		_method;
	http::HttpStatus		_status_code;
	std::string				_path;
	std::string				_uri;
	t_location*				_location;

	/* headers data */
	std::string				_host;
	size_t					_content_length;

	http::MediaType			_media_type;
	/* image */
	bool					_has_image;
	std::string				_image_type;
	std::string				_image_file_name;
	std::vector<char>*		_image;
	/* form */
	std::map<std::string, std::string> _post_data;
	/* text */
	std::string				_text;
};

#endif
