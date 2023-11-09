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
# include <vector>
# include <cstring> // memset
# include <unistd.h>

# include "Logger.hpp"
# include "Server.hpp"
# include "Request.hpp"

class RequestBuilder {
public:
	RequestBuilder(Server& server, int connection);
	RequestBuilder(int connection);
	RequestBuilder(RequestBuilder const& copy);
	RequestBuilder& operator=(RequestBuilder const& copy);
	~RequestBuilder(void);

	bool addRequestData(void);
	Request* build(void);  // params: Server, fd da conexão

	Server			getServer(void) const;
	bool			is_ready(void) const;

private:
	RequestBuilder(void);

	void parse(void);

	Server*						_server; // RequestBuilder shouldn't edit Server
	int							_fd;
	bool						_ready;
	std::vector<std::string>	_requestData;
};

#endif
