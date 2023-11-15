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

std::string getContentType(const std::string& filePath) {
    std::string extension = filePath.substr(filePath.find_last_of('.'));
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

std::string getCurrentDate(void)
{
    time_t   now;
    struct tm  ts;
    char       buf[80];

    time(&now);
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &ts);
    return buf;
}

void setResponseHeaders(Response& response, const std::string& contentType, const std::string& contentLength) {
    response.addHeader("Content-Type", contentType);
    response.addHeader("Content-Length", contentLength);
    response.addHeader("Date", getCurrentDate());
    response.addHeader("Server", "Webserv WebWizards");
    response.addHeader("Access-Control-Allow-Origin", "*");
    response.addHeader("Access-Control-Allow-Methods", "GET, POST, DELETE");
    response.addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    response.addHeader("Access-Control-Allow-Credentials", "true");
    response.addHeader("Cache-Control", "no-cache");
}


std::string responseBuilder(const std::string& filePath) {
    Response response;

    std::string contentType = getContentType(filePath);
    std::string body = getHtmlContent(filePath);
    int statusCode = body.empty() ? 404 : 200;
    std::string message = getStatusMessage(statusCode);

    response.setStatusCode(statusCode);
    response.setMessage(message);
    response.setBody(body);

    std::stringstream ss;
    ss << body.length();

    setResponseHeaders(response, contentType, ss.str());

    return response.toString();
}

