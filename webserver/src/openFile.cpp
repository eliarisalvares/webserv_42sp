/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   openFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 14:21:55 by feralves          #+#    #+#             */
/*   Updated: 2023/11/08 17:32:53 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParserServer.hpp"

int	checkFileName(std::string fileName) {
	if (fileName.substr(fileName.find_last_of(".") + 1) == "conf")
		return (true);
	return (false);
}

bool	checkFile(const std::string& fileName)
{
	std::ifstream file;

	if (!checkFileName(fileName)){
		std::cerr << "Error: wrong file extension." << std::endl;
		return (false);
	}
	file.open(fileName.c_str());
	if (file.fail()){
		std::cerr << "Error: could not open file." << std::endl;
		file.close();
		return (false);
	}
	if (file.peek() == EOF){
		std::cerr << "Error: empty file." << std::endl;
		file.close();
		return (false);
	}
	file.close();
	return (true);
}

bool	checkArgs(int argc, char *argv[]) {
	if (argc != 2) {
		if (argc < 2)
			std::cerr << "No configuration file\nUsage: ./webserv <name>.conf" << std::endl;
		else if (argc > 2)
			std::cerr << "Too many arguments\nUsage: ./webserv <name>.conf" << std::endl;
		return false;
	}
	if (!checkFile(argv[1]))
		return false;
	return true;
}

