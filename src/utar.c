#include "include/utar.h"

static Header HeaderField  = {{0,100,108,116,124,136,148,156,157,257,263,265,297,329,337,345},
                            {100,8,8,8,12,12,8,1,100,6,2,32,32,8,8,155}};

static void read_bytes(int fd, void* buffer, size_t nbytes)
{
    ssize_t bytes_read = 0;

    bytes_read = read(fd, buffer, nbytes);

    if(bytes_read != nbytes)
    {
        if(bytes_read > 0){ErrorNormal("Short read for given File\n");}
        else{CtrlRtrnNeg(bytes_read);}
    }
}

static uin64_t OctaltoDecimal(uin64_t* octalNr)
{
    uin64_t decimalNr = 0;
    uin64_t exponent = 1;

    while(*octalNr != 0)
    {
        decimalNr += (*octalNr % 10) *  exponent;
        exponent *= 8;
        *octalNr /= 10;
    }
    
    return decimalNr;
}

static uin64_t convertStringtoInt(char* string, size_t length)
{
    uin64_t mult = 1;
    uin64_t Value = 0;

    for(int i = (int)length; i > 0; i--)
    {
        Value += (string[i - 1] - '0') * mult;
        mult *= 10;
    }

    if((int)length == HeaderField.size[FIlEMODE] - 1)
        return Value;

    return OctaltoDecimal(&Value); 
}

bool isUstarFile(int fd)
{
    char buffer[100];
    off_t  new_offset;

    new_offset = lseek(fd,HeaderField.offset[USTARINDICATOR],SEEK_SET);
    CtrlRtrnNeg(new_offset);

    read_bytes(fd, buffer, HeaderField.size[USTARINDICATOR]);

    /*bytes_read = read(fd, buffer, HeaderField.size[USTARINDICATOR]);

    if(bytes_read != HeaderField.size[USTARINDICATOR])
    {
        if(bytes_read > 0){ErrorNormal("Short read for given File\n");}
        else{CtrlRtrnNeg(bytes_read);}
    }
    */
    
    if (strncmp(buffer,"ustar",HeaderField.size[USTARINDICATOR]) == 0)
        return true;
    else
        return false;
}

int readContent(int fd)
{
    int returnVal = -1;

    off_t  new_offset;
    off_t eof;
    char buffer[100];
   // int numericVal[12];

    Info FileInfo = {NULL,NULL,NULL,NULL,0,0,0};

    eof = lseek(fd,0,SEEK_END);
    CtrlRtrnNeg(eof);

    new_offset = lseek(fd,0,SEEK_SET);   //go to the beginning of the file
    CtrlRtrnNeg(new_offset);

    if(new_offset != 0)
    {
        ErrorSeveral("Offset could not be positioned at the beginning\n");
    }

    //while(new_offset < eof)
    //{
        for(int i = 0; i < HEADERFIELDS; i++)
        {
            switch (i)
            {
            case FILENAME:
                new_offset = lseek(fd,HeaderField.offset[FILENAME],SEEK_SET);
                read_bytes(fd, buffer, HeaderField.size[FILENAME]);
                FileInfo.FileName = strndup(buffer,HeaderField.size[FILENAME]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case FIlEMODE:

                new_offset = lseek(fd,HeaderField.offset[FIlEMODE],SEEK_SET);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[FIlEMODE]);
                //FileInfo.Zugriff = strndup(buffer,HeaderField.size[FIlEMODE]);
                FileInfo.Zugriff = convertStringtoInt(buffer, HeaderField.size[FIlEMODE]-1);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case FILESIZEBYTES:
                new_offset = lseek(fd,HeaderField.offset[FILESIZEBYTES],SEEK_SET);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[FILESIZEBYTES]);
                //FileInfo.FileSize = strndup(buffer,HeaderField.size[FILESIZEBYTES]);
                FileInfo.FileSize = convertStringtoInt(buffer, HeaderField.size[FILESIZEBYTES]-1);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;

            case LASTMODOCTAL:
                new_offset = lseek(fd,HeaderField.offset[LASTMODOCTAL],SEEK_SET);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[LASTMODOCTAL]);
                //FileInfo.LastModTime = strndup(buffer,HeaderField.size[LASTMODOCTAL]);
                FileInfo.LastModTime = convertStringtoInt(buffer, HeaderField.size[LASTMODOCTAL]-1);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case TYPEFLAG:
                new_offset = lseek(fd,HeaderField.offset[TYPEFLAG],SEEK_SET);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[TYPEFLAG]);
                FileInfo.FileType = strndup(buffer,HeaderField.size[TYPEFLAG]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case OWNERUSERNAME:
                new_offset = lseek(fd,HeaderField.offset[OWNERUSERNAME],SEEK_SET);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[OWNERUSERNAME]);
                FileInfo.UserName = strndup(buffer,HeaderField.size[OWNERUSERNAME]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case OWNERGRPNAME:
                new_offset = lseek(fd,HeaderField.offset[OWNERGRPNAME],SEEK_SET);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[OWNERGRPNAME]);
                FileInfo.GrpName = strndup(buffer,HeaderField.size[OWNERGRPNAME]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            default:
                break;
            }
        }

        printf("%s\n",FileInfo.FileType);
        printf("%llu\n",FileInfo.Zugriff);
        printf("%s\n",FileInfo.UserName);
        printf("%s\n",FileInfo.GrpName);
        printf("%llu\n",FileInfo.FileSize);
        printf("%llu\n",FileInfo.LastModTime);
        printf("%s\n",FileInfo.FileName);

        free(FileInfo.FileType);
        free(FileInfo.UserName);
        free(FileInfo.GrpName);
        //free(FileInfo.Zugriff);
        //free(FileInfo.FileSize);
        //free(FileInfo.LastModTime);
    //}

    return returnVal;
}


