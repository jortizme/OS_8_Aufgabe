#ifndef STRINGINT_H
    #define STRINGINT_H

#include "ustar.h"

void read_bytes(int fd, void* buffer, size_t nbytes);
void print_stdout(const char* buffer);
size_t stringlen(const char* buffer);
char* InttoStr(uint64_t val);
uint64_t StrtoInt(char* string, size_t length, bool isZugriff);
bool stringncmp(const char* str1, const char* str2, size_t nBytes);

#endif