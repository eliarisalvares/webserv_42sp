/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:50:47 by feralves          #+#    #+#             */
/*   Updated: 2023/12/10 11:15:10 by feralves         ###   ########.fr       */
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
# include "define_server.hpp"
# include "http.hpp"

class Server {
	friend class Response;
	public:
		Server(void);
		Server(int port);
		Server(std::vector<std::string> input, size_t index);
		~Server(void);
		Server(Server const& copy);
		Server& operator=(Server const& copy);

		void	setBasics(void);
		void	setBodySize(int size);
		void	setBufferSize(int size);
		void	setCGI(bool boolean);
		void	setErrorPages(std::map<int, std::string> errorPages);
		void	setIndex(std::set<std::string> index);
		void	setMethods(std::set<std::string> methods);
		void	setName(std::vector<std::string> name);
		void	setPort(std::vector<std::string> input, int index);
		void	setRedirect(std::string redirect);
		void	setRoot(std::string root);
		void	setUpPath(std::string path);
		void	addErrorPages(std::pair<int, std::string> paired);
		void	addLocation(t_location location);
		bool	configSocket(int port);

		bool						getAutoindex(void) const;
		int							getBufferSize(void) const;
		int							getBodySize(void) const;
		bool						getCGI(void) const;
		std::string					getCurrentPort(void) const;
		std::map<int, std::string>	getErrorPages(void) const;
		std::string					getErrorPages(int value) const;
		std::set<std::string>		getIndex(void) const;
		std::vector<t_location>*	getLocations(void);
		t_location					getLocations(int index);
		std::set<std::string>		getMethods(void) const;
		std::vector<std::string>	getName(void);
		std::string					getName(int index);
		int							getPort(void) const;
		std::string					getRoot(void) const;
		int							getSocket(void) const;
		std::string					getUpPath(void) const;


	private:
		bool									_cgi;
		int										_bufferSize;
		int										_client_max_body_size;
		int										_port;
		int										_socket;
		std::string								_redirect;
		std::string								_root;
		std::string								_uploadPath;
		std::set<std::string>					_allowed_methods;
		std::set<std::string>					_index;
		std::vector<t_location>					_locations;
		std::vector<std::string>				_server_name;
		std::map<int, std::string>				_error_pages;
		t_permissions							_permit;
};

#endif
