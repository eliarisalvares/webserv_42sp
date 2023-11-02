/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select_main.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/11/02 15:16:54 by feralves         ###   ########.fr       */
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

int main() {
	int server_fd = get_server_socket();  // file descriptor para o socket do servidor

	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number
	socklen_t new_addrlen;
	char remoteIP[INET6_ADDRSTRLEN];
	int nbytes;
	char buf[256];    // buffer for client data

	// add the listener->server_fd to the master set
	FD_SET(server_fd, &master);

	// keep track of the biggest file descriptor
	fdmax = server_fd;

	// main loop
	while(true) {
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		// run through the existing connections looking for data to read
		for(int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!
				if (i == server_fd) {
					// handle new connections
					new_addrlen = sizeof remoteaddr;
					newfd = accept(server_fd,
						(struct sockaddr *)&remoteaddr,
						&new_addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on "
							"socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
						   newfd);
					}
				} else {
					// handle data from a client
					// parse da request
					if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} else {
						// we got some data from a client
						for(int j = server_fd; j <= fdmax; j++) {
							// send to everyone!
							// if (FD_ISSET(j, &master)) {
							// 	// except the server_fd and ourselves
							// 	if (j != server_fd && j != i) {
							// 		if (send(j, buf, nbytes, 0) == -1) {
							// 			perror("send");
							// 		}
							// 	}
							// }
							//função para lidar com o buf
							if (j == server_fd)
								printf("from (%d), %s\n", i, buf);
							char response[] = "HTTP/1.1 200 OK\nContent-Length: 2\n\n42\n>";
							if (j == i)
								send(j, response, sizeof(response), 0);
						}
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	}

	// Close the server socket (though we won't actually get here in the current design)
	close(server_fd);

	return 0;
}
