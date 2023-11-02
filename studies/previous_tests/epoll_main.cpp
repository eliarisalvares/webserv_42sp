/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll_main.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/11/02 15:19:20 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main() {
	int server_fd, client_fd;  // Um file descriptor para o socket do servidor e um para o socket do cliente
	struct sockaddr_in address;  // Struct para o endereço do servidor
	int addrlen = sizeof(address);  // Tamanho do endereço do servidor

	// Cria o socket do servidor, AF_INET para IPv4, SOCK_STREAM para TCP, 0 para o protocolo
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Configura o endereço do servidor e a porta, AF_INET para IPv4, INADDR_ANY para o endereço do host, htons para a porta
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Associa o socket do servidor ao endereço e à porta especificados
	bind(server_fd, (struct sockaddr*)&address, sizeof(address));

	// Coloca o socket do servidor em modo de escuta, com um limite de 500 conexões pendentes (isso temos que ver, esse número os meninos usaram no projeto deles,
	// mas não sei se é o ideal, acho que a gente tem que ver isso, meu sistema mostra 4096)
	listen(server_fd, 500);

	// Cria uma instância de epoll para monitorar eventos (aqui pode ser poll, epoll ou select, temos que ver qual é o melhor,
	// mas acho que o epoll é o mais eficiente, mas não sei se é o mais fácil de usar, 1024 é o ulimit -n do meu sistema, mas também seria
	// interessante ver se é o ideal)
	int epoll_fd = epoll_create(1024); 
	struct epoll_event event; // Struct do evento que será monitorado
	struct epoll_event events[10];  // Struct para armazenar os eventos que estão prontos

	// Configura o evento para monitorar o socket do servidor para eventos de leitura
	event.data.fd = server_fd;  // Associa o socket do servidor ao evento
	event.events = EPOLLIN;  // EPOLLIN é macro para eventos de leitura

	// a função epoll_ctl adiciona o socket do servidor ao epoll_fd para monitorar eventos
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

	// Loop para manter o servidor rodando indefinidamente
	while (1) {
		// Espera por eventos, -1 para esperar indefinidamente
		int num_fds = epoll_wait(epoll_fd, events, 10, -1);

		// Loop para lidar com todos os eventos que estão prontos
		for (int i = 0; i < num_fds; i++) {
			// Checa se o evento é do socket do servidor 
			if (events[i].data.fd == server_fd) {
				// Aceita a conexão do cliente
				client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

				// Adiciona o socket do cliente ao epoll_fd
				event.data.fd = client_fd;
				event.events = EPOLLIN;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
			} else {
				// Lê os dados do cliente e armazena em um buffer de 30000 bytes, também temos
				// que ver se esse tamanho é o ideal
				char buffer[30000] = {0};
				int bytes_read = read(events[i].data.fd, buffer, 30000);

				// Se bytes_read for 0, significa que o cliente fechou a conexão
				if (bytes_read == 0) {
					// Fecha o socket do cliente e remove do epoll_fd
					close(events[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event);
				} else {
					// Printa o que foi recebido do cliente como requisição
					std::cout << buffer << std::endl;

					// Mandar uma resposta para o cliente
					char response[] = "HTTP/1.1 200 OK\nContent-Length: 12\n\n42\n>";
					send(events[i].data.fd, response, sizeof(response), 0);
				}
			}
		}
	}

	// Close the server socket (though we won't actually get here in the current design)
	close(server_fd);

	return 0;
}