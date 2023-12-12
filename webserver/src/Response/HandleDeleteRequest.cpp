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

    if (filePath.find("content/upload") == std::string::npos) {
        statusCode = 403;
    } else if (std::remove(filePath.c_str()) != 0) {
        statusCode = 500;
    } else {
        statusCode = 204;
    }

    Response response(request->fd(), statusCode);
    setResponseHeaders(response, "", "0", request);
    response.setMessage(getStatusMessage(statusCode));

	Logger::info("Response created");
    return response;
}
