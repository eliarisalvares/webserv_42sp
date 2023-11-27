#ifndef FTSTRING_HPP
#define FTSTRING_HPP

#include <sstream>
#include <string>
#include <vector>

// functions for manipulating strings

namespace ftstring {

std::string	trimWhitespace(std::string const &line);
int			strtoi(std::string const &str);
std::string	itostr(int nbr);

} /* ftstring */

#endif