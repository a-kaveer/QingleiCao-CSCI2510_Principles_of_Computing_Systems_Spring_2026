#include <stdio.h>
#include <stdlib.h>

void printReverse(char* string) {
    int length = 0;
    while (string[length] != '\0') {
        length++;
    }
    
    for (int i = length - 1; i >= 0; i--) {
        printf("%c\n", string[i]);
    }
}

char* reverseString(char* input) {
    int length = 0;
    while (input[length] != '\0') {
        length++;
    }
    char* output = (char*)malloc(length + 1);
    for (int i = 0; i < length; i++) {
        output[i] = input[length - 1 - i];
    }
    output[length] = '\0';
    return output;
}

int main() {
    char *messagePtr = "HELLOWORLD!";
    printf("%s\n", messagePtr);

    for (int i = 0; i < 11; i++) {
    printf("%c\n", messagePtr[i]);
    }
    
    printf("%c\n", *(messagePtr));

    printf("%c\n", *(messagePtr + 1));

    for (int i = 0; i < 11; i++) {
    printf("%c\n", *(messagePtr + i));
    }

    int a = 0;
    while (*(messagePtr + a) != '\0') {
    printf("%c\n", *(messagePtr + a));
    a++;
    }

    char* reversedMessage = reverseString( messagePtr );
    printf("Reversed string: %s\n", reversedMessage);
}