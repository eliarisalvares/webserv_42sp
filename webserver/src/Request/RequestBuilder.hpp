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

# include "Logger.hpp"
# include "Server.hpp"
# include "Request.hpp"

class RequestBuilder {
public:
	// Request(Server const& server); // provavelmente recebe outros dados: socket, conteúdo
	RequestBuilder(void);
	~RequestBuilder(void);
	RequestBuilder(RequestBuilder const& copy);
	RequestBuilder const& operator=(RequestBuilder const& copy);

	void addRequestData(std::string const newData);
	Request* build(void);  // params: Server, fd da conexão

private:

	void parse(void);
	// Server			getServer(void) const;

	// Server const& _server;
	int							_fd;
	std::vector<std::string>	_requestData;
};

#endif
