//
// Created by vm on 4/09/2023.
//

#ifndef DLLINJECTOR_MAIN_INJECT_H
#define DLLINJECTOR_MAIN_INJECT_H

#include <iostream>
#include <tchar.h>
#include <cwchar>
#include <Windows.h>
#include <TlHelp32.h>
#include <fileapi.h>

int loadLibrary(DWORD pid, HANDLE hProc, LPVOID lpBaseAddress, size_t szPath);
DWORD findProcessID();
BOOL FileExists(LPCTSTR szPath);

#endif //DLLINJECTOR_MAIN_INJECT_H
