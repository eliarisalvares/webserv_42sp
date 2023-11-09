#ifndef WEB_SERV_HPP
# define WEB_SERV_HPP

# include <iostream>
# include <map>
# include <vector>

# include <cstring> // memset

// for server logic
# include <poll.h>
// # include <netdb.h>
# include <unistd.h> // close()

#include <arpa/inet.h>  // inet_ntop ->>>>>> remove, function not permited!!

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "RequestBuilder.hpp"

# define PORT 8080

// server types
typedef std::map<int, Server*> t_server_map;
typedef std::map<int, Server*>::iterator t_server_iterator;

// request types
typedef std::map<int, Request*> t_request_map;
typedef std::map<int, RequestBuilder*> t_req_builder_map;

// response types
typedef std::map<int, Response*> t_response_map;

// other types
typedef std::vector<struct pollfd> t_pollfd_vector;

class WebServ {
public:
	WebServ(void);
	~WebServ(void);

	void	createServers(void);
	void	init(void);
	void	run(void);

private:
	// we want only one instance of WebServ, without copy
	WebServ(WebServ const& copy);
	WebServ const& operator=(WebServ const& copy);

	t_server_map		_servers;
	t_request_map		_requests;
	t_response_map		_responses;
	t_req_builder_map	_requestBuilderMap;

	// dealing with sockets fds
	t_pollfd_vector		_pfds;
	std::vector<int>	_serverSockets;
	bool	is_server_socket(int fd);
	void	add_connection_socket_to_pfds(int newfd);
};

#endif
