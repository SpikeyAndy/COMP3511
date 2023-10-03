#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
void handler1(int sig) {
    printf("Handler1\n");
    exit(1);
}
void handler2(int sig) {
    printf("Handler2\n");
}
int main() {
    char text[100];
    if ( fork() ) {
        signal(SIGINT, handler2);
        wait(0);
        while (1) {
            printf("Parent> ");
            scanf("%s", text);
            if (strcmp(text, "exit") == 0) exit(0);
        } 
    } else {
        signal(SIGINT, handler1);
        while (1) {
            printf("Child> ");
            scanf("%s", text);
            if (strcmp(text, "exit") == 0) exit(0);
        } 
    }
    return 0;
}
