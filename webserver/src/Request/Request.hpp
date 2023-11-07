/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 19:26:19 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/06 22:26:50 by sguilher         ###   ########.fr       */
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
	Request(void);
	~Request(void);
	Request(Request const& copy);
	Request const& operator=(Request const& copy);

	// Server			getServer(void) const;
	requestMethod	getMethod(void) const;

private:

	// Server const& _server;
	int				_fd;
	requestMethod	_method;

};

#endif
