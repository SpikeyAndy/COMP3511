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


void print_message(const char* msg) {
    printf("%s\n", msg);
    fflush(stdout); 
}
int main() {
    if ( fork() ) {
        wait(0);
        if ( fork() ) {
            wait(0);
            print_message("A");
        } else {
            print_message("B");
        }
    } else {
        if ( fork() ) {
            wait(0);
            print_message("C");
        } else {
            print_message("D");
        }
    }
    return 0;
}
