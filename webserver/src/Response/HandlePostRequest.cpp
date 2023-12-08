#include "Response.hpp"


bool isFileUpload(const std::string& body) {
    if (body.find("Content-Disposition: form-data;") != std::string::npos) {
        return true;
    }
    return false;
}

std::string getFileName(const std::string& body) {
    std::string fileName = body.substr(body.find("filename=\"") + 10);
    fileName = fileName.substr(0, fileName.find("\""));
    return fileName;
}

std::string getFilePath(const std::string& fileName) {
    std::string filePath = "/content/upload/" + fileName;
    return filePath;
}

std::string getBodyFromRequest(Request* request) {
    //std::string body = request->body();
    if (request) {
        std::cout << "blabla" << std::endl;
    }
    std::string body = "blabla";
    return body;
}

Response processFileUpload(Request* request, const std::string& body) {
    std::string fileName = getFileName(body);
    std::string filePath = getFilePath(fileName);
    std::ofstream file(filePath.c_str());
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + filePath);
    std::string fileContent = body.substr(body.find("\r\n\r\n") + 4);
    file << fileContent;
    file.close();
    Response response(request->fd(), 200);
    response.setMessage("OK");
    response.setBody("File uploaded successfully");
    return response;
}

Response handlePostRequest(Request* request) {
    std::string body = getBodyFromRequest(request);
    Logger::debug("handling POST request - body: " + body);
    if (isFileUpload(body)) {
        return processFileUpload(request, body);
    } else {
        std::string filePath = request->path();
        Logger::debug("handlePostRequest - filePath: " + filePath);
        std::string contentType = getContentType(filePath);
        std::string body = getResponseBody(filePath, contentType, request);
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
}
