/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll_main.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/11/02 15:36:49 by feralves         ###   ########.fr       */
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
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
	// If we don't have room, add more space in the pfds array
	if (*fd_count == *fd_size) {
		*fd_size *= 2; // Double it

		*pfds = (pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
	}

	(*pfds)[*fd_count].fd = newfd;
	(*pfds)[*fd_count].events = POLLIN | POLLOUT; // Check ready-to-read + write

	(*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
	// Copy the one from the end over this one
	pfds[i] = pfds[*fd_count-1];

	(*fd_count)--;
}

int main() {
	int server_fd = get_server_socket();  // file descriptor para o socket do servidor

	// Start off with room for 5 connections
	// (We'll realloc as necessary)
	int fd_count = 0;
	int fd_size = 5;
	struct pollfd *pfds = (pollfd *)malloc(sizeof *pfds * fd_size);
	
	// Add the server socket ("listener")
	pfds[0].fd = server_fd;
	pfds[0].events = POLLIN; // Report ready to read on incoming connection
	fd_count = 1; // For the listener

	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[256];    // buffer for client data
	char remoteIP[INET6_ADDRSTRLEN];

	// main loop
	while(true) {
		int poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1) {
			perror("poll");
			exit(1);
		}

		// Run through the existing connections looking for data to read
		for(int i = 0; i < fd_count; i++) {

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
						add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

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

						del_from_pfds(pfds, i, &fd_count);

					} else {
						// We got some good data from a client

						for(int j = 0; j < fd_count; j++) {
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
