/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/12/10 16:39:14 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "src/WebServ/WebServ.hpp"

WebServ	webserv;

void interrupt(int sig)
{
	std::cout << '\n';
	Logger::warning("Received signal ", sig + 128);
	webserv.stop();
	exit(0);
}

int	main(int argc, char *argv[]) {
	bool	first_init = true;

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
		Logger::info("webserv configured.");
	}
	catch (std::exception & e) {
		Logger::error(e.what());
		return 1;
	}
	while (true) {
		try {
			if (first_init)
				first_init = false;
			else
				webserv.restart_socket_servers();
			webserv.init();
			webserv.run();
		} catch (std::exception & e) {
			Logger::error(e.what());
			webserv.clean();
		}
	}

	return 0;
}
