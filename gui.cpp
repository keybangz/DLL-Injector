//
// Created by vm on 4/09/2023.
//

#include "gui.h"

// wxWidgets uses this function as new main.
bool GUI::OnInit() {
    // Here we'll call our injector intialization
    Backend bkend;
    bkend.Init();

    GUIFrame *frame = new GUIFrame("DLL Injector (v1.0)", wxPoint(50, 50), wxSize(450, 340));
    frame->Show( true );
    frame->Center();
    return true;
}

GUIFrame::GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size) {
    // If frame only has 1 child, nested child automatically resizes to size of frame.
    // To circumvent this, we add a panel to the frame, and we can draw the button on the panel.
    // wxWidgets handles deallocation of GUI children, do not directly call new instance of wxWidgets methods
    // Styles can be applied directly to new UI components, see wxWidgets documentation;
    wxPanel* panel = new wxPanel(this);
    wxButton* button = new wxButton(panel, wxID_ANY, "Button", wxPoint(0, 25), wxDefaultSize);
    wxCheckBox* checkbox = new wxCheckBox(panel, wxID_ANY, "Checkbox", wxPoint(0, 75));
    wxStaticText* textptr = new wxStaticText(panel, wxID_ANY, "Static Text", wxPoint(0, 125));
    wxTextCtrl* textCtrlptr = new wxTextCtrl(panel, wxID_ANY, "Default Text", wxPoint(0, 175),wxDefaultSize);
    // Sliders have to be positioned by panel???
    wxSlider* slider = new wxSlider(panel, wxID_ANY, 25, 0, 100);
    wxGauge* gauge = new wxGauge(panel, wxID_ANY, 100, wxPoint(0, 225), wxDefaultSize);
    gauge->SetValue(50);

    wxArrayString choices;
    choices.Add("Item 1");
    choices.Add("Item 2");
    choices.Add("Item 3");

    // dynamic control
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxPoint(50, 275), wxDefaultSize, choices);
    choice->Select(0);
    wxSpinCtrl* spinCtrl = new wxSpinCtrl(panel, wxID_ANY, "", wxPoint(0, 325), wxDefaultSize);
    // dynamic control
    wxListBox* listBox = new wxListBox(panel, wxID_ANY, wxPoint(0, 225), wxDefaultSize, choices);
    wxRadioBox* radioBox = new wxRadioBox(panel, wxID_ANY, "Radio Box", wxPoint(0, 300), wxDefaultSize, choices);
}

