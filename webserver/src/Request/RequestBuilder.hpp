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

	bool		read(void);
	void		parse(void);
	Request*	build(void);

	bool		is_ready(void) const;

private:
	RequestBuilder(void);
	Logger				log;

	int					_fd;
	bool				_ready;
	Server*				_server; // talvez não seja necessário
	Request*			_request;

	// parse data
	size_t				_bytes_readed;
	char*				_buffer;
	RequestParser		_parser;
	void				_setRequestError(http::InvalidRequest& e);
	void				_setRequestError(std::exception& e);
};

#endif
