//
// Created by vm on 4/09/2023.
//

#include "inject.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

// int inject(dword, handle, lpvoid, size_t)
// uses standard loadlibrary to inject dll into desired process.
int Inject::loadLibrary(DWORD& pid, HANDLE& hProc, LPVOID& lpBaseAddress, wxString& path, size_t& szPath) {

    if(pid == 0) {
        printf("error: somehow pid check returned 0");
        return -1;
    }

    // Get handle to target process
    hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if(!hProc) {
        printf("PROCESS NOT FOUND!");
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
    const DWORD dwWriteResult = WriteProcessMemory(hProc, lpBaseAddress, path, szPath, nullptr);
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

    wxMessageBox("Injection successful!");

    return 0;
}

DWORD Inject::findProcessID(const wchar_t* target) {
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

    // Loop through list of processes running and select process user selected.
    do {
        if (!wcscmp(pe32.szExeFile, target)) {
            CloseHandle(hSnap);
            return pe32.th32ProcessID;
        }
    } while (Process32Next(hSnap, &pe32));
}

// thanks @coconutbird <3
bool rpc_call(HANDLE handle, void* address, void* arg1, void* arg2, void* arg3, void* arg4, uint64_t* result) {
    /*
0:  48 83 ec 28             sub    rsp,0x28
4:  48 b9 00 00 00 00 00    movabs rcx,0x0
b:  00 00 00
e:  48 ba 00 00 00 00 00    movabs rdx,0x0
15: 00 00 00
18: 49 b8 00 00 00 00 00    movabs r8,0x0
1f: 00 00 00
22: 49 b9 00 00 00 00 00    movabs r9,0x0
29: 00 00 00
2c: 48 b8 00 00 00 00 00    movabs rax,0x0
33: 00 00 00
36: ff d0                   call   rax
38: 48 83 c4 28             add    rsp,0x28
3c: 49 ba 00 00 00 00 00    movabs r10,0x0
43: 00 00 00
46: 49 89 02                mov    QWORD PTR [r10],rax
49: c3                      ret
     */

    uint8_t shellcode[] = {
            0x48, 0x83, 0xEC, 0x28,
            0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x49, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xFF, 0xD0,
            0x48, 0x83, 0xC4, 0x28,
            0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x49, 0x89, 0x02,
            0xC3 };

    *(uint64_t*) (shellcode + 6) = (uint64_t) arg1;
    *(uint64_t*) (shellcode + 16) = (uint64_t) arg2;
    *(uint64_t*) (shellcode + 26) = (uint64_t) arg3;
    *(uint64_t*) (shellcode + 36) = (uint64_t) arg4;
    *(uint64_t*) (shellcode + 46) = (uint64_t) address;

    auto remote_shellcode = VirtualAllocEx(handle, nullptr, sizeof(shellcode) + 8, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // store return result at the end of the code block
    *(uint64_t*) (shellcode + 62) = (uint64_t) remote_shellcode + sizeof(shellcode);

    WriteProcessMemory(handle, remote_shellcode, shellcode, sizeof(shellcode), nullptr);

    auto thread = CreateRemoteThread(handle, nullptr, 0, (LPTHREAD_START_ROUTINE) remote_shellcode, nullptr, 0, nullptr);
    if (thread == nullptr) {
        return false;
    }

    WaitForSingleObject(thread, INFINITE);

    if (result != nullptr) {
        ReadProcessMemory(handle, (void*) *(uint64_t*) (shellcode + 62), result, sizeof(uint64_t), nullptr);
    }

    CloseHandle(thread);

    VirtualFreeEx(handle, remote_shellcode, 0, MEM_RELEASE);

    return true;
}

uint32_t rva_to_offset(PIMAGE_NT_HEADERS nt_headers, uint32_t rva) {
    auto section_header = IMAGE_FIRST_SECTION(nt_headers);

    for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i) {
        const auto section = section_header[i];

        if (rva >= section.VirtualAddress && rva <= section.VirtualAddress + section.SizeOfRawData) {
            return section.PointerToRawData + rva - section.VirtualAddress;
        }
    }

    return 0;
}

bool invoke_entry(HANDLE handle, void* address, void* image_base, DWORD reason) {
    uint64_t result = FALSE;
    if (!rpc_call(handle, address, (void*) image_base, (void*) reason, nullptr, nullptr, &result))
        return false;

    return result != FALSE;
}

void process_relocations(uint8_t* buffer, uint8_t* image, uint64_t image_base, PIMAGE_NT_HEADERS nt_headers) {
    const size_t delta = image_base - nt_headers->OptionalHeader.ImageBase;

    const auto directory = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

    if (directory.VirtualAddress == 0 || directory.Size == 0)
        return;

    auto relocation_address = buffer + rva_to_offset(nt_headers, directory.VirtualAddress);
    auto relocation_end_address = relocation_address + directory.Size;

    auto base_relocation = (PIMAGE_BASE_RELOCATION) relocation_address;
    uint32_t block_size = base_relocation->SizeOfBlock;
    while (base_relocation->VirtualAddress != 0 && relocation_address < relocation_end_address && block_size != 0) {
        block_size = base_relocation->SizeOfBlock;

        const size_t count = (block_size - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);
        auto items = (uint16_t*) (relocation_address + sizeof(IMAGE_BASE_RELOCATION));

        for (size_t i = 0; i < count; ++i) {
            const uint16_t type = items[i] >> 12;
            const uint16_t offset = items[i] & 0xFFF;

            if (type == IMAGE_REL_BASED_DIR64) {
                *reinterpret_cast<uint64_t*>(image + base_relocation->VirtualAddress + offset) += delta;
            }
        }

        relocation_address = relocation_address + base_relocation->SizeOfBlock;
        base_relocation = (PIMAGE_BASE_RELOCATION) relocation_address;
    }
}

bool process_imports(HANDLE handle, uint8_t* buffer, uint8_t* image, uint64_t image_base, PIMAGE_NT_HEADERS nt_headers) {
    const auto directory = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    if (directory.VirtualAddress == 0 || directory.Size == 0)
        return false;

    auto import_descriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(buffer + rva_to_offset(nt_headers, directory.VirtualAddress));
    while (import_descriptor->Name != 0) {
        const char* module_name = (char*) buffer + rva_to_offset(nt_headers, import_descriptor->Name);
        if (module_name == nullptr)
            return false;

        const size_t module_name_length = strlen(module_name) + 1;
        void* remote_module_name = VirtualAllocEx(handle, nullptr, module_name_length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (remote_module_name == nullptr)
            return false;

        if (!WriteProcessMemory(handle, remote_module_name, module_name, module_name_length, nullptr))
            return false;

        // TODO: change to remote LoadLibrary i.e shellcode call
        HMODULE hmodule;
        rpc_call(handle, (void*) LoadLibraryA, (void*) remote_module_name, nullptr, nullptr, nullptr, (uint64_t*) &hmodule);
        if (hmodule == nullptr)
            return false;

        VirtualFreeEx(handle, remote_module_name, 0, MEM_RELEASE);

        PIMAGE_THUNK_DATA64 thunk_data;
        PIMAGE_THUNK_DATA64 func_data;

        if (import_descriptor->OriginalFirstThunk) {
            thunk_data = (PIMAGE_THUNK_DATA64) (buffer + rva_to_offset(nt_headers, import_descriptor->OriginalFirstThunk));
            func_data = (PIMAGE_THUNK_DATA64) (image + import_descriptor->FirstThunk);
        }
        else {
            thunk_data = (PIMAGE_THUNK_DATA64) (buffer + rva_to_offset(nt_headers, import_descriptor->FirstThunk));
            func_data = (PIMAGE_THUNK_DATA64) (image + import_descriptor->FirstThunk);
        }

        while (thunk_data->u1.AddressOfData != 0) {
            uint64_t address;

            if (IMAGE_SNAP_BY_ORDINAL64(thunk_data->u1.Ordinal)) {
                // TODO: support ordinal imports

                return false;
            }
            else {
                auto import_by_name = (PIMAGE_IMPORT_BY_NAME) (buffer + rva_to_offset(nt_headers, *reinterpret_cast<uint64_t*>(thunk_data)));

                // TODO: change to remote GetProcAddress i.e shellcode call
                // address = (uint64_t) GetProcAddress(hmodule, (char*) import_by_name->Name);

                const size_t length = strlen((char*) import_by_name->Name) + 1;

                void* remote_name = VirtualAllocEx(handle, nullptr, length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                if (remote_name == nullptr)
                    return false;

                if (!WriteProcessMemory(handle, remote_name, &import_by_name->Name, length, nullptr))
                    return false;

                rpc_call(handle, (void*) GetProcAddress, (void*) hmodule, (void*) remote_name, nullptr, nullptr, &address);
                if (!VirtualFreeEx(handle, remote_name, 0, MEM_RELEASE))
                    return false;
            }

            if (address == 0)
                return false;

            func_data->u1.Function = address;

            ++thunk_data;
            ++func_data;
        }

        ++import_descriptor;
    }

    return true;
}

/*
    Load your raw DLL data
    Map sections into target process
    Inject loader shellcode (we can do most of this outside of shellcode)
    Apply relocation
    Fix imports
    Execute TLS callbacks
    Call DllMain
    Clean up
 */
bool Inject::ManualMap(DWORD& pid, const char* szDllFile) {
    if(pid == 0)
        return false;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if(!hProc) {
        CloseHandle(hProc);
        return false;
    }

    BYTE* pSrcData = nullptr;
    IMAGE_NT_HEADERS *pOldNtHeader = nullptr;
    IMAGE_OPTIONAL_HEADER *pOldOptHeader = nullptr;
    IMAGE_FILE_HEADER *pOldFileHeader = nullptr;
    BYTE* pTargetBase = nullptr; // either this is failing because of the dll specified, or i am big dumb

    DWORD dwCheck = 0;
    if(!GetFileAttributesA(szDllFile)) {
        printf("File not found\n");
        return false;
    }

    // Load raw data from DLL
    std::ifstream File(szDllFile, std::ios::binary | std::ios::ate);

    if(File.fail()) {
        printf("Opening file failed: %lX\n", (DWORD)File.rdstate());
        File.close();
        return false;
    }

    auto FileSize = File.tellg();
    if(FileSize < 0x1000) {
        printf("Filesize is invalid.\n");
        File.close();
        return false;
    }

    pSrcData = new BYTE[static_cast<UINT_PTR>(FileSize)];
    if(!pSrcData) {
        printf("Memory allocation failed!\n");
        File.close();
        return false;
    }

    File.seekg(0, std::ios::beg); // read from beginning of file
    File.read((char*)pSrcData, FileSize);
    File.close();

    if(reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_magic != 0x5A4D) {
        printf("Invalid file \n");
        delete[] pSrcData;
        return false;
    }

    // base address + new nt header = oldntheader
    auto nt_headers = PIMAGE_NT_HEADERS(pSrcData + PIMAGE_DOS_HEADER(pSrcData)->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }

    // allocate memory
    void* remote_image = VirtualAllocEx(hProc, (void*) nt_headers->OptionalHeader.ImageBase, nt_headers->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (remote_image == nullptr) {
        remote_image = VirtualAllocEx(hProc, nullptr, nt_headers->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    }

    if (remote_image == nullptr) {
        return false;
    }

    auto image = (uint8_t*) VirtualAlloc(nullptr, nt_headers->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (image == nullptr) {
        VirtualFreeEx(hProc, remote_image, 0, MEM_RELEASE);

        return false;
    }

    // copy sections
    auto section_header = IMAGE_FIRST_SECTION(nt_headers);
    for (auto i = 0; i < nt_headers->FileHeader.NumberOfSections; i++) {
        memcpy(image + section_header[i].VirtualAddress, pSrcData + section_header[i].PointerToRawData, section_header[i].SizeOfRawData);
    }

    // relocate image
    process_relocations((uint8_t*) pSrcData, image, (uint64_t) remote_image, nt_headers);

    // set imports
    if (!process_imports(hProc, (uint8_t*) pSrcData, image, (uint64_t) remote_image, nt_headers)) {
        VirtualFreeEx(hProc, remote_image, 0, MEM_RELEASE);
        VirtualFree(image, 0, MEM_RELEASE);

        return false;
    }

    // commit image
    WriteProcessMemory(hProc, remote_image, image, nt_headers->OptionalHeader.SizeOfImage, nullptr);

    // cleanup local
    (void) VirtualFree(image, 0, MEM_RELEASE);

    invoke_entry(hProc, (void*) ((uint64_t)remote_image + nt_headers->OptionalHeader.AddressOfEntryPoint), remote_image, DLL_PROCESS_ATTACH);

    wxMessageBox("Injection successful!");

    return true;
}

#endif