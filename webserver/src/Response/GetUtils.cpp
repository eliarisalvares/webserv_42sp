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
 * @brief Auxiliary function to get the directory listing as a json string.
 * Will be used to list the files in the delete request.
 * 
 * @return std::string the directory listing as a json string.
 */
std::string getJsonContent() {
    DIR *dir;
    struct dirent *ent;
    std::string directoryListing = "{\n \"files\": [\n";
    if ((dir = opendir ("content/upload")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            directoryListing += "\t\"" + std::string(ent->d_name) + "\",\n";
        }
        closedir (dir);
    } else {
        throw std::runtime_error("Could not open directory: content/upload");
    }
    directoryListing = directoryListing.substr(0, directoryListing.size() - 2);
    directoryListing += "]\n}";
    std::string jsonPath = "content/upload/listing.json";
    std::ofstream file(jsonPath.c_str());
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + jsonPath);
    file << directoryListing;
    file.close();
    return directoryListing;
}

/**
 * @brief Auxiliary function to get the content of a html file
 * as a string.
 * 
 * @param filePath path to the file.
 * @return std::string the content of the file.
 */
std::string getHtmlContent(const std::string& filePath, Server* server) {
    std::string port = server->getCurrentPort();
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    // replace localhost:8080 with the current port
    std::string content = buffer.str();
    if (content.find("localhost:8080") != std::string::npos) {
        content.replace(content.find("localhost:8080"), 13, "localhost:" + port);
    }
    file.close();
    return content;
}