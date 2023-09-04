#include "inject.h"
#include "gui.h"

// DLL Injector
// Primary goal is to give user options to inject a DLL to selected process (LoadLibrary & Manual Mapping)
// Secondary goal is to add linux so injection support
// Alternative goals:
// - Add wrapper API to inject DLL files to Proton processes on Linux
// - Add cross-platform GUI to provide injection options to end-user
// - LATER: Provide any required anti-cheat preparations before injection

#define DLL_PATH "C:\\Users\\vm\\Documents\\GitHub\\DLLInjector-main\\cmake-build-debug\\inject.dll"
#define TARGET_BINARY "notepad.exe"

wxIMPLEMENT_APP(GUI);

bool Backend::Init() {
    // FIXME: ADD OS CHECKS HERE TO SETUP APPROPRIATE FILE HEADERS

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    DWORD pid = 0;
    HANDLE hProc = nullptr;
    LPVOID lpBaseAddress = nullptr;
    size_t szPath = strlen(DLL_PATH);
    loadLibrary(pid, hProc, lpBaseAddress, szPath);
#endif

//    std::cout << "Basic DLL Injector v1.0\n" << "\n";
//    std::cout << "DLL Path: " << DLL_PATH << "\n";

    return true;
}
