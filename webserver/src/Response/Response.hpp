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
# include <fstream> /* ifstream */
# include <cstdio> /* perror */
# include <unistd.h> /* close */
# include <cstring> /* memset */
# include <sys/time.h> /* gettimeofday */
# include <iomanip> /* setfill, setw */
# include <ctime> /* time_t, localtime, strftime */
# include <unistd.h> /* fork, execve, pipe, dup2, close */
# include <sys/wait.h> /* waitpid */
# include <errno.h> /* errno */

# include "Server.hpp"
# include "Request.hpp"
# include "Logger.hpp"
# include "http.hpp"
# include "ftstring.hpp"

# define HTML_CHAR ";charset=utf-8"

class Response {

public:
	Response(void);
	Response(int fd, int status_code);
	~Response(void);
	Response(Response const& copy);
	Response const& operator=(Response const& copy);

	int getFd(void) const;
	int getStatusCode(void) const;
	std::string getMessage(void) const;
	std::string getBody(void) const;
	std::string getHeader(const std::string& key) const;
	const std::map<std::string, std::string>& getHeaders(void) const;

	void setStatusCode(int status_code);
	void setMessage(std::string message);
	void setBody(std::string body);
	void addHeader(const std::string& key, const std::string& value);

	void sendResponse(void);

	std::string toString(void) const;

private:
	int			_fd;
	int			_status_code;
	std::string	_message;

	std::string body;
	std::map<std::string, std::string> headers;

};

Response	responseBuilder(Request* request);
Response	handleGetRequest(Request* request);
Response	handlePostRequest(Request* request);
Response	handleDeleteRequest(Request* request);
Response	handleMethodNotImplemented(Request* request);

bool 		isFileUpload(const std::string& body);
std::string getFileName(const std::string& body);
std::string getFilePath(const std::string& fileName);
Response 	processFileUpload(Request* request);
std::string getDefaultFilePath();
std::string getResponseBody(const std::string& filePath, const std::string& contentType);

std::string handleCGI(Request* request);
std::string getHtmlContent(const std::string& filePath);
std::string getContentType(const std::string& filePath);
std::string getStatusMessage(int statusCode);
std::string getCurrentDate(void);
void 		setResponseHeaders(Response& response, const std::string& contentType, const std::string& contentLength, Request* request);

#endif
