#ifndef WEB_SERV_HPP
# define WEB_SERV_HPP

# include <iostream>
# include <exception>
# include <map>
# include <vector>

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
typedef std::map<int, RequestBuilder*>::iterator t_req_builder_iterator;

// response types
typedef std::map<int, Response*> t_response_map;

// other types
typedef std::vector<struct pollfd> t_pollfd_vector;

class WebServ {
public:
	WebServ(void);
	~WebServ(void);

	void	create_servers(void);
	void	init(void);
	void	run(void);
	void	clean(void);
	void	restart_socket_servers(void);

private:
	// we want only one instance of WebServ, without copy
	WebServ(WebServ const& copy);
	WebServ const& operator=(WebServ const& copy);

	t_server_map		_servers;
	t_request_map		_requests;
	t_response_map		_responses;
	t_req_builder_map	_requestBuilderMap;
	void				_create_request_builder(int fd);
	bool				_request_builder_exists(int fd);
	void				_respond(Request* request);

	// dealing with sockets fds
	int					_total_fds;
	t_pollfd_vector		_pfds;
	std::vector<int>	_serverSockets;
	std::map<int, int>	_fds_map;  // connection between fds and server
	bool	_is_server_socket(int fd);
	void	_create_connection(int newfd);
};

std::ostream& operator<<(std::ostream& o, t_pollfd_vector const& _pfds);

#endif
