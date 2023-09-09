//
// Created by vm on 4/09/2023.
//

#ifndef DLLINJECTOR_MAIN_INJECT_H
#define DLLINJECTOR_MAIN_INJECT_H

#include "setup.h"

// copy dll image to target process address space > copy loader code > do relocations > resolve imports & TLS, find entry point and call.
// FIXME: add windows check
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
using f_LoadLibraryA = HINSTANCE(WINAPI*)(const char* lpLibName);
using f_GetProcAddr = UINT_PTR(WINAPI*)(HINSTANCE hModule, const char* lpProcName);
using f_DllEntryP = BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved);
#elif __linux__
#endif

class Inject {
public:
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    struct ManualMappingData {
        f_LoadLibraryA pLoadLibrary;
        f_GetProcAddr pGetProcAddr;
        HINSTANCE hMod;
    };

    static int loadLibrary(DWORD& pid, HANDLE& hProc, LPVOID& lpBaseAddress, wxString& path, size_t& szPath);
    static bool ManualMap(DWORD& pid, const char* szDllFile);
    static DWORD findProcessID(const wchar_t* target);
#elif __linux__
#endif
};

#endif //DLLINJECTOR_MAIN_INJECT_H
