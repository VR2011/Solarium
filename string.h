#ifndef STRING_H
#define STRING_H

int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, unsigned int n);
int strlen(const char *str);

void strcpy(char *dest, const char *src);
void memmove(char *dest, const char *src, int n);

#endif