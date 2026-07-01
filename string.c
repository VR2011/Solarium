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