/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:44:59 by feralves          #+#    #+#             */
/*   Updated: 2023/11/27 13:57:01 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerParser.hpp"

ServerParser::ServerParser(void) {
	_nbrServers = 0;
}

ServerParser::~ServerParser(void) { }

ServerParser::ServerParser(ServerParser const& copy) { (void)copy; }

ServerParser const& ServerParser::operator=(ServerParser const & copy) {
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

bool	ServerParser::_beginingOfFile() {
	if (_lines[0].substr() != "server {")
		return false;
	return true;
}

bool	ServerParser::_bracketsClosed() {
	int	openBrackets = 0;
	int	closeBrackets = 0;
	
	for (size_t i = 0; i < this->_fileContent.size(); i++) {
		if (this->_fileContent[i] == '{')
			openBrackets++;
		else if (this->_fileContent[i] == '}')
			closeBrackets++;
	}
	if (openBrackets == closeBrackets)
		return true;
	return false;
}

bool	ServerParser::_minimalRequirements(int index) {
	bool	check = false;

	while (_lines[index].substr() != "}") {
		if (_lines[index].substr(0, 6) == "listen")
			check = true;
		index++;
	}
	return check;
}

void	ServerParser::getConf(std::string fileName) {
	std::ifstream		inputFile(fileName.c_str());
	std::string			line;
	bool				check = false;

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
	for (size_t i = 0; i < _lines.size(); i++)
	{
		while (_lines[i].substr() != "server {") {
			if (check == false) {
				check = _minimalRequirements(i);
			}
			if (_lines[i].substr() == "}" && _lines[i - 1].substr() == "server {")
				throw ListenNotFoundErrorExeption();
			else if ( _lines[i].substr() == "}") 
				break ;
			i++;
		}
		if (i && !check)
			throw ListenNotFoundErrorExeption();
	}
}

void	ServerParser::servers(void) {
	for (size_t i = 0; i < _lines.size(); i++) {
		// t_infoServer	newServer;

		getServerInfo(_lines, i);
		// newServer = getServerInfo(_lines, i);
		if (_lines[i].substr() == "server {") {
			while ( _lines[i].substr() != "}") 
				i++;
		}
		// _servers.push_back(newServer);
	}
}

const char* ServerParser::ListenNotFoundErrorExeption::what() const throw() {
	return ("Port to listen not found.");
}

const char* ServerParser::SyntaxErrorException::what() const throw() {
	return ("Curly brackets not closed.");
}

const char* ServerParser::ServerErrorException::what() const throw() {
	return ("Server not at begining of file.");
}
