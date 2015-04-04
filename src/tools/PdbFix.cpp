#include <stdio.h>

#include <Windows.h>
#include <Psapi.h>
#include <Dbghelp.h>

#include <boost/filesystem.hpp>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s file.exe new.pdb\n", argv[0]);
        return 1;
    }
    
    DWORD ulSize;

    boost::filesystem::path p(argv[1]);
    boost::system::error_code ec;
    uint64_t size = boost::filesystem::file_size(p, ec);
    if (ec)
        return 1;
    FILE *f = fopen(argv[1], "rb");
    if (!f)
        return 1;
    std::vector<uint8_t> file(size, 0);
    fread(file.data(), 1, size, f);
    fclose(f);

    PIMAGE_DEBUG_DIRECTORY pDebugDesc = NULL;
	pDebugDesc = (PIMAGE_DEBUG_DIRECTORY)ImageDirectoryEntryToData(file.data(), FALSE, IMAGE_DIRECTORY_ENTRY_DEBUG, &ulSize);
    
    uint8_t *ptr = file.data() + pDebugDesc->PointerToRawData + 0x18;
    memcpy(ptr, argv[2], strlen(argv[2]) + 1);

    f = fopen(argv[1], "wb");
    if (!f)
        return 1;
    fwrite(file.data(), 1, size, f);
    fclose(f);

    return 0;
}

