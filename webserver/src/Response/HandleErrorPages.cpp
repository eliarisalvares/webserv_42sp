#include "Response.hpp"

Response	handleErrorPages(Request* request) {
	int statusCode = request->status_code();
	std::string filePath;

	if (request->location()) {
		std::map<int, std::string> error_pages = request->location()->error_pages;
		if (error_pages.size()) {
			if (error_pages.find(statusCode) != error_pages.end()) {
				filePath = error_pages[statusCode];
				Logger::debug("Custom error page from location");
			}
		}
	}
	else if (request->server()->getErrorPages().size()) {
		std::map<int, std::string> error_pages = request->server()->getErrorPages();
		if (error_pages.size()) {
			if (error_pages.find(statusCode) != error_pages.end()) {
				filePath = error_pages[statusCode];
				Logger::debug("Custom error page from server");
			}
		}
	}

	if (filePath.size() == 0) {
		filePath = (
			std::string("content/error_pages/")
			+ ftstring::itostr(statusCode)
			+ std::string(".html")
		);
		if (!checkFileWorks(filePath))
			filePath = "content/error_pages/500.html";
		Logger::debug("Default error page");
	}

	std::string contentType = getContentType(filePath);
    contentType = setFlagsContent(contentType);

    std::string body = getResponseBody(filePath, contentType, request);
    std::string message = getStatusMessage(statusCode);

    Response response(request->fd(), statusCode);
    response.setMessage(message);
    response.setBody(body);

    std::stringstream ss;
    ss << body.length();
    setResponseHeaders(response, contentType, ss.str(), request);

	Logger::info("Response created");
    return response;

}

Response CGIErrorHandler(Request* request) {
    Response response(request->fd(), request->status_code());
	std::string contentType = "text/html";
    response.setMessage(getStatusMessage(request->status_code()));
    response.setBody(getHtmlContent("content/error_pages/" + ftstring::itostr(request->status_code()) + ".html", request));
    response.sendResponse();
	return response;
}