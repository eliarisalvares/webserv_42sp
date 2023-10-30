/*
Request Class
*/

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>

# include "Server.hpp"

class Request {
public:
	Request(Server const& server); // provavelmente recebe outros dados: socket, conteúdo
	~Request(void);
	Request(Request const& copy);
	Request const& operator=(Request const& copy);

	Server	getServer(void) const;

private:
	Request(void);

	Server const& server;

};

#endif