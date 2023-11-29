# include "Response.hpp"

/**
 * @brief Set environment variables for CGI script execution.
 * Because we can't use setenv() in C++98, we have to use this workaround
 * to set **envp for execve().
 * @return char** with environment variables.
*/
char** setEnvironment() {
    char **envp = static_cast<char**>(malloc(11 * sizeof(char*)));

    envp[0] = strdup("GATEWAY_INTERFACE=CGI/1.1");
    envp[1] = strdup("PATH_INFO=/cgi/current_time.py");
    envp[2] = strdup("PATH_TRANSLATED=content/cgi/current_time.py");
    envp[3] = strdup("QUERY_STRING=");
    envp[4] = strdup("REQUEST_METHOD=GET");
    envp[5] = strdup("SCRIPT_NAME=/cgi/current_time.py");
    envp[6] = strdup("SERVER_NAME=localhost");
    envp[7] = strdup("SERVER_PORT=8080");
    envp[8] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    envp[9] = strdup("SERVER_SOFTWARE=webserv");
    envp[10] = NULL;

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
std::string handleCGI() {

    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        throw std::runtime_error("pipe failed: " + std::string(strerror(errno)));
    }

    pid = fork();
    if (pid < 0) {
        throw std::runtime_error("fork failed: " + std::string(strerror(errno)));
    }

    if (pid == 0) {
        char **envp = setEnvironment();

        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            throw std::runtime_error("dup2 failed: " + std::string(strerror(errno)));
        }

        char *const argv[] = {
            const_cast<char*>("python3"),
            const_cast<char*>("content/cgi/current_time.py"),
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
