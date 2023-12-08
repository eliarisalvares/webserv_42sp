# include "Response.hpp"

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

    std::string path_info = "PATH_INFO=" + request->uri();
    std::string path_translated = "PATH_TRANSLATED=" + server->getRoot() + request->uri();
    std::string script_name = "SCRIPT_NAME=" + request->uri();
    std::string server_name = "SERVER_NAME=" + server->getName(0);
    std::string server_port = "SERVER_PORT=" + server->getCurrentPort();
    std::string final_content_length = "CONTENT_LENGTH=" + content_length_str;
    std::string gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
    std::string protocol = "SERVER_PROTOCOL=HTTP/1.1";
    std::string request_method = "REQUEST_METHOD=" + http::enum_to_str_method(request->method());
    
    char **envp = new char*[10];
    envp[0] = strdup(gateway_interface.c_str());
    envp[1] = strdup(path_info.c_str());
    envp[2] = strdup(path_translated.c_str());
    envp[3] = strdup(script_name.c_str());
    envp[4] = strdup(server_name.c_str());
    envp[5] = strdup(server_port.c_str());
    envp[6] = strdup(final_content_length.c_str());
    envp[7] = strdup(protocol.c_str());
    envp[8] = strdup(request_method.c_str());
    envp[9] = NULL;

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
std::string handleCGI(Request* request) {

    int pipefd[2];
    pid_t pid;
    std::string filePath = request->uri();

    if (pipe(pipefd) == -1) {
        throw std::runtime_error("pipe failed: " + std::string(strerror(errno)));
    }

    pid = fork();
    if (pid < 0) {
        throw std::runtime_error("fork failed: " + std::string(strerror(errno)));
    }

    if (pid == 0) {
        char **envp = setEnvironment(request);

        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            throw std::runtime_error("dup2 failed: " + std::string(strerror(errno)));
        }

        char *const argv[] = {
            const_cast<char*>("python3"),
            const_cast<char*>(filePath.c_str()),
            NULL
        };

        if (execve("/usr/bin/python3", argv, envp) == -1)
            perror("execve failed");

        std::cerr << "execve failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[1]);

        std::string result;
        char buffer[BUFFSIZE];
        ssize_t count;

        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            result += buffer;
        }

        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0); // Wait for child process to finish

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            throw std::runtime_error("CGI script execution failed");
        }

        return result;
    }
}
