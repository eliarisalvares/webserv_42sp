#include "Logger.hpp"

Logger::Logger(void) { }

Logger::~Logger(void) { }

Logger::Logger(Logger const& copy) { (void)copy; }

Logger const& Logger::operator=(Logger const & copy) {
	(void)copy;
	return *this;
}

void Logger::error(std::string const msg) {
	std::cout << ORANGE << msg << RESET;
}

void Logger::warning(std::string const msg) {
	std::cout << YELLOW << msg << RESET;
}

void Logger::info(std::string const msg) {
	std::cout << BLUE << msg << RESET;
}

void Logger::debug(std::string const msg) {
	if (DEBUG)
		std::cout << GREY << msg << RESET;
}

void Logger::success(std::string const msg) {
	std::cout << GREEN << msg << RESET;
}

void Logger::title(std::string const msg) {
	std::cout << PURPLE << msg << RESET;
}