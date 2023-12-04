/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   openFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 14:21:55 by feralves          #+#    #+#             */
/*   Updated: 2023/12/04 14:29:05 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <iomanip> //function to check name

int	checkFileName(std::string fileName) {
	if (fileName.substr(fileName.find_last_of(".") + 1) == "conf")
		return (true);
	return (false);
}

bool	checkFileWorks(const std::string& fileName) {
	std::ifstream file;

	file.open(fileName.c_str());
	if (file.fail()){
		Logger::error("Could not open file.");
		file.close();
		return (false);
	}
	if (file.peek() == EOF){
		Logger::error("Empty file.");
		file.close();
		return (false);
	}
	file.close();
	return (true);
}

bool	checkFile(const std::string& fileName) {
	std::ifstream file;

	if (!checkFileName(fileName)){
		Logger::error("Wrong file extension.");
		return (false);
	}
	file.open(fileName.c_str());
	if (file.fail()){
		Logger::error("Could not open file.");
		file.close();
		return (false);
	}
	if (file.peek() == EOF){
		Logger::error("Empty file.");
		file.close();
		return (false);
	}
	file.close();
	return (true);
}

bool	checkArgs(int argc, char *argv[]) {
	if (argc != 2) {
		if (argc < 2)
			Logger::error("No configuration file\nUsage: ./webserv <name>.conf");
		else if (argc > 2)
			Logger::error("Too many arguments\nUsage: ./webserv <name>.conf");
		return false;
	}
	if (!checkFile(argv[1]))
		return false;
	Logger::info("File oppened successfully.");
	return true;
}

