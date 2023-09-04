//
// Created by vm on 4/09/2023.
//

#ifndef DLLINJECTOR_MAIN_INJECT_H
#define DLLINJECTOR_MAIN_INJECT_H

#include "setup.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
int loadLibrary(DWORD& pid, HANDLE& hProc, LPVOID& lpBaseAddress, size_t& szPath);
DWORD findProcessID();
BOOL FileExists(LPCTSTR szPath);
#elif __linux__
#endif

#endif //DLLINJECTOR_MAIN_INJECT_H
