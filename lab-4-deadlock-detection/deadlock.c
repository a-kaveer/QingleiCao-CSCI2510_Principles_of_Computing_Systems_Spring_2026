#include <stdio.h>
#include <stdlib.h>

#define NUM_PROCS 15
#define NUM_RES 15


int p_to_r[NUM_PROCS][NUM_RES]; 
int r_to_p[NUM_RES];            

void init_rag();
int acquire(int p, int r);
int release(int p, int r);
int start_search(int start_p);
int search_process(int start_p, int current_p);
int search_resource(int start_p, int current_r);

void init_rag() {
    for (int i = 0; i < NUM_PROCS; i++) {
        r_to_p[i] = -1;
        for (int j = 0; j < NUM_RES; j++) {
            p_to_r[i][j] = 0;
        }
    }
}

int acquire(int p, int r) {
    if (r_to_p[r] == -1) {
        r_to_p[r] = p;
    } else {
        p_to_r[p][r] = 1;
    }

    
    if (start_search(p)) {
        return 1;
    }
    return 0;
}


int release(int p, int r) {
    if (r_to_p[r] == p) {
        r_to_p[r] = -1;
        
        for (int next_p = 0; next_p < NUM_PROCS; next_p++) {
            if (p_to_r[next_p][r]) {
                p_to_r[next_p][r] = 0; 
                return acquire(next_p, r); 
            }
        }
    }
    return 0;
}

int start_search(int start_p) {
    return search_process(start_p, start_p);
}

int search_process(int start_p, int current_p) {
    for (int r = 0; r < NUM_RES; r++) {
        if (p_to_r[current_p][r]) {
            if (search_resource(start_p, r)) {
                printf("PROCESS %d -> RESOURCE %d\n", current_p, r);
                return 1;
            }
        }
    }
    return 0;
}

int search_resource(int start_p, int current_r) {
    int holding_p = r_to_p[current_r];
    if (holding_p != -1) {
        if (holding_p == start_p) {
            printf("RESOURCE %d -> PROCESS %d\n", current_r, holding_p);
            return 1;
        }
        if (search_process(start_p, holding_p)) {
            printf("RESOURCE %d -> PROCESS %d\n", current_r, holding_p);
            return 1;
        }
    }
    return 0;
}

int main() {
    int p, r;
    char action;
    init_rag();

    while (scanf("%d %c %d", &p, &action, &r) == 3) {
        if (action == 'a') {
            if (acquire(p, r)) {
                return -1;
            }
        } else if (action == 'r') {
            if (release(p, r)) {
                return -1;
            }
        }
    }

    return 0;
}