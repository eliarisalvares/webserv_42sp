#include "Response.hpp"

/**
 * @brief Get the Directory Listing for the given folder path.
 *
 * @param folderPath folder path to get the listing for.
 * @return std::string html string with the directory listing.
 */
std::string getDirectoryListing(const std::string& folderPath, Request* request) {
    std::string directoryListing = "<html><head><title>Index of /</title></head><body><h1>Index of /</h1><hr><pre>";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folderPath.c_str())) != NULL) {
		std::string base_dir = request->uri();
		if (base_dir[base_dir.size() - 1] != '/')
			base_dir += "/";
        while ((ent = readdir (dir)) != NULL) {
            directoryListing += "<a href=\"" + base_dir + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a>\n";
        }
        closedir (dir);
    } else {
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
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
std::string getJsonContent(Request* request) {
    DIR *dir;
    struct dirent *ent;
    std::string directoryListing = "{\n \"files\": [\n";
    if ((dir = opendir ("content/upload")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            directoryListing += "\t\"" + std::string(ent->d_name) + "\",\n";
        }
        closedir (dir);
    } else {
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
    }
    directoryListing = directoryListing.substr(0, directoryListing.size() - 2);
    directoryListing += "]\n}";
    std::string jsonPath = "content/upload/listing.json";
    std::ofstream file(jsonPath.c_str());
    if (!file.is_open())
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
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
std::string getHtmlContent(const std::string& filePath, Request* request) {
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
