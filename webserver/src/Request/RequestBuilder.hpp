/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 22:16:26 by sguilher          #+#    #+#             */
/*   Updated: 2023/11/06 22:30:45 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILD_REQUEST_HPP
# define BUILD_REQUEST_HPP

# include <string>
# include <cstring> // memset
# include <unistd.h>

# include "Logger.hpp"
# include "Server.hpp"
# include "Request.hpp"
# include "RequestParser.hpp"

class RequestBuilder {
public:
	RequestBuilder(Server* server, int connection);
	RequestBuilder(RequestBuilder const& copy);
	RequestBuilder& operator=(RequestBuilder const& copy);
	~RequestBuilder(void);

	typedef enum e_builder_error {
		NONE,
		PARSE_ERROR,
		METHOD_NOT_ALLOWED,
	}           t_builder_error;

	bool		read(void);
	void		parse(void);
	Request*	build(void);

	Server*			getServer(void) const;
	bool			is_ready(void) const;
	e_builder_error error(void);

private:
	RequestBuilder(void);

	int					_fd;
	bool				_ready;
	Server*				_server;
	Request*			_request;
	e_builder_error		_error;
	std::string			_error_str;

	// parse data
	size_t				_bytes_readed;
	char*				_buffer;
	RequestParser		_parser;
};

#endif
