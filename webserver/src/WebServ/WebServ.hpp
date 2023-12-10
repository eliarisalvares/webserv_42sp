#ifndef WEB_SERV_HPP
# define WEB_SERV_HPP

# include <iostream>
# include <exception>
# include <map>
# include <vector>
# include <fstream> // using files

// for server logic
# include <poll.h>
// # include <netdb.h>
# include <unistd.h> // close()

#include <arpa/inet.h>  // inet_ntop ->>>>>> remove, function not permited!!

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "RequestBuilder.hpp"
# include "ftstring.hpp"

# define PORT 8080

# define POLLNOEVENT 0x000

// server types
typedef std::map<int, Server*> t_server_map;
typedef std::map<int, Server*>::iterator t_server_iterator;

// request types
typedef std::map<int, RequestBuilder*> t_req_builder_map;
typedef std::map<int, RequestBuilder*>::iterator t_req_builder_iterator;

// other types
typedef std::vector<struct pollfd> t_pollfd_vector;
typedef std::vector<struct pollfd>::iterator t_pollfd_iterator;

class WebServ {
public:
	WebServ(void);
	~WebServ(void);

	void	create_servers(std::vector<std::string> input);
	void	init(void);
	void	run(void);
	void	stop(void);
	void	clean(void); //check if still needed
	void	restart_socket_servers(void);
	bool	checkPort(int port);
	class ServerPortAlreadySetted : public std::exception {
			virtual const char* what(void) const throw();
		};
	class PollErrorException : public std::exception {
			virtual const char* what(void) const throw();
		};

private:
	WebServ(WebServ const& copy);
	WebServ const& operator=(WebServ const& copy);

	t_server_map		_servers;
	void				_receive(int fd);
	void				_respond(Request* request);

	t_req_builder_map	_requestBuilderMap;
	RequestBuilder*		_get_request_builder(int fd);
	RequestBuilder*		_create_request_builder(int fd);
	bool				_is_ready_to_respond(int fd);

	int					_total_fds;
	t_pollfd_vector		_pfds;
	std::vector<int>	_serverSockets;
	std::map<int, int>	_fds_map;
	bool	_is_server_socket(int fd);
	void	_create_connection(int newfd);
	void	_end_connection(int fd);
};

std::ostream& operator<<(std::ostream& o, t_pollfd_vector const& _pfds);

#endif
