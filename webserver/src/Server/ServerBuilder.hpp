#ifndef SERVERBUILDER_HPP
# define SERVERBUILDER_HPP

# include <iostream>
# include <sstream>
# include <fstream> // using files
# include <vector>
# include <map>
# include "ftstring.hpp"
# include "Logger.hpp"

typedef std::map<std::string, std::string>	t_infoServer;

int	getPortConf(std::vector<std::string> input, int index);

class PortNotFoundErrorExeption : public std::exception {
	virtual const char* what(void) const throw();
};

class PortNeedsSudoExeption : public std::exception {
	virtual const char* what(void) const throw();
};

#endif