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
std::string getDefaultFilePath() {
     if (access("content/index.html", F_OK) == 0) {
        return "content/index.html";
    } else {
        std::string folderPath = "content";
        std::string wholePath = folderPath + "/autoindex.html";
        std::string directoryListing = getDirectoryListing(folderPath);
        std::ofstream file(wholePath.c_str());
        if (!file.is_open())
            throw std::runtime_error("Could not open file: " + wholePath);
        file << directoryListing;
        file.close();
        return wholePath;
    }
}

/**
 * @brief If the request is for a html file, it will return the html content.
 * Prevents the server from sending the html file as a string.
 */
std::string getResponseBody(const std::string& filePath, const std::string& contentType) {
    if (contentType == "text/html") {
        return getHtmlContent(filePath);
    } else {
        return "";
    }
}

/**
 * @brief Handles the GET request. It will return the response with the
 * status code, message, body and headers.
 * 
 * @param request the request to be handled.
 * @return Response the response to be sent.
 */
Response handleGetRequest(Request* request) {
    std::string filePath = request->uri();
    Logger::debug("handleGetRequest - filePath: " + filePath);

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

