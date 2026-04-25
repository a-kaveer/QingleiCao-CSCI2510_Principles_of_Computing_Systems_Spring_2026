//This program brute-forces a given password hash by trying all possible
//passwords of a given length.
//
//Usage:
//crack <threads> <keysize> <target>
//
//Where <threads> is the number of threads to use, <keysize> is the maximum
//password length to search, and <target> is the target password hash.
//
//For example:
//
//./crack 1 5 na3C5487Wz4zw
//
//Should return the password 'apple'

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char *target;
    char *salt;
    int keysize;
    int start_char;
    int end_char;
} thread_args_t;

void brute_force(char *current, int index, int length, const char *salt, const char *target, struct crypt_data *data) {
    if (index == length) {
        current[index] = '\0';
        char *hash = crypt_r(current, salt, data);
        if (hash && strcmp(hash, target) == 0) {
            printf("%s\n", current);
            fflush(stdout);
            exit(0);
        }
        return;
    }

    for (char c = 'a'; c <= 'z'; c++) {
        current[index] = c;
        brute_force(current, index + 1, length, salt, target, data);
    }
}

void *thread_func(void *args) {
    thread_args_t *t_args = (thread_args_t *)args;
    struct crypt_data data;
    char password[9];
    
    for (int len = 1; len <= t_args->keysize; len++) {
        for (int i = t_args->start_char; i <= t_args->end_char; i++) {
            password[0] = 'a' + i;
            if (len == 1) {
                password[1] = '\0';
                char *hash = crypt_r(password, t_args->salt, &data);
                if (hash && strcmp(hash, t_args->target) == 0) {
                    printf("%s\n", password);
                    fflush(stdout);
                    exit(0);
                }
            } else {
                brute_force(password, 1, len, t_args->salt, t_args->target, &data);
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <threads> <keysize> <target>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int keysize = atoi(argv[2]);
    char *target = argv[3];

    char salt[3];
    salt[0] = target[0];
    salt[1] = target[1];
    salt[2] = '\0';

    pthread_t threads[num_threads];
    thread_args_t args[num_threads];

    int base_range = 26 / num_threads;
    int remainder = 26 % num_threads;
    int current_start = 0;

    for (int i = 0; i < num_threads; i++) {
        args[i].target = target;
        args[i].salt = salt;
        args[i].keysize = keysize;
        args[i].start_char = current_start;
        
        int range = base_range + (i < remainder ? 1 : 0);
        args[i].end_char = current_start + range - 1;
        current_start += range;

        pthread_create(&threads[i], NULL, thread_func, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}