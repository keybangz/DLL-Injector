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
    Backend::processListFull.Clear();

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

    // Loop through list of processes, add name to GUI and keep reference of ID.
    do {
        Backend::processList.Add(pe32.szExeFile);
    } while (Process32Next(hSnap, &pe32));
#elif __linux__
    int pid = -1;

    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != nullptr)
    {
        // Enumerate all entries in directory until process found
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp)))
        {
            // Skip non-numeric entries
            int id = atoi(dirp->d_name);
            if (id > 0)
            {
                // Read contents of virtual /proc/{pid}/cmdline file
                std::string cmdPath = std::string("/proc/") + dirp->d_name + "/cmdline";
                std::ifstream cmdFile(cmdPath.c_str());
                std::string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != std::string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != std::string::npos)
                        cmdLine = cmdLine.substr(pos + 1);

                    // cut off long processes from cmdlist
                    if(cmdLine.length() > 40) {
                        cmdLine = cmdLine.substr(0, 40);
                    }

                    Backend::processList.Add(cmdLine.c_str());
                    Backend::processListFull.Add(cmdLine);
                }
            }
        }
    }

    closedir(dp);
#endif


    return true;
}
