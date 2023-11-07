/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:08:43 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 19:04:16 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream> // std::cout
# include <vector> // for the container with all conections
# include <unistd.h> // read(), close()
# include <sys/epoll.h> // epoll_create(), epoll_ctl(), epoll_wait()
# include <poll.h> // poll


// ver o que é necessário aqui
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>

# include "Server.hpp"
# include "Request.hpp"
# include "RequestBuilder.hpp"
# include "ParserConfig.hpp"

# define PORT 8080

class	Server;
class	ParserConfig;


bool	checkFile(const std::string& fileName);

#endif
