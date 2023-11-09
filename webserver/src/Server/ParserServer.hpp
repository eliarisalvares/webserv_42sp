/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:46:50 by feralves          #+#    #+#             */
/*   Updated: 2023/11/09 18:38:25 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <fstream> // using files
# include <vector>

class ParserServer {
	public:
		ParserServer(void);
		~ParserServer(void);
		ParserServer(ParserServer const& copy);
		ParserServer const& operator=(ParserServer const& copy);

		void	getConf(std::string fileName);

		class SyntaxErrorException : public std::exception {
			virtual const char* what(void) const throw();
		};
		class ServerErrorException : public std::exception {
			virtual const char* what(void) const throw();
		};
	private:
		std::string					_fileContent;
		std::vector<std::string>	_lines;

		bool	_beginingOfFile();
		bool	_bracketsClosed();
};

#endif