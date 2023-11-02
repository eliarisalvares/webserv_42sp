/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/11/02 13:16:26 by feralves         ###   ########.fr       */
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


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main() {
	int server_fd;  // , client_fd; Um file descriptor para o socket do servidor e um para o socket do cliente
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

	struct sockaddr_storage remoteaddr; // client address
	int newfd;        // newly accept()ed socket descriptor
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
							char response[] = "HTTP/1.1 200 OK\nContent-Length: 12\n\n42\n>";
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

	// epoll
	// // Cria uma instância de epoll para monitorar eventos (aqui pode ser poll, epoll ou select, temos que ver qual é o melhor,
	// // mas acho que o epoll é o mais eficiente, mas não sei se é o mais fácil de usar, 1024 é o ulimit -n do meu sistema, mas também seria
	// // interessante ver se é o ideal)
	// int epoll_fd = epoll_create(1024); 
	// struct epoll_event event; // Struct do evento que será monitorado
	// struct epoll_event events[10];  // Struct para armazenar os eventos que estão prontos

	// // Configura o evento para monitorar o socket do servidor para eventos de leitura
	// event.data.fd = server_fd;  // Associa o socket do servidor ao evento
	// event.events = EPOLLIN;  // EPOLLIN é macro para eventos de leitura

	// // a função epoll_ctl adiciona o socket do servidor ao epoll_fd para monitorar eventos
	// epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

	// // Loop para manter o servidor rodando indefinidamente
	// while (1) {
	// 	// Espera por eventos, -1 para esperar indefinidamente
	// 	int num_fds = epoll_wait(epoll_fd, events, 10, -1);

	// 	// Loop para lidar com todos os eventos que estão prontos
	// 	for (int i = 0; i < num_fds; i++) {
	// 		// Checa se o evento é do socket do servidor 
	// 		if (events[i].data.fd == server_fd) {
	// 			// Aceita a conexão do cliente
	// 			client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

	// 			// Adiciona o socket do cliente ao epoll_fd
	// 			event.data.fd = client_fd;
	// 			event.events = EPOLLIN;
	// 			epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
	// 		} else {
	// 			// Lê os dados do cliente e armazena em um buffer de 30000 bytes, também temos
	// 			// que ver se esse tamanho é o ideal
	// 			char buffer[30000] = {0};
	// 			int bytes_read = read(events[i].data.fd, buffer, 30000);

	// 			// Se bytes_read for 0, significa que o cliente fechou a conexão
	// 			if (bytes_read == 0) {
	// 				// Fecha o socket do cliente e remove do epoll_fd
	// 				close(events[i].data.fd);
	// 				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event);
	// 			} else {
	// 				// Printa o que foi recebido do cliente como requisição
	// 				std::cout << buffer << std::endl;

	// 				// Mandar uma resposta para o cliente
	// 				char response[] = "HTTP/1.1 200 OK\nContent-Length: 12\n\n42\n>";
	// 				send(events[i].data.fd, response, sizeof(response), 0);
	// 			}
	// 		}
	// 	}
	// }