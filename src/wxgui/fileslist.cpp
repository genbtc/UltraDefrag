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
// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.
// This file only was totally created by genBTC <genBTC@gmx.com>
/**
 * @file fileslist.cpp
 * @brief List of files.
 * @addtogroup FilesList
 * @{
 * @todo
 *
 *DONE:	//Move to First Free Region (that fits fully) (move to first opening)
 *DONE:	//Move to  Last Free Region (that fits fully)  ""
	//Move to First Free Region(s) in order w/Fragmentation allowed
	//Move to Last Free Region(s) in order w/Fragmentation allowed
//Free up space at beginning of drive to move single file(s) into it. (Extended Part 2: make a list of files (aka prefetch concept))
*Come up with a way to display a listbox with a list of all the free regions, and select from a list of which region to move/defrag the file into.
 * GUI: Maybe make a 3rd page with a "proposed actions pending" list, where you can queue/order them, and a Commit button
 * Disk: Force a dismount of the volume (by removing the drive letter or etc.) so you can re-order all the files with no chance of unexpected changes.
 * GUI: Make currently worked on sector be highlighted in green, or maybe even two colors (read/write).
 * GUI: Click files and have them display which blocks its occupying in the clustermap (later add support for multiple-selected-files)
 *       (might be hard when the block or cluster map has been already freed up, and only the DC/BMP is left).
 * Query a specific filename and report cluster locations and percent on the drive.
 *DONE* GUI: Made Multi-Selection possible. 
 *DONE* GUI: Made Sorting Possible.
 *DONE* GUI: Changed ListCtrl to Virtual mode. Sorting is way faster.
 *
 */
/**=========================================================================**
***                        Declarations                                     **
***=========================================================================**/
#include "wx/wxprec.h"
#include "main.h"
#include "udefrag-internals_flags.h"
#include <algorithm>

//genBTC. Event ID's for fileslist.cpp right click popup menu
enum
{
    ID_RPOPMENU_OPEN_EXPLORER_1004 = 1004,
    ID_RPOPMENU_COPY_CLIPBOARD_1005 = 1005,
    ID_RPOPMENU_DEFRAG_SINGLE_1006 = 1006,
    ID_RPOPMENU_DEFRAG_MOVE2FRONT_1007 = 1007,
    ID_RPOPMENU_DEFRAG_MOVE2END_1008 = 1008
};
/**=========================================================================**
***                           ListView Sorting.                             **
***=========================================================================**/

bool sortcol0 (FilesListItem i,FilesListItem j) {
    //sorts Ascending by default
    int cmp = i.col0.Cmp(j.col0);
    if(cmp < 0)
        return true;
    else
        return false;
}
bool sortcol1 (FilesListItem i,FilesListItem j) {
    //sorts DE-scending by default
    long l1,l2;
    i.col1.ToLong(&l1);
    j.col1.ToLong(&l2);
    if(l1 < l2)
        return false;
    else
        return true;   
}
bool sortcol2 (FilesListItem i,FilesListItem j) {
    //sorts DE-scending by default
    if(i.col2bytes < j.col2bytes)
        return false;
    else
        return true;   
}
bool sortcol5 (FilesListItem i,FilesListItem j) { 
    //sorts DE-scending by default
    wxDateTime ldDate1, ldDate2;
    bool lbRetVal = true;
    ldDate1.ParseFormat( i.col5, "%m/%d/%Y %H:%M:%S" );
    ldDate2.ParseFormat( j.col5, "%m/%d/%Y %H:%M:%S" );
    if ( ldDate1.IsEarlierThan( ldDate2 ) )
        lbRetVal = false;
    return lbRetVal;
}
void FilesList::SortVirtualItems(int Column)
{
    this->Freeze();
    if(Column == m_sortinfo.Column)
    {
        // user clicked same column as last time, reverse the sorting order
        std::reverse(allitems.begin(),allitems.end());
    }
    else
    {
        // user clicked new column, sort normal.
        if (Column == 0){
            std::stable_sort (allitems.begin(), allitems.end(), sortcol0);
        }
        else if ( Column == 1) {
            std::stable_sort (allitems.begin(), allitems.end(), sortcol1);
        }
        else if ( Column == 2){
            std::stable_sort (allitems.begin(), allitems.end(), sortcol2);
        }
        else if ( Column == 5 ){
            std::stable_sort (allitems.begin(), allitems.end(), sortcol5);
        }
    }
    this->Thaw();
    m_sortinfo.Column = Column; // set the last-clicked column 
//    for (std::vector<FilesListItem>::iterator it=allitems.begin(); it!=allitems.end(); ++it){
}

void FilesList::OnColClick(wxListEvent& event)
{
    int col = event.GetColumn();
    if (col == 3 || col == 4)
        return;
    SortVirtualItems(col);
}

/**=========================================================================**
***            Instantiate ListView of fragmented files.                    **
***=========================================================================**/

void MainFrame::InitFilesList()
{
    // save default font used for the list
    m_filesListFont = new wxFont(m_filesList->GetFont());

    // set mono-space font for the list unless Burmese translation is selected
    if(g_locale->GetCanonicalName().Left(2) != "my"){
        wxFont font = m_filesList->GetFont();
        if(font.SetFaceName("Lucida")){
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

    // ensure that the list will cover integral number of items
    m_filesListHeight = 0xFFFFFFFF; // prevent expansion of the list

    Connect(wxEVT_SIZE,wxSizeEventHandler(MainFrame::FilesOnListSize), nullptr,this);

    InitPopupMenus();
    ListSortInfo *m_sortinfo = new ListSortInfo();
    m_filesList->m_sortinfo = *m_sortinfo;
}

/**
 * @brief Instantiate the right click popup context menu for FilesList
*/
void MainFrame::InitPopupMenus()
{
	m_RClickPopupMenu1 = new wxMenu("");
	m_RClickPopupMenu1->Append(ID_RPOPMENU_OPEN_EXPLORER_1004, "Open in Explorer", "", wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_COPY_CLIPBOARD_1005, "Copy path to clipboard", "", wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_DEFRAG_SINGLE_1006, "Defragment Now", "", wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_DEFRAG_MOVE2FRONT_1007, "Move to Front of Drive", "", wxITEM_NORMAL);
	m_RClickPopupMenu1->Append(ID_RPOPMENU_DEFRAG_MOVE2END_1008, "Move to End of Drive", "", wxITEM_NORMAL);
	//Last Item is "Move File to Drive *: ", created in vollist.cpp because it needs the list of drives.
}

/**
 * @brief Retrieve a List Item from FilesList
 * @details Will get currently selected item, column 0.
   Unless parameters for id, col are passed.
*/
wxListItem FilesList::GetListItem(long index,long col)
{
    wxListItem theitem;
    theitem.m_itemId = index!=-1 ? index : currentlyselected;
    theitem.m_col = col!=-1 ? col : 0;
    theitem.m_mask = wxLIST_MASK_TEXT;
    GetItem(theitem);
    return theitem;
}

void FilesList::ReSelectProperDrive(wxCommandEvent& event)
{
    ProcessCommandEvent(this,ID_SelectProperDrive);
}
void MainFrame::ReSelectProperDrive(wxCommandEvent& event)
{
    wxString itemtext = m_filesList->GetListItem().GetText();
    char letter;
    letter = (char)itemtext[0]; //find the drive-letter of the fragmented files tab.
    //DeSelect All Drives
    int n = m_vList->GetItemCount();
    for (int i = 0; i < n; i++)
        m_vList->SetItemState(i,0,wxLIST_STATE_SELECTED);
    //Select the Proper Drive. (to match the fragmented files list tab)
    for (int i = 0; i < n; i++){
        if(letter == (char)m_vList->GetItemText(i)[0]){
            m_vList->Select(i);  m_vList->Focus(i);
            break;
        }
    }
}

/**=========================================================================**
***                         Event Table                                     **
***=========================================================================**/

BEGIN_EVENT_TABLE(FilesList, wxListCtrl)
    EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY,FilesList::OnItemRClick)
    EVT_LIST_ITEM_SELECTED(wxID_ANY,   FilesList::OnSelect)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY, FilesList::OnDeSelect)
    EVT_MENU(ID_RPOPMENU_OPEN_EXPLORER_1004, FilesList::RClickOpenExplorer)
    EVT_MENU(ID_RPOPMENU_COPY_CLIPBOARD_1005,FilesList::RClickCopyClipboard)
    EVT_LIST_COL_CLICK(wxID_ANY, FilesList::OnColClick)
    EVT_MENU_RANGE(1006,1008, FilesList::RClickDefragMoveSingle)
    EVT_MENU_RANGE(2065,2090,FilesList::RClickSubMenuMoveFiletoDriveX)
END_EVENT_TABLE()
//events 2065-2090 are signifying drive A-Z (their letter's char2int)
//calls the function below to move the file to the corresponding drive's eventID.
/**=========================================================================**
***            Event Handlers  &  Right Click Popup Menu Handlers           **
***=========================================================================**/

void FilesList::RClickSubMenuMoveFiletoDriveX(wxCommandEvent& event)
{
	wxString itemtext = GetListItem().GetText();

	const wchar_t letter = wchar_t(event.GetId() - 2000);
	wchar_t *srcfilename = _wcsdup(itemtext.wc_str());
	wchar_t *dstfilename = _wcsdup(itemtext.wc_str());

	dstfilename[0] = letter;

	wchar_t *dstpath = _wcsdup(dstfilename);
	winx_path_remove_filename(dstpath);

	Utils::createDirectoryRecursively(dstpath);
	MoveFile(srcfilename, dstfilename);
	//    dtrace("srcfilename was %ws",srcfilename);
	//    dtrace("dstfilename was %ws",dstfilename);
	//    dtrace("dst path was %ws",dstpath);
	delete srcfilename;    delete dstfilename;    delete dstpath;
}

void FilesList::RClickDefragMoveSingle(wxCommandEvent& event)
{
    wxString filtertext;
    currently_being_workedon_filenames = new wxArrayString;
    ProcessCommandEvent(this,ID_SelectProperDrive);
    long i = GetFirstSelected();
    while(i != -1){
        wxString selitem = GetItemText(i);
        //do not exceed max environment variable length.
        if(filtertext.Length() + selitem.Length() + 3 > 32767) break;
        currently_being_workedon_filenames->Add(selitem);
        Utils::extendfiltertext(selitem,&filtertext);
        i = GetNextSelected(i);
    }
    wxSetEnv("UD_CUT_FILTER",filtertext);
    g_mainFrame->m_jobThread->singlefile = true;
    //Job.cpp @ MainFrame::OnJobCompletion @ Line 301-303 handles single-file mode.
    //Job.cpp @ MainFrame::OnJobCompletion @ Line 358-361 handles cleanup.
    //FilesList.cpp @ MainFrame::FilesPopulateList @ Line 370-380 handles list-item-removal.
    //works but needs a consideration:
    // 1. Every single file defrag still does a full analyze-pass.(udefrag-internals fault)
    switch(event.GetId()){
    case ID_RPOPMENU_DEFRAG_SINGLE_1006:
        ProcessCommandEvent(this,ID_Defrag);
        break;
    case ID_RPOPMENU_DEFRAG_MOVE2FRONT_1007:
        ProcessCommandEvent(this,ID_MoveToFront);
        break;
    case ID_RPOPMENU_DEFRAG_MOVE2END_1008:
        ProcessCommandEvent(this, ID_MoveToEnd);
        break;    
    default:
        break;
    }
}

void FilesList::RClickCopyClipboard(wxCommandEvent& event)
{
    wxString itemtext = GetListItem().GetText();
    if (wxTheClipboard->Open()){
        wxTheClipboard->SetData( new wxTextDataObject(itemtext) );
        wxTheClipboard->Close();
        wxTheClipboard->Flush();  //this is the way to persist data on exit.
    }
}

void FilesList::RClickOpenExplorer(wxCommandEvent& event)
{
    wxString itemtext = GetListItem().GetText();
    wxString xec;
    xec << "/select,\"" << itemtext << "\"";
    Utils::ShellExec("explorer.exe","open",xec); //This OPENS the file itself using the default handler.    
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
 * @brief Deletes the files list and re-populates.
*/
void MainFrame::FilesPopulateList(wxCommandEvent& event)
{
    struct prb_traverser trav;
    winx_file_info *file;
    int currentitem = 0;
    bool something_removed = false;
	/*
    if(!&m_jobsCache){
      etrace("FAILED to obtain currentJob CacheEntry!!");
      return;
    }*/
    //JobsCacheEntry *newEntry = (JobsCacheEntry *)event.GetClientData();
    char letter = (char)event.GetInt();
    JobsCacheEntry cacheEntry = *m_jobsCache[(int)letter];

    if (cacheEntry.pi.completion_status <= 0){
        etrace("For some odd reason, Completion status was NOT complete.");
        return;
    }
	prb_t_init(&trav,cacheEntry.pi.fragmented_files_prb);
	file = (winx_file_info *)prb_t_first(&trav,cacheEntry.pi.fragmented_files_prb);
	if (!file){
		//code here testsfor/finds/removes any files that were just defragmented.
		if (m_jobThread->singlefile == TRUE){
			int c = m_filesList->GetItemCount();
			int d = m_filesList->currently_being_workedon_filenames->Count();
			for (int i=0; i<c; i++){
				for (int j=0; j<d; j++){
					if((*m_filesList->currently_being_workedon_filenames)[j] == m_filesList->GetItemText(i)){
						m_filesList->allitems.erase(m_filesList->allitems.begin()+i);
						m_filesList->Select(i,false);   //de-select removed item.
						c--;
						m_filesList->currently_being_workedon_filenames->RemoveAt(j);
						d--; j--;
						something_removed = true;
					}
				}
			}
		}
		else
			etrace("Fragmented Files List Not Found.");
	}
	else{
		m_filesList->allitems.clear();  //clear entire list.
	}
	while (file){
		FilesListItem item;
            
		item.col0 = (const wchar_t *)(file->path + 4); /* skip the 4 chars: \??\  */

		item.col1 << file->disp.fragments;

		const int bpc = m_volinfocache.bytes_per_cluster;
		item.col2bytes = file->disp.clusters * bpc;
		char filesize_hr[32];
		winx_bytes_to_hr(item.col2bytes,2,filesize_hr,sizeof filesize_hr);
		item.col2 = wxString::FromUTF8(filesize_hr);

		if(is_directory(file))
			item.col3 = "[DIR]";
		else if(is_compressed(file))
			item.col3 = "Compressed";
		else if(is_essential_boot_file(file)) //needed a flag from udefrag-internals.h (should manually #define it and only it)
			item.col3 = "[BOOT]";
		else if(is_mft_file(file))
			item.col3 = "[MFT]";
		else
			item.col3 = "";
		if(is_locked(file))
			item.col4 = "Locked";
		else
			item.col4 = "";

		// ULONGLONG time is stored in how many of 100 nanoseconds (0.1 microseconds) or (0.0001 milliseconds) or 0.0000001 seconds.
		//WindowsTickToUnixSeconds() (alternate way. unused.)
		winx_time lmt;             //Last Modified time:
		winx_filetime2winxtime(file->last_modification_time,&lmt);

		char lmtbuffer[30];
		(void)_snprintf(lmtbuffer,sizeof lmtbuffer,
		                "%02i/%02i/%04i"
		                " "
		                "%02i:%02i:%02i",
		                (int)lmt.month,(int)lmt.day,(int)lmt.year,
		                (int)lmt.hour,(int)lmt.minute,(int)lmt.second
		);
		lmtbuffer[sizeof lmtbuffer - 1] = 0; //terminate with a 0.
		item.col5 = wxString::Format("%hs",lmtbuffer);

		m_filesList->allitems.push_back(item);  //store item in virtual list's container.
		currentitem++;

		file = (winx_file_info *)prb_t_next(&trav);
	}
	if (currentitem > 0){
        dtrace("Successfully finished with the Populate List Loop");
        m_filesList->SetItemCount(m_filesList->allitems.size());   //set new virtual-list size.        
        ProcessCommandEvent(this,ID_AdjustFilesListColumns);
    }
    else if (!something_removed)
        dtrace("Populate List Loop Did not run, no files were added.");
    else{
        dtrace("Fragmented Files List updated - item(s) removed.");
        m_filesList->SetItemCount(m_filesList->allitems.size());   //set new virtual-list size.
        m_filesList->Refresh();
        m_filesList->Focus(0);
    }

    //signal to the INTERNALS native job-thread that the GUI has finished
    //  processing files, so it can clear the lists and exit.
    gui_fileslist_finished();   //very important cleanup.
    delete file;
}

wxString FilesList::OnGetItemText(long item, long column) const
{
    wxCHECK_MSG(item >= 0 && item < (long)allitems.size(),
        "", "Invalid item index in FilesList::OnGetItemText");
    
    switch (column) {
        case 0:
            return allitems[item].col0;
        case 1:
            return allitems[item].col1;
        case 2:
            return allitems[item].col2;
        case 3:
            return allitems[item].col3;
        case 4:
            return allitems[item].col4;
        case 5:
            return allitems[item].col5;
        default:
            wxFAIL_MSG("Invalid column index in FilesList::OnGetItemText");
    }
    return "";    
}
/** @} */
