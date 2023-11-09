#include "Logger.hpp"

Logger::Logger(void) { }

Logger::~Logger(void) { }

Logger::Logger(Logger const& copy) { (void)copy; }

Logger const& Logger::operator=(Logger const & copy) {
	(void)copy;
	return *this;
}

void Logger::error(std::string const msg) {
	std::cerr << ORANGE << "[ ERROR ] " << msg << RESET << std::endl;
}

void Logger::warning(std::string const msg) {
	std::cout << YELLOW << "[ WARNING ] " << msg << RESET << std::endl;
}

void Logger::info(std::string const msg) {
	std::cout << BLUE << "[ INFO ] " << msg << RESET << std::endl;
}

void Logger::debug(std::string const msg) {
	if (DEBUG)
		std::cout << GREY << "[ DEBUG ] " << msg << RESET << std::endl;
}

void Logger::success(std::string const msg) {
	std::cout << GREEN << msg << RESET << std::endl;
}

void Logger::title(std::string const msg) {
	std::cout << PURPLE << msg << RESET << std::endl;
}
