#include "Response.hpp"

static const std::map<int, std::string> initializeMessageMap() {
    std::map<int, std::string> m;
    m.insert(std::make_pair(http::OK, "OK"));
    m.insert(std::make_pair(http::CREATED, "Created"));
    m.insert(std::make_pair(http::ACCEPTED, "Accepted"));
    m.insert(std::make_pair(http::NO_CONTENT, "No Content"));
    m.insert(std::make_pair(http::MOVED_PERMANENTLY, "Moved Permanently"));
    m.insert(std::make_pair(http::BAD_REQUEST, "Bad Request"));
    m.insert(std::make_pair(http::UNAUTHORIZED, "Unauthorized"));
    m.insert(std::make_pair(http::FORBIDDEN, "Forbidden"));
    m.insert(std::make_pair(http::NOT_FOUND, "Not Found"));
    m.insert(std::make_pair(http::METHOD_NOT_ALLOWED, "Method Not Allowed"));
    m.insert(std::make_pair(http::NOT_ACCEPTABLE, "Not Acceptable"));
    m.insert(std::make_pair(http::REQUEST_TIMEOUT, "Request Timeout"));
    m.insert(std::make_pair(http::CONFLICT, "Conflict"));
    m.insert(std::make_pair(http::LENGTH_REQUIRED, "Length Required"));
    m.insert(std::make_pair(http::CONTENT_TOO_LARGE, "Content Too Large"));
    m.insert(std::make_pair(http::URI_TOO_LONG, "URI Too Long"));
    m.insert(std::make_pair(http::UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"));
    m.insert(std::make_pair(http::INTERNAL_SERVER_ERROR, "Internal Server Error"));
    m.insert(std::make_pair(http::HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"));

    return m;
}

static const std::map<int, std::string> messageMap = initializeMessageMap();

std::string getStatusMessage(int statusCode) {
    std::map<int, std::string>::const_iterator it = messageMap.find(statusCode);
    if (it == messageMap.end())
        return "Internal Server Error";
    return it->second;
}
