/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:50:47 by feralves          #+#    #+#             */
/*   Updated: 2023/12/02 10:53:04 by feralves         ###   ########.fr       */
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
# include <set>
# include "ServerParser.hpp"
# include "ServerBuilder.hpp"

# define BUFFSIZE 256 //buffersize ?
# define CLIENT_MAX_BODY_SIZE 100 //client_max_body_size -> in KILOBYTES?
# define TIMEOUT 5000 // precisa colocar?
# define LOCATION "/" // vai ser definida através de mts possibilidades (kill me pls)
# define ROOT "content" //root
# define SERVER_NAME "WebWizards" ///server_name
# define CGI_EXECUTOR "python3" //cgi
# define CGI_LOCATION "/cgi"
# define SERVER_PORT "8080" //listen
# define METHODS "GET, POST, DELETE" //allowed_methods
# define ERROR_PAGES "404 404.html" //error_pages


typedef struct	s_location
{
	std::string                          location;
	std::vector<std::string>             allowed_methods;
	std::vector<std::string>             http_methods;
	std::pair<unsigned int, std::string> http_redirection;
	std::string                          root;
	std::string                          directory_listing;
	std::string                          response_is_dir;
	std::set<std::string>                cgi_pass;
	// std::set<std::string>                required_cookie;
	// std::vector<t_cookie>                set_cookie;
}				t_location;

class Server {
	friend class Response;
	public:
		Server(void);
		Server(int port);
		Server(std::vector<std::string> input, size_t index);
		~Server(void);
		Server(Server const& copy);
		Server& operator=(Server const& copy);

		void	configSocket(int port);
		void	setBufferSize(int size);

		int		getSocket(void) const;
		int		getPort(void) const;
		int		getBufferSize(void) const;
		static std::string	getServerName(void);
		static std::string	getCurrentPort(void);
		static std::string	getAllowedMethods(void);

	private:
		int										_port;  // obrigatório
		int										_socket;
		int										_bufferSize;
		int										_client_max_body_size;
		std::string								_root;  // geral do server; cada location vai poder ter um root diferente
		std::string								_cgi_location;
		std::set<std::string>					_index;
		std::vector<std::string>				_server_name;
		std::vector<std::string>				_allowed_methods;
		std::vector<t_location>					_locations;
		std::map<std::string, std::string>		_location_root;  // inclui o par pro cgi
		std::map<int, std::string>				_error_pages;
		std::string								_uploadPath;  // obrigatório
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
