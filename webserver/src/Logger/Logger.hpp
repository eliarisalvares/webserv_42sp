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

class Logger {
public:
	Logger(void);
	~Logger(void);

	static void error(std::string const msg);
	static void strerror(std::string const msg, int error);
	static void warning(std::string const msg);
	static void info(std::string const msg);
	static void debug(std::string const msg);
	static void success(std::string const msg);
	static void title(std::string const msg);

private:
	Logger(Logger const& copy);
	Logger const& operator=(Logger const& copy);

};

#endif
