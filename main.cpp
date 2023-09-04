#include "inject.h"

// DLL Injector
// Primary goal is to give user options to inject a DLL to selected process (LoadLibrary & Manual Mapping)
// Secondary goal is to add linux so injection support
// Alternative goals:
// - Add wrapper API to inject DLL files to Proton processes on Linux
// - Add cross-platform GUI to provide injection options to end-user
// - LATER: Provide any required anti-cheat preparations before injection

#define DLL_PATH "C:\\Users\\vm\\Documents\\GitHub\\DLLInjector-main\\cmake-build-debug\\inject.dll"
#define TARGET_BINARY "notepad.exe"

int main() {
    DWORD pid = NULL;
    HANDLE hProc = nullptr;
    LPVOID lpBaseAddress = nullptr;
    size_t szPath = strlen(DLL_PATH);

    std::cout << "Basic DLL Injector v1.0\n" << "\n";
    std::cout << "DLL Path: " << DLL_PATH << "\n";

    loadLibrary(pid, hProc, lpBaseAddress, szPath);

    return 0;
}

