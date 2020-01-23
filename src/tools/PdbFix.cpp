#include <filesystem>

#include <stdio.h>

#include <Windows.h>
#include <Psapi.h>
#include <Dbghelp.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s file.dll", argv[0]);
        return 1;
    }

    DWORD ulSize;

    std::filesystem::path p(argv[1]);
    std::error_code ec;
    uint64_t size = std::filesystem::file_size(p, ec);
    if (ec)
        return 2;
    FILE *f = fopen(argv[1], "rb");
    if (!f)
        return 3;
    std::vector<uint8_t> file(size, 0);
    fread(file.data(), 1, size, f);
    fclose(f);

    // fix pdb name
    PIMAGE_DEBUG_DIRECTORY pDebugDesc = NULL;
    pDebugDesc = (PIMAGE_DEBUG_DIRECTORY)ImageDirectoryEntryToData(file.data(), FALSE, IMAGE_DIRECTORY_ENTRY_DEBUG, &ulSize);

    std::string pdb = p.filename().stem().string() + ".pdb";

    uint8_t *ptr = file.data() + pDebugDesc->PointerToRawData + 0x18;
    memcpy(ptr, pdb.c_str(), pdb.size() + 1);

    // write
    f = fopen(argv[1], "wb");
    if (!f)
        return 4;
    fwrite(file.data(), 1, size, f);
    fclose(f);

    return 0;
}

