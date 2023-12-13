#include "Response.hpp"

/**
 * @brief Gets the default file path when the request is "/". If
 * there is a file named "index.html" in the content folder, it
 * will be returned. Otherwise, the autoindex.py file will be
 * returned.
 *
 * @return std::string
 */
std::string getDefaultFilePath(std::string directoryPath, Request* request) {
    Logger::debug("getDefaultFilePath - directoryPath: " + directoryPath);
    std::string indexPath = directoryPath + "index.html";
    std::string folderPath = directoryPath;
    std::string wholePath = folderPath + "autoindex.html";
    std::string directoryListing = getDirectoryListing(folderPath, request);
    std::ofstream file(wholePath.c_str());
    if (!file.is_open())
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
    file << directoryListing;
    file.close();
    return wholePath;
}

/**
 * @brief If the request is for a html file, it will return the html content.
 * Prevents the server from sending the html file as a string.
 */
std::string getResponseBody(const std::string& filePath, const std::string& contentType, Request* request) {
    if (filePath.find(".py") != std::string::npos) {
        handleCGI(request);
        return "";
    } else if (contentType == "text/html") {
        return getHtmlContent(filePath, request);
    } else if (filePath.find(".json") != std::string::npos) {
        return getJsonContent(request);
    }
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

/**
 * @brief Sets the content type flags for the given content type.
 *
 * @param contentType the content type to be set.
 * @return std::string the content type with the flags.
 */
std::string	setFlagsContent(std::string contentType) {
    std::string	result;

    result = contentType;
    if (contentType == "text/html") {
        result += HTML_CHAR;
    }
    return result;
}

/**
 * @brief Handles the GET request. It will return the response with the
 * status code, message, body and headers.
 *
 * @param request the request to be handled.
 * @return Response the response to be sent.
 */
Response handleGetRequest(Request* request) {
    std::string filePath = request->path();
    Logger::debug("handleGetRequest - filePath: " + filePath);
    int statusCode = request->status_code();

    if (filePath[filePath.length() - 1] == '/') {
        filePath = getDefaultFilePath(filePath, request);
    }

    std::string contentType = getContentType(filePath);
    contentType = setFlagsContent(contentType);

    std::string body = getResponseBody(filePath, contentType, request);
    std::string message = getStatusMessage(statusCode);

    Response response(request->fd(), statusCode);
    response.setMessage(message);
    if (statusCode == http::INTERNAL_SERVER_ERROR) {
        body = getHtmlContent("content/error_pages/500.html", request);
    }
    response.setBody(body);

    std::stringstream ss;
    ss << body.length();
    setResponseHeaders(response, contentType, ss.str(), request);

	Logger::info("Response created");
    return response;
}

