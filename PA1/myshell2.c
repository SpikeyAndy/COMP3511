/*
    COMP3511 Fall 2023
    PA1: Simplified Linux Shell (MyShell)

    Your name: LUI, Cheuk Kwan Andy
    Your ITSC email: ckalui@connect.ust.hk

    Declaration:

    I declare that I am not involved in plagiarism
    I understand that both parties (i.e., students providing the codes and students copying the codes) will receive 0 marks.

*/

/*
    Header files for MyShell
    Necessary header files are included.
    Do not include extra header files
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> // For constants that are required in open/read/write/close syscalls
#include <sys/wait.h> // For wait() - suppress warning messages
#include <fcntl.h>    // For open/read/write/close syscalls
#include <signal.h>   // For signal handling

// Define template strings so that they can be easily used in printf
//
// Usage: assume pid is the process ID
//
//  printf(TEMPLATE_MYSHELL_START, pid);
//
#define TEMPLATE_MYSHELL_START "Myshell (pid=%d) starts\n"
#define TEMPLATE_MYSHELL_END "Myshell (pid=%d) ends\n"
#define TEMPLATE_MYSHELL_TERMINATE "Myshell (pid=%d) terminates by Ctrl-C\n"

// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LENGTH 256

// Assume that we have at most 8 arguments
#define MAX_ARGUMENTS 8

// Assume that we only need to support 2 types of space characters:
// " " (space) and "\t" (tab)
#define SPACE_CHARS " \t"

// The pipe character
#define PIPE_CHAR "|"

// Assume that we only have at most 8 pipe segements,
// and each segment has at most 256 characters
#define MAX_PIPE_SEGMENTS 8

// Assume that we have at most 8 arguments for each segment
// We also need to add an extra NULL item to be used in execvp
// Thus: 8 + 1 = 9
//
// Example:
//   echo a1 a2 a3 a4 a5 a6 a7
//
// execvp system call needs to store an extra NULL to represent the end of the parameter list
//
//   char *arguments[MAX_ARGUMENTS_PER_SEGMENT];
//
//   strings stored in the array: echo a1 a2 a3 a4 a5 a6 a7 NULL
//
#define MAX_ARGUMENTS_PER_SEGMENT 9

// Define the standard file descriptor IDs here
#define STDIN_FILENO 0  // Standard input
#define STDOUT_FILENO 1 // Standard output

// This function will be invoked by main()
// This function is given
int get_cmd_line(char *command_line)
{
    int i, n;
    if (!fgets(command_line, MAX_CMDLINE_LENGTH, stdin))
        return -1;
    // Ignore the newline character
    n = strlen(command_line);
    command_line[--n] = '\0';
    i = 0;
    while (i < n && command_line[i] == ' ')
    {
        ++i;
    }
    if (i == n)
    {
        // Empty command
        return -1;
    }
    return 0;
}

// read_tokens function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *pipe_segments[MAX_PIPE_SEGMENTS]; // character array buffer to store the pipe segements
// int num_pipe_segments; // an output integer to store the number of pipe segment parsed by this function
// char command_line[MAX_CMDLINE_LENGTH]; // The input command line
//
// Sample usage:
//
//  read_tokens(pipe_segments, command_line, &num_pipe_segments, "|");
//
void read_tokens(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}

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

        int input_fd = 0;
        int output_fd = 1;

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

void ctrl_handling() {
    printf(TEMPLATE_MYSHELL_TERMINATE, getpid());
    exit(0);
}

/* The main function implementation */
int main()
{
    // TODO: replace the shell prompt with your ITSC account name
    // For example, if you ITSC account is cspeter@connect.ust.hk
    // You should replace ITSC with cspeter
    char *prompt = "ckalui";
    char command_line[MAX_CMDLINE_LENGTH];

    // TODO:
    // The main function needs to be modified
    // For example, you need to handle the exit command inside the main function

    printf(TEMPLATE_MYSHELL_START, getpid());

    // The main event loop
    while (1)
    {
        printf("%s> ", prompt);
        signal(SIGINT, ctrl_handling);
        if (get_cmd_line(command_line) == -1)
            continue; /* empty line handling */

        else if (strcmp(command_line, "exit") == 0) {
            printf(TEMPLATE_MYSHELL_END, getpid());
            exit(0);
            return 0;
        }
        
        pid_t pid = fork();
        if (pid == 0)
        {
            // the child process handles the command
            process_cmd(command_line);
        }
        else
        {
            // the parent process simply wait for the child and do nothing
            wait(0);
        }
    }

    return 0;
}