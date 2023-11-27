/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:26:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/27 12:54:10 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Request Class
*/

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>

# include "Server.hpp"

typedef enum requestMethod {
	GET,
	POST,
	DELETE,
	NOT_IMPLEMENTED
}			requestMethod;

class Request {
public:
	// Request(Server const& server); // provavelmente recebe outros dados: socket, conteúdo
	Request(int fd, Server* server);
	Request(void);
	~Request(void);
	Request(Request const& copy);
	Request const& operator=(Request const& copy);

	// Server			getServer(void) const;
	requestMethod	getMethod(void) const;
	int				fd(void) const;

private:

	Server*			_server;
	int				_fd;
	requestMethod	_method;

};

#endif
