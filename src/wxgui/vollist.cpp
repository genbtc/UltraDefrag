//////////////////////////////////////////////////////////////////////////
//
//  UltraDefrag - a powerful defragmentation tool for Windows NT.
//  Copyright (c) 2007-2013 Dmitri Arkhangelski (dmitriar@gmail.com).
//  Copyright (c) 2010-2013 Stefan Pendl (stefanpe@users.sourceforge.net).
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////

/**
 * @file vollist.cpp
 * @brief List of volumes.
 * @addtogroup VolList
 * @{
 */

// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.

// =======================================================================
//                            Declarations
// =======================================================================

#include "main.h"

int g_fixedIcon;
int g_fixedDirtyIcon;
int g_removableIcon;
int g_removableDirtyIcon;

// =======================================================================
//                           List of volumes
// =======================================================================

void MainFrame::InitVolList()
{
    // save default font used for the list
    m_vListFont = new wxFont(m_vList->GetFont());

    // set mono-space font for the list unless Burmese translation is selected
    if(g_locale->GetCanonicalName().Left(2) != wxT("my")){
        wxFont font = m_vList->GetFont();
        if(font.SetFaceName(wxT("Courier New"))){
            font.SetPointSize(DPI(9));
            m_vList->SetFont(font);
        }
    }

    // adjust widths so all the columns will fit to the window
    int width = m_vList->GetClientSize().GetWidth();
    int cwidth = 0, i, count = sizeof(m_w)/sizeof(m_w[0]);
    for(i = 0; i < count; i++)
        cwidth += m_w[i];

    /* m_w[count - 1] = width;

    double scale = (double)width / (double)cwidth;
    for(i = 0; i < count; i++) {
        m_w[i] = (int)floor(scale * (double)m_w[i]);
        m_w[count - 1] -= m_w[i];
    } */

    dtrace("client width ......... %d", width);
    dtrace("total column width ... %d", cwidth);

    int format[] = {
        wxLIST_FORMAT_LEFT, wxLIST_FORMAT_LEFT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT
        };

    for(i = 0; i < count; i++) {
        m_vList->InsertColumn(i, wxEmptyString, format[i], m_w[i]);
        dtrace("column %d width ....... %d", i, m_w[i]);
    }

    // attach drive icons
    int size = g_iconSize;
    wxImageList *list = new wxImageList(size,size);
    g_fixedIcon          = list->Add(wxIcon(wxT("fixed")           , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    g_fixedDirtyIcon     = list->Add(wxIcon(wxT("fixed_dirty")     , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    g_removableIcon      = list->Add(wxIcon(wxT("removable")       , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    g_removableDirtyIcon = list->Add(wxIcon(wxT("removable_dirty") , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    m_vList->SetImageList(list,wxIMAGE_LIST_SMALL);

    // ensure that the list will cover integral number of items
    m_vListHeight = 0xFFFFFFFF; // prevent expansion of the list
    m_vList->InsertItem(0,wxT("hi"),0);
    ProcessCommandEvent(ID_AdjustListHeight);

    Connect(wxEVT_SIZE,wxSizeEventHandler(MainFrame::OnListSize),NULL,this);
    m_splitter->Connect(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
        wxSplitterEventHandler(MainFrame::OnSplitChanged),NULL,this);
}

// =======================================================================
//                            Event handlers
// =======================================================================

BEGIN_EVENT_TABLE(DrivesList, wxListView)
    EVT_KEY_DOWN(DrivesList::OnKeyDown)
    EVT_KEY_UP(DrivesList::OnKeyUp)
    EVT_MOUSE_EVENTS(DrivesList::OnMouse)
    EVT_LIST_ITEM_SELECTED(wxID_ANY,DrivesList::OnSelectionChange)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY,DrivesList::OnSelectionChange)
END_EVENT_TABLE()

void DrivesList::OnKeyDown(wxKeyEvent& event)
{
    if(!g_mainFrame->m_busy) event.Skip();
}

void DrivesList::OnKeyUp(wxKeyEvent& event)
{
    if(!g_mainFrame->m_busy){
        // dtrace("Modifier: %d ... KeyCode: %d", \
        //    event.GetModifiers(), event.GetKeyCode());
        switch(event.GetKeyCode()){
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            if(event.GetModifiers() == wxMOD_NONE)
                PostCommandEvent(g_mainFrame,ID_DefaultAction);
            break;
        case 'A':
            if(event.GetModifiers() == wxMOD_CONTROL)
                PostCommandEvent(g_mainFrame,ID_SelectAll);
            break;
        default:
            break;
        }
        event.Skip();
    }
}

void DrivesList::OnMouse(wxMouseEvent& event)
{
    if(!g_mainFrame->m_busy){
        // left double click starts default action
        if(event.GetEventType() == wxEVT_LEFT_DCLICK)
            PostCommandEvent(g_mainFrame,ID_DefaultAction);
        event.Skip();
    }
}

void DrivesList::OnSelectionChange(wxListEvent& event)
{
    long i = GetFirstSelected();
    if(i != -1){
        char letter = (char)GetItemText(i)[0];
        JobsCacheEntry *currentJob = g_mainFrame->m_jobsCache[(int)letter];
        if(g_mainFrame->m_currentJob != currentJob){
            g_mainFrame->m_currentJob = currentJob;
            PostCommandEvent(g_mainFrame,ID_RedrawMap);
            PostCommandEvent(g_mainFrame,ID_UpdateStatusBar);
        }
    }
    event.Skip();
}

void MainFrame::SelectAll(wxCommandEvent& WXUNUSED(event))
{
    for(int i = 0; i < m_vList->GetItemCount(); i++)
        m_vList->Select(i); m_vList->Focus(0);
}

void MainFrame::AdjustListColumns(wxCommandEvent& event)
{
    int width = event.GetInt();
    if(width == 0) width = m_vList->GetClientSize().GetWidth();

    // get current column widths, since user could have changed them
    int cwidth = 0, i, count = m_vList->GetColumnCount();
    for(int i = 0; i < count; i++) {
        m_w[i] = m_vList->GetColumnWidth(i);
        cwidth += m_w[i];
    }
    m_w[count - 1] = width;

    int border = wxSystemSettings::GetMetric(wxSYS_BORDER_X);

    dtrace("border width ......... %d", border);
    dtrace("client width ......... %d", width);
    dtrace("total column width ... %d", cwidth);

    double scale = (double)width / (double)cwidth;
    for(i = 0; i < (count - 1); i++) {
        m_w[i] = (int)floor(scale * (double)m_w[i]);
        m_vList->SetColumnWidth(i,m_w[i]);
        m_w[count - 1] -= m_w[i];
        dtrace("column %d width ....... %d", i, m_w[i]);
    }
    m_vList->SetColumnWidth(count - 1,m_w[count - 1]);
    dtrace("column %d width ....... %d", count - 1, m_w[count - 1]);
}

void MainFrame::AdjustListHeight(wxCommandEvent& WXUNUSED(event))
{
    // get client height of the list
    int height = m_splitter->GetSashPosition();
    height -= 2 * wxSystemSettings::GetMetric(wxSYS_BORDER_Y);

    // avoid recursion
    if(height == m_vListHeight) return;
    bool expand = (height > m_vListHeight) ? true : false;
    m_vListHeight = height;

    if(!m_vList->GetColumnCount()) return;

    // get height of the list header
    HWND header = ListView_GetHeader((HWND)m_vList->GetHandle());
    if(!header){ letrace("cannot get list header"); return; }

    RECT rc;
    if(!Header_GetItemRect(header,0,&rc)){
        letrace("cannot get list header size"); return;
    }
    int header_height = rc.bottom - rc.top;

    // get height of a single row
    wxRect rect; if(!m_vList->GetItemRect(0,rect)) return;
    int item_height = rect.GetHeight();

    // force list to cover integral number of items
    int items = (height - header_height) / item_height;
    int new_height = header_height + items * item_height;
    if(expand && new_height < height){
        items ++; new_height += item_height;
    }

    m_vListHeight = new_height;

    // adjust client height of the list
    new_height += 2 * wxSystemSettings::GetMetric(wxSYS_BORDER_Y);
    m_splitter->SetSashPosition(new_height);
}

void MainFrame::OnSplitChanged(wxSplitterEvent& event)
{
    // ensure that the list control will cover integral number of items
    PostCommandEvent(this,ID_AdjustListHeight);

    // adjust list columns once again to reflect the actual layout
    PostCommandEvent(this,ID_AdjustListColumns);

    event.Skip();
}

void MainFrame::OnListSize(wxSizeEvent& event)
{
    int old_width = m_vList->GetClientSize().GetWidth();
    int new_width = this->GetClientSize().GetWidth();
    new_width -= 2 * wxSystemSettings::GetMetric(wxSYS_EDGE_X);
    if(m_vList->GetCountPerPage() < m_vList->GetItemCount())
        new_width -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    // scale list columns; avoid horizontal scrollbar appearance
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,ID_AdjustListColumns);
    evt.SetInt(new_width);
    if(new_width <= old_width)
        ProcessEvent(evt);
    else
        wxPostEvent(this,evt);

    event.Skip();
}

// =======================================================================
//                            Drives scanner
// =======================================================================

void *ListThread::Entry()
{
    while(!g_mainFrame->CheckForTermination(200)){
        if(m_rescan){
            PostCommandEvent(g_mainFrame,ID_PopulateList);
            m_rescan = false;
        }
    }

    return NULL;
}

void MainFrame::UpdateVolumeInformation(wxCommandEvent& event)
{
    int index = event.GetInt();
    volume_info *v = (volume_info *)event.GetClientData();

    if(!v){ // the request has been made from the running job
        int i;
        for(i = 0; i < m_vList->GetItemCount(); i++){
            char letter = (char)m_vList->GetItemText(i)[0];
            if((char)index == letter) break;
        }

        if(i < m_vList->GetItemCount()){
            v = new volume_info;
            int result = udefrag_get_volume_information((char)index,v);
            if(result < 0){ delete v; return; }
            index = i;
        }
    }

    if(v->is_dirty){
        if(v->is_removable) m_vList->SetItemImage(index,g_removableDirtyIcon);
        else m_vList->SetItemImage(index,g_fixedDirtyIcon);
        m_vList->SetItem(index,1,_("Disk needs to be repaired"));
    } else {
        if(v->is_removable) m_vList->SetItemImage(index,g_removableIcon);
        else m_vList->SetItemImage(index,g_fixedIcon);
    }

    char s[32]; wxString string;
    ::winx_bytes_to_hr((ULONGLONG)(v->total_space.QuadPart),2,s,sizeof(s));
    string.Printf(wxT("%hs"),s); m_vList->SetItem(index,3,string);

    ::winx_bytes_to_hr((ULONGLONG)(v->free_space.QuadPart),2,s,sizeof(s));
    string.Printf(wxT("%hs"),s); m_vList->SetItem(index,4,string);

    double total = (double)v->total_space.QuadPart;
    double free = (double)v->free_space.QuadPart;
    double d = (total > 0) ? free / total : 0;
    int p = (int)(100 * d);
    string.Printf(wxT("%u %%"),p); m_vList->SetItem(index,5,string);

    delete v;
}

void MainFrame::UpdateVolumeStatus(wxCommandEvent& event)
{
    char letter = (char)event.GetInt();
    JobsCacheEntry *cacheEntry = m_jobsCache[(int)letter];
    if(!cacheEntry) return;

    int index;
    for(index = 0; index < m_vList->GetItemCount(); index++){
        if(letter == (char)m_vList->GetItemText(index)[0]) break;
    }
    if(index >= m_vList->GetItemCount()) return;

    // each job starts with a volume analysis
    wxString caption = _("Analyzed");
    if(cacheEntry->pi.current_operation != VOLUME_ANALYSIS){
        switch(cacheEntry->jobType){
            case DEFRAGMENTATION_JOB:
                caption = _("Defragmented");
                break;
            case FULL_OPTIMIZATION_JOB:
            case QUICK_OPTIMIZATION_JOB:
            case MFT_OPTIMIZATION_JOB:
                caption = _("Optimized");
                break;
            default:
                break;
        }
    }
    wxString lcaption = caption;
    lcaption.MakeLower();

    wxString status;
    if(cacheEntry->pi.completion_status == 0 || cacheEntry->stopped){
        if(cacheEntry->pi.pass_number > 1){
            if(cacheEntry->pi.current_operation == VOLUME_OPTIMIZATION){
                status.Printf(wxT("%5.2lf %% %ls, pass %d, %I64u moves total"),
                    cacheEntry->pi.percentage,lcaption.wc_str(),
                    cacheEntry->pi.pass_number,cacheEntry->pi.total_moves
                );
            } else {
                status.Printf(wxT("%5.2lf %% %ls, pass %d"),
                    cacheEntry->pi.percentage,lcaption.wc_str(),
                    cacheEntry->pi.pass_number
                );
            }
        } else {
            if(cacheEntry->pi.current_operation == VOLUME_OPTIMIZATION){
                status.Printf(wxT("%5.2lf %% %ls, %I64u moves total"),
                    cacheEntry->pi.percentage,lcaption.wc_str(),
                    cacheEntry->pi.total_moves
                );
            } else {
                status.Printf(wxT("%5.2lf %% %ls"),
                    cacheEntry->pi.percentage,lcaption.wc_str()
                );
            }
        }
    } else {
        if(cacheEntry->pi.pass_number > 1){
            status.Printf(wxT("%ls, %d passes needed"),
                caption.wc_str(),cacheEntry->pi.pass_number
            );
        } else {
            status.Printf(wxT("%ls"),caption.wc_str());
        }
    }
    m_vList->SetItem(index,1,status);

    wxString fragmentation = wxString::Format(wxT("%5.2lf %%"),
        cacheEntry->pi.fragmentation);
    m_vList->SetItem(index,2,fragmentation);
}

void MainFrame::PopulateList(wxCommandEvent& event)
{
    volume_info *v = ::udefrag_get_vollist(m_skipRem);
    if(!v) return;

    m_vList->DeleteAllItems();

    for(int i = 0; v[i].letter; i++){
        wxString label;
        label.Printf(wxT("%-10ls %ls"),
            wxString::Format(wxT("%c: [%hs]"),
            v[i].letter,v[i].fsname).wc_str(),
            v[i].label);
        m_vList->InsertItem(i,label);

        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED,ID_UpdateVolumeInformation);
        volume_info *v_copy = new volume_info;
        memcpy(v_copy,&v[i],sizeof(volume_info));
        e.SetInt(i); e.SetClientData((void *)v_copy);
        ProcessEvent(e);

        e.SetId(ID_UpdateVolumeStatus);
        e.SetInt((int)v[i].letter);
        ProcessEvent(e);
    }

    // adjust list columns once again to reflect the actual layout
    ProcessCommandEvent(ID_AdjustListColumns);

    m_vList->Select(0);
    m_vList->Focus(0);

    m_currentJob = m_jobsCache[(int)v[0].letter];
    ProcessCommandEvent(ID_RedrawMap);
    ProcessCommandEvent(ID_UpdateStatusBar);

    ::udefrag_release_vollist(v);
}

void MainFrame::OnSkipRem(wxCommandEvent& WXUNUSED(event))
{
    if(!m_busy){
        m_skipRem = m_menuBar->FindItem(ID_SkipRem)->IsChecked();
        m_listThread->m_rescan = true;
    }
}

void MainFrame::OnRescan(wxCommandEvent& WXUNUSED(event))
{
    if(!m_busy) m_listThread->m_rescan = true;
}

/** @} */
