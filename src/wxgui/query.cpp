//////////////////////////////////////////////////////////////////////////
//
//  UltraDefrag - a powerful defragmentation tool for Windows NT.
//  Copyright (c) 2007-2017 Dmitri Arkhangelski (dmitriar@gmail.com).
//  Copyright (c) 2010-2013 Stefan Pendl (stefanpe@users.sourceforge.net).
//  Copyright (c) 2015-2017 genBTC (genBTC@gmx.com)
// Original Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.
// This file only was totally created by genBTC <genBTC@gmx.com>
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
 * @file query.cpp
 * @brief Query the internals and provide information.
 * @addtogroup Query
 * @note query.c in udefrag.dll is the backend.
 */
/**=========================================================================**
***                        Declarations                                     **
***=========================================================================**/
#include "prec.h"
#include "main.h"

/**=========================================================================**
***                        Create Query GUI                                 **
***=========================================================================**/
void MainFrame::InitQueryMenu()
{
     //create Query tab, Tab3.

	m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4	= new wxBoxSizer( wxVERTICAL );

    m_WxFilePickerQuery1 = new wxFilePickerCtrl(m_panel3, ID_WXFILEPICKERCTRL1, wxEmptyString, wxFileSelectorPromptStr, 
                                             wxFileSelectorDefaultWildcardStr, wxDefaultPosition, wxDefaultSize);
    m_WxFilePickerQuery1->SetFocus();
  
	m_WxTextCtrl1 = new wxTextCtrl(m_panel3, ID_WXTEXTCTRL1, _("Output"), wxDefaultPosition,wxSize(wxDefaultCoord,350), 
                              wxVSCROLL | wxTE_READONLY | wxTE_MULTILINE, wxDefaultValidator, _("WxTextCtrl1"));
	m_WxTextCtrl1->SetMaxLength(0);
	m_WxTextCtrl1->SetInsertionPointEnd();

	m_PerformQuery1 = new wxButton(m_panel3, ID_PerformQueryBUTTON, _("Perform Query!"), wxDefaultPosition, wxDefaultSize, 
                             0, wxDefaultValidator, _("PerformQuery"));

	bSizer4->Add( m_WxFilePickerQuery1, 0, wxEXPAND | wxALL, 5);
	bSizer4->Add( m_WxTextCtrl1, 0, wxEXPAND | wxALL, 5);
	bSizer4->Add( m_PerformQuery1, 0, wxEXPAND | wxALL, 5);
	
	m_panel3->SetSizer( bSizer4 );
    bSizer4->Fit( m_panel3 );
    m_notebook1->AddPage( m_panel3, "Query", false );   
}

/**=========================================================================**
***                  Functions in the Query Menu/Tab.                       **
***=========================================================================**/
//right now this is equivalent to Job.cpp @ void MainFrame::OnStartJob(wxCommandEvent& event)
//All these handlers are crazy.
void MainFrame::QueryClusters(wxCommandEvent& event){
    wxString filtertext,itemtext;
    
    int id = event.GetId();
    m_queryThread->m_qp = new udefrag_query_parameters();
    m_queryThread->m_qp->engineFinished = FALSE;
    m_queryThread->m_qp->startGUI = TRUE;
    m_WxTextCtrl1->Clear();
    
//Launched from Menu:
    if (id == ID_QueryClusters){
        itemtext = m_filesList->GetListItem().GetText();
        m_WxFilePickerQuery1->SetPath(itemtext);
        
        ProcessCommandEvent(this,ID_SelectProperDrive);
        long i = m_filesList->GetFirstSelected();
        while(i != -1){
            wxString selitem = m_filesList->GetItemText(i);
            Utils::extendfiltertext(selitem,&filtertext);
            i = m_filesList->GetNextSelected(i);
        }
    }
//Launched from Tab
    else if (id == ID_PerformQueryBUTTON){
        itemtext = m_WxFilePickerQuery1->GetPath();
        Utils::extendfiltertext(itemtext,&filtertext);
    }
    if (itemtext.empty())
        return; //find the drive-letter of the fragmented files tab.
    const char letter = (char)itemtext[0];
   
    //set the Analysis Mode to SINGLE file mode.
    // This probably can't work for all queries, but is fast.
    wxSetEnv(L"UD_CUT_FILTER",filtertext);
    m_queryThread->singlefile = true;
    m_queryThread->m_flags |= UD_JOB_CONTEXT_MENU_HANDLER;

    m_queryThread->m_querypath = itemtext;
    m_queryThread->m_qp->path = ws(m_queryThread->m_querypath);
    m_queryThread->m_letter = letter;
    
    m_queryThread->m_mapSize = GetMapSize();
    
    switch(id){
    case ID_QueryClusters:
    case ID_PerformQueryBUTTON:
        m_queryThread->m_qType = QUERY_GET_VCNLIST;
        break;
    case ID_QueryFreeGaps:
    case ID_QueryOperation2:
        m_queryThread->m_qType = QUERY_GET_FREE_REGIONS;
        break;    
    default:
        break;
    }
    m_busy = true; m_paused = false; m_stopped = false;
    m_queryThread->m_startquery = true; //BEGIN LAUNCH. (entry)

    m_WxTextCtrl1->AppendText(L"Starting Query!.....\n");    
}

//event handler for operation 3, spawns QueryThread.
void MainFrame::QueryOperation3(wxCommandEvent& event)
{
    const int id = event.GetId();
    if (id == ID_QueryOperation3) {
        switch (id) {
        case ID_QueryOperation3:
            m_queryThread->m_qType = QUERY_OPERATION_3;
            break;
        default:
            break;
        }
    }
    m_queryThread->m_qp = new udefrag_query_parameters();
    m_queryThread->m_qp->engineFinished = FALSE;
    m_queryThread->m_qp->startGUI = TRUE;
    m_WxTextCtrl1->Clear();
    m_WxTextCtrl1->AppendText("LCN Query Operation 3 is Running...\n");
    m_busy = true; m_paused = false; m_stopped = false;
    m_queryThread->m_startquery = true; //BEGIN LAUNCH. (entry)
}

//QueryThread calls stopgap and paints the textbox.
int QueryThread::stopgap_finish_operation3()
{
    dtrace("Query.cpp starts calling stopgap_enumerate_gaps()");
    const char letter = g_mainFrame->GetDriveLetter();
    const wxString GapsList(stopgap_enumerate_gaps(letter));
    g_mainFrame->m_WxTextCtrl1->AppendText(GapsList);
    if (winx_init_library() < 0)
        return (-1);
    return 1;
}


//Direct wx menu event Call (no threading) stopgap is quick
void MainFrame::QueryOperation4(wxCommandEvent& event)
{
    dtrace("Query.cpp starts calling stopgap_count_gaps()");
    const char letter = g_mainFrame->GetDriveLetter();
    wxString GapsList("There were this many gaps: ");
    GapsList << stopgap_count_gaps(letter) << " \n";
    g_mainFrame->m_WxTextCtrl1->AppendText(GapsList);
    if (winx_init_library() < 0)
    return;
}

/**=========================================================================**
***                  Dedicated Query Thread.                                **
***=========================================================================**/
//better off with a struct to pass any variables we need IN. Such as:
// drive_letter. query_type. File Path.  (combine/variables set above)
void* QueryThread::Entry()
{
    int result;
    dtrace("Entry into QueryThread");
    while(!g_mainFrame->CheckForTermination(200)){
        if(m_startquery){
            if (m_qType != QUERY_OPERATION_3)
                result = udefrag_starts_query(m_letter, m_qType, m_flags, m_mapSize,
                    reinterpret_cast<udefrag_query_progress_callback>(PostProgressCallback),
                    reinterpret_cast<udefrag_terminator>(Terminator), *m_qp, NULL);
            else
                result = stopgap_finish_operation3();
            if(result < 0 && !g_mainFrame->m_stopped){
                etrace("Disk Processing Failure.");
                g_mainFrame->m_WxTextCtrl1->AppendText(L"Error executing Query.");
            }
            QueueCommandEvent(g_mainFrame, ID_QueryCompletion);
            m_startquery = false;
        }
    }
    return NULL;
}

//Copied from Job.cpp function of same-name
void QueryThread::PostProgressCallback(udefrag_query_parameters *qp, void *p)
{
    dtrace("Entry into Query Post Progress Callback");
    if (qp->startGUI) {
        dtrace("Begin Populating The Query Tab Here!:");
        //This pointer chain needs to exist (the non static function of member instance) to the query object. ? idk.
        g_mainFrame->m_queryThread->DisplayQueryResults(qp);
    }
    else
        etrace("Query CallBack, Should not Be Here. Did Nothing....");
}

/**
 * \brief Show the results of the query: (Fragments, Clusters, VCNs) in the Query Tab Textboxes.
 * \param qp Object Data clump for passing info back and forth
 */
void QueryThread::DisplayQueryResults(udefrag_query_parameters *qp)
{
    ULONGLONG i;
    winx_blockmap *block;
    memcpy(m_qp, qp, sizeof(udefrag_query_parameters));
    itrace("The File's path is: %ws", m_qp->path);
    itrace("The File has #clusters: %d", m_qp->filedisp.clusters);
    itrace("The File has #fragments: %d", m_qp->filedisp.fragments);
    wxString line;
    line.Printf(_("Path: %s \n" \
        "Totals: Fragments: %I64u , Clusters: %I64u \n"),
        m_qp->path, m_qp->filedisp.fragments, m_qp->filedisp.clusters);
    g_mainFrame->m_WxTextCtrl1->Freeze();
    g_mainFrame->m_WxTextCtrl1->AppendText(line.c_str());
    for (block = m_qp->filedisp.blockmap, i = 0; block; block = block->next, i++) {
        itrace("file part #%I64u start: %I64u, length: %I64u\n", i, block->lcn, block->length);
        line.Printf(_("Fragment #%I64u starts @ %I64u, length: %I64u clusters.\n"), i, block->lcn, block->length);

        g_mainFrame->m_WxTextCtrl1->AppendText(line.c_str());

        if (block->next == m_qp->filedisp.blockmap) break;
    }
    g_mainFrame->m_WxTextCtrl1->Thaw();
    gui_query_finished();
}

//ID_QueryCompletion 
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

