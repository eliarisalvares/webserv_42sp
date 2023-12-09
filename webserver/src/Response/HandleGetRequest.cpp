#include "Response.hpp"

/**
 * @brief Get the Directory Listing for the given folder path.
 *
 * @param folderPath folder path to get the listing for.
 * @return std::string html string with the directory listing.
 */
std::string getDirectoryListing(const std::string& folderPath) {
    std::string directoryListing = "<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre>";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folderPath.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            directoryListing += "<a href=\"" + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a>\n";
        }
        closedir (dir);
    } else {
        throw std::runtime_error("Could not open directory: " + folderPath);
    }
    directoryListing += "</pre><hr></body></html>";
    return directoryListing;
}

/**
 * @brief Gets the default file path when the request is "/". If
 * there is a file named "index.html" in the content folder, it
 * will be returned. Otherwise, the autoindex.py file will be
 * returned.
 *
 * @return std::string
 */
std::string getDefaultFilePath(std::string directoryPath) {
    Logger::debug("getDefaultFilePath - directoryPath: " + directoryPath);
    std::string indexPath = directoryPath + "index.html";
    std::string folderPath = directoryPath;
    std::string wholePath = folderPath + "autoindex.html";
    std::string directoryListing = getDirectoryListing(folderPath);
    std::ofstream file(wholePath.c_str());
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + wholePath);
    file << directoryListing;
    file.close();
    return wholePath;
}

/**
 * @brief If the request is for a html file, it will return the html content.
 * Prevents the server from sending the html file as a string.
 */
std::string getResponseBody(const std::string& filePath, const std::string& contentType, Request* request) {
    if (contentType == "text/html") {
        return getHtmlContent(filePath);
    } else if (filePath.find(".py") != std::string::npos) {
        return handleCGI(request);
    }
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
        return "";
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

    if (request->has_error()) {
		filePath = (
			std::string("content/error_pages/")
			+ ftstring::itostr(statusCode)
			+ std::string(".html")
		);
	}

    Logger::debug("handleGetRequest - filePath 2: " + filePath);

    if (filePath[filePath.length() - 1] == '/') {
        Logger::debug("inside if");
        filePath = getDefaultFilePath(filePath);
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

    return response;
}

