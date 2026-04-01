#include <stdio.h>
#define MAX_BUFFER 1024

int main() {
    char input[MAX_BUFFER];
    while (fgets(input, MAX_BUFFER, stdin) != NULL) {
        printf("Program 2 got: %s", input);
    }

    return 0;
}