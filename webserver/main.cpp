/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/12/02 11:38:36 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "src/WebServ/WebServ.hpp"

int	main(int argc, char *argv[]) {
	Logger log;

	if (!checkArgs(argc, argv))
		return 1;

	WebServ	webserv;
	ServerParser	port;
	try {
		port.setConf(argv[1]);
		webserv.create_servers(port.getConf());
	}
	catch (std::exception & e) {
		log.error(e.what());
		return 1;
	}
	(void)port;

	bool	first_init = true;

	// o servidor deve sempre restartar em caso de algum erro na execução
	while (true) {
		try {
			// create servers and sockets for each server
			if (first_init)
			{
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
