#include <stdio.h>
#include <unistd.h>

int main( int argc, char* argv[] ){
    write(STDOUT_FILENO, "Ali Kaveer, 2/19/26.\n", 21);
    return 0;
}