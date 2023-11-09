#include "Response.hpp"

static const std::map<int, std::string> initializeMessageMap() {
    std::map<int, std::string> m;
    m.insert(std::make_pair(200, "OK"));
    m.insert(std::make_pair(201, "Created"));
    m.insert(std::make_pair(202, "Accepted"));
    m.insert(std::make_pair(204, "No Content"));
    m.insert(std::make_pair(400, "Bad Request"));
    m.insert(std::make_pair(401, "Unauthorized"));
    m.insert(std::make_pair(404, "Not Found"));
    m.insert(std::make_pair(405, "Method Not Allowed"));
    m.insert(std::make_pair(406, "Not Acceptable"));
    m.insert(std::make_pair(408, "Request Timeout"));
    m.insert(std::make_pair(409, "Conflict"));
    m.insert(std::make_pair(413, "Payload Too Large"));
    m.insert(std::make_pair(414, "URI Too Long"));
    m.insert(std::make_pair(415, "Unsupported Media Type"));
    m.insert(std::make_pair(418, "I'm a teapot"));
    m.insert(std::make_pair(500, "Internal Server Error"));

    return m;
}

static const std::map<int, std::string> messageMap = initializeMessageMap();

std::string getStatusMessage(int statusCode) {
    std::map<int, std::string>::const_iterator it = messageMap.find(statusCode);
    if (it == messageMap.end())
        return "Internal Server Error";
    return it->second;
}