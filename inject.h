//
// Created by vm on 4/09/2023.
//

#ifndef DLLINJECTOR_MAIN_INJECT_H
#define DLLINJECTOR_MAIN_INJECT_H

#include "setup.h"

class Inject {
public:
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    static int loadLibrary(DWORD& pid, HANDLE& hProc, LPVOID& lpBaseAddress, wxString& path, size_t& szPath);
    static DWORD findProcessID(const wchar_t* target);
#elif __linux__
#endif
};

#endif //DLLINJECTOR_MAIN_INJECT_H
