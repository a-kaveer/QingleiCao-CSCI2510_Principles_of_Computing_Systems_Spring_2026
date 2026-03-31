#include <stdio.h>
#include <string.h>
#include <unistd.h> 

int main() {
    int max_args = 15;
    int max_argv_size = max_args + 2;
    char* cmd;
    char* my_argv[max_argv_size];
    char buffer[1024];

    printf("Enter input: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        cmd = strtok(buffer, " ");
        if (cmd != NULL) {
            my_argv[0] = cmd;
            int i = 1;
            char *res;
            while ((res = strtok(NULL, " ")) != NULL && i <= max_args) {
                my_argv[i] = res;
                i++;
            }
            my_argv[i] = NULL;
            execvp(cmd, my_argv);
            perror("execvp failed");
        }
    }
    return 0;
}