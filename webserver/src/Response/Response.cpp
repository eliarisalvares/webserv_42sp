#include "Response.hpp"

Response::Response(void) { }

Response::~Response(void) { }

Response::Response(Response const& copy) { (void)copy; }

Response const& Response::operator=(Response const & copy) {
	(void)copy;
	return *this;
}
