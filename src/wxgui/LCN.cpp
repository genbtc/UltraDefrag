#include "prec.h"
#include "main.h"


void MainFrame::InitLCNPanel()
{
    //create LCN  tab, Panel 4.
    m_panel4 = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);

    wxStaticText *m_staticText2 = new wxStaticText(m_panel4, wxID_ANY, wxT("textLogBox"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText2->Wrap(250);
    bSizer4->Add(m_staticText2, 0, wxALL | wxEXPAND, 5);

    wxListBox *m_listBox1 = new wxListBox(m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    bSizer4->Add(m_listBox1, 0, wxALL, 5);

    m_toggleBtn1 = new wxToggleButton(m_panel4, ID_LCNButton1, wxT("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer4->Add(m_toggleBtn1, 0, wxALL, 5);

    m_toggleBtn2 = new wxButton(m_panel4, ID_LCNButton2, wxT("Toggle me!"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer4->Add(m_toggleBtn2, 0, wxALL, 5);

    wxDataViewListCtrl *m_dataViewListCtrl1 = new wxDataViewListCtrl(m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    bSizer4->Add(m_dataViewListCtrl1, 0, wxALL | wxEXPAND, 5);

    wxTextCtrl *m_WxTextCtrl2 = new wxTextCtrl(m_panel4, ID_WXTEXTCTRL1, _("Output2"), wxDefaultPosition, wxSize(wxDefaultCoord, 350),
        wxVSCROLL | wxTE_READONLY | wxTE_MULTILINE, wxDefaultValidator, _("WxTextCtrl2"));
    m_WxTextCtrl2->SetMaxLength(0);
    m_WxTextCtrl2->SetInsertionPointEnd();
    bSizer4->Add(m_WxTextCtrl2, 0, wxEXPAND | wxALL, 5);

    m_panel4->SetSizer(bSizer4);
    m_panel4->Layout();
    bSizer4->Fit(m_panel4);
    m_notebook1->AddPage(m_panel4, wxT("LCN"), false);
}

void MainFrame::GetAllLCNs(wxCommandEvent& event)
{
    stopgap_init_run();
    return;
}

void MainFrame::GetSpecificLCNRange(wxCommandEvent& event)
{
    Options opts;
    winx_file_info file;
    winx_volume_region region;
    stopgap_move_file(opts,&file,&region);
    return;
}