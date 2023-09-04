//
// Created by vm on 4/09/2023.
//

#include "gui.h"

// wxWidgets uses this function as new main.
bool GUI::OnInit()
{
    // Here we'll call our injector intialization
    Backend bkend;
    bkend.Init();

    GUIFrame *frame = new GUIFrame("DLL Injector (v1.0)", wxPoint(50, 50), wxSize(450, 340));
    frame->Show( true );
    return true;
}

GUIFrame::GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText("Please select DLL.");
}
void GUIFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}
void GUIFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( "This is a wxWidgets' Hello world sample",
                  "About Hello World", wxOK | wxICON_INFORMATION );
}
void GUIFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

