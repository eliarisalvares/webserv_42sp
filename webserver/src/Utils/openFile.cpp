/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   openFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 14:21:55 by feralves          #+#    #+#             */
/*   Updated: 2023/12/03 16:19:11 by feralves         ###   ########.fr       */
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
	Logger	log;
	std::ifstream file;

	file.open(fileName.c_str());
	if (file.fail()){
		log.error("Could not open file.");
		file.close();
		return (false);
	}
	if (file.peek() == EOF){
		log.error("Empty file.");
		file.close();
		return (false);
	}
	file.close();
	return (true);
}

bool	checkFile(const std::string& fileName) {
	Logger	log;
	std::ifstream file;

	if (!checkFileName(fileName)){
		log.error("Wrong file extension.");
		return (false);
	}
	file.open(fileName.c_str());
	if (file.fail()){
		log.error("Could not open file.");
		file.close();
		return (false);
	}
	if (file.peek() == EOF){
		log.error("Empty file.");
		file.close();
		return (false);
	}
	file.close();
	return (true);
}

bool	checkArgs(int argc, char *argv[]) {
	Logger	log;

	if (argc != 2) {
		if (argc < 2)
			log.error("No configuration file\nUsage: ./webserv <name>.conf");
		else if (argc > 2)
			log.error("Too many arguments\nUsage: ./webserv <name>.conf");
		return false;
	}
	if (!checkFile(argv[1]))
		return false;
	log.info("File oppened successfully.");
	return true;
}

