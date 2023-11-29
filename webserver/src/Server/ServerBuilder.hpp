#ifndef SERVERBUILDER_HPP
# define SERVERBUILDER_HPP

# include <iostream>
# include <sstream>
# include <fstream> // using files
# include <vector>
# include <map>
# include "ftstring.hpp"
# include "Logger.hpp"

int							getPortConf(std::vector<std::string> input, int index);
int							getBodySizeConf(std::vector<std::string> input, int index);
std::string					getRootConf(std::vector<std::string> input, int index);
std::vector<std::string>	getNameConf(std::vector<std::string> input, int index);

class PortNotFoundErrorExeption : public std::exception {
	virtual const char* what(void) const throw();
};

class PortNeedsSudoExeption : public std::exception {
	virtual const char* what(void) const throw();
};

#endif