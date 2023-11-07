/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:46:50 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 16:56:27 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>

class ParserConfig {
	public:
		ParserConfig(void);
		ParserConfig(int port);
		~ParserConfig(void);
		ParserConfig(ParserConfig const& copy);
		ParserConfig const& operator=(ParserConfig const& copy);

		void	readFile(std::string fileName);
};

#endif