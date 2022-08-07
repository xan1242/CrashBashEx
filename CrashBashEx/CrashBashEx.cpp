// Crash Bash file extractor
// by Tenjoin/Xan

#include <iostream>
#include <string> 

struct BashOffset
{
    uint32_t LBA;
    uint32_t size;
};

char OutFilename[1024];
char OutType[32];

// Crash Bash defines its types in the first half-word (16bit) value of the file it loads
// This function is designed to somewhat demistify the output files at a glance via the filenames
// the final block is a code overlay and it usually has something written on the upper 8 bits, so we can just assume that's a code overlay
// also there are some TGA files in the game for SOME reason...
const char* GetCrashBshTypeString(void* filebuf, uint32_t size)
{
    uint16_t type = *(uint16_t*)filebuf;

    // if the type is undefined, check the EOF for the targa string
    if (!type)
    {
        char* targastr = (char*)(((int)filebuf) + (size - 0x12));
        if (strcmp(targastr, "TRUEVISION-XFILE.") == 0)
        {
            return "image.tga";
        }
    }

    if (type & 0xFF00)
        return "code.bin";

    switch (type)
    {
    //case 0x7:
    //    return "image.bin";
    //case 0x14:
    //    return "music.bin";
    default:
        sprintf(OutType, "type_0x%hX.bin", type);
        return OutType;
    }
    return NULL;
}

// InExeName = executable filename
// InDataName = CRASHBSH.DAT
// OutFolder = self explanatory
// TableStart = start offset of the table where the disc addresses are stored
// TableEnd = end offset of that table
int ExtractBashData(char* InExeName, char* InDataName, char* OutFolder, uint32_t TableStart, uint32_t TableEnd)
{
    void* ExeFileBuffer;
    void* copybuffer;
    uint32_t TableSize = TableEnd - TableStart;
    uint32_t TableCount = TableSize / sizeof(BashOffset);
    BashOffset* OffsetTable;
    FILE* fout;

    struct stat st = {0};
    if (stat(OutFolder, &st))
    {
        wchar_t* widename = (wchar_t*)calloc(strlen(OutFolder), sizeof(wchar_t));
        mbstowcs(widename, OutFolder, 512);
        printf("Creating directory: %s\n", OutFolder);
        _wmkdir(widename);
        //free(widename);
    }

    FILE* fexe = fopen(InExeName, "rb");
    if (!fexe)
    {
        printf("ERROR: Can't open %s\n", InExeName);
        perror("ERROR");
        return -1;
    }
    stat(InExeName, &st);
    ExeFileBuffer = malloc(st.st_size);
    fread(ExeFileBuffer, st.st_size, 1, fexe);
    fclose(fexe);

    OffsetTable = (BashOffset*)(((int)ExeFileBuffer) + TableStart);

    FILE* fdat = fopen(InDataName, "rb");
    if (!fdat)
    {
        printf("ERROR: Can't open %s\n", InDataName);
        perror("ERROR");
        return -1;
    }

    for (int i = 0; i < TableCount; i++)
    {
        copybuffer = malloc(OffsetTable[i].size);
        fseek(fdat, OffsetTable[i].LBA * 0x800, SEEK_SET);
        fread(copybuffer, OffsetTable[i].size, 1, fdat);

        sprintf(OutFilename, "%s\\%d.%s", OutFolder, i, GetCrashBshTypeString(copybuffer, OffsetTable[i].size));
        printf("Extracting: %s (@0x%X size: 0x%X)\n", OutFilename, OffsetTable[i].LBA * 0x800, OffsetTable[i].size);
        fout = fopen(OutFilename, "wb");

        fwrite(copybuffer, OffsetTable[i].size, 1, fout);

        fclose(fout);
        free(copybuffer);
    }
    
    fclose(fdat);
    free(ExeFileBuffer);
    return 0;

}
int main(int argc, char* argv[])
{
    printf("Crash Bash File Extractor\n");
    if (argc < 6)
    {
        printf("USAGE: %s InExeName InDataName OutFolder TableStart TableEnd\n", argv[0]);
        return -1;
    }

    return ExtractBashData(argv[1], argv[2], argv[3], std::stoi(argv[4], nullptr, 16), std::stoi(argv[5], nullptr, 16));
}

