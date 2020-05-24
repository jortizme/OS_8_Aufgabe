#include "include/utar.h"

Header HeaderField  = {{0,100,108,116,124,136,148,156,157,257,263,265,297,329,337,345},
                            {100,8,8,8,12,12,8,1,100,6,2,32,32,8,8,155}};

bool isUstarFile(int fd, char* buffer)
{
    off_t  new_offset;
    ssize_t bytes_read;

    new_offset = lseek(fd,HeaderField.offset[USTARINDICATOR],SEEK_SET);
    CtrlRtrnNeg(new_offset);

    bytes_read = read(fd, buffer, HeaderField.size[USTARINDICATOR]);

    if(bytes_read != HeaderField.size[USTARINDICATOR])
    {
        if(bytes_read > 0){ErrorNormal("Short read for given File\n");}
        else{CtrlRtrnNeg(bytes_read);}
    }
    
    if (strncmp(buffer,"ustar",HeaderField.size[USTARINDICATOR]) == 0)
        return true;
    else
        return false;
}