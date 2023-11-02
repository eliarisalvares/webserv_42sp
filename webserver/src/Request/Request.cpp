#include "Request.hpp"

Request::Request(void) { }

Request::~Request(void) { }

Request::Request(Request const& copy) {
	*this = copy;
}

Request const& Request::operator=(Request const & copy) {
	// if (this != &copy)
	// 	this->server = copy.getServer();
	return *this;
}
