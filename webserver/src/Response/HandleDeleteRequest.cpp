#include "Response.hpp"

/**
 * @brief Deletes the file in the given path.
 * 
 * @param request the request object holding the request data that originated the response
 * @return Response the response object to be sent to the client containing the response data
 * (status code, message and headers)
 */
Response handleDeleteRequest(Request* request) {
    std::string filePath = request->path();
    Logger::debug("handleDeleteRequest - filePath: " + filePath);
    int statusCode = request->status_code();

    if (remove(filePath.c_str()) != 0) {
        statusCode = 500;
        Logger::error("handleDeleteRequest - Could not delete file: " + filePath);
    } else {
        statusCode = 204;
        Logger::debug("handleDeleteRequest - File deleted: " + filePath);
    }

    Response response(request->fd(), statusCode);
    setResponseHeaders(response, "", "0", request);
    response.setMessage(getStatusMessage(statusCode));
    return response;
}
