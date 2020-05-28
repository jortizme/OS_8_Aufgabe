#ifndef STRINGINT_H
    #define STRINGINT_H

#include "ustar.h"

void read_bytes(int fd, void* buffer, size_t nbytes);
void printf_Stdout(const void* buffer, size_t nbytes);
size_t stringlen(const char* buffer);
char* convInttoStr(uint64_t val);
uint64_t convStrtoInt(char* string, size_t length, bool isZugriff);

#endif