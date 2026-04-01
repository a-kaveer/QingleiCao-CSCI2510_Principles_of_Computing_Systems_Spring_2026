#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pid_t pid1, pid2;

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid1 = fork();
    if (pid1 == 0) {
        dup2(fd[1], STDOUT_FILENO); 
        close(fd[0]); 
        close(fd[1]); 
        char* myargv[] = {"program1", NULL};
        execvp("./program1", myargv);
        perror("Error exec'ing program1");
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0) {
        dup2(fd[0], STDIN_FILENO); 
        close(fd[1]); 
        close(fd[0]); 
        char* myargv[] = {"program2", NULL};
        execvp("./program2", myargv);
        perror("Error exec'ing program2");
        exit(1);
    }
    
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}