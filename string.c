#include "string.h"

int strcmp(const char *a, const char *b) {
    while(*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(unsigned char *)a - *(unsigned char *)b;
}

int strncmp(const char *a, const char *b, unsigned int n) {
    while(n && *a && (*a == *b)) {
        a++;
        b++;
        n--;
    }
    if(n == 0) {return 0;}
    return *(unsigned char *)a - *(unsigned char *)b;
}

int strlen(const char *str) {
    int len = 0;
    while(str[len]) {
        len++;
    }
    return len;
}

void strcpy(char *dest, const char *src) {
    while(*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void memmove(char *dest, const char *src, int n) {
    if(dest < src) {
        while(n--) {*dest++ = *src++;}
    }
    else {
        dest += n;
        src += n;
        while(n--) {*--dest = *--src;}
    }
}