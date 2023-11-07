/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   openFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 14:21:55 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 14:41:09 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	openInFile(char *fileName, std::ifstream &input)
{
	input.open(fileName);
	if (!input.is_open())
	{
		std::cout << "Can't open file" << std::endl;
		return (false);
	}
	return (true);
}

int	getPort(char *fileName) {
	std::ifstream	inFile;
	if (!openInFile(fileName, inFile))
		return (false);
	return (PORT);
}
