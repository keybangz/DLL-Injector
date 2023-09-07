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
    // FIXME: ADD OS CHECKS HERE TO SETUP APPROPRIATE FILE HEADER
    Backend::processList.Clear();

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

    // loop through process list, add to gui box.
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32; // PE Header

    if(hSnap == INVALID_HANDLE_VALUE)
        return FALSE;

    // Grab snapshot of all apps running
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First(hSnap, &pe32)) {
        CloseHandle(hSnap);
        return FALSE;
    }

    // idk why I didn't do this originally, should fix process finding errors.
    const wchar_t* wTargetBin = L"" TARGET_BINARY;

    // Loop through list of processes, add name to GUI and keep reference of ID.
    do {
        Backend::processList.Add(pe32.szExeFile);
    } while (Process32Next(hSnap, &pe32));
#elif __linux__
    // look into so injection after manual mapping implmentation
#endif


    return true;
}
