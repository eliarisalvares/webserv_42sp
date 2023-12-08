#include "Response.hpp"

Response handleDeleteRequest(Request* request) {
    std::string filePath = request->path();
    Logger::debug("handling DELETE request - filePath: " + filePath);
    int statusCode = request->status_code();
    
    std::string contentType = getContentType(filePath);
    std::string body = getResponseBody(filePath, contentType, request);
    std::string message = getStatusMessage(statusCode);

    Response response(request->fd(), statusCode);
    response.setMessage(message);
    response.setBody(body);

    std::stringstream ss;
    ss << body.length();
    setResponseHeaders(response, contentType, ss.str(), request);

    return response;
}
