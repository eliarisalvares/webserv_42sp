#include "WebServ.hpp"

WebServ::WebServ(void): _total_fds(0) {
	this->_servers.clear();
	this->_pfds.clear();
	this->_fds_map.clear();
	this->_serverSockets.clear();
	this->_requestBuilderMap.clear();
}

WebServ::~WebServ(void) { }

WebServ::WebServ(WebServ const& copy) {
	*this = copy;
}

WebServ const& WebServ::operator=(WebServ const& copy) {
	if (this != &copy) {
		(void)copy;
	}
	return *this;
}

bool	WebServ::checkPort(int port) {
	for (t_server_iterator it = _servers.begin(); it!= _servers.end(); ++it) {
		if (it->second->getPort() == port)
			return false;
	}
	return true;
}

void	WebServ::create_servers(std::vector<std::string> input) {
	int	brackets = 0;
	size_t	i;

	for (size_t index = 0; index < input.size(); index++) {
		Server *oneServer = new Server(input, index);

		if (input[index] == input.back())
			break ;
		if (input[index].substr(0, 8) == "server {") {
			for (i = index; i < input.size(); i++) {
				if (input[i].substr(0, 9) == "location ")
					brackets++;
				if (input[i].substr() == "}" && brackets == 0)
					break ;
				else if (input[i].substr() == "}")
					brackets--;
			}
			index = i;
		}
		if (!checkPort(oneServer->getPort()))
			throw ServerPortAlreadySetted();
		Logger::info("Server initialized on port", oneServer->getPort());
		this->_servers.insert(std::pair<int, Server*>(oneServer->getSocket(), oneServer));
	}
}

void	WebServ::init(void) {
	pollfd init;
	t_server_iterator it, end = this->_servers.end();

	for (it = this->_servers.begin(); it != end; ++it) {
		init.fd = it->first;
		init.events = POLLIN;
		init.revents = POLLNOEVENT;
		this->_pfds.push_back(init);
		this->_serverSockets.push_back(init.fd);
		this->_total_fds++;
	}
}

void	WebServ::run(void) {
	int poll_count, fd;
	clock_t	current;

	Logger::info("Running webserv...");
	while(true) {
		poll_count = poll(this->_pfds.data(), this->_pfds.size(), -1);

		if (poll_count == -1) {
			perror("poll");
			throw PollErrorException();
		}
		for (int i = 0; i < this->_total_fds; i++) {
			fd = this->_pfds[i].fd;

			if (this->_pfds[i].revents & POLLIN) {
				Logger::debug("POLLIN event");
				if (_is_server_socket(fd)) {
					Logger::debug("Creating connection...");
					_create_connection(fd);
				} else {
					_receive(fd);
				}
			}
			if (this->_pfds[i].revents & POLLOUT) {
				if (_is_ready_to_respond(fd)) {
					Logger::debug("POLLOUT event and request parsing ended");
					RequestBuilder* builder = this->_requestBuilderMap[fd];
					_respond(builder->build());
					_end_connection(fd);
					delete this->_requestBuilderMap[fd];
					this->_requestBuilderMap.erase(fd);
				}
			}
			if (!_is_server_socket(fd)) {
				current = std::clock();
				double	duration = double(current - _fds_time[fd]) / CLOCKS_PER_SEC;
				if (duration > TIMEOUT) {
					delete this->_requestBuilderMap[fd];
					this->_requestBuilderMap.erase(fd);
					_end_connection(fd);
					_fds_time.erase(fd);
					Logger::warning("Connection innactive, closing conection on fd", fd);
				}
			}

		}
	}
}

void	WebServ::stop(void) {
	t_req_builder_iterator it, end = this->_requestBuilderMap.end();

	for (it = _requestBuilderMap.begin(); it != end; ++it) {
		delete it->second;
	}
	for (int i = 0; i < _total_fds; i++)
		close(_pfds[i].fd);
	for (t_server_iterator it = _servers.begin(); it!= _servers.end(); ++it) {
		delete it->second;
	}
	_servers.clear();
}

bool	WebServ::_is_server_socket(int fd) {
	std::vector<int>::iterator it, end = this->_serverSockets.end();

	for (it = this->_serverSockets.begin(); it != end; ++it) {
		if (*it == fd)
			return true;
	}
	return false;
}

void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void	WebServ::_create_connection(int server_fd) {
	struct		sockaddr_storage remoteaddr;
	socklen_t	addrlen;
	struct		pollfd a;
	int			newfd;
	std::string	error_str;

	addrlen = sizeof remoteaddr;
	newfd = accept(server_fd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		Logger::strerror("accept", errno);
	else {
		a.fd = newfd;
		a.events = POLLIN | POLLOUT;
		a.revents = POLLNOEVENT;
		this->_pfds.push_back(a);
		this->_fds_map.insert(std::make_pair(newfd, server_fd));
		this->_total_fds++;
		_fds_time.insert(std::make_pair(newfd, std::clock()));

		Logger::info("New connection requested and created.");
		Logger::debug("poll server: new connection on socket", newfd);
	}
}

void WebServ::_receive(int fd) {
	RequestBuilder* request_builder;

	request_builder = _get_request_builder(fd);
	if (request_builder->read())
		request_builder->parse();
	else
		_end_connection(fd);
}

RequestBuilder* WebServ::_create_request_builder(int fd) {
	int server_fd;
	RequestBuilder *builder;

	Logger::debug("creating RequestBuilder...");
	server_fd = this->_fds_map[fd];
	builder = new RequestBuilder(this->_servers[server_fd], fd);
	this->_requestBuilderMap.insert(std::make_pair(fd, builder));
	return builder;
}

RequestBuilder* WebServ::_get_request_builder(int fd) {
	t_req_builder_iterator builder;

	builder = this->_requestBuilderMap.find(fd);
	if (builder == this->_requestBuilderMap.end())
		return _create_request_builder(fd);
	return builder->second;
}

bool WebServ::_is_ready_to_respond(int fd) {
	t_req_builder_iterator builderFound;

	builderFound = this->_requestBuilderMap.find(fd);
	if (builderFound != this->_requestBuilderMap.end()) {
		if (builderFound->second->is_ready())
			return true;
	}
	return false;
}

void WebServ::_respond(Request* request) {
	Response response = responseBuilder(request);

	response.sendResponse();
	Logger::info("Response sent");
}

void WebServ::clean(void) {
	t_req_builder_iterator it, end = this->_requestBuilderMap.end();

	for (it = _requestBuilderMap.begin(); it != end; ++it) {
		delete it->second;
	}
	for(int i = 0; i < _total_fds; i++)
		close(_pfds[i].fd);
	this->_total_fds = 0;
}

void WebServ::_end_connection(int fd) {
	t_pollfd_iterator it, end = this->_pfds.end();
	t_req_builder_iterator builder;
	std::map<int, int>::iterator fds_map;

	builder = this->_requestBuilderMap.find(fd);
	if (builder != this->_requestBuilderMap.end()) {
		delete builder->second;
		this->_requestBuilderMap.erase(fd);
	}

	for (it = this->_pfds.begin(); it != end; ++it) {
		if ((*it).fd == fd) {
			this->_pfds.erase(it);
			break;
		}
	}
	fds_map = this->_fds_map.find(fd);
	if (fds_map != this->_fds_map.end())
		this->_fds_map.erase(fd);

	--this->_total_fds;
	close(fd);
	Logger::info("Close connection on socket", fd);
}

void WebServ::restart_socket_servers(void) {
	t_server_iterator it, end = this->_servers.end();

	Logger::info("Restarting servers");
	for (it = this->_servers.begin(); it != end; ++it)
		it->second->configSocket(it->second->getPort());
}

std::ostream& operator<<(std::ostream& o, t_pollfd_vector const& _pfds) {
	t_pollfd_vector::const_iterator it, end = _pfds.end();

	o << GREY << "fds: ";
	for (it = _pfds.begin(); it != end; ++it)
		o << (*it).fd << " | ";
	o << RESET;
	return o;
}

const char* WebServ::ServerPortAlreadySetted::what() const throw() {
	return ("Port already setted to other server.");
}

const char* WebServ::PollErrorException::what() const throw() {
	return ("Poll error in the main loop.");
}
