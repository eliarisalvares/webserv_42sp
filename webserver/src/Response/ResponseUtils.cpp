#include "Response.hpp"

static const std::map<std::string, std::string> initializeContentTypeMap() {
    std::map<std::string, std::string> m;
    m.insert(std::make_pair(".html", "text/html"));
    m.insert(std::make_pair(".htm", "text/html"));
    m.insert(std::make_pair(".txt", "text/plain"));
    m.insert(std::make_pair(".css", "text/css"));
    m.insert(std::make_pair(".js", "application/javascript"));
    m.insert(std::make_pair(".json", "application/json"));
    m.insert(std::make_pair(".xml", "application/xml"));
    m.insert(std::make_pair(".png", "image/png"));
    m.insert(std::make_pair(".jpg", "image/jpeg"));
    m.insert(std::make_pair(".jpeg", "image/jpeg"));
    m.insert(std::make_pair(".gif", "image/gif"));
    m.insert(std::make_pair(".py", "text/html"));
    return m;
}

static const std::map<std::string, std::string> contentTypeMap = initializeContentTypeMap();

/**
 * @brief Checks for the file extension and returns the content type of it.
 *
 * @param filePath path to the file.
 * @return std::string the content type of the file.
 */
std::string getContentType(const std::string& filePath) {
	size_t point_position;

	point_position = filePath.find_last_of('.');
	if (point_position == std::string::npos)
		return "text/plain";

	std::string extension = filePath.substr(point_position);
	std::map<std::string, std::string>::const_iterator it = contentTypeMap.find(extension);
	if (it == contentTypeMap.end())
		return "text/plain";
	return it->second;
}

/**
 * @brief Auxiliary function to get the current date and time
 * to be used in the response headers.
 *
 * @return std::string the current date and time string
 */
std::string getCurrentDate() {
    time_t   now;
    std::tm *local_time;
    char     buffer[64];

    now        = std::time(NULL);
    local_time = std::localtime(&now);
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T %Z", local_time);

    return (buffer);
}

