#include "Response.hpp"

/* Canonical Form
*/

Response::Response(void) { 
	status_code = 200;
	message = "OK";
}

Response::Response(int status_code, std::string message, std::string body, std::map<std::string, std::string> headers) {
	this->status_code = status_code;
	this->message = message;
	this->body = body;
	this->headers = headers;
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
	this->status_code = status_code;
}

void Response::setMessage(std::string message) {
	this->message = message;
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
	return status_code;
}

std::string Response::getMessage(void) const {
	return message;
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

    response_stream << "HTTP/1.1 " << status_code << " " << message << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response_stream << it->first << ": " << it->second << "\r\n";
    }
    response_stream << "\r\n";
    response_stream << body;

    return response_stream.str();
}
