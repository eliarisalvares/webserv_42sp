#ifndef SERVERBUILDER_HPP
# define SERVERBUILDER_HPP

# include <iostream>
# include <sstream>
# include <fstream> // using files
# include <vector>
# include <map>
# include <set>
# include "define_server.hpp"
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
	bool									cgi; // .py python3
	std::string								location; // "/"
	std::string								root; // "/content"
	std::set<std::string>					allowed_methods; //std::set<std::string> _fill_methods(void)
	std::set<std::string>					index;
	t_permissions							permit;
	// std::vector<std::string>				http_methods;
	// std::pair<unsigned int, std::string>	http_redirection;
}				t_location;

bool						obtainCGI(std::vector<std::string> input, int index);
int							obtainPort(std::vector<std::string> input, int index);
int							obtainBodySize(std::vector<std::string> input, int index);
int							obtainBufferSize(std::vector<std::string> input, int index);
std::string					obtainRoot(std::vector<std::string> input, int index);
std::vector<std::string>	obtainName(std::vector<std::string> input, int index);
t_location					obtainLoc(std::vector<std::string> input, int index);
std::set<std::string>		obtainMethod(std::vector<std::string> input, int index);
std::pair<int, std::string>	obtainErrorPages(std::vector<std::string> input, int index);
std::set<std::string>		obtainIndex(std::vector<std::string> input, int index);

t_location					initLocation(void);

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

class TooLargeException : public std::exception {
	virtual const char* what(void) const throw();
};

#endif