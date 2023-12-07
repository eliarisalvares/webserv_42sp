#include "Logger.hpp"

namespace Logger {
void	error(std::string const msg) {
	std::cerr << ORANGE << ERROR_LOG << msg << RESET << std::endl;
}

void	error(std::string const msg, std::string const value) {
	std::cerr << ORANGE << ERROR_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void	error_no_lf(std::string const msg) {
	std::cerr << ORANGE << ERROR_LOG << msg << RESET;
}

void	strerror(std::string const msg, int error) {
	std::cerr << ORANGE << ERROR_LOG << msg << ": "
		<< std::strerror(error) << RESET;
}

void	warning(std::string const msg) {
	std::cout << YELLOW << WARNING_LOG << msg << RESET << std::endl;
}

void	warning(std::string const msg, std::string const value) {
	std::cout << YELLOW << WARNING_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void	warning(std::string const msg, int const value) {
	std::cout << YELLOW << WARNING_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void	warning_no_lf(std::string const msg) {
	std::cout << YELLOW << WARNING_LOG << msg << RESET;
}

void	info(std::string const msg) {
	std::cout << BLUE << INFO_LOG << msg << RESET << std::endl;
}

void	info(std::string const msg, std::string const value) {
	std::cout << BLUE << INFO_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void	info(std::string const msg, int const value) {
	std::cout << BLUE << INFO_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void	info_no_lf(std::string const msg) {
	std::cout << BLUE << INFO_LOG << msg << RESET;
}

void	debug(std::string const msg) {
	if (DEBUG)
		std::cout << GREY << DEBUG_LOG << msg << RESET << std::endl;
}

void	debug(std::string const msg, std::string const value) {
	if (DEBUG)
		std::cout << GREY << DEBUG_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void	debug(std::string const msg, int const value) {
	if (DEBUG)
		std::cout << GREY << DEBUG_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void	debug_no_lf(std::string const msg) {
	if (DEBUG)
		std::cout << GREY << DEBUG_LOG << msg << RESET;
}

void	success(std::string const msg) {
	std::cout << GREEN << msg << RESET << std::endl;
}

void	title(std::string const msg) {
	std::cout << PURPLE << msg << RESET << std::endl;
}
}