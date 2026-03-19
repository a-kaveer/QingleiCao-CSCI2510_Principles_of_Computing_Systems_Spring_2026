//Runlength compression in C
// Usage: ./rle <input file> <output file> <runlength> <mode>
//        Where mode=0 is compress and mode=1 is decompress

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

void usage(char *prog) {
    fprintf(stderr, "Usage: %s <input file> <output file> <compression length> <mode>\n", prog);
    fprintf(stderr, "Mode: 0 = compress, 1 = decompress\n");
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc != 5) usage(argv[0]);

    char *infile = argv[1];
    char *outfile = argv[2];
    int K = atoi(argv[3]);
    int mode = atoi(argv[4]);

    if (K < 1) {
        fprintf(stderr, "Error: compression length must be >= 1\n");
        exit(-1);
    }

    int fd_in = open(infile, O_RDONLY);
    if (fd_in < 0) { perror("Error opening input file"); exit(-1); }

    int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd_out < 0) { perror("Error opening output file"); exit(-1); }

    if (mode == 0) { // COMPRESS
        unsigned char *current_pattern = malloc(K);
        unsigned char *next_pattern = malloc(K);
        unsigned char count = 1;

        ssize_t bytes_read = read(fd_in, current_pattern, K);
        
        if (bytes_read > 0) {
            ssize_t n;
            while ((n = read(fd_in, next_pattern, K)) > 0) {
                // Check if patterns match AND count < 255 AND we read a full K bytes
                if (n == bytes_read && memcmp(current_pattern, next_pattern, n) == 0 && count < 255) {
                    count++;
                } else {
                    // Write count and the pattern
                    if (write(fd_out, &count, 1) != 1) { perror("Write error"); exit(-1); }
                    if (write(fd_out, current_pattern, bytes_read) != bytes_read) { perror("Write error"); exit(-1); }
                    
                    // Move next to current
                    memcpy(current_pattern, next_pattern, n);
                    bytes_read = n;
                    count = 1;
                }
            }
            // Write the final run
            write(fd_out, &count, 1);
            write(fd_out, current_pattern, bytes_read);
        }
        free(current_pattern);
        free(next_pattern);

    } else if (mode == 1) { // DECOMPRESS
        unsigned char count;
        unsigned char *pattern = malloc(K);
        
        while (read(fd_in, &count, 1) == 1) {
            ssize_t n = read(fd_in, pattern, K);
            if (n <= 0) break; 
            
            for (int i = 0; i < (int)count; i++) {
                if (write(fd_out, pattern, n) != n) { perror("Write error"); exit(-1); }
            }
        }
        free(pattern);
    } else {
        usage(argv[0]);
    }

    close(fd_in);
    close(fd_out);
    return 0;
}