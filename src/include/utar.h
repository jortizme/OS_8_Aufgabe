
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

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
#define FILENAMEPREFIX   15     

// Flags for TYPEFLAG

#define NORMALFILE      '0'
#define HARDLINK        '1'
#define SYMLINK         '2'
#define CHARSPECIAL     '3'
#define BLOCKSPECIAL    '4'
#define DIRECTORY       '5'
#define FIFO            '6'

#define ErrorSeveral(x) fprintf(stderr,x); exit(EXIT_FAILURE) 

#define CtrlRtrnNeg(y,x) if(x == -1){\
                        fprintf(stderr,"%s failed at line %d of file %s (function %s)\n",y,__LINE__,__FILE__,__func__);\
                        exit(EXIT_FAILURE);}

#define CtrlRtrnNULL(y,x) if(x == NULL){ \
                        fprintf(stderr,"%s failed at line %d of file %s (function %s)\n",y,__LINE__,__FILE__,__func__);\
                        exit(EXIT_FAILURE);}

#define HEADERFIELDS    16

struct Header{

    int offset[HEADERFIELDS];
    int size[HEADERFIELDS];
} HeaderField = {{0,100,108,116,124,136,148,156,157,257,263,265,297,329,337,345},
                {100,8,8,8,12,12,8,1,100,6,2,32,32,8,8,155}
};