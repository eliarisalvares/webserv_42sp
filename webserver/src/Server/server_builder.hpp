#ifndef SERVERBUILDER_HPP
# define SERVERBUILDER_HPP

# include <iostream>
# include <sstream>
# include <fstream> // using files
# include <vector>
# include <map>
# include <set>
# include "ftstring.hpp"
# include "Logger.hpp"
# include "http.hpp"

typedef struct	s_permissions
{
	bool	autoindex;
	bool	directory_listing;
}				t_permissions;

typedef struct	s_location
{
	std::string								location;
	std::string								root;
	std::set<std::string>					allowed_methods; //std::set<std::string> _fill_methods(void)
	std::set<std::string>					cgi;
	std::vector<std::string>				http_methods;
	std::pair<unsigned int, std::string>	http_redirection;
	t_permissions							permit;
	// std::string								response_is_dir;
	// std::string								directory_listing;
}				t_location;

int							obtainPort(std::vector<std::string> input, int index);
int							obtainBodySize(std::vector<std::string> input, int index);
std::string					obtainRoot(std::vector<std::string> input, int index);
std::vector<std::string>	obtainName(std::vector<std::string> input, int index);
t_location					obtainLoc(std::vector<std::string> input, int index);
std::vector<std::string>	obtainCGI(std::vector<std::string> input, int index);
std::set<std::string>		obtainMethod(std::vector<std::string> input, int index);
std::pair<int, std::string>	obtainErrorPages(std::vector<std::string> input, int index);
std::set<std::string>		obtainIndex(std::vector<std::string> input, int index);

typedef unsigned int uint_t;

class PortNotFoundErrorExeption : public std::exception {
	virtual const char* what(void) const throw();
};

class PortNeedsSudoExeption : public std::exception {
	virtual const char* what(void) const throw();
};

class CGIMissconfigurationException : public std::exception {
	virtual const char* what(void) const throw();
};

class CGINotSupportedException : public std::exception {
	virtual const char* what(void) const throw();
};

class InvalidNbrMethodsException : public std::exception {
	virtual const char* what(void) const throw();
};

class InvalidMethodsException : public std::exception {
	virtual const char* what(void) const throw();
};

#endif