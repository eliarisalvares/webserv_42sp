/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:50:47 by feralves          #+#    #+#             */
/*   Updated: 2023/12/02 20:53:54 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstdlib> // exit
# include <sys/socket.h> // socket(), bind(), listen(), accept()
# include <netinet/in.h> // struct sockaddr_in
# include <stdio.h> // errors
# include "ServerParser.hpp"
# include "server_builder.hpp"
# include "http.hpp"

# define BUFFSIZE 256 //buffersize ?
# define CLIENT_MAX_BODY_SIZE 1000 //in bytes
# define CGI_LOCATION "/cgi"
# define CGI_EXECUTOR "python3"
# define ERROR_PAGES "404 404.html" //error_pages
# define LOCATION "/" 
# define METHODS "GET, POST, DELETE" //allowed_methods
# define ROOT "content"
# define SERVER_NAME "WebWizards"
# define SERVER_PORT "8080"
# define TIMEOUT 5000 // precisa colocar?

class Server {
	friend class Response;
	public:
		Server(void);
		Server(int port);
		Server(std::vector<std::string> input, size_t index);
		~Server(void);
		Server(Server const& copy);
		Server& operator=(Server const& copy);

		void	setPort(std::vector<std::string> input, int index);
		void	configSocket(int port);
		void	setBasics(void);
		void	setBufferSize(int size);
		void	setSocket(int port);
		void	setBodySize(int size);
		void	setRoot(std::string root);
		void	setCGI(std::vector<std::string> cgi);
		void	addErrorPages(std::pair<int, std::string> paired);
		void	setErrorPages(std::map<int, std::string> errorPages);
		void	setUpPath(std::string path);
		void	setMethods(std::set<std::string> methods);
		void	setIndex(std::set<std::string> index);
		void	setName(std::vector<std::string> name);

		int							getSocket(void) const;
		int							getPort(void) const;
		int							getBufferSize(void) const;
		int							getBodySize(void) const;
		std::string					getRoot(void) const;
		std::string					getUpPath(void) const;
		std::set<std::string>		getMethods(void) const;
		std::set<std::string>		getIndex(void) const;
		std::vector<t_location>		getLocations(void);
		t_location					getLocations(int index);
		std::vector<std::string>	getCGI(void) const;
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
		t_permissions							_permit; //autoindex && directory_listing
		std::string								_root;
		std::string								_uploadPath;
		std::set<std::string>					_allowed_methods;
		std::set<std::string>					_index;
		std::vector<t_location>					_locations;
		std::vector<std::string>				_cgi;
		std::vector<std::string>				_server_name;
		std::map<int, std::string>				_error_pages;
		std::map<std::string, std::string>		_location_root;  // inclui o par pro cgi -> colocar dentro de locations
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


