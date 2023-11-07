/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   openFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 14:21:55 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 14:30:54 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

// int	openInFile(char *fileName, std::ifstream &input)
// {
// 	input.open(fileName);
// 	if (!input.is_open())
// 	{
// 		errorInput("Can't open input file");
// 		return (false);
// 	}
// 	return (true);
// }


int	getPort(char *fileName) {
	// std::ifstream	inFile;
	(void)fileName;
	return (PORT);
}
