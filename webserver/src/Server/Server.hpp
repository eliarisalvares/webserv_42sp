/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:50:47 by feralves          #+#    #+#             */
/*   Updated: 2023/12/02 13:31:42 by feralves         ###   ########.fr       */
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
	std::string								location;
	std::string								root;
	std::set<std::string>					allowed_methods;
	std::set<std::string>					cgi_pass;
	std::vector<std::string>				http_methods;
	std::pair<unsigned int, std::string>	http_redirection;
	// std::string								response_is_dir;
	// std::string								directory_listing;
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
		void	setSocket(int port);
		void	setBodySize(int size);
		void	setRoot(std::string root);
		void	setCGILocation(std::string cgi);
		void	setErrorPages(std::map<int, std::string> errorPages);
		void	setUpPath(std::string path);
		void	setMethods(std::set<std::string> methods);
		void	setIndex(std::set<std::string> index);
		void	setName(std::vector<std::string> name);

		int							getSocket(void) const;
		int							getPort(void) const;
		int							getBufferSize(void) const;
		int							getBodySize(void) const;
		std::string					getCGILoc(void) const;
		std::string					getRoot(void) const;
		std::string					getUpPath(void) const;
		std::set<std::string>		getMethods(void) const;
		std::set<std::string>		getIndex(void) const;
		std::vector<t_location>		getLocations(void);
		t_location					getLocations(int index);
		std::vector<std::string>	getName(void);
		std::string					getName(int index);
		std::map<int, std::string>	getErrorPages(void) const;

		
		static std::string	getServerName(void);
		static std::string	getCurrentPort(void);
		static std::string	getAllowedMethods(void);

	private:
		int										_bufferSize;
		int										_client_max_body_size;
		int										_port;  // único item obrigatório no arquivo
		int										_socket;
		std::string								_cgi_location;
		std::string								_root;  // geral do server; cada location vai poder ter um root diferente
		std::string								_uploadPath;
		std::set<std::string>					_allowed_methods;
		std::set<std::string>					_index; //autoindex
		std::vector<t_location>					_locations;
		std::vector<std::string>				_server_name;
		std::map<int, std::string>				_error_pages;
		std::map<std::string, std::string>		_location_root;  // inclui o par pro cgi -> remover?
		// o que sabemos que falta: redirect -> for locations is ok
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


