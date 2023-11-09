/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:09:07 by feralves          #+#    #+#             */
/*   Updated: 2023/11/08 21:50:56 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	main(int argc, char *argv[]) {
	if (!checkArgs(argc, argv))
		return 1;

	ParserServer	port;
	try {
		port.getConf(argv[1]);
	}
	catch (std::exception & e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	(void)port;

	WebServ	webserv;

	// create servers and sockets for each server
	webserv.createServers();
	webserv.init();

	// main loop
	webserv.run();

	return 0;
}
