#include "include/ustar.h"

extern int errno;

int main (int argc, char **argv)
{
    if(argc != 2) {ErrorSeveral("Usage : ./<executable> <filename>\n");}
    
    int fd;
    mode_t mode = S_IRUSR | S_IRGRP | S_IROTH;
    int closed;
    off_t offset_begin;

    errno = 0;

    fd = open(argv[argc - 1],O_RDONLY, mode);
    CtrlRtrnNeg(fd);

    offset_begin = lseek(fd, 0, SEEK_SET);
    CtrlRtrnNeg(offset_begin);

    if(isUstarFile(fd,offset_begin))
        readContent(fd,offset_begin);

    closed = close(fd);
    CtrlRtrnNeg(closed);


    exit(EXIT_SUCCESS);
}
