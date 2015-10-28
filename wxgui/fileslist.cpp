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
 * @file fileslist.cpp
 * @brief List of files.
 * @addtogroup FilesList
 * @{
 */

// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.

// =======================================================================
//                            Declarations
// =======================================================================

#include "main.h"

int f_fixedIcon;
int f_fixedDirtyIcon;
int f_removableIcon;
int f_removableDirtyIcon;

// =======================================================================
//                           List of volumes
// =======================================================================

void MainFrame::InitFilesList()
{
    // save default font used for the list
    m_filesListFont = new wxFont(m_filesList->GetFont());

    // set mono-space font for the list unless Burmese translation is selected
    if(g_locale->GetCanonicalName().Left(2) != wxT("my")){
        wxFont font = m_filesList->GetFont();
        if(font.SetFaceName(wxT("Lucida Console"))){
            font.SetPointSize(DPI(9));
            m_filesList->SetFont(font);
        }
    }

    //account for the borders
    //int border = wxSystemSettings::GetMetric(wxSYS_BORDER_X);//genBTC

    // adjust widths so all the columns will fit to the window
    int width = m_filesList->GetClientSize().GetWidth();// - border*4;//genBTC
    int lastColumnWidth = width;

    dtrace("INIT - client width ......... %d", width);

    int format[] = {
        wxLIST_FORMAT_LEFT, wxLIST_FORMAT_LEFT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT
    };

    for(int i = 0; i < LIST_COLUMNS - 1; i++) {
        int w = m_w[i] = (int)floor(m_r[i] * width);
        m_filesList->InsertColumn(i, wxEmptyString, format[i], w);
        dtrace("FilesList column %d width ....... %d", i, w);
        lastColumnWidth -= w;
    }

    int w = (int)floor(m_r[LIST_COLUMNS - 1] * width);
    if(w > 0) w = lastColumnWidth;
    m_w[LIST_COLUMNS - 1] = w;
    m_filesList->InsertColumn(LIST_COLUMNS - 1,
        wxEmptyString, format[LIST_COLUMNS - 1], w
    );
    dtrace("FilesList column %d width ....... %d", LIST_COLUMNS - 1, w);

    // attach drive icons
    int size = g_iconSize;
    wxImageList *list = new wxImageList(size,size);
    f_fixedIcon          = list->Add(wxIcon(wxT("fixed")           , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    f_fixedDirtyIcon     = list->Add(wxIcon(wxT("fixed_dirty")     , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    f_removableIcon      = list->Add(wxIcon(wxT("removable")       , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    f_removableDirtyIcon = list->Add(wxIcon(wxT("removable_dirty") , wxBITMAP_TYPE_ICO_RESOURCE, size, size));
    m_filesList->SetImageList(list,wxIMAGE_LIST_SMALL);

    // ensure that the list will cover integral number of items
    m_filesListHeight = 0xFFFFFFFF; // prevent expansion of the list
    m_filesList->InsertItem(0,wxT("hi"),0);
    ProcessCommandEvent(EventID_AdjustFilesListHeight);

    Connect(wxEVT_SIZE,wxSizeEventHandler(MainFrame::FilesOnListSize),NULL,this);
//    m_splitter->Connect(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
//        wxSplitterEventHandler(MainFrame::FilesOnSplitChanged),NULL,this);
}

// =======================================================================
//                            Event handlers
// =======================================================================

BEGIN_EVENT_TABLE(FilesList, wxListView)
    EVT_KEY_DOWN(FilesList::OnKeyDown)
    EVT_KEY_UP(FilesList::OnKeyUp)
    EVT_MOUSE_EVENTS(FilesList::OnMouse)
    EVT_LIST_ITEM_SELECTED(wxID_ANY,FilesList::OnSelectionChange)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY,FilesList::OnSelectionChange)
END_EVENT_TABLE()

void FilesList::OnKeyDown(wxKeyEvent& event)
{
    if(!g_mainFrame->m_busy) event.Skip();
}

void FilesList::OnKeyUp(wxKeyEvent& event)
{
    if(!g_mainFrame->m_busy){
        // dtrace("Modifier: %d ... KeyCode: %d", \
        //    event.GetModifiers(), event.GetKeyCode());
        switch(event.GetKeyCode()){
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            if(event.GetModifiers() == wxMOD_NONE)
                PostCommandEvent(g_mainFrame,EventID_DefaultAction);
            break;
        case 'A':
            if(event.GetModifiers() == wxMOD_CONTROL)
                PostCommandEvent(g_mainFrame,EventID_SelectAll);
            break;
        default:
            break;
        }
        event.Skip();
    }
}

void FilesList::OnMouse(wxMouseEvent& event)
{
    if(!g_mainFrame->m_busy){
        // left double click starts default action
        if(event.GetEventType() == wxEVT_LEFT_DCLICK)
            PostCommandEvent(g_mainFrame,EventID_DefaultAction);
        event.Skip();
    }
}

void FilesList::OnSelectionChange(wxListEvent& event)
{
    long i = GetFirstSelected();
    if(i != -1){
        char letter = (char)GetItemText(i)[0];
        JobsCacheEntry *currentJob = g_mainFrame->m_jobsCache[(int)letter];
        if(g_mainFrame->m_currentJob != currentJob){
            g_mainFrame->m_currentJob = currentJob;
            PostCommandEvent(g_mainFrame,EventID_RedrawMap);
            PostCommandEvent(g_mainFrame,EventID_UpdateStatusBar);
            PostCommandEvent(g_mainFrame,EventID_FilesAnalyzedUpdateFilesList);
        }
    }
    event.Skip();
}

void MainFrame::FilesSelectAll(wxCommandEvent& WXUNUSED(event))
{
    for(int i = 0; i < m_filesList->GetItemCount(); i++)
        m_filesList->Select(i); m_filesList->Focus(0);
}

void MainFrame::FilesAdjustListColumns(wxCommandEvent& event)
{
    int width = event.GetInt();
    if(width == 0) width = m_filesList->GetClientSize().GetWidth();

    // get current column widths, since user could have changed them
    int cwidth = 0; bool changed = false;
    for(int i = 0; i < LIST_COLUMNS; i++){
        int w = m_filesList->GetColumnWidth(i);
        cwidth += w;
        if(w != m_w[i])
            changed = true;
    }

    if(changed){
        for(int i = 0; i < LIST_COLUMNS; i++)
            m_r[i] = (double)m_filesList->GetColumnWidth(i) / (double)cwidth;
    }
    else{
        return;
    }

    int lastColumnWidth = width;

    int border = wxSystemSettings::GetMetric(wxSYS_BORDER_X);

    dtrace("FilesList border width ......... %d", border);
    dtrace("FilesList client width ......... %d", width);
    dtrace("FilesList total column width ... %d", cwidth);

    for(int i = 0; i < (LIST_COLUMNS - 1); i++) {
        int w = m_w[i] = (int)floor(m_r[i] * width);
        m_filesList->SetColumnWidth(i, w);
        dtrace("column %d width ....... %d", i, w);
        lastColumnWidth -= w;
    }

    int w = (int)floor(m_r[LIST_COLUMNS - 1] * width);
    if(w > 0) w = lastColumnWidth;
    m_w[LIST_COLUMNS - 1] = w;

    m_filesList->SetColumnWidth(LIST_COLUMNS - 1, w);
    dtrace("column %d width ....... %d", LIST_COLUMNS - 1, w);
}

void MainFrame::FilesAdjustListHeight(wxCommandEvent& WXUNUSED(event))
{
    // get client height of the list
    int height = m_filesList->GetClientSize().GetHeight();
    dtrace("FilesList getclient.getheight  of the list ......... %d", height);
//    int height = m_splitter->GetSashPosition();
//    height -= 2 * wxSystemSettings::GetMetric(wxSYS_BORDER_Y);

    // avoid recursion
    if(height == m_filesListHeight) return;
    bool expand = (height > m_filesListHeight) ? true : false;
    m_filesListHeight = height;

    if(!m_filesList->GetColumnCount()) return;

    // get height of the list header
    HWND header = ListView_GetHeader((HWND)m_filesList->GetHandle());
    if(!header){ letrace("cannot get list header"); return; }

    RECT rc;
    if(!Header_GetItemRect(header,0,&rc)){
        letrace("cannot get list header size"); return;
    }
    int header_height = rc.bottom - rc.top;

    // get height of a single row
    wxRect rect; if(!m_filesList->GetItemRect(0,rect)) return;
    int item_height = rect.GetHeight();
    dtrace("FilesList one itemheight ......... %d", item_height);

    // force list to cover integral number of items
//    int items = (height - header_height) / item_height;
//    dtrace("FilesList numitems ......... %d", items);
//    int new_height = header_height + items * item_height;
//    dtrace("FilesList newheight ......... %d", new_height);
//    if(expand && new_height < height){
//        items ++; new_height += item_height;
//    }
    //genBTC
    int items = m_filesList->GetItemCount();
    int new_height = items * item_height + header_height;
    m_filesListHeight = new_height;

    dtrace("FilesList mfileslistheight-end......... %d", m_filesListHeight);
    // adjust client height of the list
    new_height += 2 * wxSystemSettings::GetMetric(wxSYS_BORDER_Y);
//    m_splitter->SetSashPosition(new_height);
}

void MainFrame::FilesOnSplitChanged(wxSplitterEvent& event)
{
    PostCommandEvent(this,EventID_AdjustFilesListHeight);
    PostCommandEvent(this,EventID_AdjustFilesListColumns);
    PostCommandEvent(this,EventID_RedrawMap);

    event.Skip();
}

void MainFrame::FilesOnListSize(wxSizeEvent& event)
{
    int old_width = m_filesList->GetClientSize().GetWidth();
    int new_width = this->GetClientSize().GetWidth();
    new_width -= 2 * wxSystemSettings::GetMetric(wxSYS_EDGE_X);
    if(m_filesList->GetCountPerPage() < m_filesList->GetItemCount())
        new_width -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    // scale list columns; avoid horizontal scrollbar appearance
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,EventID_AdjustFilesListColumns);
    evt.SetInt(new_width);
    if(new_width <= old_width)
        ProcessEvent(evt);
    else
        wxPostEvent(this,evt);

    event.Skip();
}

//=======================================================================
//                           Drives scanner
//=======================================================================

void *ListFilesThread::Entry()
{
    while(!g_mainFrame->CheckForTermination(200)){
        if(m_rescan){
            PostCommandEvent(g_mainFrame,EventID_PopulateFilesList);
            m_rescan = false;
        }
    }

    return NULL;
}

//genBTC
/**
 * @brief Gets called everytime a DiskDrive is detected (from FilesPopulateList)
*/
void MainFrame::FilesAnalyzedUpdateFilesList (wxCommandEvent& event)
{
    TraceEnter;
    //m_filesList->DeleteAllItems();

    //was volume information
//    int vindex = event.GetInt();
//    char letter = (char)vindex;
//    //volume_info *v = (volume_info *)event.GetClientData();
    //winx_volume_information volume_info;
//    int success = winx_get_volume_information(letter,&volume_info);
//    dtrace("Got Volume Info for: %s",letter);
//    if (success == 0){
//        etrace("exiting because no Get_Volume_info could exist for exists for %s",letter);
//        return;
//    }
//            v = new volume_info;
//            int result = udefrag_get_volume_information((char)index,v);
//
    long i = m_vList->GetFirstSelected();
    wxString letter = m_vList->GetItemText(i);
    wchar_t charletter;
    wxStrncpy(&charletter,letter,1);
    dtrace("printing out letter wchar_t %c",charletter);

    wchar_t *path = NULL;
    struct prb_traverser t;
    winx_file_info *file;
    char *comment;
    char *status;

    JobsCacheEntry *cacheEntry = m_jobsCache[int(charletter)];
    if(!cacheEntry){
        etrace("exiting because no CacheEntry exists");
        return;
    }
    dtrace("cache entry's number of files: %u",cacheEntry->pi.files);
    //prb_table *fragfileslist = m_currentJob->pi.fragmented_files;
    //prb_table *fragfileslist = cacheEntry->pi.fragmented_files;
    struct prb_table *fragfileslist = prb_copy(cacheEntry->pi.fragmented_files,NULL,NULL,NULL);
    dtrace("Got to the part where it stored the fragfileslist");
    /* Iterate through PRB_Table, filling the m_filesList as we go*/
    prb_t_init(&t,cacheEntry->pi.fragmented_files);
    dtrace("Got to Initialize the Traverser");
    //file = prb_t_first(&t,cacheEntry->pi.fragmented_files);
    //file = prb_t_first(&t,fragfileslist);
    file = (winx_file_info *)prb_t_first(&t,fragfileslist);
    if (file != NULL)
        dtrace("Got to the part where it casted void to winx_file_info");
    else
        dtrace("FILE WAS NULL!!!!");
    int prbindex = 0;
    //wxString thestring(file->name);
    dtrace("Printing file CTime %d",(file)->name);
    while(file){
        dtrace("Got to the part where it starts the loop");
        if(is_directory(file))
            comment = "[DIR]";
        else if(is_compressed(file))
            comment = "[CMP]";
//        else if(is_over_limit(file))
//            comment = "[SKIP]";
//        else if(is_essential_boot_file(file))
//            comment = "[BOOT]";
//        else if(is_mft_file(file))
//            comment = "[MFT]";
        else
            comment = "OK";
        status = "OK";


        /*
        * On change of status strings don't forget
        * also to adjust write_file_status routine
        * in udreportcnv.lua file.
        */
//        if(is_locked(file))
//            status = "locked";
//        else if(is_moving_failed(file))
//            status = "move failed";
//        else if(is_in_improper_state(file))
//            status = "invalid";
//        else
//            status = " - ";

//        (void)_snprintf(buffer, sizeof(buffer),
//            "\t{fragments = %u,"
//            "size = %I64u,"
//            "comment = \"%s\","
//            "status = \"%s\","
//            "path = \"",
//            (UINT)file->disp.fragments,
//            file->disp.clusters * volume_info.bytes_per_cluster,
//            comment,
//            status
//            );

//        char s[32];
//        wxString thestring;
//        ::winx_bytes_to_hr((volume_info.total_bytes),2,s,sizeof(s));
//        thestring.Printf(wxT("%hs"),s);
        wchar_t name;
        wxStrncpy(&name,file->name,255);
        dtrace("printing out letter wchar_t %c",name);

        m_filesList->InsertItem(0,wxT("Random # File"),0);
        dtrace("Got after the part where it inserts a random item");
        m_filesList->SetItem(prbindex,3,name);
        dtrace("Got after part where it sets a data");

        //file = prb_t_next(&t);
        file = (winx_file_info *)prb_t_next(&t);
        prbindex++;
    }
    ProcessCommandEvent(EventID_AdjustFilesListHeight);



//
//    for(index = 0; index < m_filesList->GetItemCount(); index++){
//        if(letter == (char)m_filesList->GetItemText(index)[0]) break;
//    }
//    if(index >= m_filesList->GetItemCount()) return;
//
//    wxString status;
//    int skip = 0;
//    if(cacheEntry->pi.completion_status == 0 || cacheEntry->stopped){
//        if(cacheEntry->pi.current_operation == VOLUME_ANALYSIS)
//            skip++;
//
//    } else {
//        if(cacheEntry->jobType == ANALYSIS_JOB)
//            skip++;
//    }
//    m_filesList->SetItem(index,1,status);
//
//    wxString fragmentation = wxString::Format(wxT("%5.2lf %%"),
//        cacheEntry->pi.fragmentation);
//    m_filesList->SetItem(index,2,fragmentation);
//
//    delete v;
}

/**
 * @brief Only called Once. Deletes the list and re-populates.
*/
void MainFrame::FilesPopulateList(wxCommandEvent& event)
{
    volume_info *v = ::udefrag_get_vollist(m_skipRem);
    if(!v) return;

    m_filesList->DeleteAllItems();

    for(int i = 0; v[i].letter; i++){
        wxString label;
        label.Printf(wxT("%-10ls %ls"),
            wxString::Format(wxT("%c: [%hs]"),
            v[i].letter,v[i].fsname).wc_str(),
            v[i].label);
        m_filesList->InsertItem(i,label);

        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED,EventID_FilesAnalyzedUpdateFilesList);
        volume_info *v_copy = new volume_info;
        memcpy(v_copy,&v[i],sizeof(volume_info));
        e.SetInt(i); e.SetClientData((void *)v_copy);
        ProcessEvent(e);

        e.SetId(EventID_FilesAnalyzedUpdateFilesList);
        e.SetInt((int)v[i].letter);
        //ProcessEvent(e);
    }

    ProcessCommandEvent(EventID_AdjustFilesListColumns);

    m_filesList->Select(0);
    m_filesList->Focus(0);

    m_currentJob = m_jobsCache[(int)v[0].letter];
    ProcessCommandEvent(EventID_RedrawMap);
    ProcessCommandEvent(EventID_UpdateStatusBar);

    ::udefrag_release_vollist(v);
}

void MainFrame::FilesOnSkipRem(wxCommandEvent& WXUNUSED(event))
{
    if(!m_busy){
        m_skipRem = m_menuBar->FindItem(EventID_SkipRem)->IsChecked();
        m_listfilesThread->m_rescan = true;
    }
}

void MainFrame::FilesOnRescan(wxCommandEvent& WXUNUSED(event))
{
    if(!m_busy) m_listfilesThread->m_rescan = true;
}

/** @} */
