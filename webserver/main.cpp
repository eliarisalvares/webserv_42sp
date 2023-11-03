/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 18:58:45 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "./src/Response/Response.hpp"



// get sockaddr, IPv4 or IPv6:
void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Add a new file descriptor to the set
void	add_to_pfds(std::vector<struct pollfd> *pfds, int newfd)
{
	struct pollfd a;

	a.fd = newfd;
	a.events = POLLIN | POLLOUT; // Check ready-to-read + write

	pfds->push_back(a);
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

int	main() {
	Logger log;
	int port = PORT;
	Server oneServer(port);  // file descriptor para o socket do servidor

	std::vector<struct pollfd> pfds(0);
	struct pollfd init;

	char buf[256];    // buffer for client data
	char remoteIP[INET6_ADDRSTRLEN];

	// Add the server socket ("listener")
	init.fd = oneServer.getSocket();
	init.events = POLLIN; // Report ready to read on incoming connection
	pfds.push_back(init);

	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	// main loop
	std::vector<Request*> requests(0);
	// std::vector<RequestBuilder> requestBuilders(0);
	Request* request;
	while(true) {
		int poll_count = poll(pfds.data(), pfds.size(), -1);

		if (poll_count == -1) {
			perror("poll");
			exit(1);
		}

		// Run through the existing connections looking for data to read
		int fd_size = pfds.size();
		for(int i = 0; i < fd_size; i++) {

			// Check if someone's ready to read
			if (pfds[i].revents & POLLIN) { // We got one!!

				// if server
				if (pfds[i].fd == oneServer.getSocket()) {
					// If server_fd is ready to read, handle new connection

					addrlen = sizeof remoteaddr;
					newfd = accept(oneServer.getSocket(),
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
						add_to_pfds(&pfds, newfd);

						printf("pollserver: new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
					}
				} else {
					// If not the server_fd, we're just a regular client
					int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);

					int sender_fd = pfds[i].fd;

					if (nbytes <= 0) {
						// Got error or connection closed by client
						if (nbytes == 0) {
							// Connection closed
							printf("pollserver: socket %d hung up\n", sender_fd);
						} else {
							perror("recv");
						}

						close(pfds[i].fd); // Bye!
						pfds.erase(pfds.begin() + i); // Remove an index from the set
					} else {
						Response response;
						// We got some good data from a client

						// considering that we read only one time, create Request
						request = RequestBuilder().build();
						log.debug("Request pointer:");
						std::cout << request << std::endl;
						requests.push_back(request);

						for(int j = 0; j < fd_size; j++) {
							// Send to everyone!
							int dest_fd = pfds[j].fd; // não estamos usando isso, é igual ao pfds[i].fd no caso do j == i

							// Except the listener and ourselves
							if (j == i){
								if (pfds[i].revents & POLLOUT)
								{
									std::string response_string = get_response();
									send(dest_fd, response_string.c_str(), response_string.length(), 0);
								}
								printf("from (%d): %s", i, buf);}

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
		}
	}

	// Close the server socket (though we won't actually get here in the current design)
	close(oneServer.getSocket());

	return 0;
}
