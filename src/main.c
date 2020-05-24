#include "include/utar.h"

extern int errno;
extern struct Header HeaderField;

int main (int argc, char **argv)
{
    if(argc != 2) {ErrorSeveral("Usage : ./<executable> <filename>\n");}

    char buf[20];
    int fd;
    mode_t mode = S_IRUSR | S_IRGRP | S_IROTH; //fast sicher wir brauchen das nicht;
    int closed;

    errno = 0;

    fd = open(argv[argc-1],O_RDONLY, mode);
    CtrlRtrnNeg(fd);

    if(isUstarFile(fd,buf))
        printf("is a ustar File\n");
        //hier comes what to do ist
    
    else
    {
        printf("It's not a ustar File\n");
    }
    
    
    
    closed = close(fd);
    CtrlRtrnNeg(closed);


    exit(EXIT_SUCCESS);
}
