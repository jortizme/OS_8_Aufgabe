#ifndef UTAR_H
    #define UTAR_H

#define _POSIX_C_SOURCE 200112L
#define  _DEFAULT_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

//Offsets

#define FILENAME        0
#define FIlEMODE        1
#define OWN_USERID      2
#define GROUP_USERID    3
#define FILESIZEBYTES   4 //maybe esto me ayudara a saltar la innformacion de la file en si
#define LASTMODOCTAL    5
#define CHECKSUMHEADER  6
#define TYPEFLAG        7
#define NAMELINKFILE    8
#define USTARINDICATOR  9
#define USTARV00        10
#define OWNERUSERNAME   11
#define OWNERGRPNAME    12
#define DEVICEMAJORNR   13
#define DEVICEMINORNR   14
#define FILENAMEPREFIX  15     

// Flags for TYPEFLAG

#define NORMALFILE      '0'
#define HARDLINK        '1'
#define SYMLINK         '2'
#define CHARSPECIAL     '3'
#define BLOCKSPECIAL    '4'
#define DIRECTORY       '5'
#define FIFO            '6'

#define B_READ          0x4   
#define B_WRITE         0x2
#define B_EXEC          0x1

#define ErrorSeveral(x) fprintf(stderr,x); exit(EXIT_FAILURE)
#define ErrorNormal(x) fprintf(stderr,x); exit(EXIT_FAILURE) 

#define CtrlRtrnNeg(x) if(x < 0){\
                        fprintf(stderr,"ERROR at line %d of file %s (function %s)\n%s\n",__LINE__,__FILE__,__func__,strerror(errno));\
                        exit(EXIT_FAILURE);}

#define H_FIELDS    16

typedef struct{
    
    int offset[H_FIELDS];
    size_t size[H_FIELDS];
}Header;

typedef unsigned long long uint64;
typedef struct {

    char* FileName;
    char* GrpName;
    char* UserName;
    char* FileType;
    uint64 Zugriff; //  DIES WIRD SOWIESO WIE AUFGABE 7 IN EINEM STRING UMGEWANDELT
    uint64 FileSize;
    char* LastModTime;

}Info;

bool isUstarFile(int fd , off_t actual_offset);
void readContent(int fd, off_t actual_offset);

#endif