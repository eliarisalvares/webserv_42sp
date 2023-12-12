#include "Response.hpp"

/* Canonical Form
*/

Response::Response(void) {
	this->_status_code = http::OK;
	this->_message = "OK";
}

Response::Response(int fd, int status_code) {
	this->_fd = fd;
	this->_status_code = status_code;
}

Response::~Response(void) { }

Response::Response(Response const& copy) {
	*this = copy;
}

Response const& Response::operator=(Response const & copy) {
	if (this != &copy) {
		this->_fd = copy.getFd();
		this->_status_code = copy.getStatusCode();
		this->_message = copy.getMessage();
		this->body = copy.getBody();
		this->headers = copy.getHeaders();
	}
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

int Response::getFd(void) const {
	return _status_code;
}

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

/**
 * @brief Method to convert the response object to a string to be sent to the client.
 *
 * @return std::string
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

/**
 * @brief Method to send the response to the client.
 *
 */
void Response::sendResponse(void) {
	Logger::debug("sending response...");
	std::string response_string = this->toString();

	send(this->_fd, response_string.c_str(), response_string.length(), 0);
}
