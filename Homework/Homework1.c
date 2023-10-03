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

int main()
{
    if(fork() == 0) {
      printf("CMD1\n");
      printf("1");
    }
    else {
      if(fork() == 0) {
        wait(NULL);

      }
      else {
        printf("CMD2\n");
        wait(NULL);
      }
      printf("2");
    } 
    printf("3");
}