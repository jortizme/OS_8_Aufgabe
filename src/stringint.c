#include "include/stringint.h"

void read_bytes(int fd, void* buffer, size_t nbytes)
{
    ssize_t bytes_read = -1;
    bytes_read = read(fd, buffer, nbytes);
    if(bytes_read != nbytes)
    {
        if(bytes_read > 0){ErrorNormal("Short read for given File\n");}
        else{CtrlRtrnNeg(bytes_read);}
    }
}

void printf_Stdout(const void* buffer, size_t nbytes)
{
    ssize_t rtrnWrite = -1;
    rtrnWrite = write(STDOUT_FILENO,buffer,nbytes);
    CtrlRtrnNeg(rtrnWrite);
}

size_t stringlen(const char* buffer)
{
    size_t size = 0;

    while(*buffer++ != '\0')
        size++;

    return ++size;
}

static uint64_t OctaltoDecimal(uint64_t* octalNr)
{
    uint64_t decimalNr = 0;
    uint64_t exponent = 1;

    while(*octalNr != 0)
    {
        decimalNr += (*octalNr % 10) *  exponent;
        exponent *= 8;
        *octalNr /= 10;
    }
    return decimalNr;
}

char* convInttoStr(uint64_t val)
{
    char const items[] = "0123456789";
    uint64_t shift = val;
    unsigned int strlen = 1;
    char* string;

    do{
        strlen++;
        shift /= 10;
    }while(shift);

    string = (char*)malloc(sizeof(char)*strlen);

    string[--strlen] = '\0';

    do{
        string[--strlen] = items[val % 10];
        val /= 10;
    }while(val);    

    return string;
}

uint64_t convStrtoInt(char* string, size_t length, bool isDecimal)
{
    uint64_t mult = 1;
    uint64_t Value = 0;

    for(int i = (int)length; i > 0; i--)
    {
        Value += (string[i - 1] - '0') * mult;
        mult *= 10;
    }

    if(isDecimal)
        return Value;

    return OctaltoDecimal(&Value); 
}

