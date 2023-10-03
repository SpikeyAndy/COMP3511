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


int main(){
int index = 0;
for(int i = 0; i < 2; i++){
    int pid = fork(); 
    if(pid != 0){
        fork();
        printf("a");
    }
    else if(index == 0){
        fork();
        index++;
        printf("b");
    }
    else{
        fork();
        printf("c");
    }
}

}


