//
// Created by vm on 4/09/2023.
//

#include "gui.h"

BEGIN_EVENT_TABLE(GUIFrame, wxFrame)
    EVT_BUTTON(addDllButton, GUIFrame::addDLLHandler)
    EVT_BUTTON(injectButton, GUIFrame::injectHandler)
    EVT_BUTTON(removeDllButton, GUIFrame::removeDllHandler)
    EVT_BUTTON(clearInjectButton, GUIFrame::clearInjectHandler)
END_EVENT_TABLE()

wxArrayString Backend::processList;

wxListCtrl* injectListPtr = nullptr;
wxComboBox* processTargetComboPtr = nullptr;

// wxWidgets uses this function as new main.
bool GUI::OnInit() {
    Backend::Init();

    GUIFrame *frame = new GUIFrame("DLL Injector (v1.0) by keybangz", wxPoint(50, 50), wxSize(450, 200));
    frame->Show( true );
    frame->Center();
    return true;
}

GUIFrame::GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER) {
    wxArrayString selProcessMethod;
    selProcessMethod.Add("Existing Process");
    selProcessMethod.Add("New Process");

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
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 200)); // top panel
    panel->SetBackgroundColour((wxColour(100, 100, 200))); // blue
#elif __linux__
    wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 400)); // top panel
#endif

    wxPanel* panel_mid = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 100)); // middle
    panel->SetBackgroundColour((wxColour(100, 200, 200))); // yellow?
    wxPanel* panel_bot = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 50)); // bottom panel
    panel_bot->SetBackgroundColour((wxColour(100, 200, 100))); // green

    // CREATE SIZERS USED FOR LAYOUT
    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer* sizer_middle = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sizer_bot = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* sizer_inject = new wxBoxSizer(wxVERTICAL);

    // COMPONENTS
    // top panel
    injectListPtr = new wxListCtrl(panel, wxID_ANY, wxPoint(0, 20), wxSize(420, 200), wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES);

    // mid panel`

    wxRadioBox* processTypePtr = new wxRadioBox(panel_mid, wxID_ANY, "Process Type", wxPoint(0, 0), wxSize(420, 50), selProcessMethod);
    wxStaticText* processTitlePtr = new wxStaticText(panel_mid, wxID_ANY, "Process:", wxPoint(5,65), wxSize(45, 20));
    processTargetComboPtr = new wxComboBox(panel_mid, wxID_ANY, "Select process", wxPoint(50, 60), wxSize(80, 45), Backend::processList, wxCB_READONLY);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    wxButton* selectDllPtr = new wxButton(panel_mid, addDllButton, "Add File", wxPoint(140, 60), wxDefaultSize);
    wxButton* removeDllPtr = new wxButton(panel_mid, removeDllButton, "Remove File", wxPoint(230, 60), wxDefaultSize);
    wxButton* clearDllPtr = new wxButton(panel_mid, wxID_ANY, "Clear List", wxPoint(330, 60), wxDefaultSize);
#elif __linux__
    wxButton* selectDllPtr = new wxButton(panel_mid, wxID_ANY, "Add File", wxPoint(150, 60), wxDefaultSize);
    wxButton* removeDllPtr = new wxButton(panel_mid, wxID_ANY, "Remove File", wxPoint(240, 60), wxDefaultSize);
    wxButton* clearDllPtr = new wxButton(panel_mid, wxID_ANY, "Clear List", wxPoint(335, 60), wxDefaultSize);
#endif
    // bottom left panel
    wxButton* injectButtonPtr = new wxButton(panel_bot, injectButton, "Inject", wxPoint(100, 15), wxDefaultSize);
    wxButton* advancedButtonPtr = new wxButton(panel_bot, injectButton, "Advanced", wxPoint(200, 15), wxDefaultSize);

    // SETUP DLL INJECTION LIST
    injectListPtr->InsertColumn(0, "DLL Name", NULL, 210);
    injectListPtr->InsertColumn(1, "Architecture", NULL, 215);

    // LAYOUT PROJECT WITH SIZERS & PADDING
    sizer->Add(panel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    sizer_middle->Add(panel_mid, 1, wxBOTTOM, 5);
    sizer_bot->Add(panel_bot, 1,  wxRIGHT | wxBOTTOM, 5);
    sizer->Add(sizer_middle, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
    sizer->Add(sizer_bot, 0, wxEXPAND, 5);

    // ADD OTHER COMPONENTS TO SIZER & ADJUST
    // sizer_inject->Add(titlePtr, 0, wxEXPAND | wxLEFT, 0);
    // FIXME: Can't add components to sizer if different type. (wxWidgets checks for byte size and compares?)
    sizer_inject->Add(injectListPtr, 0, wxEXPAND | wxLEFT, 0);

    // FIT TO FRAME
    this->SetSizerAndFit(sizer);
    // Unlike other sizers, the inject sizer is locked to the top panel aka the parent panel
    // Meaning we must call the panel and set sizer to accommodate for the panel instead of the window.
    // SEE: injectList declaration - parent panel is first panel created.
    panel->SetSizerAndFit(sizer_inject);
}

// EVENTS

void GUIFrame::addDLLHandler(wxCommandEvent &e) {
    //wxMessageBox("Test Add DLL BUTTON");
    wxFileDialog* dllSelected = new wxFileDialog(this, "Open DLL File", "", "", ".dll files (*.dll)|*.dll", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dllSelected->ShowModal() == wxID_CANCEL)
        return;

    wxFileInputStream input_stream(dllSelected->GetPath());
    if (!input_stream.IsOk()) {
        wxLogError("Cannot open file '%s'.", dllSelected->GetPath());
        return;
    }

    if (dllSelected->ShowModal() == wxID_OK) {
        long index = injectListPtr->InsertItem(0, dllSelected->GetFilename());
        injectListPtr->SetItem(index, 1, dllSelected->GetPath());
        return;
    }
}

void GUIFrame::removeDllHandler(wxCommandEvent &e) {
    long index = injectListPtr->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if(index != wxNOT_FOUND) {
        injectListPtr->DeleteItem(index);
    }
}

void GUIFrame::injectHandler(wxCommandEvent &e) {
    long index = injectListPtr->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    // grab first item if none selected.
    if(index == -1) {
        for(int i = 0; i < injectListPtr->GetItemCount(); i++) {
            if(i >> 0 && !injectListPtr->GetItemState(i, wxLIST_STATE_SELECTED)) {
                index = i;
                break;
            }
        }
    }

    if (index != wxNOT_FOUND) {
        wxString path = injectListPtr->GetItemText(index, 1);
        wxString selProc = processTargetComboPtr->GetStringSelection();

        std::wstring ProcConvert = std::wstring(selProc.begin(), selProc.end());
        const wchar_t* curProc = ProcConvert.c_str();

        DWORD pid = Inject::findProcessID(curProc);
        HANDLE hProc = nullptr;
        LPVOID lpBaseAddress = nullptr;
        size_t szPath = strlen(path);
        Inject::loadLibrary(pid, hProc, lpBaseAddress, path, szPath);
    }
}

void GUIFrame::clearInjectHandler(wxCommandEvent &e) {
    injectListPtr->DeleteAllItems();
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