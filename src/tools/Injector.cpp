#include <algorithm>

#include <Windows.h>

#include <Psapi.h>
#include <Dbghelp.h>

#include "Hotpatch.h"
#include "Logger.h"

DECLARE_STATIC_LOGGER(logger, "patcher");

DWORD read_pid()
{
    auto temp = boost::filesystem::temp_directory_path().string() + hotpatch_pid_filename;
    std::ifstream ifile(temp);
    if (!ifile)
        return 0;
    DWORD pid = 0;
    ifile >> pid;
    return pid;
}

HANDLE GetProcessHandle()
{
    DWORD pid = read_pid();
    if (pid == 0)
        return NULL;
	return OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
}

int main(int argc, char *argv[])
{
	PVOID pAllocEx = 0;
	HANDLE hThread = NULL;
	HANDLE hProcess = GetProcessHandle();
	HMODULE hKernel32 = ::GetModuleHandle("Kernel32");

    std::string hotpatch_image = read_new_module_filename();

	pAllocEx = VirtualAllocEx(hProcess, NULL, hotpatch_image.size(), MEM_COMMIT, PAGE_READWRITE);

	if (pAllocEx)
	{
		WriteProcessMemory(hProcess, pAllocEx, (void*)hotpatch_image.data(), hotpatch_image.size(), NULL);

		hThread = CreateRemoteThread(hProcess, NULL, 0,	
						(LPTHREAD_START_ROUTINE) ::GetProcAddress(hKernel32,"LoadLibraryA"), pAllocEx, 0, NULL);

		if (NULL != hThread)
		{
	        DWORD exit_code = 0;
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &exit_code);
			CloseHandle(hThread);		
		}

		VirtualFreeEx(hProcess, pAllocEx, hotpatch_image.size(), MEM_RELEASE);
	}

	return 0;
}

