#include "Response.hpp"

Response handleDeleteRequest(Request* request) {
    std::string filePath = request->uri();
    Logger::debug("handleDeleteRequest - filePath: " + filePath);

    if (filePath == "/") {
        filePath = getDefaultFilePath();
    }

    std::string contentType = getContentType(filePath);
    std::string body = getResponseBody(filePath, contentType);
    int statusCode = body.empty() ? 404 : 200;
    std::string message = getStatusMessage(statusCode);

    Response response(request->fd(), statusCode);
    response.setMessage(message);
    response.setBody(body);

    std::stringstream ss;
    ss << body.length();
    setResponseHeaders(response, contentType, ss.str(), request);

    return response;
}