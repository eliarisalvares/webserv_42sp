#include "Response.hpp"

/* Canonical Form
*/

Response::Response(void) { 
	this->_status_code = http::OK;
	this->_message = "OK";
}

Response::Response(Request* request) {
	this->_fd = request->fd();
	this->_status_code = request->status_code();
	if (request->has_error()) {
		//resposta vai ser uma página de erro
	}
	
	

	Logger::debug("creating response...");
	this->_message = responseBuilder(request, response);
}

Response::~Response(void) { }

Response::Response(Response const& copy) { 
	(void)copy;
}

Response const& Response::operator=(Response const & copy) {
	(void)copy;
	return *this;
}

/* Setters 
*/

void Response::setStatusCode(int status_code) {
	this->_status_code = status_code;
}

void Response::setMessage(std::string message) {
	this->_message = message;
}

void Response::setBody(std::string body) {
	this->body = body;
}

void Response::addHeader(const std::string& key, const std::string& value) {
	headers[key] = value;
}

/* Getters
*/

int Response::getStatusCode(void) const {
	return _status_code;
}

std::string Response::getMessage(void) const {
	return _message;
}

std::string Response::getBody(void) const {
	return body;
}

std::string Response::getHeader(const std::string& key) const {
	return headers.at(key);
}

const std::map<std::string, std::string>& Response::getHeaders(void) const {
	return headers;
}

/* Method to return the response as a string
*/
std::string Response::toString() const {
    std::ostringstream response_stream;

    response_stream << "HTTP/1.1 " << _status_code << " " << _message << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response_stream << it->first << ": " << it->second << "\r\n";
    }
    response_stream << "\r\n";
    response_stream << body;

    return response_stream.str();
}

void Response::sendResponse(void) {
	Logger::debug("sending response...");
	send(this->_fd, this->_message.c_str(), this->_message.length(), 0);
}