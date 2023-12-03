/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:46:50 by feralves          #+#    #+#             */
/*   Updated: 2023/12/03 15:09:51 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "server_builder.hpp"
# include "ftstring.hpp"
# include "Logger.hpp"

class ServerParser {
	public:
		ServerParser(void);
		~ServerParser(void);
		ServerParser(ServerParser const& copy);
		ServerParser const& operator=(ServerParser const& copy);

		size_t	nbrServers;

		void						setConf(std::string fileName);
		std::vector<std::string>	getConf(void);

		class SyntaxErrorException : public std::exception {
			virtual const char* what(void) const throw();
		};
		class ServerErrorException : public std::exception {
			virtual const char* what(void) const throw();
		};
		class ListenNotFoundErrorExeption : public std::exception {
			virtual const char* what(void) const throw();
		};
		class EmptyServerErrorExeption : public std::exception {
			virtual const char* what(void) const throw();
		};
	private:
		std::vector<std::string>	_lines;

		bool	_beginingOfFile();
		bool	_bracketsClosed();
		bool	_minimalRequirements(int index);
};

#endif