#include "Response.hpp"

static const std::map<std::string, std::string> initializeContentTypeMap() {
	std::map<std::string, std::string> m;
	m.insert(std::make_pair(".html", "text/html"));
	m.insert(std::make_pair(".htm", "text/html"));
	m.insert(std::make_pair(".txt", "text/plain"));
	m.insert(std::make_pair(".css", "text/css"));
	m.insert(std::make_pair(".js", "application/javascript"));
	m.insert(std::make_pair(".json", "application/json"));
	m.insert(std::make_pair(".xml", "application/xml"));
	m.insert(std::make_pair(".png", "image/png"));
	m.insert(std::make_pair(".jpg", "image/jpeg"));
	m.insert(std::make_pair(".jpeg", "image/jpeg"));
	m.insert(std::make_pair(".gif", "image/gif"));
	return m;
}

static const std::map<std::string, std::string> contentTypeMap = initializeContentTypeMap();

std::string getContentType(int statusCode, const std::string& filePath) {
	size_t	dot;

	if (statusCode >= 400 && statusCode >= 505)
		return "text/html";
	dot = filePath.find_last_of('.');
	if (dot == std::string::npos)
		return "text/plain";
	std::string extension = filePath.substr(dot);
	std::map<std::string, std::string>::const_iterator it = contentTypeMap.find(extension);
	if (it == contentTypeMap.end())
		return "text/plain";
	return it->second;
}

std::string getHtmlContent(const std::string& filePath) {
	std::ifstream file(filePath.c_str());
	if (!file.is_open())
		throw std::runtime_error("Could not open file: " + filePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string getCurrentDate() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t now = tv.tv_sec;
	struct tm *bt = localtime(&now);
	char dateStr[20];
	strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", bt);
	std::ostringstream oss;
	oss << dateStr;
	oss << '.' << std::setfill('0') << std::setw(6) << tv.tv_usec;
	return oss.str();
}

void setResponseHeaders(Response& response, const std::string& contentType, const std::string& contentLength) {
	Request request;

	response.addHeader("Content-Type", contentType);
	response.addHeader("Content-Length", contentLength);
	response.addHeader("Date", getCurrentDate());
	response.addHeader("Server", "localhost"); // TODO: get hostname from server
	response.addHeader("Access-Control-Allow-Origin", "*");
	response.addHeader("Access-Control-Allow-Methods", "GET, POST, DELETE"); // TODO: get allowed methods from server
	response.addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
	response.addHeader("Access-Control-Allow-Credentials", "true");
	response.addHeader("Cache-Control", "no-cache");
}

std::string	setFlagsContent(std::string contentType) {
	std::string	result;

	result = contentType;
	if (contentType == "text/html") {
		result += HTML_CHAR;
	}
	return result;
}

Response responseBuilder(Request* request) {
	Response response(request->fd(), request->status_code());
	std::string filePath;
	int		statusCode = request->status_code();

	Logger::debug("creating response...");
	if (request->has_error()) {
		// adicionar lógica pra pegar o arquivo certo se tiver setado no server
		filePath = (
			std::string("content/error_pages/")
			+ ftstring::itostr(statusCode)
			+ std::string(".html") // pode dar erro se o arquivo do erro não existir
		);
	}
	else {
		// get uri from request
		filePath = request->uri();
		std::cout << "filePath: " << filePath << std::endl;

		// isso aqui é um teste:
		if (filePath == "/") {
			if (access("content/index.html", F_OK) == 0) {
				filePath = "content/index.html";
			} else {
				filePath = "content/cgi/autoindex.py";
			}
		}
	}
	std::cout << "filePath: " << filePath << std::endl;

	std::string contentType = getContentType(statusCode, filePath);

	std::string body = "";
	if (filePath.find("/cgi/") != std::string::npos) {
		body = handleCGI();
		contentType = "text/html";
	} else if (contentType == "text/html") {
		body = getHtmlContent(filePath);
	}
	contentType = setFlagsContent(contentType);
	if (!request->has_error())
		statusCode = body.empty() ? 404 : 200;
	std::string message = getStatusMessage(statusCode);

	response.setStatusCode(statusCode);
	response.setMessage(message);
	response.setBody(body);

	std::stringstream ss;
	ss << body.length();

	setResponseHeaders(response, contentType, ss.str());

	return response;
}
