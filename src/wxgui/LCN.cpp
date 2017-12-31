#include "prec.h"
#include "main.h"


void MainFrame::InitLCNPanel()
{
    //create LCN  tab, Panel 4.
    m_panel4 = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);

    wxStaticText *m_staticText2 = new wxStaticText(m_panel4, wxID_ANY, wxT("LCN Information Tab"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText2->Wrap(250);
    bSizer4->Add(m_staticText2, 0, wxLEFT, 5);

    m_toggleBtn1 = new wxToggleButton(m_panel4, ID_LCNButton1, wxT("Start Engine!"), wxDefaultPosition, wxSize(100, wxDefaultCoord), 0);
    bSizer4->Add(m_toggleBtn1, 0, wxALL, 5);

    wxBoxSizer* bSizer5 = new wxBoxSizer(wxHORIZONTAL);
    m_WxTextCtrl_LCNno = new wxTextCtrl(m_panel4, ID_LCNnumTextInput, _("Enter the LCN Number: "), wxDefaultPosition, wxSize(150, wxDefaultCoord),
        wxTE_CENTER, wxDefaultValidator, _("m_WxTextCtrl_LCNno"));
    m_WxTextCtrl_LCNno->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MainFrame::OnLCNnumInputClicked), NULL,this);
    bSizer5->Add(m_WxTextCtrl_LCNno, 0, wxALL, 5);

    m_toggleBtn2 = new wxButton(m_panel4, ID_LCNButton2, wxT("Get Specific LCN Range!"), wxDefaultPosition, wxSize(180, wxDefaultCoord), 0);
    bSizer5->Add(m_toggleBtn2, 0, wxALL, 5);
    bSizer4->Add(bSizer5);

    wxDataViewListCtrl *m_dataViewListCtrl1 = new wxDataViewListCtrl(m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    bSizer4->Add(m_dataViewListCtrl1, 0, wxALL | wxEXPAND, 5);

    m_WxTextCtrl2 = new wxTextCtrl(m_panel4, ID_WXTEXTCTRL1, _("Output2"), wxDefaultPosition, wxSize(wxDefaultCoord, 350),
        wxVSCROLL | wxTE_READONLY | wxTE_MULTILINE, wxDefaultValidator, _("WxTextCtrl2"));
    m_WxTextCtrl2->SetMaxLength(0);
    m_WxTextCtrl2->SetInsertionPointEnd();
    bSizer4->Add(m_WxTextCtrl2, 0, wxEXPAND | wxALL, 5);

    m_panel4->SetSizer(bSizer4);
    m_panel4->Layout();
    bSizer4->Fit(m_panel4);
    m_notebook1->AddPage(m_panel4, wxT("LCN"), false);
}


void MainFrame::OnLCNnumInputClicked(wxMouseEvent& event)
{
    m_WxTextCtrl_LCNno->Clear();
    event.Skip();
}

void MainFrame::GetAllLCNs(wxCommandEvent& event)
{
    stopgap_init_run();
    return;
}

//Given an LCN number, query the current drive letter for what files are directly on it.(Range works too)
void MainFrame::GetSpecificLCNRange(wxCommandEvent& event)
{
    //Options opts;
    //winx_file_info file;
    //winx_volume_region region;
    const char letter = g_mainFrame->GetDriveLetter();
    ULONGLONG LCN;
    m_WxTextCtrl_LCNno->GetValue().ToULongLong(&LCN);
    //TODO: Replace 2000 with the actual cluster map's current cell size.
    const wxString resultName = stopgap_findfiles_at_LCN(letter, LCN, 2000);
    //direct hit! output: from 785233
    //Found \??\A:\VM\Windows 10 x64\Windows 10 x64-000001-s004.vmdk at the LCN in question.
    g_mainFrame->m_WxTextCtrl2->Clear();
    g_mainFrame->m_WxTextCtrl2->AppendText(resultName);
    //Re-Initialize the ZenWinX lib, (because everytime Stopgap Runs, it unloads it)
    if (winx_init_library() < 0)
        return;
}