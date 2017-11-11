//Cluster Map Legend popup.
//genBTC 2017  Picture 10/14 final draft @ https://puu.sh/xYa55/cddf6c0c11.png                    

/*
(NOTE: The Cluster Map is the parent of all this. Cluster map is a wxWindow with a custom OnPaint handler using Windows primitives.)
Explanation:
1. Entry point @ funcion void ClusterMap::ClusterMapLegend(wxMouseEvent& event)..
2.     m_legendPopup = new LegendTransientPopup(this);
*/

#include "prec.h"
#include "main.h"

// =======================================================================
// LegendTransientPopup .CPP Implementation
// =======================================================================

LegendTransientPopup::LegendTransientPopup(wxWindow* parent)
    : wxPopupTransientWindow(parent)
{
    m_owner = parent;
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxHORIZONTAL);
    m_window = new wxWindow(this, wxID_ANY);
    m_window->SetBackgroundColour(*wxLIGHT_GREY);

    //Add Static Text - Colored Boxes (Cluster map legend)
    wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
    for (int eI = 0; eI < SPACE_STATES; ++eI) {
        auto txt1 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
        txt1->Wrap(-1);
        txt1->SetBackgroundColour(g_colors[eI]);
        if (eI == 1 || eI == 7 || eI == 8 || eI == 12)
            txt1->SetForegroundColour(*wxWHITE);
        bSizer2->Add(txt1, 0, wxALL, 1);
    }
    bSizer1->Add(bSizer2, 1, wxALL, 5);

    //Add All.
    bSizer1->Fit(m_window);
    SetSizerAndFit(bSizer1);
}

/** @} */
