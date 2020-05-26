#include "include/utar.h"

static Header HFld  = {{0,100,108,116,124,136,148,156,157,257,263,265,297,329,337,345},
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

    new_offset = lseek(fd,HFld.offset[USTARINDICATOR],SEEK_CUR);
    CtrlRtrnNeg(new_offset);

    read_bytes(fd, buffer, HFld.size[USTARINDICATOR]);

    lseek(fd, actual_offset, SEEK_SET);
    
    if (strncmp(buffer,"ustar",HFld.size[USTARINDICATOR]) == 0)
        return true;
    else
        return false;
}

static bool eof(int fd, char* buffer, off_t actual_offset)
{
    //char buffer[100];
    off_t  new_offset;

    bool isUstar = isUstarFile(fd , actual_offset);

    new_offset = lseek(fd,HFld.size[FILESIZEBYTES],SEEK_CUR);
    CtrlRtrnNeg(new_offset);

    read_bytes(fd, buffer, HFld.size[FILESIZEBYTES]);

    lseek(fd, actual_offset, SEEK_SET);

    if(*buffer == '\0' && !isUstar)
        return true;
    else 
        return false;
}

int readContent(int fd , off_t actual_offset)
{
    int returnVal = -1;     //!!!!!!!!!!!!!!!!!!!!!!!1
    char buffer[100];
    Info FInfo = {NULL,NULL,NULL,NULL,0,0,0};
    int count = 0;

    off_t Pad1 = HFld.size[OWN_USERID] + HFld.size[GROUP_USERID];
    off_t Pad2 = HFld.size[CHECKSUMHEADER];
    off_t Pad3 = HFld.size[NAMELINKFILE] + HFld.size[USTARINDICATOR] + HFld.size[USTARV00];
    off_t Pad4 = HFld.size[DEVICEMAJORNR] + HFld.size[DEVICEMINORNR] + HFld.size[FILENAMEPREFIX] + 12;

    while(!eof(fd, buffer,actual_offset))
    {
        count++;

        for(int i = 0; i < H_FIELDS; i++)
        {
            switch (i)
            {
            case FILENAME:
                read_bytes(fd, buffer, HFld.size[FILENAME]);
                FInfo.FileName = strndup(buffer,HFld.size[FILENAME]);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            case FIlEMODE:
                read_bytes(fd, buffer, HFld.size[FIlEMODE]);
                FInfo.Zugriff = convertStringtoInt(buffer, HFld.size[FIlEMODE]-1, true);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            case FILESIZEBYTES:
                actual_offset = lseek(fd,Pad1,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                read_bytes(fd, buffer, HFld.size[FILESIZEBYTES]);
                FInfo.FileSize = convertStringtoInt(buffer, HFld.size[FILESIZEBYTES]-1, false);
                actual_offset = lseek(fd,0,SEEK_CUR);
                break;

            case LASTMODOCTAL:
                read_bytes(fd, buffer, HFld.size[LASTMODOCTAL]);
                FInfo.LastModTime = convertStringtoInt(buffer, HFld.size[LASTMODOCTAL]-1 , false);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            case TYPEFLAG:
                actual_offset = lseek(fd,Pad2,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                read_bytes(fd, buffer, HFld.size[TYPEFLAG]);
                FInfo.FileType = strndup(buffer,HFld.size[TYPEFLAG]);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            case OWNERUSERNAME:
                actual_offset = lseek(fd,Pad3,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                read_bytes(fd, buffer, HFld.size[OWNERUSERNAME]);
                FInfo.UserName = strndup(buffer,HFld.size[OWNERUSERNAME]);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            case OWNERGRPNAME:
                read_bytes(fd, buffer, HFld.size[OWNERGRPNAME]);
                FInfo.GrpName = strndup(buffer,HFld.size[OWNERGRPNAME]);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            default:
                break;
            }
        }

        //The block's must always be 512 divisible
        if(FInfo.FileSize != 0 && FInfo.FileSize % 512 != 0)
        {
            int mult = FInfo.FileSize / 512;
            mult++;
            FInfo.FileSize = mult*512;
        }

        actual_offset = lseek(fd,Pad4 + FInfo.FileSize, SEEK_CUR);

        //printf("%s\n",FInfo.FileType);
        //printf("%llu\n",FInfo.Zugriff);
        //printf("%s\n",FInfo.UserName);
        //printf("%s\n",FInfo.GrpName);
        //printf("%llu\n",FInfo.FileSize);
        //printf("%llu\n",FInfo.LastModTime);
        printf("%s\n",FInfo.FileName);
        free(FInfo.FileType);
        free(FInfo.UserName);
        free(FInfo.GrpName);

    }

    printf("%d\n", count);

    return returnVal;
}


