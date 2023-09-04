//
// Created by vm on 4/09/2023.
//

#include "inject.h"

// FIXME: Change to user-selected process & DLL's
#define DLL_PATH "C:\\Users\\vm\\Documents\\GitHub\\DLLInjector-main\\cmake-build-debug\\inject.dll"
#define TARGET_BINARY "notepad.exe"

// int inject(dword, handle, lpvoid, size_t)
// uses standard loadlibrary to inject dll into desired process.
int loadLibrary(DWORD pid, HANDLE hProc, LPVOID lpBaseAddress, size_t szPath) {
    pid = findProcessID();

    // FIXME: We want to be stealing another applications base address later
    // Get handle to target process
    hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if(!hProc) {
        printf("PROCESS NOT FOUND!");
        return -1;
    }

    if(!FileExists(DLL_PATH)) {
        printf("FILE DOES NOT EXIST!");
        return -1;
    }

    lpBaseAddress  = VirtualAllocEx(hProc, nullptr, szPath, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (lpBaseAddress == nullptr)
    {
        printf("An error is occured when trying to allocate memory in the target process.\n");
        return -1;
    }

    printf("Memory allocate at 0x%X\n", (UINT)(uintptr_t)lpBaseAddress);

    // Write DLL path to the target binary's memory
    const DWORD dwWriteResult = WriteProcessMemory(hProc, lpBaseAddress, DLL_PATH, szPath, nullptr);
    if(dwWriteResult == 0) {
        printf("An error occured trying to write DLL path to target process.");
        return -1;
    }

    printf("LoadLibraryA address at 0x%X\n", (UINT)(uintptr_t)LoadLibraryA);

    // create remote thread in target binary
    HANDLE hThread = CreateRemoteThread(hProc, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, lpBaseAddress, 0, nullptr);

    if(!hThread) {
        printf("Thread creation failed 0x%X\n", GetLastError());
        return -1;
    }

    CloseHandle(hThread);
    CloseHandle(hProc);
    VirtualFreeEx(hProc, lpBaseAddress, 0, MEM_RELEASE);

    printf("Inject successful!");

    return 0;
}

// FIXME: Update function to use a GUI selection.
// UPDATE: findProcessByString?
DWORD findProcessID() {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    PROCESSENTRY32 pe32; // PE Header

    if(hSnap == INVALID_HANDLE_VALUE)
        return FALSE;

    // Grab snapshot of all apps running
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First(hSnap, &pe32)) {
        CloseHandle(hSnap);
        return FALSE;
    }

    const char* exeName = TARGET_BINARY;

    // Loop through list of processes running and select process user selected.
    do {
        if (!strcmp((CHAR*)pe32.szExeFile, exeName))
            return pe32.th32ProcessID;
    } while (Process32Next(hSnap, &pe32));

    CloseHandle(hSnap);
    return 0;
}

BOOL FileExists(LPCTSTR szPath) {
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}