//
// Created by vm on 4/09/2023.
//

#ifndef DLLINJECTOR_MAIN_GUI_H
#define DLLINJECTOR_MAIN_GUI_H

#include "setup.h"

enum ButtonId {
    addDllButton = wxID_LAST + 1,
    injectButton,
    removeDllButton,
    clearInjectButton,
    advancedButton,
    otherButton
};

class Backend {
public:
    static bool Init();
    static wxArrayString processList;
};

class GUI: public wxApp {
public:
    bool OnInit() override;
};

class GUIFrame: public wxFrame
{
public:
    GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void addDLLHandler(wxCommandEvent &e);
    void injectHandler(wxCommandEvent &e);
    void removeDllHandler(wxCommandEvent &e);
    void clearInjectHandler(wxCommandEvent &e);

    DECLARE_EVENT_TABLE()
};

#endif //DLLINJECTOR_MAIN_GUI_H
