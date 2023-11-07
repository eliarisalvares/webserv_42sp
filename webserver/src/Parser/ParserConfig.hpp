/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:46:50 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 18:13:15 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <fstream> // using files
# include <iomanip> //function to check name
# include <vector>

class ParserConfig {
	public:
		ParserConfig(void);
		ParserConfig(int port);
		~ParserConfig(void);
		ParserConfig(ParserConfig const& copy);
		ParserConfig const& operator=(ParserConfig const& copy);

		void	keepFile(std::string fileName);
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