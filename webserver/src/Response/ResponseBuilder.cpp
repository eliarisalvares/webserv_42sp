#include "Response.hpp"

/**
 * @brief Sets the response headers for the response.
 *
 * @param response the response to set the headers to
 * @param contentType the content type of the response
 * @param contentLength the content length of the response
 * @param request the request object holding the request data that originated the response
 */
void setResponseHeaders(Response& response, const std::string& contentType, const std::string& contentLength, Request* request) {
    Server *server = request->server();
	int statusCode = request->status_code();

    std::string serverName = server->getName(0);
    std::set<std::string> serverMethods = server->getMethods();
    std::string methodsStr = "";
    for (std::set<std::string>::iterator it = serverMethods.begin(); it != serverMethods.end(); it++) {
        methodsStr += *it;
        if (it != --serverMethods.end())
            methodsStr += ", ";
    }

	if (statusCode == http::MOVED_PERMANENTLY) {
		response.addHeader("Location", request->path());
	}
	else {
		response.addHeader("Content-Type", contentType);
		response.addHeader("Content-Length", contentLength);
	}
    response.addHeader("Date", getCurrentDate());
    response.addHeader("Host", request->host());
    response.addHeader("Server", serverName);
    response.addHeader("Access-Control-Allow-Methods", methodsStr);
    response.addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    response.addHeader("Access-Control-Allow-Credentials", "true");
}


/**
 * @brief Checks the requested method and calls the appropriate
 * handler function for it.
 */
Response responseBuilder(Request* request) {
    http::RequestMethod method = request->method();
    Logger::debug("Creating response...");

    std::string methodStr = http::enum_to_str_method(method);

    if (request->status_code() == http::MOVED_PERMANENTLY)
		return handleRedirect(request);
	if (methodStr == "DELETE") {
        return handleDeleteRequest(request);
    } else if (methodStr == "POST") {
        return handlePostRequest(request);
    }
    return handleGetRequest(request);
}
