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

std::string responseBuilder(int statusCode, const std::string& message, const std::string& body, const std::string& contentType) {
    Response response;
    response.setStatusCode(statusCode);
    response.setMessage(message);
    response.setBody(body);
    std::stringstream ss;
    ss << body.length();
    response.addHeader("Content-Type", contentType);
    response.addHeader("Content-Length", ss.str());
    return response.toString();
}