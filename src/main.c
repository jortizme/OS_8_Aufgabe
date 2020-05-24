#include "include/utar.h"

extern int errno;

int main (int argc, char **argv)
{
    //if(argc != 2) {ErrorSeveral("Usage : ./<executable> <filename>\n");}
    int fd;
    mode_t mode = S_IRUSR | S_IRGRP | S_IROTH; //fast sicher wir brauchen das nicht;
    int closed;

    errno = 0;

    fd = open("tar-1.32.tar",O_RDONLY, mode);
    CtrlRtrnNeg(fd);

    if(isUstarFile(fd))
        readContent(fd);

    closed = close(fd);
    CtrlRtrnNeg(closed);


    exit(EXIT_SUCCESS);
}
