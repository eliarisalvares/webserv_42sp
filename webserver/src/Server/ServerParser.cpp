/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feralves <feralves@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 16:44:59 by feralves          #+#    #+#             */
/*   Updated: 2023/11/28 15:39:30 by feralves         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerParser.hpp"

ServerParser::ServerParser(void) {
	nbrServers = 0;
}

ServerParser::~ServerParser(void) { }

ServerParser::ServerParser(ServerParser const& copy) { (void)copy; }

ServerParser const& ServerParser::operator=(ServerParser const & copy) {
	if (this != &copy)
		return copy;
	return *this;
}

bool	ServerParser::_beginingOfFile() {
	if (_lines[0].substr() != "server {")
		return false;
	return true;
}

bool	ServerParser::_bracketsClosed() {
	int	openBrackets = 0;
	int	closeBrackets = 0;

	for (size_t i = 0; i < _lines.size(); i++) {
		for (size_t j = 0; j < _lines[i].length(); j++) {
			if (_lines[i][j] == '{')
				openBrackets++;
			else if (_lines[i][j] == '}')
				closeBrackets++;
		}
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

void	ServerParser::setConf(std::string fileName) {
	std::ifstream		inputFile(fileName.c_str());
	std::string			line;
	bool				check = false;
	int					extraBrackets = 0;

	while (std::getline(inputFile, line)) {
		line = ftstring::trimWhitespace(line);
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
		if (_lines[i].substr() == "server {")
			nbrServers++;
		while (_lines[i].substr() != "server {") {
			if (check == false) {
				check = _minimalRequirements(i);
			}
			if (_lines[i].substr(0, 9) == "location ")
				extraBrackets++;
			if (_lines[i].substr() == "}" && _lines[i - 1].substr() == "server {")
				throw EmptyServerErrorExeption();
			else if ( _lines[i].substr() == "}" && extraBrackets)
				extraBrackets--;
			else if ( _lines[i].substr() == "}" && !extraBrackets) 
				break ;
			i++;
		}
		if (i && !check)
			throw ListenNotFoundErrorExeption();
	}
}

std::vector<std::string>	ServerParser::getConf(void) {
	return _lines;
}

const char* ServerParser::ListenNotFoundErrorExeption::what() const throw() {
	return ("Listen token to set port not found.");
}

const char* ServerParser::EmptyServerErrorExeption::what() const throw() {
	return ("Server with no content.");
}

const char* ServerParser::SyntaxErrorException::what() const throw() {
	return ("Close brackets \'}\' not closed.");
}

const char* ServerParser::ServerErrorException::what() const throw() {
	return ("Unknown token, expect server.");
}
