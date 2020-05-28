#include "include/ustar.h"
#include "include/stringint.h"

static const Header HFld  = {{0,100,108,116,124,136,148,156,157,257,263,265,297,329,337,345},
                            {100,8,8,8,12,12,8,1,100,6,2,32,32,8,8,155}};

bool isUstarFile(int fd, off_t actual_offset)
{
    char buffer[100];
    off_t  new_offset;
    bool retVal = false;

    new_offset = lseek(fd,HFld.offset[USTARINDICATOR],SEEK_CUR);
    CtrlRtrnNeg(new_offset);

    read_bytes(fd, buffer, HFld.size[USTARINDICATOR]);

    lseek(fd, actual_offset, SEEK_SET);
    
    if (stringncmp(buffer,"ustar",HFld.size[USTARINDICATOR]))
        retVal = true;

    return retVal;
}

static bool eof(int fd, char* buffer, off_t actual_offset)
{
    off_t  new_offset;
    bool retVal = false;

    bool isUstar = isUstarFile(fd , actual_offset);

    new_offset = lseek(fd,HFld.size[FILESIZEBYTES],SEEK_CUR);
    CtrlRtrnNeg(new_offset);

    read_bytes(fd, buffer, HFld.size[FILESIZEBYTES]);

    lseek(fd, actual_offset, SEEK_SET);

    if(*buffer == '\0' && !isUstar)
        retVal = true;

    return retVal;
}

void readContent(int fd , off_t actual_offset)
{
    char buffer[100];
    char* perm;
    unsigned int us_gr_oth[3];
    char TimeFormat[TIME_FORMAT_LENGTH];
    char* fileSize;
    Info FInfo = {NULL,NULL,NULL,NULL,0,0,0};
    uint64_t fileAmount = 0;

    off_t const Pad1 = HFld.size[OWN_USERID] + HFld.size[GROUP_USERID];
    off_t const Pad2 = HFld.size[CHECKSUMHEADER];
    off_t const Pad3 = HFld.size[NAMELINKFILE] + HFld.size[USTARINDICATOR] + HFld.size[USTARV00];
    off_t const Pad4 = HFld.size[DEVICEMAJORNR] + HFld.size[DEVICEMINORNR] + HFld.size[FILENAMEPREFIX] + 12;

    while(!eof(fd, buffer,actual_offset))
    {
        fileAmount++;

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
                FInfo.Zugriff = StrtoInt(buffer, HFld.size[FIlEMODE]-1, true);
                actual_offset = lseek(fd,0,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                break;
            
            case FILESIZEBYTES:
                actual_offset = lseek(fd,Pad1,SEEK_CUR);
                CtrlRtrnNeg(actual_offset);
                read_bytes(fd, buffer, HFld.size[FILESIZEBYTES]);
                FInfo.FileSize = StrtoInt(buffer, HFld.size[FILESIZEBYTES]-1, false);
                actual_offset = lseek(fd,0,SEEK_CUR);
                break;

            case LASTMODOCTAL:
                read_bytes(fd, buffer, HFld.size[LASTMODOCTAL]);
                FInfo.LastModTime = StrtoInt(buffer, HFld.size[LASTMODOCTAL]-1 , false);
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

        fileSize = InttoStr(FInfo.FileSize);

        //The block info's size must always be 512 divisible
        if(FInfo.FileSize % 512 != 0)
        {
            int mult = FInfo.FileSize / 512;
            mult++;
            FInfo.FileSize = mult*512;
        }

        actual_offset = lseek(fd,Pad4 + FInfo.FileSize, SEEK_CUR);

        char typePerm[] = "----------";
        perm = InttoStr(FInfo.Zugriff);

        switch (*FInfo.FileType)
        {
            case NORMALFILE:                        break;
            case HARDLINK:      typePerm[0] = 'h';  break;
            case SYMLINK:       typePerm[0] = 'l';  break;
            case CHARSPECIAL:   typePerm[0] = 'c';  break;
            case BLOCKSPECIAL:  typePerm[0] = 'b';  break;
            case DIRECTORY:     typePerm[0] = 'd';  break;
            case FIFO:          typePerm[0] = 'p';  break;
            default:                                break;
        }

        for(int i = 0; i < 3; i++)
        {
            us_gr_oth[i] = perm[i] - '0';

            if(us_gr_oth[i] & B_READ)
                typePerm[1 + i*3] = 'r';
            
            if(us_gr_oth[i] & B_WRITE)
                typePerm[2 + i*3] = 'w';
            
            if(us_gr_oth[i] & B_EXEC)
                typePerm[3 + i*3] = 'x';
        }

        //Get a proper time format
        strftime(TimeFormat,TIME_FORMAT_LENGTH,"%Y-%m-%d %H:%M",localtime(&FInfo.LastModTime));

        print_stdout(typePerm);
        print_stdout(" ");
        print_stdout(FInfo.UserName);
        print_stdout("/");
        print_stdout(FInfo.GrpName);
        print_stdout("\t");
        print_stdout(fileSize);
        print_stdout("\t");
        print_stdout(TimeFormat);
        print_stdout(" ");
        print_stdout(FInfo.FileName);
        print_stdout("\r\n");

        free(FInfo.FileType);
        free(FInfo.UserName);
        free(FInfo.GrpName);
        free(fileSize);
        free(perm);
    }

    char* fileAmountstr = InttoStr(fileAmount);

    print_stdout("\n");
    print_stdout("Amount of files = ");
    print_stdout(fileAmountstr);
    print_stdout("\n");
    free(fileAmountstr);
}


