/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:50:47 by feralves          #+#    #+#             */
/*   Updated: 2023/11/28 14:02:15 by sguilher         ###   ########.fr       */
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
# include <stdio.h> // errors
# include <map>

# define BUFFSIZE 1024 // segfault com 20, 50
# define CLIENT_MAX_BODY_SIZE 100
# define TIMEOUT 5000
# define LOCATION "/"
# define ROOT "content"
# define SERVER_NAME "Server WebWizards"
# define CGI_EXECUTOR "python3"
# define CGI_LOCATION "/cgi"
# define SERVER_PORT "8080"
# define METHODS "GET, POST, DELETE"
# define ERROR_PAGES "404 404.html"

class Server {
	friend class Response;
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
		static std::string	getServerName(void);
		static std::string getCurrentPort(void);
		static std::string getAllowedMethods(void);

	private:
		int									_port;  // obrigatório
		int									_socket;
		int									_bufferSize;
		int									_client_max_body_size;
		std::string							_server_name;
		std::string							_root;  // geral do server; cada location vai poder ter um root diferente
		std::string							_cgi_location;
		std::map<std::string, std::string>	_location_root;  // inclui o par pro cgi
		std::map<int, std::string>			_error_pages;  // obrigatório
		// o que sabemos que falta: autoindex, redirect
};

#endif

/* {
	root content

	location /cgi {
		root bla
		limit_except GET
	}

} */

/* Rota: métodos permitidos, redirecionamento http, root dr/file,
arquivo padrão(/) se usuário requisitar um diretório,
cgi .py python3;
cgi .php php-cgi;
 */
