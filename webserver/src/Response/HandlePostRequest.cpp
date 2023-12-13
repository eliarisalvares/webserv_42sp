#include "Response.hpp"

/**
 * @brief Checks if the request has a multipart/form-data media type.
 * If it does, it means that the request is a file upload.
 *
 * @param request the request object holding the request data that originated the response
 * @return true if the request is a file upload
 * @return false if the request is not a file upload
 */
bool isFileUpload(Request* request) {
    std::string mediaType = http::enum_to_str_media_type(request->media_type());

    if (mediaType == "multipart/form-data") {
        return true;
    }

    return false;
}

/**
 * @brief The chosen folder to save the uploaded file is the content/upload folder.
 * This function sets the file name to be saved in this folder and adds a number
 * to the file name if it already exists.
 *
 * @param request the request object holding the request data that originated the response
 * @return std::string the file name to be saved
 */
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

/**
 * @brief Processes the file upload request. It saves the file in the content/upload folder.
 * and set a 201 status code if the file was successfully saved. If the file was not saved,
 * it sets a 500 status code. If there is no image in the request, it sets a 404 status code.
 * The response body is the file name so the client can access the correct file that was uploaded.
 *
 * @param request the request object holding the request data that originated the response
 * @return Response the response object to be sent to the client containing the response data
 * (status code, message and headers)
 */
Response processFileUpload(Request* request) {
    int statusCode = request->status_code();
    std::vector<char> *image = request->image();
    if (image == NULL) {
        statusCode = http::BAD_REQUEST;
    }

    std::string name = setFileName(request);
    std::ofstream file(name.c_str(), std::ios::binary);

    if (!file.is_open()) {
        statusCode = http::INTERNAL_SERVER_ERROR;
    }

    file.write(&(*image)[0], image->size());
    statusCode = http::CREATED;
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

Response processTextUpload(Request* request) {
	int statusCode = request->status_code();
    std::string text = request->text() + "\n";
    if (text.size() == 0) {
        statusCode = http::BAD_REQUEST;
    }

    std::string name = "content/upload/hello.txt";
    std::ofstream file(name.c_str(), std::ios::app | std::ios::binary);

    if (!file.is_open()) {
        statusCode = http::INTERNAL_SERVER_ERROR;
    }

    file.write(text.c_str(), text.size());
    statusCode = http::OK;
    file.close();

    std::string responseBody = text;
    size_t contentLength = responseBody.length();
    Response response(request->fd(), statusCode);
    response.setMessage(getStatusMessage(statusCode));
    response.setBody(responseBody);
    std::stringstream ss;
    ss << contentLength;
    setResponseHeaders(response, "text/plain", ss.str(), request);
    return response;
}

/**
 * @brief Main function to handle POST requests. It checks if the request is a file upload
 * or not and calls the respective function to process the request.
 *
 * @param request the request object holding the request data that originated the response
 * @return Response the response object to be sent to the client containing the response data
 * (status code, message and headers)
 */
Response handlePostRequest(Request* request) {
	http::MediaType media_type = request->media_type();
	std::string body, filePath, contentType, message;

	if (media_type == http::IMAGE_PNG || media_type == http::IMAGE_JPEG)
        return processFileUpload(request);
	else if (media_type == http::TEXT_PLAIN)
        return processTextUpload(request);
	else if (media_type != http::FORM_URLENCODED) {
		// retornar erro
	}

	filePath = request->uri();
	Logger::debug("handlePostRequest - filePath: " + filePath);

	contentType = getContentType(filePath);
	contentType = setFlagsContent(contentType);

	message = getStatusMessage(request->status_code());
    body = getResponseBody(filePath, contentType, request);

	Response response(request->fd(), 201);
	response.setMessage(message);
	response.setBody(body);

	std::stringstream ss;
	ss << body.length();
	setResponseHeaders(response, contentType, ss.str(), request);

	Logger::info("POST response created");
	return response;
}
