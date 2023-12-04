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

void	WebServ::create_servers(std::vector<std::string> input) {
	// for now working with one server only
	// int	extraBrackets = 0;

	// for (size_t index = 0; index < input.size(); index++) {
		Server *oneServer = new Server(input, 0);

		// if (input[index] == input.back())
			// break ;
		// oneServer->setServer(input, 0);
		// if (input[index].substr(0, 8) == "server {") {
			// while (input[index].substr() != "}" && extraBrackets == 0) {
				// if (input[index].substr(0, 9) == "location ")
					// extraBrackets++;
				// else if (input[index].substr() == "}")
					// extraBrackets--;
				// index++;
			// }
		// }
		this->_servers.insert(std::pair<int, Server*>(oneServer->getSocket(), oneServer));
	// }

	//will get the socket for the server, initialize the server
}

void	WebServ::init(void) {
	pollfd init;
	t_server_iterator it, end = this->_servers.end();

	// Add each server socket ("listeners")
	for (it = this->_servers.begin(); it != end; ++it) {
		init.fd = it->first;
		init.events = POLLIN; // Report ready to read on incoming connection
		init.revents = POLLNOEVENT;
		this->_pfds.push_back(init);
		this->_serverSockets.push_back(init.fd);
		this->_total_fds++;
	}
}

void	WebServ::run(void) {
	int poll_count, fd;
	Logger log;

	log.info("Running webserv...");
	while(true) {
		// &(*this->_pfds.begin()
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
					_receive(fd);
				}
			}
			if (this->_pfds[i].revents & POLLOUT) {  // can send
				if (_is_ready_to_respond(fd)) {  // when request parsing ends*
					log.debug("POLLOUT event and request parsing ended");
					// *pode ser que o parsing da request pegue um erro sem ter recebido
					// todos os dados; pra ser mais eficaz e seguro a gente já
					// vai retornar uma resposta com o erro adequado

					// create Request object
					Request* request;
					request = this->_requestBuilderMap[fd]->build();

					_respond(request);
					delete request;
					delete this->_requestBuilderMap[fd];  // ou dá um clean nele
					this->_requestBuilderMap.erase(fd);
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
		log.strerror("accept", errno);
	} else {
		a.fd = newfd;
		a.events = POLLIN | POLLOUT; // Check ready-to-read + write
		a.revents = POLLNOEVENT;
		this->_pfds.push_back(a);
		// ensures we keep the relation between the connection and it's server
		this->_fds_map.insert(std::make_pair(newfd, server_fd));
		this->_total_fds++;

		log.info("New connection requested and created.");
		// >>>>>>>>>>>>>>>>>>>>>>> remove this
		printf("poll server: new connection from %s on socket %d\n",
			inet_ntop(
				remoteaddr.ss_family,
				get_in_addr((struct sockaddr*)&remoteaddr),
				remoteIP, INET6_ADDRSTRLEN
			),
			newfd);
		std::cout << this->_pfds << std::endl;
	}
}

void WebServ::_receive(int fd) {
	RequestBuilder* request_builder;

	request_builder = _get_request_builder(fd);
	if (request_builder->read()) {
		request_builder->parse();
		// if (request_builder->is_ready())
		// 	request_builder->build();
	}
	else // verificar se é a tratativa certa
		_end_connection(fd);
}

RequestBuilder* WebServ::_create_request_builder(int fd) {
	int server_fd;
	RequestBuilder *builder;

	log.debug("creating RequestBuilder...");
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
	Response response;
	Logger log;
	int response_fd = request->fd();

	//std::string filePath = request->_requestData;
	std::string filePath = "content/cgi/current_time.py";

	log.debug("creating response...");
	std::string response_string = responseBuilder(filePath);
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

// for server we need to create other function
void WebServ::_end_connection(int fd) {
	t_pollfd_iterator it, end = this->_pfds.end();
	t_request_iterator request;
	t_response_iterator response;
	t_req_builder_iterator builder;
	std::map<int, int>::iterator fds_map;

	// fazer um template pra esses deletes - std::map<int, T*>
	// clean structures data
	builder = this->_requestBuilderMap.find(fd);
	if (builder != this->_requestBuilderMap.end()) {
		delete builder->second;
		this->_requestBuilderMap.erase(fd);
	}
	request = this->_requests.find(fd);
	if (request != this->_requests.end()) {
		delete request->second;
		this->_requests.erase(fd);
	}
	response = this->_responses.find(fd);
	if (response != this->_responses.end()) {
		delete response->second;
		this->_responses.erase(fd);
	}

	// clean fds structures
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
}

void WebServ::restart_socket_servers(void) {
	t_server_iterator it, end = this->_servers.end();

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
