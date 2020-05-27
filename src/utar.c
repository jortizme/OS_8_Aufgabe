#include "include/utar.h"

static const Header HFld  = {{0,100,108,116,124,136,148,156,157,257,263,265,297,329,337,345},
                            {100,8,8,8,12,12,8,1,100,6,2,32,32,8,8,155}};

static void read_bytes(int fd, void* buffer, size_t nbytes)
{
    ssize_t bytes_read = -1;
    bytes_read = read(fd, buffer, nbytes);
    if(bytes_read != nbytes)
    {
        if(bytes_read > 0){ErrorNormal("Short read for given File\n");}
        else{CtrlRtrnNeg(bytes_read);}
    }
}

static void printf_Stdout(const void* buffer, size_t nbytes)
{
    ssize_t rtrnWrite = -1;
    rtrnWrite = write(STDOUT_FILENO,buffer,nbytes);
    CtrlRtrnNeg(rtrnWrite);
}

static size_t stringlen(const char* buffer)
{
    size_t size = 0;

    while(*buffer++ != '\0')
        size++;

    return ++size;
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

static char* convInttoStr(uint64 val)
{
    char const items[] = "0123456789";
    uint64 shift = val;
    unsigned int strlen = 1;
    char* string;

    do{
        strlen++;
        shift /= 10;
    }while(shift);

    string = (char*)malloc(sizeof(char)*strlen);

    string[--strlen] = '\0';

    do{
        string[--strlen] = items[val % 10];
        val /= 10;
    }while(val);    

    return string;
}

static uint64 convStrtoInt(char* string, size_t length, bool isZugriff)
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
    
    if (strncmp(buffer,"ustar",HFld.size[USTARINDICATOR]) == 0) //STRNCMP IMPLEMENTIEREN
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

void readContent(int fd , off_t actual_offset)
{
    char buffer[100];
    char* fileSize;
    Info FInfo = {NULL,NULL,NULL,NULL,0,0,0};
    int count = 0;

    off_t const Pad1 = HFld.size[OWN_USERID] + HFld.size[GROUP_USERID];
    off_t const Pad2 = HFld.size[CHECKSUMHEADER];
    off_t const Pad3 = HFld.size[NAMELINKFILE] + HFld.size[USTARINDICATOR] + HFld.size[USTARV00];
    off_t const Pad4 = HFld.size[DEVICEMAJORNR] + HFld.size[DEVICEMINORNR] + HFld.size[FILENAMEPREFIX] + 12;

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
                FInfo.Zugriff = convStrtoInt(buffer, HFld.size[FIlEMODE]-1, true);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            case FILESIZEBYTES:
                actual_offset = lseek(fd,Pad1,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                read_bytes(fd, buffer, HFld.size[FILESIZEBYTES]);
                FInfo.FileSize = convStrtoInt(buffer, HFld.size[FILESIZEBYTES]-1, false);
                actual_offset = lseek(fd,0,SEEK_CUR);
                break;

            case LASTMODOCTAL:
                read_bytes(fd, buffer, HFld.size[LASTMODOCTAL]);
                FInfo.LastModTime = convInttoStr(convStrtoInt(buffer, HFld.size[LASTMODOCTAL]-1 , false));
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

        fileSize = convInttoStr(FInfo.FileSize);

        //The block info's size must always be 512 divisible
        if(FInfo.FileSize % 512 != 0)
        {
            int mult = FInfo.FileSize / 512;
            mult++;
            FInfo.FileSize = mult*512;
        }

        actual_offset = lseek(fd,Pad4 + FInfo.FileSize, SEEK_CUR);

        printf_Stdout(FInfo.FileType, stringlen(FInfo.FileType));
        printf_Stdout("\t", sizeof("\t"));
        printf_Stdout((void*)&FInfo.Zugriff, sizeof(FInfo.Zugriff));
        printf_Stdout("\t", sizeof("\t"));
        printf_Stdout(FInfo.GrpName, stringlen(FInfo.GrpName));
        printf_Stdout("\t", sizeof("\t"));
        printf_Stdout(FInfo.UserName, stringlen(FInfo.UserName));
        printf_Stdout("\t", sizeof("\t"));
        printf_Stdout(fileSize, stringlen(fileSize));
        printf_Stdout("\t", sizeof("\t"));
        printf_Stdout(FInfo.LastModTime, strlen(FInfo.LastModTime));
        printf_Stdout("\t", sizeof("\t"));
        printf_Stdout(FInfo.FileName, stringlen(FInfo.FileName));
        printf_Stdout("\r\n", sizeof("\r\n"));



        //printf("%s\n",FInfo.FileType);
        //printf("%llu\n",FInfo.Zugriff);
        //printf("%s\n",FInfo.UserName);
        //printf("%s\n",FInfo.GrpName);
        //printf("%llu\n",FInfo.FileSize);
        //printf("%llu\n",FInfo.LastModTime);
        //printf("%s\n",FInfo.FileName);

        free(FInfo.FileType);
        free(FInfo.UserName);
        free(FInfo.GrpName);

    }

    printf("%d\n", count);//!!!!!!!!!!!!
}


