//
// Created by vm on 5/09/2023.
//

#ifndef DLLINJECTOR_MAIN_SETUP_H
#define DLLINJECTOR_MAIN_SETUP_H

#include <iostream>
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>
#include <wx/combobox.h>
#include <wx/wfstream.h>

#include "inject.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <tchar.h>
#include <TlHelp32.h>
#include <fileapi.h>
#include <fstream>
#elif __linux
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#endif
#include <cwchar>

#endif //DLLINJECTOR_MAIN_SETUP_H
