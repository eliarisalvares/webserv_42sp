/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:46:50 by feralves          #+#    #+#             */
/*   Updated: 2023/11/27 16:31:16 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "ServerBuilder.hpp"
# include "ftstring.hpp"

class ServerParser {
	public:
		ServerParser(void);
		~ServerParser(void);
		ServerParser(ServerParser const& copy);
		ServerParser const& operator=(ServerParser const& copy);

		void						servers(void);
		void						getConf(std::string fileName);
		std::vector<t_infoServer>	getServers(void);

		class SyntaxErrorException : public std::exception {
			virtual const char* what(void) const throw();
		};
		class ServerErrorException : public std::exception {
			virtual const char* what(void) const throw();
		};
		class ListenNotFoundErrorExeption : public std::exception {
			virtual const char* what(void) const throw();
		};
	private:
		std::string					_fileContent;
		size_t						_nbrServers;
		std::vector<std::string>	_lines;
		std::vector<t_infoServer>	_servers;

		bool	_beginingOfFile();
		bool	_bracketsClosed();
		bool	_minimalRequirements(int index);
};

#endif