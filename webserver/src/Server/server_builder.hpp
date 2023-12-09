#ifndef SERVERBUILDER_HPP
# define SERVERBUILDER_HPP

# include <iostream>
# include <sstream>
# include <fstream> // using files
# include <vector>
# include <map>
# include <set>
# include <algorithm>
# include <dirent.h> //verifying or opening directories

# include "define_server.hpp"
# include "ftstring.hpp"
# include "Logger.hpp"
# include "http.hpp"

typedef struct	s_permissions
{
	bool	autoindex;
	bool	directory_listing;
	bool	has_redir;
	bool	redirExternal;
}				t_permissions;

typedef struct	s_location
{
	bool									cgi;
	std::string								location;
	std::string								root;
	std::string								redirection;
	std::set<std::string>					allowed_methods;
	std::set<std::string>					index;
	std::map<int, std::string>				error_pages;
	t_permissions							permit;
}				t_location;

bool						obtainAutoIndex(std::vector<std::string> input, int index);
bool						obtainDirList(std::vector<std::string> input, int index);
bool						obtainCGI(std::vector<std::string> input, int index);
int							obtainPort(std::vector<std::string> input, int index);
int							obtainBodySize(std::vector<std::string> input, int index);
int							obtainBufferSize(std::vector<std::string> input, int index);
std::string					obtainRoot(std::vector<std::string> input, int index);
std::string					obtainRedirect(std::vector<std::string> input, int index);
std::vector<std::string>	obtainName(std::vector<std::string> input, int index);
t_location					obtainLoc(std::vector<std::string> input, int index);
std::set<std::string>		obtainMethod(std::vector<std::string> input, int index);
std::pair<int, std::string>	obtainErrorPages(std::vector<std::string> input, int index);
std::set<std::string>		obtainIndex(std::vector<std::string> input, int index);
std::set<std::string>		obtainIndex(std::vector<std::string> input, int index, std::string root);

t_location					initLocation(void);

typedef unsigned int uint_t;

class PortNotFoundErrorExeption : public std::exception {
	virtual const char* what(void) const throw();
};

class PortNeedsSudoExeption : public std::exception {
	virtual const char* what(void) const throw();
};

class CGIWrongArgumentException : public std::exception {
	virtual const char* what(void) const throw();
};

class CGIInvalidException : public std::exception {
	virtual const char* what(void) const throw();
};

class MethodsWrongArgumentException : public std::exception {
	virtual const char* what(void) const throw();
};

class InvalidMethodsException : public std::exception {
	virtual const char* what(void) const throw();
};

class TooLargeException : public std::exception {
	virtual const char* what(void) const throw();
};

class BufferSizeInvalidException : public std::exception {
	virtual const char* what(void) const throw();
};

class LocationNotOpenedException : public std::exception {
	virtual const char* what(void) const throw();
};

class ErrPagesWrongArgumentException : public std::exception {
	virtual const char* what(void) const throw();
};

class ErrPagesInvalidException : public std::exception {
	virtual const char* what(void) const throw();
};

class InvalidFileException : public std::exception {
	virtual const char* what(void) const throw();
};

class AutoIndexWrongArgumentException : public std::exception {
	virtual const char* what(void) const throw();
};

class AutoIndexInvalidException : public std::exception {
	virtual const char* what(void) const throw();
};

class DirListWrongArgumentException : public std::exception {
	virtual const char* what(void) const throw();
};

class DirListInvalidException : public std::exception {
	virtual const char* what(void) const throw();
};

class RedirWrongArgumentException : public std::exception {
	virtual const char* what(void) const throw();
};

class PortAlreadyInUseException : public std::exception {
	virtual const char* what(void) const throw();
};

#endif
