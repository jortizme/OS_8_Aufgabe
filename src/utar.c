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

static uint64 OctaltoDecimal(uint64* octalNr)
{
    uint64 decimalNr = 0;
    uint64 exponent = 1;

    while(*octalNr != 0)
    {
        decimalNr += (*octalNr % 10) *  exponent;
        exponent *= 8;
        *octalNr /= 10;
    }
    
    return decimalNr;
}

static uint64 convertStringtoInt(char* string, size_t length, bool isZugriff)
{
    uint64 mult = 1;
    uint64 Value = 0;

    for(int i = (int)length; i > 0; i--)
    {
        Value += (string[i - 1] - '0') * mult;
        mult *= 10;
    }

    if(isZugriff)
        return Value;

    return OctaltoDecimal(&Value); 
}

bool isUstarFile(int fd, off_t actual_offset)
{
    char buffer[100];
    off_t  new_offset;

    new_offset = lseek(fd,HeaderField.offset[USTARINDICATOR],SEEK_CUR);
    CtrlRtrnNeg(new_offset);

    read_bytes(fd, buffer, HeaderField.size[USTARINDICATOR]);

    lseek(fd, actual_offset, SEEK_SET);
    
    if (strncmp(buffer,"ustar",HeaderField.size[USTARINDICATOR]) == 0)
        return true;
    else
        return false;
}

bool eof(int fd, char* buffer, off_t actual_offset)
{
    //char buffer[100];
    off_t  new_offset;

    bool isUstar = isUstarFile(fd , actual_offset );

    new_offset = lseek(fd,HeaderField.size[FILESIZEBYTES],SEEK_CUR);
    CtrlRtrnNeg(new_offset);

    read_bytes(fd, buffer, HeaderField.size[FILESIZEBYTES]);

    lseek(fd, actual_offset, SEEK_SET);

    if(*buffer == '\0' && !isUstar)
        return true;
    else 
        return false;
}

int readContent(int fd)
{
    int returnVal = -1;

    off_t  new_offset;
   // off_t eof;
    char buffer[100];

    Info FileInfo = {NULL,NULL,NULL,NULL,0,0,0};

    //eof = lseek(fd,0,SEEK_END);
   // CtrlRtrnNeg(eof);

    new_offset = lseek(fd,0,SEEK_SET);   //go to the beginning of the file
    CtrlRtrnNeg(new_offset);

    if(new_offset != 0)
    {
        ErrorSeveral("Offset could not be positioned at the beginning\n");
    }

    off_t padding1 = HeaderField.size[OWN_USERID] + HeaderField.size[GROUP_USERID];
    off_t padding2 = HeaderField.size[CHECKSUMHEADER];
    off_t padding3 = HeaderField.size[NAMELINKFILE] + HeaderField.size[USTARINDICATOR] + HeaderField.size[USTARV00];
    off_t padding4 = HeaderField.size[DEVICEMAJORNR] + HeaderField.size[DEVICEMINORNR] + HeaderField.size[FILENAMEPREFIX] + 12;
    uint64 InfoBlock;
    int count = 0;

    while(true)
    {
        if(eof(fd, buffer,new_offset))
            break;
        

        count++;

        for(int i = 0; i < HEADERFIELDS; i++)
        {
            switch (i)
            {
            case FILENAME:
                read_bytes(fd, buffer, HeaderField.size[FILENAME]);
                FileInfo.FileName = strndup(buffer,HeaderField.size[FILENAME]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case FIlEMODE:
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[FIlEMODE]);
                FileInfo.Zugriff = convertStringtoInt(buffer, HeaderField.size[FIlEMODE]-1, true);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case FILESIZEBYTES:
                new_offset = lseek(fd,padding1,SEEK_CUR);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[FILESIZEBYTES]);
                FileInfo.FileSize = convertStringtoInt(buffer, HeaderField.size[FILESIZEBYTES]-1, false);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;

            case LASTMODOCTAL:
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[LASTMODOCTAL]);
                FileInfo.LastModTime = convertStringtoInt(buffer, HeaderField.size[LASTMODOCTAL]-1 , false);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case TYPEFLAG:
                new_offset = lseek(fd,padding2,SEEK_CUR);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[TYPEFLAG]);
                FileInfo.FileType = strndup(buffer,HeaderField.size[TYPEFLAG]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case OWNERUSERNAME:
                new_offset = lseek(fd,padding3,SEEK_CUR);
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[OWNERUSERNAME]);
                FileInfo.UserName = strndup(buffer,HeaderField.size[OWNERUSERNAME]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            case OWNERGRPNAME:
                CtrlRtrnNeg(new_offset);
                read_bytes(fd, buffer, HeaderField.size[OWNERGRPNAME]);
                FileInfo.GrpName = strndup(buffer,HeaderField.size[OWNERGRPNAME]);
                new_offset = lseek(fd,0,SEEK_CUR);
                break;
            
            default:
                break;
            }
        }

        InfoBlock = FileInfo.FileSize;

        if(InfoBlock != 0 && InfoBlock % 512 != 0)
        {
            int mult = InfoBlock / 512;
            mult++;
            InfoBlock = mult*512;
        }

        new_offset = lseek(fd,padding4 + InfoBlock, SEEK_CUR);

        //printf("%s\n",FileInfo.FileType);
        //printf("%llu\n",FileInfo.Zugriff);
        //printf("%s\n",FileInfo.UserName);
        //printf("%s\n",FileInfo.GrpName);
        //printf("%llu\n",FileInfo.FileSize);
        //printf("%llu\n",FileInfo.LastModTime);
        printf("%s\n",FileInfo.FileName);
        

        free(FileInfo.FileType);
        free(FileInfo.UserName);
        free(FileInfo.GrpName);

    }

    printf("%d\n", count);

    return returnVal;
}


