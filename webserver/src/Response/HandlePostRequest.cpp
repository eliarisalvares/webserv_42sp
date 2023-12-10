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

    Response response(request->fd(), statusCode);
    response.setMessage(getStatusMessage(statusCode));
    setResponseHeaders(response, "", "0", request);
    return response;
}

Response handlePostRequest(Request* request) {
    if (isFileUpload(request)) {
        return processFileUpload(request);
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
