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
    ID_RPOPMENU_MOVE2FRONT_1007 = 1007,
    ID_RPOPMENU_MOVE2END_1008 = 1008,

    ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
};

// =======================================================================
//                           ListView of fragmented files.
// =======================================================================

void MainFrame::InitFilesList()
{
    m_filesList->currentlyselected = -1;
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
    int borderx = wxSystemSettings::GetMetric(wxSYS_BORDER_X);
    int width = this->GetClientSize().GetWidth() - borderx * 8;
    int lastColumnWidth = width;
    dtrace("INIT - client width ........... %d", width);
    dtrace("INIT - border width ........... %d", borderx);

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
        dtrace("column %d width ......... %d", i, w);
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
    dtrace("column %d width ......... %d", LIST_COLUMNS - 1, w);

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

    Connect(wxEVT_SIZE,wxSizeEventHandler(MainFrame::FilesOnListSize),NULL,this);
//    m_splitter->Connect(wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
//        wxSplitterEventHandler(MainFrame::FilesOnSplitChanged),NULL,this);

    InitPopupMenus();
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
void MainFrame::InitPopupMenus(){
	m_RClickPopupMenu1 = new wxMenu(wxT(""));
	m_RClickPopupMenu1->Append(ID_RPOPMENU_DEFRAG_SINGLE_1003, wxT("Defragment Now"), wxT(""), wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_OPEN_EXPLORER_1004, wxT("Open in Explorer"), wxT(""), wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_COPY_CLIPBOARD_1005, wxT("Copy path to clipboard"), wxT(""), wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_MOVE2FRONT_1007, wxT("Move to Front of Drive"), wxT(""), wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_MOVE2END_1008, wxT("Move to End of Drive"), wxT(""), wxITEM_NORMAL);
}

wxListItem FilesList::GetListItem(int id = NULL,int col = NULL)
{
    //will gets column 0, of what is currently selected.
    //unless parameters for id, col are passed.
    wxListItem theitem;
    theitem.m_itemId = (id!=NULL) ? id : currentlyselected;
    theitem.m_col = (col!=NULL) ? col : 0;
    theitem.m_mask = wxLIST_MASK_TEXT;
    GetItem(theitem);
    return theitem;
}

// =======================================================================
//                            Event Table
// =======================================================================

BEGIN_EVENT_TABLE(FilesList, wxListView)
    EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY,FilesList::OnItemRClick)
    EVT_LIST_ITEM_SELECTED(wxID_ANY,   FilesList::OnSelect)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY, FilesList::OnDeSelect)
    EVT_MENU(ID_RPOPMENU_DEFRAG_SINGLE_1003, FilesList::RClickDefragSingleEntry)
    EVT_MENU(ID_RPOPMENU_OPEN_EXPLORER_1004, FilesList::RClickOpenExplorer)
    EVT_MENU(ID_RPOPMENU_COPY_CLIPBOARD_1005,FilesList::RClickCopyClipboard)
    EVT_MENU(ID_RPOPMENU_MOVE2FRONT_1007, FilesList::RClickMoveToFirstFreeRegion)
    EVT_MENU(ID_RPOPMENU_MOVE2END_1008, FilesList::RClickMoveToLastFreeRegion)
    EVT_MENU_RANGE(2065,2090,FilesList::RClickSubMenuMoveFiletoDriveX)
END_EVENT_TABLE()
//events 2065-2090 are signifying drive A-Z (their letter's char2int)
//calls function below to move the file to the corresponding drive's eventID.
// =======================================================================
//              Event Handlers   &   Right Click Popup Menu Handlers
// =======================================================================
void FilesList::RClickSubMenuMoveFiletoDriveX(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString itemtext = theitem.m_text;

    wchar_t letter = (wchar_t)(event.GetId() - 2000);
    wchar_t *srcfilename = _wcsdup(itemtext.wc_str());
    wchar_t *dstfilename = _wcsdup(itemtext.wc_str());

    dstfilename[0] = letter;

    wchar_t *dstpath = _wcsdup(dstfilename);
    winx_path_remove_filename(dstpath);

    Utils::createDirectoryRecursively(dstpath);
    CopyFile(srcfilename,dstfilename,1);
//    dtrace("srcfilename was %ws",srcfilename);
//    dtrace("dstfilename was %ws",dstfilename);
//    dtrace("dst path was %ws",dstpath);
    delete srcfilename;    delete dstfilename;    delete dstpath;
}

void FilesList::RClickMoveToFirstFreeRegion(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString filtertext;
    filtertext << L"\"" << theitem.m_text << L"\";";
    wxSetEnv(L"UD_CUT_FILTER",filtertext);
    g_mainFrame->m_jobThread->singlefile = TRUE;
    ProcessCommandEvent(ID_MoveToFront);
}
void FilesList::RClickMoveToLastFreeRegion(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString filtertext;
    filtertext << L"\"" << theitem.m_text << L"\";";
    wxSetEnv(L"UD_CUT_FILTER",filtertext);
    g_mainFrame->m_jobThread->singlefile = TRUE;
    ProcessCommandEvent(ID_MoveToEnd);
}
//Should really combine all these into one and check for the event.GetID() to call the right CommandEvent
void FilesList::RClickDefragSingleEntry(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString filtertext;
    filtertext << L"\"" << theitem.m_text << L"\";";
    wxSetEnv(L"UD_CUT_FILTER",filtertext);
    g_mainFrame->m_jobThread->singlefile = TRUE;
    ProcessCommandEvent(ID_Defrag); //calls the defrag routine.
    //Job.cpp @ MainFrame::OnJobCompletion @ Line 301-303 handles single-file mode.
    //Job.cpp @ MainFrame::OnJobCompletion @ Line 358-361 handles cleanup.
    //works but needs two considerations:
    //x1x. The fragmented files list goes away after you finish defragmenting.
    //  This causes you to have to analyze again just to get the list back.
    //x2x. After you get the list back, defragging another file causes ANOTHER analysis before the defrag.
    //  we have to cut down on the number of these analyses.
    //OK I Cut down on the analyses but now the issue is:
    // 1. The defragmented file does not get removed from the list after the defrag job.
    // 2. Every single file defrag still does a full analyze-pass.(udefrag-internals fault)
    // 3. Does not work if you selected another Drive on page 1.
    //    (can be easily fixed by pre-selecting in this function before we call defrag).
}

void FilesList::RClickCopyClipboard(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString itemtext = theitem.m_text;
    //std::string stlstring = std::string(itemtext.mb_str());
    //UtilsWin32::toClipboard(std::string(itemtext.mb_str()));  //other way.
    if (wxTheClipboard->Open()){
        wxTheClipboard->SetData( new wxTextDataObject(itemtext) );
        wxTheClipboard->Close();
        wxTheClipboard->Flush();  //this is the way to persist data on exit.
    }
}

void FilesList::RClickOpenExplorer(wxCommandEvent& event)
{
    wxListItem theitem = GetListItem();
    wxString itemtext = theitem.m_text;
    //UtilsWin32::BrowseToFile(itemtext.wc_str());  //this has typedef issues. works on VC++, not working on G++
    //Utils::ShellExec(wxT("explorer.exe"),wxT("open"),itemtext); //This OPENS the file itself using the default handler.
    wxString xec;
    xec << L"/select,\"" << itemtext.wc_str() << L"\"";
    //xec.Printf(L"/select,\"%s\"",itemtext.wc_str());
//    system(xec.mb_str().data());  // this does work but it quickly flashes open a black command prompt, and looks really sketchy
    ShellExecuteW(0, L"open", L"explorer.exe", xec.wc_str(), 0, SW_NORMAL);
    //this actually works, wish i found this earlier.
}

void FilesList::OnItemRClick(wxListEvent& event)
{
    if (currentlyselected != -1)
        this->PopupMenu(g_mainFrame->m_RClickPopupMenu1);
    event.Skip();
}

void FilesList::OnSelect(wxListEvent& event)
{
    currentlyselected = event.m_itemIndex;
    event.Skip();
}

void FilesList::OnDeSelect(wxListEvent& event)
{
    currentlyselected = -1;
    event.Skip();
}

void MainFrame::FilesAdjustListColumns(wxCommandEvent& event)
{
    int width = event.GetInt();
    if(width == 0)
        width = m_filesList->GetClientSize().GetWidth();

    dtrace("client width ............ %d", width);

    for(int i = LIST_COLUMNS - 1; i > 0; i--){
        width -= m_filesList->GetColumnWidth(i);
    }

    m_filesList->SetColumnWidth(0, width);
    dtrace("column %d width .......... %d", 0, width);
}

void MainFrame::FilesOnListSize(wxSizeEvent& event)
{
    int old_width = m_filesList->GetClientSize().GetWidth();
    int new_width = this->GetClientSize().GetWidth();
    new_width -= 4 * wxSystemSettings::GetMetric(wxSYS_EDGE_X);
    if(m_filesList->GetCountPerPage() < m_filesList->GetItemCount())
        new_width -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    // scale list columns; avoid horizontal scrollbar appearance
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED,ID_AdjustFilesListColumns);
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

        prb_t_init(&trav,cacheEntry.pi.fragmented_files_prb);
        file = (winx_file_info *)prb_t_first(&trav,cacheEntry.pi.fragmented_files_prb);
        if (!file){
            etrace("Fragmented Files List File Not Found.");
        }
        else{
            m_filesList->DeleteAllItems();
        }

        while (file){

            wxString test((const wchar_t *)(file->path + 4)); /* skip the 4 chars: \??\  */
            m_filesList->InsertItem(currentitem,test,2);

            wxString numfragments = wxString::Format(wxT("%d"),file->disp.fragments);
            m_filesList->SetItem(currentitem,1,numfragments);

            int bpc = m_volinfocache.bytes_per_cluster;
            ULONGLONG filesizebytes = file->disp.clusters * bpc;
            char filesize_hr[32];
            winx_bytes_to_hr(filesizebytes,2,filesize_hr,sizeof(filesize_hr));
//            wxString filename;
//            filename.Printf(wxT("%hs"),filesize_hr);
            wxString filesize = wxString::FromUTF8(filesize_hr);
            m_filesList->SetItem(currentitem,2,filesize);

            if(is_directory(file))
                comment = wxT("[DIR]");
            else if(is_compressed(file))
                comment = wxT("Compressed");
            else if(is_essential_boot_file(file)) //needed a flag
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
            //WindowsTickToUnixSeconds() (alternate way. unused.)
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
            lmtbuffer[sizeof(lmtbuffer) - 1] = 0; //terminate with a 0.
            wxString lastmodtime;
            lastmodtime.Printf(wxT("%hs"),lmtbuffer);
            m_filesList->SetItem(currentitem,5,lastmodtime);

            currentitem++;

            file = (winx_file_info *)prb_t_next(&trav);
        }
    }
    if (currentitem > 0){
        dtrace("Successfully finished with the Populate List Loop");
        PostCommandEvent(this,ID_AdjustFilesListColumns);
    }
    else
        dtrace("Populate List Loop Did not run, no files were added.");

    //signal to the INTERNALS native job-thread that the GUI has finished
    //  processing files, so it can clear the lists and exit.
    gui_fileslist_finished();   //very important cleanup.
    delete file;
}

/** @} */
