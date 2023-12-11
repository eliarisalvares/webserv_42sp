#include "Response.hpp"

bool isFileUpload(Request* request) {
    std::string mediaType = http::enum_to_str_media_type(request->media_type());

    if (mediaType == "multipart/form-data") {
        std::cout << "isFileUpload: true" << std::endl;
        return true;
    }

    return false;
}

std::string setFileName(Request* request) {
    std::string imageType = request->image_type();

    std::string fileName = "content/upload/image." + imageType;
    std::ifstream file;
    file.open(fileName.c_str());
    int i = 1;

    while (file.is_open()) {
        file.close();
        std::stringstream ss;
        ss << "content/upload/image" << i << "." << imageType;
        fileName = ss.str();
        file.open(fileName.c_str());
        i++;
    }

    file.close();
    return fileName;
}

Response processFileUpload(Request* request) {
    int statusCode = request->status_code();
    std::vector<char> *image = request->image();
    if (image == NULL) {
        statusCode = 404;
    }

    std::string name = setFileName(request);
    std::ofstream file(name.c_str(), std::ios::binary);

    if (!file.is_open()) {
        statusCode = 500;
    }

    file.write(&(*image)[0], image->size());
    statusCode = 201;
    file.close();

    std::string responseBody = name;
    size_t contentLength = responseBody.length();
    Response response(request->fd(), statusCode);
    response.setMessage(getStatusMessage(statusCode));
    response.setBody(responseBody);
    std::stringstream ss;
    ss << contentLength;
    setResponseHeaders(response, "text/plain", ss.str(), request);
    return response;
}

Response handlePostRequest(Request* request) {
    if (isFileUpload(request)) {
        return processFileUpload(request);
    } else {
        std::string filePath = request->uri();
        Logger::debug("handlePostRequest - filePath: " + filePath);
        
        std::string contentType = getContentType(filePath);
        contentType = setFlagsContent(contentType);

        std::string body = getResponseBody(filePath, contentType, request);
        std::string message = getStatusMessage(request->status_code());

        Response response(request->fd(), 201);
        response.setMessage(message);
        response.setBody(body);

        std::stringstream ss;
        ss << body.length();
        setResponseHeaders(response, contentType, ss.str(), request);
    
        return response;
    }
}
