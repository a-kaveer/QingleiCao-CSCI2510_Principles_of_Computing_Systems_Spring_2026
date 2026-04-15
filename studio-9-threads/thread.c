#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

struct thread_data {
    int arg1;
    char arg2[100];
    int ret;
};

void* thread_entry(void* args) {
    printf("ooooo!\n");
    struct thread_data* arg_ptr = (struct thread_data*) args;
    
    printf("Thread %d: Received '%s'\n", arg_ptr->arg1, arg_ptr->arg2);
    
    arg_ptr->ret = arg_ptr->arg1 * arg_ptr->arg1;
    
    return NULL;
}

int main() {
    int n = 5;
    pthread_t threads[n];
    struct thread_data args[n];
    int result;

    for (int i = 0; i < n; i++) {
        args[i].arg1 = i;
        snprintf(args[i].arg2, 100, "This is thread %d", i);
        args[i].ret = 0;

        result = pthread_create(&threads[i], NULL, thread_entry, &args[i]);

        if (result != 0) {
            fprintf(stderr, "Error creating thread %d: %s\n", i, strerror(result));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n; i++) {
        result = pthread_join(threads[i], NULL);

        if (result != 0) {
            fprintf(stderr, "Error joining thread %d: %s\n", i, strerror(result));
            exit(EXIT_FAILURE);
        }
        
        printf("Main: Thread %d returned square value: %d\n", i, args[i].ret);
    }

    return 0;
}