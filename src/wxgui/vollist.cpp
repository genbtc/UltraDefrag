//////////////////////////////////////////////////////////////////////////
//
//  UltraDefrag - a powerful defragmentation tool for Windows NT.
//  Copyright (c) 2007-2015 Dmitri Arkhangelski (dmitriar@gmail.com).
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
#include "wx/wxprec.h"
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
    if(g_locale->GetCanonicalName().Left(2) != "my"){
        wxFont font = m_vList->GetFont();
        if(font.SetFaceName(wxT("Lucida Console"))){
            font.SetPointSize(DPI(9));
            m_vList->SetFont(font);
        }
    }

    //account for the borders
    //int border = wxSystemSettings::GetMetric(wxSYS_BORDER_X);//genBTC

    // adjust widths so all the columns will fit to the window
    int width = m_vList->GetClientSize().GetWidth();// - border*4;//genBTC
    //int width = this->GetClientSize().GetWidth() - borderx * 8;
    int lastColumnWidth = width;
    dtrace("INIT - client width ......... %d", width);
    //dtrace("INIT - border width ......... %d", borderx);

    int format[] = {
        wxLIST_FORMAT_LEFT, wxLIST_FORMAT_LEFT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT
    };

    for(int i = 0; i < LIST_COLUMNS - 1; i++) {
        int w = m_w[i] = (int)floor(m_r[i] * width);
        m_vList->InsertColumn(i, wxEmptyString, format[i], w);
        // dtrace("column %d width ....... %d", i, w);
        lastColumnWidth -= w;
    }

    int w = (int)floor(m_r[LIST_COLUMNS - 1] * width);
    if(w > 0) w = lastColumnWidth;
    m_w[LIST_COLUMNS - 1] = w;
    m_vList->InsertColumn(LIST_COLUMNS - 1,
        wxEmptyString, format[LIST_COLUMNS - 1], w
    );
    // dtrace("column %d width ....... %d", LIST_COLUMNS - 1, w);

    // attach drive icons
    int size = g_iconSize;
    wxImageList *list = new wxImageList(size,size);
    g_fixedIcon          = list->Add(wxIcon("fixed"           , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    g_fixedDirtyIcon     = list->Add(wxIcon("fixed_dirty"     , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    g_removableIcon      = list->Add(wxIcon("removable"       , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    g_removableDirtyIcon = list->Add(wxIcon("removable_dirty" , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    m_vList->SetImageList(list,wxIMAGE_LIST_SMALL);

    // ensure that the list will cover integral number of items
    m_vListHeight = 0xFFFFFFFF; // prevent expansion of the list
    m_vList->InsertItem(0,"hi",0);
    ProcessCommandEvent(this,ID_AdjustListHeight);

    GetEventHandler()->Connect(wxEVT_SIZE,wxSizeEventHandler(MainFrame::OnListSize),NULL,this);
    m_splitter->GetEventHandler()->Connect(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
        wxSplitterEventHandler(MainFrame::OnSplitChanged), nullptr,this);
}

// =======================================================================
//                            Event handlers
// =======================================================================

BEGIN_EVENT_TABLE(DrivesList, wxListView)
    EVT_KEY_DOWN(DrivesList::OnKeyDown)
    EVT_KEY_UP(DrivesList::OnKeyUp)
    EVT_LEFT_DCLICK(DrivesList::OnMouse)
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
/*         dtrace("Modifier: %d ... KeyCode: %d", \
 *             event.GetModifiers(), event.GetKeyCode());
 */
        switch(event.GetKeyCode()){
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            if(event.GetModifiers() == wxMOD_NONE)
                QueueCommandEvent(g_mainFrame,ID_DefaultAction);
            break;
        case 'A':
            if(event.GetModifiers() == wxMOD_CONTROL)
                QueueCommandEvent(g_mainFrame,ID_SelectAll);
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
            QueueCommandEvent(g_mainFrame,ID_DefaultAction);
        event.Skip();
    }
}

///TODO: if a defrag job is going off, allow it to Switch.
///  But we have to disable the Defrag job from auto redrawing into that space if we have switched.
void DrivesList::OnSelectionChange(wxListEvent& event)
{
    long i = GetFirstSelected();
    if(i != -1){
        char letter = (char)GetItemText(i)[0];
        JobsCacheEntry *currentJob = g_mainFrame->m_jobsCache[(int)letter];
        if(g_mainFrame->m_currentJob != currentJob){
            g_mainFrame->m_currentJob = currentJob;
            QueueCommandEvent(g_mainFrame,ID_RedrawMap);
            QueueCommandEvent(g_mainFrame,ID_UpdateStatusBar);
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
    if(width == 0)
        width = m_vList->GetClientSize().GetWidth();

    // get current column widths, since user could have changed them
    int cwidth = 0; bool changed = false;
    for(int i = 0; i < LIST_COLUMNS; i++){
        int w = m_vList->GetColumnWidth(i);
        cwidth += w;
        if(w != m_w[i])
            changed = true;
    }

    if(changed){
        for(int i = 0; i < LIST_COLUMNS; i++)
            m_r[i] = (double)m_vList->GetColumnWidth(i) / (double)cwidth;
    }
//genBTC - this stops dynamic column adjusting when you resize the windowframe
//     else{
//        return;
//     }

    int lastColumnWidth = width;

    // int border = wxSystemSettings::GetMetric(wxSYS_BORDER_X);
    // dtrace("border width ......... %d", border);
    // dtrace("client width ......... %d", width);
    // dtrace("total column width ... %d", cwidth);

    for(int i = 0; i < LIST_COLUMNS - 1; i++) {
        int w = m_w[i] = (int)floor(m_r[i] * width);
        m_vList->SetColumnWidth(i, w);
        // dtrace("column %d width ....... %d", i, w);
        lastColumnWidth -= w;
    }

    int w = (int)floor(m_r[LIST_COLUMNS - 1] * width);
    if(w > 0) w = lastColumnWidth;
    m_w[LIST_COLUMNS - 1] = w;

    m_vList->SetColumnWidth(LIST_COLUMNS - 1, w);
    // dtrace("column %d width ....... %d", LIST_COLUMNS - 1, w);
}

void MainFrame::AdjustListHeight(wxCommandEvent& WXUNUSED(event))
{
    // get client height of the list
    int height = m_splitter->GetSashPosition();
    height -= 2 * wxSystemSettings::GetMetric(wxSYS_BORDER_Y);

    // avoid recursion
    if(height == m_vListHeight) return;
    bool expand = height > m_vListHeight ? true : false;
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
    QueueCommandEvent(this,ID_AdjustListHeight);
    QueueCommandEvent(this,ID_AdjustListColumns);
    QueueCommandEvent(this,ID_RedrawMap);

    event.Skip();
}

void MainFrame::OnListSize(wxSizeEvent& event)
{
    int old_width = m_vList->GetClientSize().GetWidth();
    int new_width = this->GetClientSize().GetWidth();
    new_width -= 4 * wxSystemSettings::GetMetric(wxSYS_EDGE_X);
    if(m_vList->GetCountPerPage() < m_vList->GetItemCount())
        new_width -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    // scale list columns; avoid horizontal scrollbar appearance
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,ID_AdjustListColumns);
    evt.SetInt(new_width);
    if(new_width < old_width)
        GetEventHandler()->ProcessEvent(evt);
    else
        GetEventHandler()->AddPendingEvent(evt);

    event.Skip();
}

// =======================================================================
//                            Drives scanner
// =======================================================================

void *ListThread::Entry()
{
    while(!g_mainFrame->CheckForTermination(200)){
        if(m_rescan){
            QueueCommandEvent(g_mainFrame,ID_PopulateList);
            m_rescan = false;
        }
    }

    return nullptr;
}

void MainFrame::UpdateVolumeInformation(wxCommandEvent& event)
{
    int index = event.GetInt();
    volume_info *v = (volume_info *)event.GetClientData();

    if(!v){ // the request has been made from the running job (job.cpp@ProcessVolume)
        int i;
        for(i = 0; i < m_vList->GetItemCount(); i++){
            char letter = (char)m_vList->GetItemText(i)[0];
            if((char)index == letter) break;
        }

        if(i < m_vList->GetItemCount()){
            v = new volume_info;
            dtrace("The running job wants to refresh volume information for Drive: %c",(char)index);
            int result = udefrag_get_volume_information((char)index,v);
            if(result < 0){ delete v; return; }
            m_volinfocache = *v;    //genBTC, make a copy/cache of the volume info.(for fileslist.cpp)
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
    dtrace("Updated Volume Information for Drive: %c", v->letter);
    char s[32]; wxString string;
    ::winx_bytes_to_hr((ULONGLONG)v->total_space.QuadPart,2,s,sizeof s);
    string.Printf("%hs",s); m_vList->SetItem(index,3,string);

    ::winx_bytes_to_hr((ULONGLONG)v->free_space.QuadPart,2,s,sizeof s);
    string.Printf("%hs",s); m_vList->SetItem(index,4,string);

    double total = (double)v->total_space.QuadPart;
    double free = (double)v->free_space.QuadPart;
    double d = total > 0 ? free / total : 0;
    int p = (int)(100 * d);
    string.Printf("%u %%",p); m_vList->SetItem(index,5,string);

    delete v;
}

void MainFrame::UpdateVolumeStatus(wxCommandEvent& event)
{
    char letter = (char)event.GetInt();
    JobsCacheEntry *cacheEntry = m_jobsCache[(int)letter];
    if(!cacheEntry) return;
	//search for which drive we are updating by iterating through the vol list
    int index;
    for(index = 0; index < m_vList->GetItemCount(); index++){
        if(letter == (char)m_vList->GetItemText(index)[0]) break;
    }
    if(index >= m_vList->GetItemCount()) return; //exit if we cant find any

    wxString status;
    if(cacheEntry->pi.completion_status == 0){
        if(cacheEntry->pi.current_operation == VOLUME_ANALYSIS){
            //: Status of the running disk analysis,
            //: expands to "10 % analyzed".
            //: Make sure that "%5.2lf" is included in the
            //: translated string at the correct position.
            status.Printf(_("%5.2lf %% analyzed"),cacheEntry->pi.percentage);
        } else if(cacheEntry->jobType == DEFRAGMENTATION_JOB){
            //: Status of the running disk defragmentation,
            //: expands to "10 % defragmented, pass 5".
            //: Make sure that "%5.2lf" and "%d" are included
            //: in the translated string at the correct positions.
            status.Printf(_("%5.2lf %% defragmented, pass %d"),
                cacheEntry->pi.percentage,cacheEntry->pi.pass_number
            );
        } else {
            //: Status of the running disk optimization, expands to
            //: "10 % optimized, pass 5, 1024 moves total".
            //: Make sure that "%5.2lf", "%d" and "%I64u" are included
            //: in the translated string at the correct positions.
            status.Printf(_("%5.2lf %% optimized, pass %d, %I64u moves total"),
                cacheEntry->pi.percentage,cacheEntry->pi.pass_number,cacheEntry->pi.total_moves
            );
        }
    } 
    if (cacheEntry->stopped)
        status << " - " << _("STOPPED") << ".";
    if (cacheEntry->pi.completion_status != 0) {
        if(cacheEntry->jobType == ANALYSIS_JOB){
            //: Status of the completed disk analysis.
            status << _("Analyzed");
        } else if(cacheEntry->jobType == DEFRAGMENTATION_JOB){
            //: Status of the completed disk defragmentation,
            //: expands to "Defragmented, in 5 passes".
            //: Make sure that "%d" is included in the
            //: translated string at the correct position.
            status.Printf(_("Defragmented, in %d passes"),
                cacheEntry->pi.pass_number
            );
        } else {
            //: Status of the completed disk optimization, expands to
            //: "Optimized, in 5 passes, 1024 moves total".
            //: Make sure that "%d" and "%I64u" are included
            //: in the translated string at the correct positions.
            status.Printf(_("Optimized, in %d passes, %I64u moves total"),
                cacheEntry->pi.pass_number,cacheEntry->pi.total_moves
            );
        }
    }
    m_vList->SetItem(index,1,status);

    wxString fragmentation = wxString::Format("%5.2lf %%",
        cacheEntry->pi.fragmentation);
    m_vList->SetItem(index,2,fragmentation);
}

void MainFrame::PopulateList(wxCommandEvent& event)
{
    //should only happen once.
    volume_info *v = ::udefrag_get_vollist(m_skipRem);
    if(!v) return;

    m_vList->DeleteAllItems();
    m_DriveSubMenu = new wxMenu();  //make the submenu of fileslist popupmenu.

    for(int i = 0; v[i].letter; i++){
        wxString s = wxString::Format(
            wxT("%c: [%hs]"),v[i].letter,
            v[i].fsname
        );
        wxString label;
        label.Printf("%-10ls %ls",
            ws(s),v[i].label);
        m_vList->InsertItem(i,label);

        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED,ID_UpdateVolumeInformation);
        volume_info *v_copy = new volume_info;
        memcpy(v_copy,&v[i],sizeof(volume_info));
        e.SetInt(i); e.SetClientData((void *)v_copy);
        GetEventHandler()->ProcessEvent(e);

        e.SetId(ID_UpdateVolumeStatus);
        e.SetInt((int)v[i].letter);
        GetEventHandler()->ProcessEvent(e);

        m_DriveSubMenu->Append(2000+(int)v[i].letter,label,L""); //Adding each drive to submenu
        // encode the drive-letter char as an int + 2000 in the EventID, and listen on a range of ID's from 2065-2090 (A-Z)
        // when clicked, this ID will run FilesList::RClickSubMenuMoveFiletoDriveX(wxCommandEvent& event) @ fileslist.cpp
    }
    m_RClickPopupMenu1->AppendSubMenu(m_DriveSubMenu,"Move file to Drive:");    //add the submenu to the menu.

    ProcessCommandEvent(this,ID_AdjustListColumns);

    m_vList->Select(0);
    m_vList->Focus(0);

    m_currentJob = m_jobsCache[(int)v[0].letter];
    ProcessCommandEvent(this,ID_RedrawMap);
    ProcessCommandEvent(this,ID_UpdateStatusBar);

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
