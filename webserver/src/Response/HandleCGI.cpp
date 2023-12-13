# include "Response.hpp"

/**
 * @brief Handles the error response for CGI scripts.
 * 
 * @param request the request to be handled.
 * @return Response the response to be sent.
 */
Response CGIErrorHandler(Request* request) {
    Response response(request->fd(), request->status_code());
	std::string contentType = "text/html";
    response.setMessage(getStatusMessage(request->status_code()));
    response.setBody(getHtmlContent("content/error_pages/" + ftstring::itostr(request->status_code()) + ".html", request));
    response.sendResponse();
	return response;
}

/**
 * @brief Set environment variables for CGI script execution.
 * Because we can't use setenv() in C++98, we have to use this workaround
 * to set **envp for execve().
 * @return char** with environment variables.
*/
char** setEnvironment(Request* request) {
    Server *server = request->server();

    size_t content_length = request->content_length();
    std::stringstream ss;
    ss << content_length;
    std::string content_length_str = ss.str();

    std::string path_info = "PATH_INFO=" + request->path();
    std::string path_translated = "PATH_TRANSLATED=" + server->getRoot() + request->uri();
    std::string script_name = "SCRIPT_NAME=" +  request->uri().substr(request->uri().find_last_of("/") + 1, request->uri().length());
    std::string server_name = "SERVER_NAME=" + server->getName(0);
    std::string server_port = "SERVER_PORT=" + server->getCurrentPort();
    std::string final_content_length = "CONTENT_LENGTH=" + content_length_str;
    std::string gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
    std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
    std::string request_method = "REQUEST_METHOD=" + http::enum_to_str_method(request->method());
    std::string delete_path_info = "DELETE_PATH=" + server->getRoot() + "/upload";
    std::string content_type = "CONTENT_TYPE=" + http::enum_to_str_media_type(request->media_type());
    std::string content_length_env = "CONTENT_LENGTH=" + content_length_str;
    std::map<std::string, std::string> post_data = request->post_data();
    std::string content_body = "QUERY_STRING=";
    for (std::map<std::string, std::string>::iterator it = post_data.begin(); it != post_data.end(); it++) {
        content_body += it->first + "=" + it->second + "&";
    }

    char **envp = new char*[14];
    envp[0] = strdup(gateway_interface.c_str());
    envp[1] = strdup(path_info.c_str());
    envp[2] = strdup(path_translated.c_str());
    envp[3] = strdup(script_name.c_str());
    envp[4] = strdup(server_name.c_str());
    envp[5] = strdup(server_port.c_str());
    envp[6] = strdup(final_content_length.c_str());
    envp[7] = strdup(protocol.c_str());
    envp[8] = strdup(request_method.c_str());
    envp[9] = strdup(content_body.c_str());
    envp[10] = strdup(delete_path_info.c_str());
    envp[11] = strdup(content_type.c_str());
    envp[12] = strdup(content_length_env.c_str());
    envp[13] = NULL;

    return envp;
}

/**
 * @brief This function creates a pipe and a child process using fork().
 * In the child process, it executes a specified Python CGI script using
 * execve() and captures its output. The output of the script is then read
 * through the pipe and returned as a string.
 * @return std::string The output of the executed CGI script.
 *
 * Detailed Workflow:
 * 1. Creates a pipe for inter-process communication.
 * 2. Forks the current process to create a child process.
 * 3. In the child process:
 *    a. Closes the read end of the pipe.
 *    b. Duplicates the write end of the pipe to standard output.
 *    c. Executes the CGI script using execve().
 *    d. On failure, prints an error message and exits.
 * 4. In the parent process:
 *    a. Closes the write end of the pipe.
 *    b. Reads the output of the CGI script from the read end of the pipe.
 *    c. Closes the read end of the pipe.
 *    d. Waits for the child process to finish.
 *    e. Checks the exit status of the child process.
 *    f. Returns the read output.
 */
Response handleCGI(Request* request) {
    Response response(request->fd(), request->status_code());

    int pipefd[2];
    pid_t pid;
    std::string filePath = request->path();
    request->setStatusCode(http::OK);

    if (pipe(pipefd) == -1) {
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
        return CGIErrorHandler(request);
    }

    pid = fork();
    if (pid < 0) {
        request->setStatusCode(http::INTERNAL_SERVER_ERROR);
    }

    if (pid == 0) { // Child process
        char **envp = setEnvironment(request);

        std::string cgiDir =  "content/cgi";
        filePath = filePath.substr(cgiDir.length(), filePath.length() - cgiDir.length());
        filePath = filePath.substr(1, filePath.length() - 1);
        if (chdir(cgiDir.c_str()) == -1) {
            request->setStatusCode(http::INTERNAL_SERVER_ERROR);
            return CGIErrorHandler(request);
        }

        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            request->setStatusCode(http::INTERNAL_SERVER_ERROR);
            return CGIErrorHandler(request);
        }

        char *const argv[] = {
            const_cast<char*>("python3"),
            const_cast<char*>(filePath.c_str()),
            NULL
        };

        if (execve("/usr/bin/python3", argv, envp) == -1) {
            request->setStatusCode(http::INTERNAL_SERVER_ERROR);
            return CGIErrorHandler(request);
        }

        exit(EXIT_FAILURE);
    } else {
        close(pipefd[1]);

        std::string result;
        char buffer[BUFFSIZE];
        ssize_t count;
        struct pollfd fds[1];

        fds[0].fd = pipefd[0];
        fds[0].events = POLLIN;

        int ret = poll(fds, 1, 10000);

        if (ret == -1) {
            request->setStatusCode(http::INTERNAL_SERVER_ERROR);
            return CGIErrorHandler(request);
        } else if (ret == 0) {
            request->setStatusCode(http::REQUEST_TIMEOUT);
            CGIErrorHandler(request);
            kill(pid, SIGKILL);
            return response;
        }

        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            result += buffer;
        }

        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0); // Wait for child process to finish

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            request->setStatusCode(http::INTERNAL_SERVER_ERROR);
            return CGIErrorHandler(request);
        }

        std::string contentType = "text/html";
        std::string flagsContent = setFlagsContent(contentType);
        std::stringstream ss;
        ss << result.length();
        std::string contentLength = ss.str();
        Response response(request->fd(), request->status_code());
        response.setMessage(getStatusMessage(request->status_code()));
        response.setBody(result);
        setResponseHeaders(response, flagsContent, contentLength, request);
        response.sendResponse();
        return response;
    }
}
