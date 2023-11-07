/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:44:59 by feralves          #+#    #+#             */
/*   Updated: 2023/11/07 18:16:09 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParserConfig.hpp"

ParserConfig::ParserConfig(void) { }

ParserConfig::~ParserConfig(void) { }

ParserConfig::ParserConfig(ParserConfig const& copy) { (void)copy; }

ParserConfig const& ParserConfig::operator=(ParserConfig const & copy) {
	(void)copy;
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

void	ParserConfig::keepFile(std::string fileName) {
	std::ifstream		inputFile(fileName.c_str());
	std::stringstream	inFile;
	std::string			line;

	inFile << inputFile.rdbuf();
	this->_fileContent = inFile.str();

	if (!_beginingOfFile())
		throw ;//error of not server in the begining
	if (!_bracketsClosed())
		throw ;//error of syntax

	while (std::getline(inputFile, line)) {
		line = trim(line);
		_lines.push_back(line);
	}
	inputFile.close();
}

bool	ParserConfig::_beginingOfFile() {
	//check if server block is at begining
	return (true);
}

bool	ParserConfig::_bracketsClosed() {
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

const char* ParserConfig::SyntaxErrorException::what() const throw() {
	return ("Error: Curly brackets not closed.");
}

const char* ParserConfig::ServerErrorException::what() const throw() {
	return ("Error: Server not at begining of file.");
}
