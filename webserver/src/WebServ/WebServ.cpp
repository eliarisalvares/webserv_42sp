#include "WebServ.hpp"

WebServ::WebServ(void) {
	// ensure all maps and vectors are empty
	this->_servers.empty();
	this->_pfds.empty();
	this->_serverSockets.empty();
	this->_requests.empty();
	this->_responses.empty();
	this->_requestBuilderMap.empty();
}

WebServ::~WebServ(void) {
	t_server_iterator it, end = this->_servers.end();

	for (it = this->_servers.begin(); it != end; ++it) {
		// close each server socket
		close(it->first);
	}

	// is this necessary?
	this->_servers.empty();
	this->_requests.empty();
	this->_responses.empty();
	this->_requestBuilderMap.empty();
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

void	WebServ::createServers(void) {
	// for now working with one server only
	Server oneServer(PORT);

	this->_servers.insert(std::pair<int, Server*>(oneServer.getSocket(), &oneServer));
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
	}
}

// get sockaddr, IPv4 or IPv6:
void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


std::string get_response() {
	Response response;
	response.setStatusCode(200);
	response.setMessage("OK");
	response.setBody("Hello World!");
	response.addHeader("Content-Type", "text/plain");
	response.addHeader("Content-Length", "12");
	std::string response_string = response.toString();
	return response_string;
}

void	WebServ::run(void) {
	Logger log;
	char buf[BUFFSIZE];    // buffer for client data
	char remoteIP[INET6_ADDRSTRLEN];

	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	// main loop
	std::vector<Request*> requests(0);
	std::map<int, RequestBuilder> requestBuilderMap;
	Request* request;
	int fd;
	while(true) {
		int poll_count = poll(this->_pfds.data(), this->_pfds.size(), -1);

		if (poll_count == -1) {
			perror("poll");
			exit(1);
		}

		// Run through the existing connections looking for data to read
		int fd_size = this->_pfds.size();
		for(int i = 0; i < fd_size; i++) {
			fd = this->_pfds[i].fd;

			// Check if someone's ready to read
			if (this->_pfds[i].revents & POLLIN) { // We got one!!

				// if server
				if (is_server_socket(fd)) {
					// If server_fd is ready to read, handle new connection

					addrlen = sizeof remoteaddr;
					newfd = accept(fd,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
						add_connection_socket_to_pfds(newfd);

						// create RequestBuilder which will handle request parsing and creation
						requestBuilderMap.insert(
							std::pair<int, RequestBuilder>(
								fd, RequestBuilder()
							)
						);

						printf("pollserver: new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
					}
				} else {
					// If not the server_fd, we're just a regular client
					int nbytes = recv(fd, buf, sizeof buf, 0);

					// add request data in RequestBuilder
					std::map<int, RequestBuilder>::const_iterator builderFound = requestBuilderMap.find(fd);
					if (builderFound != requestBuilderMap.end()) {
						requestBuilderMap[fd].addRequestData(buf);
					}

					int sender_fd = fd;
					if (nbytes <= 0) {
						// Got error or connection closed by client
						if (nbytes == 0) {
							// Connection closed
							printf("pollserver: socket %d hung up\n", sender_fd);
						} else {
							perror("recv");
						}

						close(fd); // Bye!
						this->_pfds.erase(this->_pfds.begin() + i); // Remove an index from the set
					} else {
						Response response;
						// We got some good data from a client

						// considering that we read only one time, create Request
						request = requestBuilderMap[fd].build();
						log.debug("Request pointer:");
						std::cout << request << std::endl;
						requests.push_back(request);

						for(int j = 0; j < fd_size; j++) {
							// Send to everyone!
							int dest_fd = this->_pfds[j].fd; // não estamos usando isso, é igual ao pfds[i].fd no caso do j == i

							// Except the listener and ourselves
							if (j == i){
								if (this->_pfds[i].revents & POLLOUT)
								{
									std::string response_string = get_response();
									send(dest_fd, response_string.c_str(), response_string.length(), 0);
								}
								printf("from (%d): %s", i, buf);}
								// usar um map - pfd[i] - para o builder
								// delete request after sending response - problem here
								// request = requests[i - 1]; // i - 1 because server doesn't is a request
								// log.debug("Request pointer:");
								// std::cout << request << std::endl;
								// requests.erase(requests.begin() + i - 1); // Remove request
								// delete request;
						}
					}
					memset(&buf, 0, sizeof(buf));
				} // END handle data from client
			} // END got ready-to-read from poll()
			// if (pfds[i].revents & POLLOUT) {
			// 	if requestBuilderMap[pfds[i].fd].status == send
			// }
		}
	}
}

bool	WebServ::is_server_socket(int fd) {
	std::vector<int>::iterator it, end = this->_serverSockets.end();

	for (it = this->_serverSockets.begin(); it != end; ++it) {
		if (*it == fd)
			return true;
	}
	return false;
}

void	WebServ::add_connection_socket_to_pfds(int newfd) {
	struct pollfd a;

	a.fd = newfd;
	a.events = POLLIN | POLLOUT; // Check ready-to-read + write
	this->_pfds.push_back(a);
}
