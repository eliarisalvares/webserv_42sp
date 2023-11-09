/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:44:59 by feralves          #+#    #+#             */
/*   Updated: 2023/11/09 18:40:30 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParserServer.hpp"

ParserServer::ParserServer(void) { }

ParserServer::~ParserServer(void) { }

ParserServer::ParserServer(ParserServer const& copy) { (void)copy; }

ParserServer const& ParserServer::operator=(ParserServer const & copy) {
	if (this != &copy)
		return copy;
	return *this;
}

static std::string trim(const std::string& line)
{
	std::string trimmed = line;

	size_t start = trimmed.find_first_not_of(" \t");
	if (start != std::string::npos) {
		trimmed = trimmed.substr(start);
	}

	size_t end = trimmed.find_last_not_of(" \t");
	if (end != std::string::npos) {
		trimmed = trimmed.substr(0, end + 1);
	}
	return trimmed;
}

void	ParserServer::getConf(std::string fileName) {
	std::ifstream		inputFile(fileName.c_str());
	std::string			line;

	while (std::getline(inputFile, line)) {
		line = trim(line); //clear spaces and tabs -> do we need to keep the tabs to check for the spacing?
		if (line.size() > 0)
			_lines.push_back(line);
	}
	inputFile.close();
	if (!_beginingOfFile())
		throw ServerErrorException(); //error of not server in the begining
	if (!_bracketsClosed())
		throw SyntaxErrorException(); //error of syntax
	std::cout << _lines.size() << std::endl;
	for (size_t i = 0; i < _lines.size(); i++)
	{
		std::cout << _lines[i] << std::endl;
	}
}

bool	ParserServer::_beginingOfFile() {
	if (_lines[0].substr() != "server {")
		return false;
	return true;
}

bool	ParserServer::_bracketsClosed() {
	int	openBrackets = 0;
	int	closeBrackets = 0;
	
	for (size_t i = 0; i < this->_fileContent.size(); i++) {
		if (this->_fileContent[i] == '{')
			openBrackets++;
		else if (this->_fileContent[i] == '}')
			closeBrackets++;
	}
	if (openBrackets == closeBrackets)
		return (true);
	return (false);
}

const char* ParserServer::SyntaxErrorException::what() const throw() {
	return ("Curly brackets not closed.");
}

const char* ParserServer::ServerErrorException::what() const throw() {
	return ("Server not at begining of file.");
}
