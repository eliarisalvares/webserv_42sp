/*
Class to log error, warning or debug messages
*/

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <iostream>
# include <cerrno>
# include <cstring>

# define ORANGE	"\033[0;38;5;166m"
# define BLUE	"\033[38;5;75m"
# define GREEN	"\033[1;32m"
# define RED	"\033[38;5;196m"
# define PURPLE	"\033[38;5;200m"
# define YELLOW	"\033[1;33m"
# define GREY	"\033[38;5;244m"
# define RESET	"\033[0m"

# ifndef DEBUG
    #define DEBUG true
# endif

# define DEBUG_LOG "[ DEBUG ] "
# define INFO_LOG "[ INFO ] "
# define WARNING_LOG "[ WARNING ] "
# define ERROR_LOG "[ ERROR ] "

namespace Logger {
void error(std::string const msg);
void error(std::string const msg, std::string const value);
void error_no_lf(std::string const msg);
void strerror(std::string const msg, int error);
void warning(std::string const msg);
void warning(std::string const msg, std::string const value);
void warning(std::string const msg, int const value);
void warning_no_lf(std::string const msg);
void info(std::string const msg);
void info(std::string const msg, std::string const value);
void info(std::string const msg, int const value);
void info_no_lf(std::string const msg);
void debug(std::string const msg);
void debug(std::string const msg, std::string const value);
void debug(std::string const msg, int const value);
void debug_no_lf(std::string const msg);
void success(std::string const msg);
void title(std::string const msg);
} /* Logger */

#endif
