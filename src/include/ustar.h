#ifndef USTAR_H
    #define USTAR_H

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
#include <inttypes.h>

//Fields
#define FILENAME        0
#define FIlEMODE        1
#define OWN_USERID      2
#define GROUP_USERID    3
#define FILESIZEBYTES   4
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

//Bit positions of the file permissions
#define B_READ          0x4   
#define B_WRITE         0x2
#define B_EXEC          0x1

#define TIME_FORMAT_LENGTH 20

//Error handling macros
#define ErrorSeveral(x) fprintf(stderr,x); exit(EXIT_FAILURE)
#define ErrorNormal(x) fprintf(stderr,x); exit(EXIT_FAILURE) 

#define CtrlRtrnNeg(x) if(x < 0){\
                        fprintf(stderr,"ERROR at line %d of file %s (function %s)\n%s\n",__LINE__,__FILE__,__func__,strerror(errno));\
                        exit(EXIT_FAILURE);}

//How much fields are in a header
#define H_FIELDS    16

/*
    Struct to store the size and offset of
    every field contained in the header.
*/
typedef struct{
    
    uint16_t offset[H_FIELDS];
    size_t size[H_FIELDS];
}Header;

/*
    Struct to store the information contained
    in the selected header's fields.
*/
typedef struct {

    char* FileName;
    char* GrpName;
    char* UserName;
    char* FileType;
    uint64_t Zugriff; 
    uint64_t FileSize;
    time_t LastModTime;

}Info;

/*
    +checks if a header field is in a ustar archive
    -param(1) -> file descriptor
    -param(2) -> beginning of the header
    -return -> true if it's in a ustar archive
*/
bool isUstarFile(int fd , off_t actual_offset);

/*
    +reads the whole content of an ustar format file
    -param(1) -> file descriptor
    -param(2) -> beginning of the file
    +in case of any error in the process the program
    exits and leaves a proper error message
*/
void readContent(int fd, off_t actual_offset);

#endif