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
#include "udefrag-internals_flags.h"
#include "utils_win32.h"
//#include <string>

int f_fixedIcon;
int f_fixedDirtyIcon;
int f_removableIcon;
int f_removableDirtyIcon;

//genBTC fileslist.cpp right click menu
enum
{
    ID_RPOPMENU_DEFRAG_SINGLE_1003 = 1003,
    ID_RPOPMENU_OPEN_EXPLORER_1004 = 1004,
    ID_RPOPMENU_COPY_CLIPBOARD_1005 = 1005,
    ID_RPOPMENU_MOVE_FILE_1006 = 1006,

    ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
};

// =======================================================================
//                           ListView of fragmented files.
// =======================================================================

void MainFrame::InitFilesList()
{
    // save default font used for the list
    m_filesListFont = new wxFont(m_filesList->GetFont());

    // set mono-space font for the list unless Burmese translation is selected
    if(g_locale->GetCanonicalName().Left(2) != wxT("my")){
        wxFont font = m_filesList->GetFont();
        if(font.SetFaceName(wxT("Lucida"))){
            font.SetPointSize(DPI(9));
            m_filesList->SetFont(font);
        }
    }


    // adjust widths so all the columns will fit to the window
    //int width = m_filesList->GetClientSize().GetWidth();
    int borderx = wxSystemSettings::GetMetric(wxSYS_BORDER_X);
    int width = this->GetClientSize().GetWidth() - borderx * 8;
    int lastColumnWidth = width;
    dtrace("INIT - client width ......... %d", width);
    dtrace("INIT - border width ......... %d", borderx);

    int format[] = {
        wxLIST_FORMAT_LEFT, wxLIST_FORMAT_LEFT,
        wxLIST_FORMAT_RIGHT, wxLIST_FORMAT_CENTER,
        wxLIST_FORMAT_CENTER, wxLIST_FORMAT_LEFT
    };

    double ratios[LIST_COLUMNS] = {
            510.0/900, 72.0/900, 78.0/900,
            55.0/900, 55.0/900, 130.0/900
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
    //m_filesList->InsertItem(0,wxT("hi"),0);
    ProcessCommandEvent(EventID_AdjustFilesListHeight);

    Connect(wxEVT_SIZE,wxSizeEventHandler(MainFrame::FilesOnListSize),NULL,this);
//    m_splitter->Connect(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
//        wxSplitterEventHandler(MainFrame::FilesOnSplitChanged),NULL,this);

    m_filesList->InitMembers();

}
/*Ideas:
	//Move to First Free Region (that fits fully)
	//Move to Last Free Region that fits fully)
	//Move to First Free Region(s) in order w/Fragmentation allowed
	//Move to Last Free Region(s) in order w/Fragmentation allowed
 * GUI: Maybe make a 3rd page with a "proposed actions pending" list, where you can queue/order them, and a Commit button
 * Disk: Force a dismount of the volume (by removing the drive letter or etc.) so you can re-order all the files with no chance of unexpected changes.
 * GUI: Make currently worked on sector be highlighted in green, or maybe even two colors (read/write).
 * GUI: Click files and have them display which blocks its occupying in the clustermap (later add support for multiple-selected-files)
 *       (might be hard when the block or cluster map has been already freed up, and only the DC/BMP is left).
 *
 *
 *
*/
void FilesList::InitMembers(){
	WxPopupMenu1 = new wxMenu(wxT(""));
	WxPopupMenu1->Append(ID_RPOPMENU_DEFRAG_SINGLE_1003, wxT("Defragment Now"), wxT(""), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_RPOPMENU_OPEN_EXPLORER_1004, wxT("Open in Explorer"), wxT(""), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_RPOPMENU_COPY_CLIPBOARD_1005, wxT("Copy path to clipboard"), wxT(""), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_RPOPMENU_MOVE_FILE_1006, wxT("Move file from E: to C:"), wxT(""), wxITEM_NORMAL);

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
    EVT_LEFT_DCLICK(FilesList::OnMouseLDClick)
    EVT_RIGHT_DOWN(FilesList::OnMouseRClick)

    EVT_LIST_ITEM_SELECTED(wxID_ANY,FilesList::OnSelectionChange)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY,FilesList::OnSelectionChange)

    EVT_MENU(ID_RPOPMENU_DEFRAG_SINGLE_1003,FilesList::RClickDefragSingleEntry)
    EVT_MENU(ID_RPOPMENU_OPEN_EXPLORER_1004,FilesList::RClickOpenExplorer)
    EVT_MENU(ID_RPOPMENU_COPY_CLIPBOARD_1005,FilesList::RClickCopyClipboard)
    EVT_MENU(ID_RPOPMENU_MOVE_FILE_1006,FilesList::RClickMoveFile)
END_EVENT_TABLE()

wxListItem FilesList::GetListItem()
{
    //Currently gets column 0, of what is currently selected.
    wxListItem theitem;
    theitem.m_itemId = currentlyselected;
    theitem.m_col = 0;
    theitem.m_mask = wxLIST_MASK_TEXT;
    GetItem(theitem);
    return theitem;
}

void FilesList::RClickDefragSingleEntry(wxCommandEvent& event)
{
    //See Code in
    wxListItem theitem = GetListItem();
    wxString itemtext;
    itemtext.Printf(L"\"%s\";",theitem.m_text);
    winx_setenv(L"UD_CUT_FILTER",itemtext.wchar_str());
    dtrace("Cut Filter currently stands at: %ws",winx_getenv(L"UD_CUT_FILTER"));
    g_mainFrame->m_jobThread->singlefile = TRUE;
    ProcessCommandEvent(EventID_Defrag);
    //Job.cpp @ MainFrame::OnJobCompletion @ Line 301-303 handles single-file mode.
    //Job.cpp @ MainFrame::OnJobCompletion @ Line 358-361 handles cleanup.
    //works but needs two considerations:
    //1. The fragmented files list goes away after you finish defragmenting.
    //  This causes you to have to analyze again just to get the list back.
    //2. After you get the list back, defragging another file causes ANOTHER analysis before the defrag.
    //  we have to cut down on the number of these analyses.
}

void FilesList::RClickCopyClipboard(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString itemtext = theitem.m_text;
    //std::string stlstring = std::string(itemtext.mb_str());
    UtilsWin32::toClipboard(std::string(itemtext.mb_str()));
//    if (wxTheClipboard->Open()){
//        // This data objects are held by the clipboard,
//        // so do not delete them in the app.
//        wxTheClipboard->SetData( new wxTextDataObject(itemtext) );
//        wxTheClipboard->Close();
//        // This is stupid because when the app exits,
//        // the clipboard loses its contents...
//    }
//    wxTheClipboard->Flush();  //this is the way to persist data on exit.
}

void FilesList::RClickOpenExplorer(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString itemtext = theitem.m_text;
    //UtilsWin32::BrowseToFile(itemtext.wc_str());  //this has typedef issues. works on VC++, not working on G++
    //Utils::ShellExec(wxT("explorer.exe"),wxT("open"),itemtext); //This OPENS the file itself using the default handler.
    wxString xec;
    xec.Printf(L"/select,\"%s\"",itemtext.wc_str());
//    system(xec.mb_str().data());  // this does work but it quickly flashes open a black command prompt, and looks really sketchy
    ShellExecuteW(0, L"open", L"explorer.exe", xec.wc_str(), 0, SW_NORMAL);
    //this actually works, wish i found this earlier.
}

void FilesList::RClickMoveFile(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString itemtext = theitem.m_text;

    wchar_t *srcfilename = _wcsdup(itemtext.wc_str());
    dtrace("srcfilename was %ws",srcfilename);

    itemtext.Replace(wxT("E:\\"),wxT("C:\\"));
    wchar_t *dstfilename = _wcsdup(itemtext.wc_str());
    dtrace("dstfilename was %ws",dstfilename);

    const wchar_t *dstpath = itemtext.wc_str();
    winx_path_remove_filename((wchar_t *)dstpath);
    dtrace("dst path was %ws",dstpath);

    Utils::createDirectoryRecursively(dstpath);

    CopyFile(srcfilename,dstfilename,1);    //works.
//    winx_free(srcfilename); //since wcsdup calls malloc
//    winx_free(dstfilename); // ^^^
//      calling these crash the program.
}

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

void FilesList::OnMouseLDClick(wxMouseEvent& event)
{
    if(!g_mainFrame->m_busy){
        // left double click starts default action
        if(event.GetEventType() == wxEVT_LEFT_DCLICK)
            PostCommandEvent(g_mainFrame,EventID_DefaultAction);
    }
    event.Skip();
}

void FilesList::OnMouseRClick(wxMouseEvent& event)
{
    if (currentlyselected != -1){
        //right click brings up popup menu. (but only if something is selected.
        if(event.GetEventType() == wxEVT_RIGHT_DOWN)
            this->PopupMenu(WxPopupMenu1);
    }
    event.Skip();
}

void FilesList::OnSelectionChange(wxListEvent& event)
{
//    wxListItem info;
//    info.m_itemId = event.m_itemIndex;
//    info.m_col = 0;
//    info.m_mask = wxLIST_MASK_TEXT;
//    GetItem(info);
//    dtrace("path was %s",info.m_text.mb_str(wxConvUTF8).data());
    currentlyselected =  event.m_itemIndex;
//    if(currentlyselected != -1){
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
    if(width == 0)
        width = m_filesList->GetClientSize().GetWidth();

    //int border = wxSystemSettings::GetMetric(wxSYS_BORDER_X);

    //dtrace("FilesList border width ......... %d", border);
    dtrace("FilesList client width ......... %d", width);

    //int firstwidth = width - border*2;
    int firstwidth = width;
    for(int i = LIST_COLUMNS - 1; i > 0; i--){
        firstwidth -= m_filesList->GetColumnWidth(i);
    }
    m_filesList->SetColumnWidth(0, firstwidth);
    dtrace("column %d width ....... %d", 0, firstwidth);
}

void MainFrame::FilesAdjustListHeight(wxCommandEvent& WXUNUSED(event))
{
    //Commented out, because its not needed.
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
    new_width -= 4 * wxSystemSettings::GetMetric(wxSYS_EDGE_X);
    if(m_filesList->GetCountPerPage() < m_filesList->GetItemCount())
        new_width -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    // scale list columns; avoid horizontal scrollbar appearance
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,EventID_AdjustFilesListColumns);
    evt.SetInt(new_width);
    if(new_width < old_width){
        ProcessEvent(evt);
        //dtrace("Files new_width %d was < %d", new_width,old_width);
    }
    else if(new_width > old_width){
        wxPostEvent(this,evt);
        //dtrace("Files new_width %d was > %d", new_width,old_width);
    }

    event.Skip();
}

/**
 * @brief Deletes the files list and re-populates. Should Only called Once.
*/
void MainFrame::FilesPopulateList(wxCommandEvent& event)
{

    struct prb_traverser trav;
    winx_file_info *file;
    wxString comment, status;
    int currentitem = 0;

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
        }

        while (file){

            wxString test((const wchar_t *)(file->path + 4));
            m_filesList->InsertItem(currentitem,test,2);

            wxString numfragments = wxString::Format(wxT("%d"),file->disp.fragments);
            m_filesList->SetItem(currentitem,1,numfragments);

            int bpc = m_volinfocache.bytes_per_cluster;
//            if (bpc > 32768 || bpc < 512){
//                etrace("m_volinfocache was not available!");
//                return;
//            }
            ULONGLONG filesizebytes = file->disp.clusters * bpc;
            char filesize_hr[32];
            winx_bytes_to_hr(filesizebytes,2,filesize_hr,sizeof(filesize_hr));
            wxString filename;
            filename.Printf(wxT("%hs"),filesize_hr);
            m_filesList->SetItem(currentitem,2,filename);

            if(is_directory(file))
                comment = wxT("[DIR]");
            else if(is_compressed(file))
                comment = wxT("Compressed");
            else if(is_essential_boot_file(file))
                comment = wxT("[BOOT]");
            else if(is_mft_file(file))
                comment = wxT("[MFT]");
            else
                comment = wxT("");
            status = wxT("");
            if(is_locked(file))
                status = wxT("Locked");

            m_filesList->SetItem(currentitem,3,comment);
            m_filesList->SetItem(currentitem,4,status);

            // ULONGLONG time is stored in how many of 100 nanoseconds (0.1 microseconds) or (0.0001 milliseconds) or 0.0000001 seconds.
            //WindowsTickToUnixSeconds() (alternate way.)
            winx_time lmt;             //Last Modified time:
            winx_filetime2winxtime(file->last_modification_time,&lmt);

            char lmtbuffer[30];
            (void)_snprintf(lmtbuffer,sizeof(lmtbuffer),
                "%02i/%02i/%04i"
                " "
                "%02i:%02i:%02i",
                (int)lmt.month,(int)lmt.day,(int)lmt.year,
                (int)lmt.hour,(int)lmt.minute,(int)lmt.second
            );
            lmtbuffer[sizeof(lmtbuffer) - 1] = 0; //terminate witha 0.
            wxString lastmodtime;
            lastmodtime.Printf(wxT("%hs"),lmtbuffer);
            m_filesList->SetItem(currentitem,5,lastmodtime);

            currentitem++;

            file = (winx_file_info *)prb_t_next(&trav);
        }
    }
    if (currentitem > 0)
        dtrace("Successfully finished with the Populate List Loop");
    else
        dtrace("Populate List Loop Did not run, no files were added.");

    PostCommandEvent(this,EventID_AdjustFilesListColumns);
    //signal to the job-thread that the GUI has finished processing files, so it can clear the lists and exit.
    gui_fileslist_finished();   //very important.
}

void MainFrame::FilesAnalyzedUpdateFilesList (wxCommandEvent& event)
{
    //commented out because its not needed.
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
