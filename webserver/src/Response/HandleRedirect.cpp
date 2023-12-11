
#include "Response.hpp"

Response	handleRedirect(Request* request) {
	std::string redirection = request->path();
    Logger::debug("handleRedirect - redirects to: " + redirection);
    int statusCode = request->status_code();

	Response response(request->fd(), statusCode);
	std::string message = getStatusMessage(statusCode);
    response.setMessage(message);

	std::string contentType, contentLength;
    Logger::debug("entering setResponseHeaders");
	setResponseHeaders(response, contentType, contentLength, request);
    Logger::debug("saiu setResponseHeaders");

    return response;
}
