/*
Server Class
*/

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstdlib> // exit
# include <sys/socket.h> // socket(), bind(), listen(), accept()
# include <netinet/in.h> // struct sockaddr_in
#include <stdio.h> // errors

class Server {
	public:
		Server(void);
		Server(int port);
		~Server(void);
		Server(Server const& copy);
		Server const& operator=(Server const& copy);

		int		getSocket(void);
		void	setSocket(int port);

	private:
		int	_socket;
};

#endif