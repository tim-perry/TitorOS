#include <stdint.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <libc/syscalls.h>
#include <libc/stdlib.h>

void echo();

void main() {
    //initialize IO streams
    char* command = malloc(100);

    uint8_t file = open("/file", 0x3); //will get fd 3

    write(file, "YO HELLO", 9);
    read(file, command, 9);

    printf("The written string was: %s\n", command);

    //launch stuff
    printf("PRINTF TEST\n");
    printf("capital hex: %X\n", 0x1234abcd);
    printf("lowercase hex: %x\n", 0xabcd1234);
    printf("unsigned decimal: %u\n", 123456789);
    printf("character: %c\n", '#');
    printf("string: %s\n\n", "wooyeah");

    
    uint32_t pid = fork();
    if (pid == 0) {
        printf("I am the child of a fork");
        exec("/mnt/ls.bin");
    }
    printf("I am the parent and I am still a shell\n");

    while(1) {
        printf("shell>");
        read(stdin, command, 100);
        char* token = strtok(command, " ");
        if (token == NULL);
        else if (strcmp(token, "echo") == 0) echo();
        else printf("unrecognized command \"%s\"\n", token);
    }
}

void echo() {
    char* token = strtok(NULL, " ");
    while(token != NULL) {
        printf(token);
        printf(" ");
        token = strtok(NULL, " ");
    }
    printf("\n");
}