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
 * @file query.cpp
 * @brief Query the internals and provide information.
 * @addtogroup Query
 * @note query.c in udefrag.dll is the backend.
 * @{
 */
/**=========================================================================**
***                        Declarations                                     **
***=========================================================================**/
#include "wx/wxprec.h"
#include "main.h"
#include "udefrag-internals_flags.h"
//#include "../dll/udefrag/query.h" //definitions are defined in udefrag.dll's .h's
// The best thing would be to have the query functions return the original structs, and then have the GUI deal with it.
// We could make the udefrag-internals struct's available to the GUI. (such as blockmap, regions) 

/**=========================================================================**
***                        Create GUI                                       **
***=========================================================================**/
void MainFrame::InitQueryMenu()
{
    m_queryThread = new QueryThread();
    
     //create Query tab, Tab3.
	m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4	= new wxBoxSizer( wxVERTICAL );
	Analyze = new wxButton(m_panel3, ID_ANALYZE, _("Analyze"), wxDefaultPosition, wxDefaultSize, 0, 
                        wxDefaultValidator, _("Analyze"));

	wxArrayString arrayStringFor_WxComboBox1;
	WxComboBox1 = new wxComboBox(m_panel3, ID_WXCOMBOBOX1, _("WxComboBox1"), wxDefaultPosition, wxDefaultSize, 
                              arrayStringFor_WxComboBox1,wxTE_READONLY, wxDefaultValidator, _("WxComboBox1"));

	WxStaticText1 = new wxStaticText(m_panel3, ID_WXSTATICTEXT1, _("WxStaticText1"), wxDefaultPosition, 
                                  wxSize(wxDefaultCoord,100), 0, _("WxStaticText1"));
	
    WxFilePickerCtrl1 = new wxFilePickerCtrl(m_panel3, ID_WXFILEPICKERCTRL1, wxEmptyString, wxFileSelectorPromptStr, 
                                             wxFileSelectorDefaultWildcardStr, wxDefaultPosition, wxDefaultSize);
  
	WxTextCtrl1 = new wxTextCtrl(m_panel3, ID_WXTEXTCTRL1, _(""), wxDefaultPosition,wxSize(wxDefaultCoord,400), 
                              wxVSCROLL | wxTE_READONLY | wxTE_MULTILINE, wxDefaultValidator, _("WxTextCtrl1"));
	WxTextCtrl1->SetMaxLength(0);
	WxTextCtrl1->SetFocus();
	WxTextCtrl1->SetInsertionPointEnd();

	PerformQuery = new wxButton(m_panel3, ID_PERFORMQUERY, _("Perform Query!"), wxDefaultPosition, wxDefaultSize, 
                             0, wxDefaultValidator, _("PerformQuery"));
	
	bSizer4->Add( Analyze, 0, wxEXPAND | wxALL, 5);
	bSizer4->Add( WxComboBox1, 0, wxEXPAND | wxALL, 5);
	bSizer4->Add( WxStaticText1, 0, wxEXPAND | wxALL, 5);
	bSizer4->Add( WxFilePickerCtrl1, 0, wxEXPAND | wxALL, 5);
	bSizer4->Add( WxTextCtrl1, 0, wxEXPAND | wxALL, 5);
	bSizer4->Add( PerformQuery, 0, wxEXPAND | wxALL, 5);
	
	m_panel3->SetSizer( bSizer4 );
    bSizer4->Fit( m_panel3 );
    m_notebook1->AddPage( m_panel3, "Query", false );   
}

/**=========================================================================**
***                  Functions in the Query Menu/Tab.                       **
***=========================================================================**/
//right now this is equivalent to Job.cpp @ void MainFrame::OnStartJob(wxCommandEvent& event)
void MainFrame::QueryClusters(wxCommandEvent& event){
    wxString filtertext,itemtext;
    
    int id = event.GetId();
    m_queryThread->m_qp = new udefrag_query_parameters();
    m_queryThread->m_qp->engineFinished = FALSE;
    m_queryThread->m_qp->startGUI = TRUE;
    WxTextCtrl1->Clear();
    
//Launched from Menu:
    if (id == ID_QueryClusters){
        itemtext = m_filesList->GetListItem().GetText();
        WxFilePickerCtrl1->SetPath(itemtext);
        
        ProcessCommandEvent(ID_SelectProperDrive);
        long i = m_filesList->GetFirstSelected();
        while(i != -1){
            wxString selitem = m_filesList->GetItemText(i);
            Utils::extendfiltertext(selitem,&filtertext);
            i = m_filesList->GetNextSelected(i);
        }
    }
//Launched from Tab
    else if (id == ID_PERFORMQUERY){
        itemtext = WxFilePickerCtrl1->GetPath();
        Utils::extendfiltertext(itemtext,&filtertext);
    }
   
    //set the Analysis Mode to SINGLE file mode.
    // This probably can't work for all queries, but is fast.
    wxSetEnv(L"UD_CUT_FILTER",filtertext);
    m_queryThread->singlefile = true;
    m_queryThread->m_flags |= UD_JOB_CONTEXT_MENU_HANDLER;

    m_queryThread->m_querypath = (wchar_t *)itemtext.fn_str();
    m_queryThread->m_qp->path = m_queryThread->m_querypath;
    m_queryThread->m_letter = (char)itemtext[0]; //find the drive-letter of the file
    
    m_queryThread->m_mapSize = GetMapSize();
    
    switch(id){
    case ID_QueryClusters:
    case ID_PERFORMQUERY:
        m_queryThread->m_qType = QUERY_GET_VCNLIST;
        break;
    case ID_QueryClusters+1:
        m_queryThread->m_qType = QUERY_GET_FREE_REGIONS;
        break;    
    default:
        break;
    }
    m_busy = true; m_paused = false; m_stopped = false;
    m_queryThread->m_startquery = true; //BEGIN LAUNCH.
    
}

/**=========================================================================**
***                  Dedicated Query Thread.                                **
***=========================================================================**/
//better off with a struct to pass any variables we need IN. Such as:
// drive_letter. query_type. File Path.  (combine/variables set above)
void* QueryThread::Entry()
{
    int result;

    while(!g_mainFrame->CheckForTermination(200)){
        if(m_startquery){
            result = udefrag_starts_query(m_letter,m_qType,m_flags,m_mapSize,
                reinterpret_cast<udefrag_query_progress_callback>(PostProgressCallback),
                reinterpret_cast<udefrag_terminator>(Terminator),*m_qp, nullptr
            );
            if(result < 0 && !g_mainFrame->m_stopped){
                etrace("Disk Processing Failure.");
                g_mainFrame->WxTextCtrl1->AppendText(L"Error executing Query.");
            }
            PostCommandEvent(g_mainFrame,ID_QueryCompletion);
            m_startquery = false;
        }
    }
    return nullptr;
}

//Copied from Job.cpp function of same-name
void QueryThread::PostProgressCallback(udefrag_query_parameters *qp, void *p)
{    
    if (qp->startGUI) {
        dtrace("Begin Populating The Query Tab Here!:");
        g_mainFrame->m_queryThread->DisplayQueryResults(qp);
    }
    else
        dtrace("Query CallBack, Did Nothing...waaiting.");
}

/**
 * \brief Show the results of the query: (Fragments, Clusters, VCNs) in the Query Tab Textboxes.
 * \param m_qp make Param dont overrides the querythreads external m_qp var for now.
 */
void QueryThread::DisplayQueryResults(udefrag_query_parameters *qp)
{
    ULONGLONG i;
    winx_blockmap *block;
    memcpy(m_qp,qp,sizeof(udefrag_query_parameters));
    //store the values again
    //qp.path = jp->qp.path;
    //qp.filedisp = jp->qp.filedisp;
    dtrace("The File's path is: %ws",m_qp->path);
    wxString line;
    dtrace("The File has #clusters: %d",m_qp->filedisp.clusters);
    dtrace("The File has #fragments: %d",m_qp->filedisp.fragments);
    line.Printf(_("Fragments: %I64u , Length(clusters): %I64u \n"),m_qp->filedisp.fragments, m_qp->filedisp.clusters);
    g_mainFrame->WxTextCtrl1->Freeze(); 
    g_mainFrame->WxTextCtrl1->AppendText(line.c_str());
    for(block = m_qp->filedisp.blockmap, i = 0; block; block = block->next, i++){
        itrace("file part #%I64u start: %I64u, length: %I64u\n",i,block->lcn,block->length);
        line.Printf(_("Fragment #%I64u starts @ %I64u, length: %I64u clusters.\n"),i,block->lcn,block->length);
        
        g_mainFrame->WxTextCtrl1->AppendText(line.c_str());

        if(block->next == m_qp->filedisp.blockmap) break;
    }
    //memcpy(qp,g_mainFrame->m_queryThread->m_qp,sizeof(udefrag_query_parameters));
    g_mainFrame->WxTextCtrl1->Thaw();
    gui_query_finished();
}

void MainFrame::OnQueryCompletion(wxCommandEvent& WXUNUSED(event))
{
    wxUnsetEnv(L"UD_CUT_FILTER");
    m_busy = false;
    m_queryThread->m_flags = 0;
    m_queryThread->singlefile = false;    
    m_queryThread->m_qp->startGUI = FALSE;
}

int QueryThread::Terminator(void *p)
{
    while(g_mainFrame->m_paused) ::Sleep(300);
    return g_mainFrame->m_stopped;
}

