/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/11/02 16:05:21 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>

// vamos usar para cada server do arquivo de config
int	get_server_socket()
{
	int server_fd;
	struct sockaddr_in address;  // Struct para o endereço do servidor
	// int addrlen = sizeof(address);  // Tamanho do endereço do servidor

	// Cria o socket do servidor, AF_INET para IPv4, SOCK_STREAM para TCP, 0 para o protocolo
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Configura o endereço do servidor e a porta, AF_INET para IPv4, INADDR_ANY para o endereço do host, htons para a porta
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// - error on rerunning a server: "Address already in use" - 
	// significa que um socket que estava conectado ainda está segurando a porta no 
	// kernel e, portando, monopolizando-a. Para poder reutilizar a porta, podemos usar `setsockopt()`:
	int yes=1;
	// lose the pesky "Address already in use" error message
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
		perror("setsockopt");
		exit(1);
	}
	
	// Associa o socket do servidor ao endereço e à porta especificados
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
		perror("bind");
	}

	// Coloca o socket do servidor em modo de escuta, com um limite de 500 conexões pendentes (isso temos que ver, esse número os meninos usaram no projeto deles,
	// mas não sei se é o ideal, acho que a gente tem que ver isso, meu sistema mostra 4096)
	listen(server_fd, 500);
	return (server_fd);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Add a new file descriptor to the set
void add_to_pfds(std::vector<struct pollfd> *pfds, int newfd)
{
	struct pollfd a;

	a.fd = newfd;
	a.events = POLLIN | POLLOUT; // Check ready-to-read + write
	
	pfds->push_back(a);
}

// Remove an index from the set
void del_from_pfds(std::vector<struct pollfd> *pfds, int i)
{
	pfds->erase(pfds->begin() + i);
}

int main() {
	int server_fd = get_server_socket();  // file descriptor para o socket do servidor

	std::vector<struct pollfd> pfds(0);
	struct pollfd init;
	
	// Add the server socket ("listener")
	init.fd = server_fd;
	init.events = POLLIN; // Report ready to read on incoming connection
	pfds.push_back(init);

	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[256];    // buffer for client data
	char remoteIP[INET6_ADDRSTRLEN];

	// main loop
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
				if (pfds[i].fd == server_fd) {
					// If server_fd is ready to read, handle new connection

					addrlen = sizeof remoteaddr;
					newfd = accept(server_fd,
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

						del_from_pfds(&pfds, i);
					} else {
						// We got some good data from a client

						for(int j = 0; j < fd_size; j++) {
							// Send to everyone!
							int dest_fd = pfds[j].fd;

							// Except the listener and ourselves
							if (j == i){
								if (pfds[i].revents & POLLOUT)
								{
									char response[] = "HTTP/1.1 200 OK\nContent-Length: 2\n\n42\n>";
									send(dest_fd, response, sizeof(response), 0);
								}
								printf("from (%d): %s", i, buf);}
							// if (dest_fd != listener && dest_fd != sender_fd) {
							//     if (send(dest_fd, buf, nbytes, 0) == -1) {
							//         perror("send");
							//     }
							// }
						}
					}
					memset(&buf, 0, sizeof(buf));
				} // END handle data from client
			} // END got ready-to-read from poll()
		}
	}

	// Close the server socket (though we won't actually get here in the current design)
	close(server_fd);

	return 0;
}
