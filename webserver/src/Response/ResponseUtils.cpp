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
    std::string extension = filePath.substr(filePath.find_last_of('.'));
    std::map<std::string, std::string>::const_iterator it = contentTypeMap.find(extension);
    if (it == contentTypeMap.end())
        return "text/plain";
    return it->second;
}

/**
 * @brief Auxiliary function to get the content of a html file
 * as a string.
 * 
 * @param filePath path to the file.
 * @return std::string the content of the file.
 */
std::string getHtmlContent(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * @brief Auxiliary function to get the current date and time 
 * to be used in the response headers.
 * 
 * @return std::string the current date and time string
 */
std::string getCurrentDate() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm *bt = localtime(&now);
    char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", bt);
    std::ostringstream oss;
    oss << dateStr;
    oss << '.' << std::setfill('0') << std::setw(6) << tv.tv_usec;
    return oss.str();
}
