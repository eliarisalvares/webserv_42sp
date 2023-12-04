/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/12/04 13:54:47 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "src/WebServ/WebServ.hpp"

WebServ	webserv;

void interrupt(int sig)
{
	Logger log;
	std::cout << '\n';
	log.warning("Received signal ", sig + 128);
	webserv.stop();
	exit(0);
}

int	main(int argc, char *argv[]) {
	Logger log;

	struct sigaction interruptHandler;
	interruptHandler.sa_handler = interrupt;
	sigemptyset(&interruptHandler.sa_mask);
	interruptHandler.sa_flags = 0;
	sigaction(SIGINT, &interruptHandler, 0);

	if (!checkArgs(argc, argv))
		return 1;

	try {
		ServerParser	port;
		port.setConf(argv[1]);
		webserv.create_servers(port.getConf());
		log.info("webserv configured.");
	}
	catch (std::exception & e) {
		log.error(e.what());
		return 1;
	}

	bool	first_init = true;

	// o servidor deve sempre restartar em caso de algum erro na execução
	while (true) {
		try {
			// create servers and sockets for each server
			if (first_init) {
				// webserv.create_servers(port.getServersConf()); //send port.servers
				first_init = false;
			}
			else
				webserv.restart_socket_servers(); // criar os sockets novamente
			webserv.init();

			// main loop
			webserv.run();

		} catch (std::exception & e) {
			log.error(e.what());
			webserv.clean(); // para limparmos tudo que precisará ser limpo
		}
	}

	return 0;
}
