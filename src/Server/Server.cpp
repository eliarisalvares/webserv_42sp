#include "Server.hpp"

Server::Server(void) { }

Server::~Server(void) { }

Server::Server(Server const& copy) { (void)copy; }

Server const& Server::operator=(Server const & copy) {
	// if (this != &copy)
		// do stuff
	return *this;
}
