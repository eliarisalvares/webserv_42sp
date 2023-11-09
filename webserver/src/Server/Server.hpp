/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:50:47 by feralves          #+#    #+#             */
/*   Updated: 2023/11/09 10:02:04 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

# define BUFFSIZE 256

class Server {
	public:
		Server(void);
		Server(int port);
		~Server(void);
		Server(Server const& copy);
		Server& operator=(Server const& copy);

		int		getSocket(void) const;
		int		getPort(void) const;
		void	setSocket(int port);
		int		getBufferSize(void) const;
		void	setBufferSize(int size);

	private:
		int	_port;
		int	_socket;
		int	_bufferSize;
};

#endif
