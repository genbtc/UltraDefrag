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
    //int width = m_filesList->GetClientSize().GetWidth();
    int width = m_vList->GetClientSize().GetWidth();
    int lastColumnWidth = width;

    dtrace("INIT - client width ......... %d", width);

    int format[] = {
        wxLIST_FORMAT_LEFT, wxLIST_FORMAT_LEFT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_RIGHT
    };

    double ratios[LIST_COLUMNS] = {
            375.0/640, 65.0/640, 70.0/640,
            65.0/640, 64.0/640, 1.0/640
    };

    for(int i = 0; i < LIST_COLUMNS - 1; i++) {
        m_fcolsr[i] = ratios[i];    //initialize with fixed values from above
        int w = m_fcolsw[i] = (int)floor(m_fcolsr[i] * width);        //genBTC - set to default ratios.
        m_filesList->InsertColumn(i, wxEmptyString, format[i], w);
        dtrace("FilesList column %d width ....... %d", i, w);
        lastColumnWidth -= w;
    }

    //initialize with values (needed because the loop above goes 1 column less on purpose
    m_fcolsr[LIST_COLUMNS - 1] = ratios[LIST_COLUMNS - 1];
    //adjust the last column to be the precise size needed.
    int w = (int)floor(m_fcolsr[LIST_COLUMNS - 1] * width);
    if(w > 0) w = lastColumnWidth;
    m_fcolsw[LIST_COLUMNS - 1] = w;
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

//=======================================================================
//                           scanner thread
//=======================================================================
//currently mostly-disabled.
void *ListFilesThread::Entry()
{
    while(!g_mainFrame->CheckForTermination(200)){
        if(m_rescan){
            //PostCommandEvent(g_mainFrame,EventID_PopulateFilesList);
            m_rescan = false;
        }
    }

    return NULL;
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
/*         dtrace("Modifier: %d ... KeyCode: %d", \
 *             event.GetModifiers(), event.GetKeyCode());
 */
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
//    long i = GetFirstSelected();
//    if(i != -1){
//        char letter = (char)GetItemText(i)[0];
//        JobsCacheEntry *currentJob = g_mainFrame->m_jobsCache[(int)letter];
//        if(g_mainFrame->m_currentJob != currentJob){
//            g_mainFrame->m_currentJob = currentJob;
//            PostCommandEvent(g_mainFrame,EventID_UpdateStatusBar);
//            PostCommandEvent(g_mainFrame,EventID_FilesAnalyzedUpdateFilesList);
//        }
//    }
//    PostCommandEvent(g_mainFrame,EventID_FilesAnalyzedUpdateFilesList);
    event.Skip();
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
        if(w != m_fcolsw[i])
            changed = true;
    }

    if(changed){
        for(int i = 0; i < LIST_COLUMNS; i++)
            m_fcolsr[i] = (double)m_filesList->GetColumnWidth(i) / (double)cwidth;
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
        int w = m_fcolsw[i] = (int)floor(m_fcolsr[i] * width);
        m_filesList->SetColumnWidth(i, w);
        dtrace("column %d width ....... %d", i, w);
        lastColumnWidth -= w;
    }

    int w = (int)floor(m_fcolsr[LIST_COLUMNS - 1] * width);
    if(w > 0) w = lastColumnWidth;
    m_fcolsw[LIST_COLUMNS - 1] = w;

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
    //bool expand = (height > m_filesListHeight) ? true : false;    //unused variable as of now.
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

//genBTC
/**
 * @brief Gets called everytime a DiskDrive is detected (from FilesPopulateList)
*/
void MainFrame::FilesAnalyzedUpdateFilesList (wxCommandEvent& event)
{
    TraceEnter;
//    #define MAX_UTF8_PATH_LENGTH (256 * 1024)
//    long i = m_vList->GetFirstSelected();
//    wxString letter = m_vList->GetItemText(i);
//    wchar_t charletter;
//    wxStrncpy(&charletter,letter,1);
//    dtrace("DRIVE LETTER %c (wchar_t)",charletter);
//
//    struct prb_traverser trav,t;
//    winx_file_info *file;
//    winx_file_info *fileprb;
//
//    int length;
//    char *cnv_path = NULL;
//    char *utf8_path;
//    dtrace("fileslist.cpp letter was %d",int(charletter));
//    JobsCacheEntry *cacheEntry = m_jobsCache[int(charletter)];
//    if(!cacheEntry){
//        etrace("exiting because no CacheEntry exists");
//        return;
//    }
//
////    dtrace("cache entry's number of files: %u",cacheEntry->pi.files);
////    prb_t_init(&t,cacheEntry->pi.fragmented_files_prb);
////    dtrace("Got to Initialize the Traverser");
////    fileprb = (winx_file_info *)prb_t_first(&t,cacheEntry->pi.fragmented_files_prb);
////    dtrace("Successfully retrieved the file's winx_file_info.");
////    while(fileprb){
////        //file = *fileprb;
////        dtrace("Did a loop.");
////        dtrace("Some other thing: %d",fileprb->creation_time);
////        fileprb = (winx_file_info *)prb_t_next(&t);
////    }
//    dtrace("Got to inside the fileslist.cpp complettion status.");
//    //cacheEntry->pi.fragmented_files_prb = prb_copy(pi->fragmented_files_prb,NULL,NULL,NULL);
//    //dtrace("Copied the PRB inside fileslist.cpp");
//    //pi->isfragfileslist = 1;
//
//    prb_t_init(&trav,cacheEntry->pi.fragmented_files_prb);
//    file = (winx_file_info *)prb_t_first(&trav,cacheEntry->pi.fragmented_files_prb);
//
//    while (file){
//        //jp->pi.fragmented_files[copycount] = file;
////        temparray[copycount] = *file;
//        length = ((int)wcslen((wchar_t *)file->path) + 1) * sizeof(wchar_t) * 2;/* enough to hold UTF-8 string */
//        cnv_path = (char *)winx_tmalloc(length);
//        winx_to_utf8(cnv_path,length,file->path);
//        etrace("File->Path: %s",cnv_path);
//        etrace("File->CTIME: %d",file->creation_time);
//        //memcpy(&jp->pi.fragmented_files[copycount],file,sizeof(winx_file_info));
//        //copycount++;
//        file = (winx_file_info *)prb_t_next(&trav);
//        winx_free(cnv_path);
//    }
}
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
//    char *comment;
//    char *status;
//    udefrag_progress_info pi;
//    wchar_t *path = NULL;
//    char utf8_name[(length + 1) * 4];
//    char filepathname[512];
//    char *utf8_path;
    //utf8_path = winx_tmalloc(MAX_UTF8_PATH_LENGTH);
//    winx_to_utf8(utf8_compname,sizeof(file.path),compname);

    //winx_file_info* ptr = cacheEntry->pi.fragmented_files;
//    if(cacheEntry->pi.isfragfileslist == 1){
//        for (int i=0; i < cacheEntry->pi.fragmented_files_count; i++){
//            dtrace("Entered the for Loop. Run %d",i);
//        //while (ptr){
//            file = cacheEntry->pi.fragmented_files[i];
//            dtrace("Successfully retrieved the file's winx_file_info.");
//            if(file.path != NULL){
//                /* skip \??\ sequence in the beginning of the path */
//                length = (int)wcslen(file.path);
//                if(length > 4){
//                    convert_to_utf8_path(utf8_path,MAX_UTF8_PATH_LENGTH,file.path + 4);
//                } else {
//                    convert_to_utf8_path(utf8_path,MAX_UTF8_PATH_LENGTH,file.path);
//                }
//                dtrace("Successfully did UTF-8 path conversions");
//                //dtrace("The Filename was: %hs",utf8_path);
//                int itemscount = m_filesList->GetItemCount();
//                //m_filesList->InsertItem(itemscount,utf8_path);
//                wxString filepathname = wxString::FromUTF8(cnv_path);
//                dtrace("Trying to print filename before adding item: %hs",filepathname.wc_str());
//                m_filesList->InsertItem(itemscount,filepathname,2);
//                winx_free(utf8_path);
//                return;
//                //dtrace("Successfully added the file to the list");
//            }
//        }
//    }
//}
            //wchar_t name;
            //wxStrncpy(&name,file.name,255);

//            length = ((int)wcslen((wchar_t *)file.path) + 1) * sizeof(wchar_t);
//            length *= 2;     /* enough to hold UTF-8 string */
//            cnv_path = (char *)winx_tmalloc(length);
//            winx_to_utf8(cnv_path,length,file.path);
//
//            dtrace("The cnv_file is: %hs",cnv_path);

//            dtrace("Successfully added the file to the list");

            //winx_dbg_print(0,"%s",filepathname);
            //wxString filepathname;
            //filepathname.Printf(wxT("%ls"),wxString::Format(wxT("%hs"),cnv_path).wc_str());

//            filepathname.Printf(wxT("%-10ls %ls"),
//                wxString::Format(wxT("%c: [%hs]"),
//                'ABC ',cnv_path).wc_str(),
//                file.name);

//        label.Printf(
//             wxT("%-10ls %ls"),    wxString::Format(wxT("%c: [%hs]"),v[i].letter,v[i].fsname).wc_str(),      v[i].label );
//                                                                        char        char                    wchar_t
            //m_filesList->InsertItem(i,label);
            //m_filesList->InsertItem(0,filepathname,0);
            //winx_free(cnv_path);
            //trace(D"%ws",file.name);
            //ptr++;


    //cacheEntry->jobType.ANALYSIS_JOB
    //cacheEntry->pi.completion_status
    //memcpy(&pi,&cacheEntry->pi,sizeof(udefrag_progress_info));

//    //prb_table *fragfileslist = m_currentJob->pi.fragmented_files;
    //prb_table *fragfileslist = cacheEntry->pi.fragmented_files;
//    //struct prb_table *fragfileslist = prb_copy(cacheEntry->pi.fragmented_files,NULL,NULL,NULL);
//    dtrace("Got to the part where it stored the fragfileslist");
//    /* Iterate through PRB_Table, filling the m_filesList as we go*/


//    file = prb_t_first(&t,cacheEntry->pi.fragmented_files);
//    file = prb_t_first(&t,fragfileslist);

//    fileprb = prb_t_first(&t,cacheEntry->pi.fragmented_files_prb);

    //wxString thestring(fileprb->name);
    //dtrace("Printing file->name %d",(fileprb)->name);
    //utf8_path = (char *)winx_tmalloc(MAX_UTF8_PATH_LENGTH);

//    while (fileprb){
        //length = (wcslen(fileprb->path) + 1) * sizeof(wchar_t) * 2;/* enough to hold UTF-8 string */
        //cnv_path = (char *)winx_tmalloc(length);
        //winx_to_utf8(utf8_path,MAX_UTF8_PATH_LENGTH,fileprb->path);
        //dtrace("File->Path: %d",length);
        //winx_free(cnv_path);
//        length = ((int)wcslen((wchar_t *)fileprb->name) + 1) * sizeof(wchar_t) * 2;/* enough to hold UTF-8 string */
//        cnv_path = (char *)winx_tmalloc(length);
//        winx_to_utf8(cnv_path,length,fileprb->name);
//        etrace("File->name: %hs",cnv_path);
//        fileprb = (winx_file_info *)prb_t_next(&t);
//        winx_free(cnv_path);
//    }
//    while (fileprb){
//        //file = (winx_file_info *)prb_t_first(&t,fragfileslist);
//        if (fileprb == NULL){
//            dtrace("FILE WAS NULL!!!! exiting.");
//            return;
//        }
//        else {
//            /* skip \??\ sequence in the beginning of the path */
//            length = (int)wcslen(fileprb->path);
//            if(length > 4){
//                convert_to_utf8_path(utf8_path,MAX_UTF8_PATH_LENGTH,fileprb->path + 4);
//            } else {
//                convert_to_utf8_path(utf8_path,MAX_UTF8_PATH_LENGTH,fileprb->path);
//            }
//            dtrace("Successfully did UTF-8 path conversions");
//            dtrace("The Filename was: %hs",utf8_path);
//            //int itemscount = m_filesList->GetItemCount();
//            //m_filesList->InsertItem(itemscount,utf8_path);
//            //wxString filepathname = wxString::FromUTF8(utf8_path);
//            //dtrace("Trying to print filename before adding item: %hs",filepathname.wc_str());
//            //m_filesList->InsertItem(itemscount,filepathname,2);
//            //winx_free(utf8_path);
//            //return;
//        }
//        fileprb = (winx_file_info *)prb_t_next(&t);
//    }
//}
        //dtrace("Successfully added the file to the list");
//    int prbindex = 0;
//    //wxString thestring(file->name);
//    dtrace("Printing file CTime %d",(file)->name);
//    while(file){



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
//        wchar_t name;
//        wxStrncpy(&name,file->name,255);
//        dtrace("printing out letter wchar_t %c",name);
//
//        m_filesList->InsertItem(0,wxT("Random # File"),0);
//        dtrace("Got after the part where it inserts a random item");
//        m_filesList->SetItem(prbindex,3,name);
//        dtrace("Got after part where it sets a data");
//
//        //file = prb_t_next(&t);
//        file = (winx_file_info *)prb_t_next(&t);
//        prbindex++;
//    }
//    ProcessCommandEvent(EventID_AdjustFilesListHeight);




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


/**
 * @brief Should Only called Once. Deletes the list and re-populates.
*/
void MainFrame::FilesPopulateList(wxCommandEvent& event)
{
    TraceEnter;
    //udefraggui_destroy_lists(g_jpPtr);

    struct prb_traverser trav;
//    int length;
//    char *cnv_path = NULL;
    //char *utf8_path = NULL;
    winx_file_info *file;
    wxString comment, status;
    int loopcount = 0;

    if(!&m_jobsCache){
      etrace("FAILED to obtain currentJob CacheEntry!!");
      return;
    }
    //JobsCacheEntry *newEntry = (JobsCacheEntry *)event.GetClientData();
    char letter = (char)event.GetInt();
    JobsCacheEntry cacheEntry = *m_jobsCache[(int)letter];

    if (cacheEntry.pi.completion_status <= 0){
        etrace("For some odd reason, Completion status was NOT complete.");
        return;
    }
    else {
        m_filesList->DeleteAllItems();

        prb_t_init(&trav,cacheEntry.pi.fragmented_files_prb);
        file = (winx_file_info *)prb_t_first(&trav,cacheEntry.pi.fragmented_files_prb);
        if (!file){
            etrace("Fragmented Files List File Not Found.");
            return;
        }

        while (file){
//            wchar_t *filepath;
//            filepath = file->path;
            //dtrace("Starting loop # %i",loopcount);
            int itemscount = m_filesList->GetItemCount();
            //dtrace("Passed get-itemscount()");
//            length = (wcslen(file->path) + 1) * sizeof(wchar_t) * 2;/* enough to hold UTF-8 string */
//            dtrace("Passed length-size op. # %i",loopcount);
//            cnv_path = (char *)winx_tmalloc(length);
//            if (cnv_path == 0){
//                dtrace("Cnv_path was 0");
//                return;
//            }
//            //memset(cnv_path, 0, length); //Good Practice, Can use buffer[0] = '\0' also.
//            dtrace("Passed memset(malloc) # %i",loopcount);
//            winx_to_utf8(cnv_path,length,file->path);
//            length = ((int)wcslen((wchar_t *)filepath) + 1) * sizeof(wchar_t);
//            length *= 2; /* enough to hold UTF-8 string */
//            etrace("File->Path Length: %i",length);
//            cnv_path = (char *)winx_tmalloc(length);
//            if(cnv_path){
//                /* write message to log in UTF-8 encoding */
//                winx_to_utf8(cnv_path,length,(wchar_t *)filepath);
//                dtrace("Passed WINX UTF8 CONV %i",loopcount);
//            }
//            wxString filepathname = Utils::ConvertChartoWxString(cnv_path);
//            dtrace("Passed wxstring of convert to wxstring of WINX UTF8 CONV %i",loopcount);

//            wxEncodingConverter wxec;
//            bool canconvert = wxec.Init(wxFONTENCODING_ISO8859_2, wxFONTENCODING_ISO8859_2, wxCONVERT_SUBSTITUTE);
//            if (!canconvert){
//                dtrace("COULD NOT CONVERT BECAUSE WX ERROR.");
//                return;
//            }
//            char *buffer = new char[length * 2];
//            memset(buffer, 0, length); //Good Practice, Can use buffer[0] = '\0' also.
//            wxec.Convert(file->path, buffer);
//            wxString temp;
//            temp.Printf(wxT("%hs"),&buffer);
//            delete buffer;
//            wxString test = wxPrintf(L"%s", (wchar_t *)filepath);
            wxString test((const wchar_t *)(file->path + 4));
            m_filesList->InsertItem(itemscount,test,2);
            //m_filesList->InsertItem(itemscount,wxString::FromUTF8(cnv_path,length),2);
            //dtrace("Passed inserting the converted wxstring. %i",loopcount);

            wxString numfragments = wxString::Format(wxT("%d"),file->disp.fragments);
            m_filesList->SetItem(itemscount,1,numfragments);

            int bpc = m_volinfocache.bytes_per_cluster;
            if (bpc > 32768 || bpc < 512){
                etrace("m_volinfocache was not available!");
                return;
            }
            ULONGLONG filesizebytes = file->disp.clusters * bpc;
            char filesize_hr[32];
            winx_bytes_to_hr(filesizebytes,2,filesize_hr,sizeof(filesize_hr));
            wxString filename;
            filename.Printf(wxT("%hs"),filesize_hr);
            m_filesList->SetItem(itemscount,2,filename);

            if(is_directory(file))
                comment = wxT("[DIR]");
            else if(is_compressed(file))
                comment = wxT("[CMP]");
    //        else if(is_over_limit(file))
    //            comment = "[SKIP]";
    //        else if(is_essential_boot_file(file))
    //            comment = "[BOOT]";
    //        else if(is_mft_file(file))
    //            comment = "[MFT]";
            else
                comment = wxT("OK");
            status = wxT("OK");

            m_filesList->SetItem(itemscount,3,comment);
            m_filesList->SetItem(itemscount,4,status);

            //wxString commentstr;
            //commentstr.Printf(wxT("\"%hs\""),comment);
            //wxString statusstr;
            //statusstr.Printf(wxT("\"%hs\""),status);

            loopcount++;
            //winx_free(cnv_path);
            //if (loopcount > 9)
            //    return;
            file = (winx_file_info *)prb_t_next(&trav);
        }
    }
    if (loopcount > 0)
        dtrace("Successfully finished with the Populate List Loop");

    else
        dtrace("While Loop Did not run, no files were added.");
//    m_filesList->Layout();
    if(cacheEntry.pi.fragmented_files_prb){
        dtrace("Deleting cacheEntry.pi.fragmented_files_prb");
        winx_free(cacheEntry.pi.fragmented_files_prb);
        //prb_destroy(cacheEntry.pi.fragmented_files_prb,NULL);
        delete &cacheEntry;
    }


}

//    if(!v) return;
//
//    m_filesList->DeleteAllItems();
//
//    for(int i = 0; v[i].letter; i++){
//        wxString label;
//        label.Printf(wxT("%-10ls %ls"),
//            wxString::Format(wxT("%c: [%hs]"),
//            v[i].letter,v[i].fsname).wc_str(),
//            v[i].label);
//        m_filesList->InsertItem(i,label);
//
//        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED,EventID_FilesAnalyzedUpdateFilesList);
//        volume_info *v_copy = new volume_info;
//        memcpy(v_copy,&v[i],sizeof(volume_info));
//        e.SetInt(i); e.SetClientData((void *)v_copy);
//        ProcessEvent(e);
//
//        e.SetId(EventID_FilesAnalyzedUpdateFilesList);
//        e.SetInt((int)v[i].letter);
//        //ProcessEvent(e);
//    }
//
//    ProcessCommandEvent(EventID_AdjustFilesListColumns);
//
//    m_filesList->Select(0);
//    m_filesList->Focus(0);
//
//    m_currentJob = m_jobsCache[(int)v[0].letter];
//    ProcessCommandEvent(EventID_UpdateStatusBar);
//
//    ::udefrag_release_vollist(v);
//}

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
