#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_LINE_LENGTH 256
#define MAX_ARGS 15
#define MAX_CMDS 10

void handle_sigint(int sig) {
    write(STDOUT_FILENO, "\n", 1);
}

typedef struct {
    char* args[MAX_ARGS + 2];
    int arg_count;
} Command;

int parse_input(char* input, Command* program) {
    int count = 0;
    char* token = strtok(input, " \t\n");
    if (token == NULL) return 0;

    program[count].arg_count = 0;
    while (token != NULL) {
        if (strcmp(token, "(") == 0) {
            if (program[count].arg_count == 0) {
                fprintf(stderr, "Invalid null command\n");
                return -1;
            }
            program[count].args[program[count].arg_count] = NULL;
            count++;
            if (count >= MAX_CMDS) break;
            program[count].arg_count = 0;
        } else {
            if (program[count].arg_count >= MAX_ARGS) {
                fprintf(stderr, "Too many arguments\n");
                return -1;
            }
            program[count].args[program[count].arg_count++] = strdup(token);
        }
        token = strtok(NULL, " \t\n");
    }

    if (program[count].arg_count == 0 && count > 0) {
        fprintf(stderr, "Invalid null command\n");
        return -1;
    }
    program[count].args[program[count].arg_count] = NULL;
    return count + 1;
}

void execute_command(Command* program, int index, int out_fd) {
    if (index < 0) return;

    int pipefd[2];
    if (index > 0) {
        if (pipe(pipefd) == -1) {
            perror("pipe");
            return;
        }
    }

    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        if (index > 0) {
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
        }
        if (out_fd != STDOUT_FILENO) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        execvp(program[index].args[0], program[index].args);
        fprintf(stderr, "%s: Not found\n", program[index].args[0]);
        exit(EXIT_FAILURE);
    }

    if (index > 0) close(pipefd[0]);
    if (out_fd != STDOUT_FILENO) close(out_fd);
    if (index > 0) {
        execute_command(program, index - 1, pipefd[1]);
    }
}

int main() {
    char input[MAX_LINE_LENGTH];
    Command programs[MAX_CMDS];
    char cwd[1024];
    signal(SIGINT, handle_sigint);

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            char* home = getenv("HOME");
            if (home != NULL && strncmp(cwd, home, strlen(home)) == 0) {
                printf("slush|%s> ", cwd + strlen(home));
            } else {
                printf("slush|%s> ", cwd);
            }
        } else {
            printf("slush> ");
        }
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) break;
            clearerr(stdin);
            continue;
        }

        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        int prog_count = parse_input(input, programs);
        if (prog_count <= 0) continue;

        if (strcmp(programs[0].args[0], "cd") == 0) {
            char* target = (programs[0].arg_count < 2) ? getenv("HOME") : programs[0].args[1];
            if (chdir(target) != 0) perror("cd");
        } else {
            execute_command(programs, prog_count - 1, STDOUT_FILENO);
            for (int i = 0; i < prog_count; i++) wait(NULL);
        }

        for (int i = 0; i < prog_count; i++) {
            for (int j = 0; j < programs[i].arg_count; j++) {
                free(programs[i].args[j]);
            }
        }
    }
    return 0;
}