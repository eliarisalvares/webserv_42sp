#include "WebServ.hpp"

WebServ::WebServ(void): _total_fds(0) {
	// ensure all maps and vectors are empty
	this->_servers.clear();
	this->_pfds.clear();
	this->_fds_map.clear();
	this->_serverSockets.clear();
	this->_requests.clear();
	this->_responses.clear();
	this->_requestBuilderMap.clear();
}

WebServ::~WebServ(void) {
	t_server_iterator it, end = this->_servers.end();

	for (it = this->_servers.begin(); it != end; ++it) {
		// close each server socket
		close(it->first);
	}

	// is this necessary?
	this->_servers.clear();
	this->_requests.clear();
	this->_responses.clear();
	this->_requestBuilderMap.clear();
}

WebServ::WebServ(WebServ const& copy) {
	*this = copy;
}

WebServ const& WebServ::operator=(WebServ const& copy) {
	if (this != &copy) {
		// we don't do anything because this operator is private
		(void)copy;
	}
	return *this;
}

void	WebServ::create_servers(void) {
	// for now working with one server only
	Server* oneServer = new Server(PORT);

	this->_servers.insert(
		std::pair<int, Server*>(oneServer->getSocket(), oneServer)
	);
}

void	WebServ::init(void) {
	struct pollfd init;
	t_server_iterator it, end = this->_servers.end();

	// Add each server socket ("listeners")
	for (it = this->_servers.begin(); it != end; ++it) {
		init.fd = it->first;
		init.events = POLLIN; // Report ready to read on incoming connection
		this->_pfds.push_back(init);
		this->_serverSockets.push_back(init.fd);
		this->_total_fds++;
	}
}

void	WebServ::run(void) {
	int poll_count, fd;
	Logger log;

	log.debug("run");
	while(true) {
		poll_count = poll(this->_pfds.data(), this->_pfds.size(), -1);

		if (poll_count == -1) {
			perror("poll");
			exit(1);  // precisamos dar um raise aqui talvez
		}

		// Run through the existing connections looking for data to read
		for(int i = 0; i < this->_total_fds; i++) {
			fd = this->_pfds[i].fd;

			// Check if someone's ready to read
			if (this->_pfds[i].revents & POLLIN) {
				log.debug("POLLIN event");
				if (_is_server_socket(fd)) {
					log.debug("creating connection...");
					_create_connection(fd);
				} else {
					// If not the server_fd, we're just a regular client
					if (!_request_builder_exists(fd)) {
						log.debug("creating RequestBuilder...");
						// create RequestBuilder which will handle request parsing and creation
						_create_request_builder(fd);
					}
					// addRequestData() lê e vai indicar se a request é válida estruturalmente
					this->_requestBuilderMap[fd]->addRequestData();

				} // END got ready-to-read from poll()
			}
			if (this->_pfds[i].revents & POLLOUT) {  // can send
				if (
					_request_builder_exists(fd)
					&& this->_requestBuilderMap[fd]->is_ready()  // when request parsing ends*
				) {
					log.debug("POLLOUT event and request parsing ended");
					// *pode ser que o parsing da request pegue um erro sem ter recebido
					// todos os dados; pra ser mais eficaz e seguro a gente já
					// vai retornar uma resposta com o erro adequado
					Request* request;

					// create Request object
					request = this->_requestBuilderMap[fd]->build();
					log.debug("creating request...");
					// std::cout << request << std::endl;

					_respond(request);

					// delete request after sending response - problem here
					// requests.erase(requests.begin() + i - 1); // Remove request
					// delete request;
					// clean requestBuilder or delete it?
				}
			}
		}
	}
}


bool	WebServ::_is_server_socket(int fd) {
	std::vector<int>::iterator it, end = this->_serverSockets.end();

	for (it = this->_serverSockets.begin(); it != end; ++it) {
		if (*it == fd)
			return true;
	}
	return false;
}

// provavelmente vai ser uma função auxiliar para podermos colocar infos de log
// get sockaddr, IPv4 or IPv6:
void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void	WebServ::_create_connection(int server_fd) {
	struct sockaddr_storage remoteaddr; // client address
	char remoteIP[INET6_ADDRSTRLEN];
	socklen_t addrlen;
	struct pollfd a;
	int newfd;

	addrlen = sizeof remoteaddr;
	newfd = accept(server_fd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1) {
		perror("accept");
	} else {
		a.fd = newfd;
		a.events = POLLIN | POLLOUT; // Check ready-to-read + write
		this->_pfds.push_back(a);
		// ensures we keep the relation between the connection and it's server
		this->_fds_map.insert(std::make_pair(newfd, server_fd));
		this->_total_fds++;

		// >>>>>>>>>>>>>>>>>>>>>>> remove this
		printf("pollserver: new connection from %s on socket %d\n",
			inet_ntop(
				remoteaddr.ss_family,
				get_in_addr((struct sockaddr*)&remoteaddr),
				remoteIP, INET6_ADDRSTRLEN
			),
			newfd);
		std::cout << this->_pfds << std::endl;
	}
}

void WebServ::_create_request_builder(int fd) {
	int server_fd;
	RequestBuilder *builder;

	server_fd = this->_fds_map[fd];
	builder = new RequestBuilder(*(this->_servers[server_fd]), fd);
	this->_requestBuilderMap.insert(std::make_pair(fd, builder));
}

bool WebServ::_request_builder_exists(int fd) {
	t_req_builder_iterator builderFound;

	builderFound = this->_requestBuilderMap.find(fd);
	if (builderFound == this->_requestBuilderMap.end())
		return false;
	return true;
}

void WebServ::_respond(Request* request) {
	Response response;
	Logger log;
	int response_fd = request->fd();
	int status_code = 200;

	//std::string filePath = request->_requestData;
	std::string filePath = "content/index.html";
	std::string contentType = getContentType(filePath);
	std::string body = getHtmlContent(filePath);
	std::string message = getStatusMessage(status_code);

	// Lili:
	// create Response object using Request object
	// parse response
	// send response
	log.debug("creating response...");
	std::string response_string = responseBuilder(status_code, message, body, contentType);
	log.debug("sending response...");
	send(response_fd, response_string.c_str(), response_string.length(), 0);
}

void WebServ::clean(void) {
	t_pollfd_vector::iterator it, end = this->_pfds.end();

	// close all connections, including server ports?
	// não sei se pode fechar tudo, incluindo as portas dos servidores...
	for (it = this->_pfds.begin(); it != end; ++it)
		close((*it).fd);

	// precisa verificar se isso gera leak
	this->_servers.clear();
	this->_pfds.clear();
	this->_fds_map.clear();
	this->_serverSockets.clear();
	this->_requests.clear();
	this->_responses.clear();
	this->_requestBuilderMap.clear();

	this->_total_fds = 0;
}

void WebServ::restart_socket_servers(void) {
	t_server_iterator it, end = this->_servers.end();

	for (it = this->_servers.begin(); it != end; ++it)
		it->second->setSocket(it->second->getPort());
}

std::ostream& operator<<(std::ostream& o, t_pollfd_vector const& _pfds) {
	t_pollfd_vector::const_iterator it, end = _pfds.end();

	o << GREY << "fds: ";
	for (it = _pfds.begin(); it != end; ++it)
		o << (*it).fd << " | ";
	o << RESET;
	return o;
}
