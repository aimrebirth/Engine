#include <stdio.h>

#include <Windows.h>
#include <Psapi.h>
#include <Dbghelp.h>

#include <boost/filesystem.hpp>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s file.exe", argv[0]);
        return 1;
    }
    
    DWORD ulSize;

    boost::filesystem::path p(argv[1]);
    boost::system::error_code ec;
    uint64_t size = boost::filesystem::file_size(p, ec);
    if (ec)
        return 2;
    FILE *f = fopen(argv[1], "rb");
    if (!f)
        return 3;
    std::vector<uint8_t> file(size, 0);
    fread(file.data(), 1, size, f);
    fclose(f);

    // dll
    uint8_t *ptr = file.data();
    int offset = -0x20;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)ptr;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(ptr + dos->e_lfanew);
    int addr = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

    PIMAGE_EXPORT_DIRECTORY pExportDesc = NULL;
	pExportDesc = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(file.data(), FALSE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);

    int start = (uint8_t *)pExportDesc - ptr;
    int diff = pExportDesc->Name - addr;
    int real_diff = pExportDesc->Name - (start + diff);

    ptr += start + offset;
    memcpy(
        ptr,
        p.filename().string().c_str(),
        p.filename().string().size());
    
    ptr = file.data() + pExportDesc->Name - real_diff;
    memcpy(ptr, "test", strlen("test") + 1);

    pExportDesc->Name = start + offset + real_diff;

    // pdb
    PIMAGE_DEBUG_DIRECTORY pDebugDesc = NULL;
    pDebugDesc = (PIMAGE_DEBUG_DIRECTORY)ImageDirectoryEntryToData(file.data(), FALSE, IMAGE_DIRECTORY_ENTRY_DEBUG, &ulSize);

    std::string pdb = p.filename().stem().string() + ".pdb";

    ptr = file.data() + pDebugDesc->PointerToRawData + 0x18;
    memcpy(ptr, pdb.c_str(), pdb.size() + 1);

    // write
    f = fopen(argv[1], "wb");
    if (!f)
        return 4;
    fwrite(file.data(), 1, size, f);
    fclose(f);

    return 0;
}

