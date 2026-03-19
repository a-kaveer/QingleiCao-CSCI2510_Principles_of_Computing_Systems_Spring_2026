#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    
   
    if (fd == -1) {
        perror("Error opening file");
        exit(-1);
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    
    while ((bytes_read = read(fd, buffer, BUF_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    
    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd);
        exit(-1);
    }

    close(fd);
    return 0;
}
