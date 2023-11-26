# include "Response.hpp"

void setEnvironment() {
    std::string requestMethod = "GET";
    std::string queryString = "";
    std::string scriptName = "current_time.py";

    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("SERVER_NAME", Server::getServerName().c_str(), 1);
    setenv("SERVER_PORT", Server::getCurrentPort().c_str(), 1);
    setenv("SCRIPT_FILENAME", ("content/cgi/" + scriptName).c_str(), 1);
    setenv("SCRIPT_NAME", ("/cgi/" + scriptName).c_str(), 1);
    setenv("PATH_TRANSLATED", ("content/cgi/" + scriptName).c_str(), 1); // Full path to the script on the server
    setenv("PATH_INFO", ("/cgi/" + scriptName).c_str(), 1); // Virtual path to the script
    setenv("REQUEST_METHOD", requestMethod.c_str(), 1);
    setenv("QUERY_STRING", queryString.c_str(), 1);
}

std::string handleCGI() {
    Logger log;
    setEnvironment();

    log.debug("executing CGI...");
    std::string command = CGI_EXECUTOR + std::string(" ") + getenv("SCRIPT_FILENAME");
    std::string result = "";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    char buffer[BUFFSIZE];
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}
