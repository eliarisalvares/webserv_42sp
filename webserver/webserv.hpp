/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sguilher <sguilher@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:08:43 by feralves          #+#    #+#             */
/*   Updated: 2023/11/08 22:31:22 by sguilher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream> // std::cout

# include "WebServ.hpp"
# include "ParserServer.hpp"

class	Server;
class	ParserServer;

bool	checkArgs(int argc, char *argv[]);

#endif
