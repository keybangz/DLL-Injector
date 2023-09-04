//
// Created by vm on 4/09/2023.
//

#ifndef DLLINJECTOR_MAIN_GUI_H
#define DLLINJECTOR_MAIN_GUI_H

#include "setup.h"

class Backend {
public:
    static bool Init();
};

class GUI: public wxApp {
public:
    bool OnInit() override;
};

class GUIFrame: public wxFrame
{
public:
    GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

enum
{
    ID_Hello = 1
};

#endif //DLLINJECTOR_MAIN_GUI_H
