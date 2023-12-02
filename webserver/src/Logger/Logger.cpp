#include "Logger.hpp"

Logger::Logger(void) { }

Logger::~Logger(void) { }

Logger::Logger(Logger const& copy) { (void)copy; }

Logger const& Logger::operator=(Logger const & copy) {
	(void)copy;
	return *this;
}

void Logger::error(std::string const msg) {
	std::cerr << ORANGE << ERROR_LOG << msg << RESET << std::endl;
}

void Logger::error(std::string const msg, std::string const value) {
	std::cerr << ORANGE << ERROR_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void Logger::error_no_lf(std::string const msg) {
	std::cerr << ORANGE << ERROR_LOG << msg << RESET;
}

void Logger::strerror(std::string const msg, int error) {
	std::cerr << ORANGE << ERROR_LOG << msg << ": "
		<< std::strerror(error) << RESET;
}

void Logger::warning(std::string const msg) {
	std::cout << YELLOW << WARNING_LOG << msg << RESET << std::endl;
}

void Logger::warning(std::string const msg, std::string const value) {
	std::cout << YELLOW << WARNING_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void Logger::warning_no_lf(std::string const msg) {
	std::cout << YELLOW << WARNING_LOG << msg << RESET;
}

void Logger::info(std::string const msg) {
	std::cout << BLUE << INFO_LOG << msg << RESET << std::endl;
}

void Logger::info(std::string const msg, std::string const value) {
	std::cout << BLUE << INFO_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void Logger::info_no_lf(std::string const msg) {
	std::cout << BLUE << INFO_LOG << msg << RESET;
}

void Logger::debug(std::string const msg) {
	if (DEBUG)
		std::cout << GREY << DEBUG_LOG << msg << RESET << std::endl;
}

void Logger::debug(std::string const msg, std::string const value) {
	if (DEBUG)
		std::cout << GREY << DEBUG_LOG << msg << ": |" << value << "|"
			<< RESET << std::endl;
}

void Logger::debug_no_lf(std::string const msg) {
	if (DEBUG)
		std::cout << GREY << DEBUG_LOG << msg << RESET;
}

void Logger::success(std::string const msg) {
	std::cout << GREEN << msg << RESET << std::endl;
}

void Logger::title(std::string const msg) {
	std::cout << PURPLE << msg << RESET << std::endl;
}
