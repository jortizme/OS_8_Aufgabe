#ifndef STRINGINT_H
    #define STRINGINT_H

#include "ustar.h"

/*
    +reads nbytes from the file pointed to by fd
    and checks if the operation was succesful
    -param(1) -> file descriptor
    -param(2) -> char array to store nbytes
    -param(3) -> amount of bytes to be read
    +the program exits when operation unsuccessful 
*/
void read_bytes(int fd, char* buffer, size_t nbytes);

/*
    +writes the content of buffer in the standard output
    -checks if the operation was succesful
    -param(1) -> char array containing the desired information
    +the program exits when operation unsuccessful 
*/
void print_stdout(const char* buffer);

/*
    +own implementation of strlen()
    -return -> size of the char array (including the '\0')
*/
size_t stringlen(const char* buffer);

/*
    +own implementation of strncmp()
    -param(1) -> first char array
    -param(2) -> second char array
    -param(3) -> amount of bytes to be compared
    -return -> true if both arrays are equal
*/
bool stringncmp(const char* str1, const char* str2, size_t nBytes);

/*
    +converts an uint64_t number into a string
    -param(1) -> value of the number
    -return -> malloc allocated char pointer
*/
char* InttoStr(uint64_t val);

/*
    +converts a string number representation into an uint64_t number
    -param(1) -> char array 
    -param(2) -> if param(1) represents a decimal number then true
                otherwise in case of octal number false
    -return -> uint64_t number
*/
uint64_t StrtoInt(char* string, bool isDecimal);

#endif