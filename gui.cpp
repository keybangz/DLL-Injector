//
// Created by vm on 4/09/2023.
//

#include "gui.h"

// wxWidgets uses this function as new main.
bool GUI::OnInit() {
    Backend::Init();

    GUIFrame *frame = new GUIFrame("DLL Injector (v1.0)", wxPoint(50, 50), wxSize(450, 200));
    frame->Show( true );
    frame->Center();
    return true;
}

GUIFrame::GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER) {
    wxArrayString selProcessMethod;
    selProcessMethod.Add("Existing Process");
    selProcessMethod.Add("New Process");

    wxArrayString testChoices;
    testChoices.Add("Item 1");
    testChoices.Add("Item 2");
    testChoices.Add("Item 3");

    /*
     * COMPONENT LAYOUT
     * The current application is divided into three panels (1x 1/2 vertical && 2x 1/2 horizontal)
     * We will work from top to bottom, adding required components as they are required functionally.
     *
     * HIERARCHY:
     * TOP PANEL
     * BOTTOM PANEL LEFT
     * BOTTOM PANEL RIGHT
     */

    // PANELS
    wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 400)); // top panel
    panel->SetBackgroundColour((wxColour(100, 100, 200))); // blue
    wxPanel* panel_mid = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 100)); // middle
    panel->SetBackgroundColour((wxColour(100, 200, 200))); // yellow?
    wxPanel* panel_bot = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 100)); // bottom panel
    panel_bot->SetBackgroundColour((wxColour(100, 200, 100))); // green

    // CREATE SIZERS USED FOR LAYOUT
    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* sizer_middle = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sizer_bot = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sizer_inject = new wxBoxSizer(wxVERTICAL);
    wxSizer* sizer_btn_bot = new wxBoxSizer(wxHORIZONTAL);

    // COMPONENTS
    // top panel
    wxStaticText* titlePtr = new wxStaticText(panel, wxID_ANY, "DLL INJECTOR (v1.0) by keybangz", wxPoint(0, 0), wxSize(400, 20), wxALIGN_CENTRE_HORIZONTAL);
    wxListCtrl* injectListPtr = new wxListCtrl(panel, wxID_ANY, wxPoint(0, 20), wxSize(420, 200), wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);

    // mid panel
    wxRadioBox* processTypePtr = new wxRadioBox(panel_mid, wxID_ANY, "Process Type", wxPoint(0, 0), wxSize(420, 50), selProcessMethod);
    wxStaticText* processTitlePtr = new wxStaticText(panel_mid, wxID_ANY, "Process:", wxPoint(0,65), wxSize(420, 20));
    wxComboBox* processTargetComboPtr = new wxComboBox(panel_mid, wxID_ANY, "Select process", wxPoint(55, 60), wxDefaultSize, testChoices, wxCB_READONLY);
    wxButton* selectDllPtr = new wxButton(panel_mid, wxID_ANY, "Add File", wxPoint(150, 60), wxDefaultSize);
    wxButton* removeDllPtr = new wxButton(panel_mid, wxID_ANY, "Remove File", wxPoint(240, 60), wxDefaultSize);
    wxButton* clearDllPtr = new wxButton(panel_mid, wxID_ANY, "Clear List", wxPoint(335, 60), wxDefaultSize);

    // bottom left panel
    wxButton* injectButtonPtr = new wxButton(panel_bot, wxID_ANY, "Inject", wxPoint(0, 0), wxDefaultSize);

    // SETUP DLL INJECTION LIST
    injectListPtr->InsertColumn(0, "DLL Name", NULL, 210);
    injectListPtr->InsertColumn(1, "Architecture", NULL, 215);
    long index =  injectListPtr->InsertItem(0, "Placeholder DLL");
    injectListPtr->SetItem(index, 1, "x64");

    // LAYOUT PROJECT WITH SIZERS & PADDING
    sizer->Add(panel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    sizer_middle->Add(panel_mid, 1, wxBOTTOM, 5);
    sizer_bot->Add(panel_bot, 1,  wxRIGHT | wxBOTTOM, 5);
    sizer->Add(sizer_middle, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    sizer->Add(sizer_bot, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);

    // ADD OTHER COMPONENTS TO SIZER & ADJUST
    sizer_inject->Add(titlePtr, 0, wxEXPAND | wxLEFT, 0);
    // FIXME: Can't add components to sizer if different type. (wxWidgets checks for byte size and compares?)
    sizer_inject->Add(injectListPtr, 0, wxEXPAND | wxLEFT, 0);

    // FIT TO FRAME
    this->SetSizerAndFit(sizer);
    // Unlike other sizers, the inject sizer is locked to the top panel aka the parent panel
    // Meaning we must call the panel and set sizer to accommodate for the panel instead of the window.
    // SEE: injectList declaration - parent panel is first panel created.
    panel->SetSizerAndFit(sizer_inject);
}

/*  GUIFrame(title, pos, size,) : wxFrame(parent, ID, title, pos, size)

    Entry point for newly created frame.
    If frame only has 1 child, nested child automatically resizes to size of frame.
    To circumvent this, we add a panel to the frame, and we can draw the button on the panel.
    wxWidgets handles deallocation of GUI children, do not directly call new instance of wxWidgets methods
    Styles can be applied directly to new UI components, see wxWidgets documentation;

    wxPanel* panel = new wxPanel(this);
    wxButton* button = new wxButton(panel, wxID_ANY, "Button", wxPoint(0, 25), wxDefaultSize);
    wxCheckBox* checkbox = new wxCheckBox(panel, wxID_ANY, "Checkbox", wxPoint(0, 75));
    wxStaticText* textptr = new wxStaticText(panel, wxID_ANY, "Static Text", wxPoint(0, 125));
    wxTextCtrl* textCtrlptr = new wxTextCtrl(panel, wxID_ANY, "Default Text", wxPoint(0, 175),wxDefaultSize);

    Sliders have to be positioned by panel?
    wxSlider* slider = new wxSlider(panel, wxID_ANY, 25, 0, 100);
    wxGauge* gauge = new wxGauge(panel, wxID_ANY, 100, wxPoint(0, 225), wxDefaultSize);
    gauge->SetValue(50);

    // from <wx/spinctrl.h>
    wxSpinCtrl* spinCtrl = new wxSpinCtrl(panel, wxID_ANY, "", wxPoint(0, 325), wxDefaultSize);

    wxArrayString choices;
    choices.Add("Item 1");
    choices.Add("Item 2");
    choices.Add("Item 3");

    DYNAMIC CONTROLS BELOW:
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxPoint(50, 275), wxDefaultSize, choices);
    choice->Select(0);
    wxListBox* listBox = new wxListBox(panel, wxID_ANY, wxPoint(0, 225), wxDefaultSize, choices);
    wxRadioBox* radioBox = new wxRadioBox(panel, wxID_ANY, "Radio Box", wxPoint(0, 300), wxDefaultSize, choices);

    // use dpi to keep space even on any screen
    const int width = FromDIP(30);
    const int height = FromDIP(30);

    // testing
    const int COLS = 20;
    const int ROWS = 13;

    // calling wxWidget methods directly causes crashes, REMEMBER THIS!
    auto sizer = new wxGridSizer(ROWS, COLS, 0, 0);

    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            auto square = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(width, height));
            bool isDark = (i + j) % 2 == 0;
            square->SetBackgroundColour(isDark ? wxColour(80, 80, 80) : *wxWHITE);

            sizer->Add(square, 1, wxEXPAND);
        }
    }

    this->SetSizerAndFit(sizer);
*/