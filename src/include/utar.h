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

#define ErrorSeveral(x) fprintf(stderr,x); exit(EXIT_FAILURE)
#define ErrorNormal(x) fprintf(stderr,x); exit(EXIT_FAILURE) 

#define CtrlRtrnNeg(x) if(x < 0){\
                        fprintf(stderr,"ERROR at line %d of file %s (function %s)\n%s\n",__LINE__,__FILE__,__func__,strerror(errno));\
                        exit(EXIT_FAILURE);}

#define CtrlRtrnNULL(x) if(x == NULL){ \
                        fprintf(stderr," ERROR at line %d of file %s (function %s)\n%s\n",y,__LINE__,__FILE__,__func__,strerror(errno));\
                        exit(EXIT_FAILURE);}

#define HEADERFIELDS    16

typedef struct{

    int offset[HEADERFIELDS];
    size_t size[HEADERFIELDS];
}Header;

/*typedef struct 
{
    int closed;
    off_t  new_offset;
    mode_t mode; //fast sicher wir brauchen das nicht
    ssize_t bytes_read;
}HelpVariables;
*/
typedef unsigned long long uin64_t;
typedef struct 
{
    char* FileName;
    char* GrpName;
    char* UserName;
    char* FileType;
    uin64_t Zugriff;
    uin64_t FileSize;
    uin64_t LastModTime;

/*   int Zugriff;
    int FileSize;
    int LastModTime;
*/
}Info;

/*
typedef struct 
{
    char FileName[100];
    char GrpName[32];
    char UserName[32];
    char FileType;
    char Zugriff[8];
    int FileSize[12];
    int LastModTime[12];

}Info;
*/

bool isUstarFile(int fd);
int readContent(int fd);


#endif