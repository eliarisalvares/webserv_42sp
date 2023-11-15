/*
Response Class
*/

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream> /* cout */
# include <cstdlib> /* exit */
# include <map> /* map */
# include <string> /* string */
# include <sstream> /* stringstream */
#include <fstream> /* ifstream */

class Response {

public:
	Response(void);
	Response(int status_code, std::string message, std::string body, std::map<std::string, std::string> headers);
	~Response(void);
	Response(Response const& copy);
	Response const& operator=(Response const& copy);

	int getStatusCode(void) const;
	std::string getMessage(void) const;
	std::string getBody(void) const;
	std::string getHeader(const std::string& key) const;
	const std::map<std::string, std::string>& getHeaders(void) const;

	void setStatusCode(int status_code);
	void setMessage(std::string message);
	void setBody(std::string body);
	void addHeader(const std::string& key, const std::string& value);

	std::string toString(void) const;

private:
	int status_code;
	std::string message;
	std::string body;
	std::map<std::string, std::string> headers;

};

std::string responseBuilder(int statusCode, const std::string& message, const std::string& body, const std::string& contentType);
std::string getHtmlContent(const std::string& filePath);
std::string getContentType(const std::string& filePath);
std::string getStatusMessage(int statusCode);
std::string getCurrentDate(void);
void setResponseHeaders(Response& response, const std::string& contentType, const std::string& contentLength);

#endif