void process_cmd(char* command_line) {
    printf("Debug: The command line is [%s]\n", command_line);
    int numseg;
    char* pipe_segments[MAX_PIPE_SEGMENTS] = { NULL };
    read_tokens(pipe_segments, command_line, &numseg, "|");
    pipe_segments[numseg] = NULL;

    for (int i = 0; i < numseg; i++) {
        char* arg_segments[MAX_ARGUMENTS] = { NULL };
        int numarg;

        read_tokens(arg_segments, pipe_segments[i], &numarg, " ");
        arg_segments[numarg] = NULL;

        int input_fd = STDIN_FILENO;
        int output_fd = STDOUT_FILENO;

        for (int j = 0; j < numarg; j++) {
            if (strcmp(arg_segments[j], ">") == 0) {
                if (j < numarg - 1) {
                    output_fd = open(arg_segments[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (output_fd == -1) {
                        perror("Failed to open output file");
                        return;
                    }
                    // Remove the ">" and the output file name from the arguments
                    arg_segments[j] = NULL;
                    arg_segments[j + 1] = NULL;
                    j++; // Skip to the next argument
                } else {
                    printf("Invalid command: missing output file name\n");
                    return;
                }
            } else if (strcmp(arg_segments[j], "<") == 0) {
                if (j < numarg - 1) {
                    input_fd = open(arg_segments[j + 1], O_RDONLY);
                    if (input_fd == -1) {
                        perror("Failed to open input file");
                        return;
                    }
                    // Remove the "<" and the input file name from the arguments
                    arg_segments[j] = NULL;
                    arg_segments[j + 1] = NULL;
                    j++; // Skip to the next argument
                } else {
                    printf("Invalid command: missing input file name\n");
                    return;
                }
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (input_fd != STDIN_FILENO) {
                if (dup2(input_fd, STDIN_FILENO) == -1) {
                    perror("Failed to redirect input");
                    return;
                }
                close(input_fd);
            }
            if (output_fd != STDOUT_FILENO) {
                if (dup2(output_fd, STDOUT_FILENO) == -1) {
                    perror("Failed to redirect output");
                    return;
                }
                close(output_fd);
            }

            execvp(arg_segments[0], arg_segments);
            perror("Failed to execute command");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Error forking
            perror("Failed to fork");
            return;
        } else {
            // Parent process
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("Failed to wait for child process");
                return;
            }
        }
    }
}