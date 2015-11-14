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
#include "main.h"
#include "udefrag-internals_flags.h"
//#include "../dll/udefrag/query.h" //definitions are defined in udefrag.dll's .h's
// The best thing would be to have the query functions return the original structs, and then have the GUI deal with it.
// We could make the udefrag-internals struct's available to the GUI. (such as blockmap, regions) 

void MainFrame::InitQueryMenu()
{
    m_queryThread = new QueryThread();
}

/**=========================================================================**
***                  Functions in the Query Menu/Tab.                       **
***=========================================================================**/
//right now this is equivalent to Job.cpp @ void MainFrame::OnStartJob(wxCommandEvent& event)
void MainFrame::QueryClusters(wxCommandEvent& event){
    m_queryThread->m_qp = new udefrag_query_parameters();
    
    wxListItem theitem = m_filesList->GetListItem(-1,-1);
    wxString itemtext = theitem.m_text;
    
    wxString filtertext;
    ProcessCommandEvent(ID_SelectProperDrive);
    long i = m_filesList->GetFirstSelected();
    while(i != -1){
        wxString selitem = m_filesList->GetItemText(i);
        Utils::extendfiltertext(selitem,&filtertext);
        i = m_filesList->GetNextSelected(i);
    }
    //set the Analysis Mode to SINGLE file mode.
    // This probably can't work for all queries, but is fast.
    wxSetEnv(L"UD_CUT_FILTER",filtertext);
    m_queryThread->singlefile = TRUE;
    m_queryThread->m_flags |= UD_JOB_CONTEXT_MENU_HANDLER;

    m_queryThread->m_querypath = (wchar_t *)itemtext.fn_str();
    m_queryThread->m_letter = (char)(itemtext[0]); //find the drive-letter of the fragmented files tab.
    
    m_queryThread->m_qp->path = m_queryThread->m_querypath;
    m_queryThread->m_mapSize = m_jobThread->m_mapSize;
    
    switch(event.GetId()){
    case ID_QueryClusters:
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
void *QueryThread::Entry()
{
    int result;

    while(!g_mainFrame->CheckForTermination(200)){
        if(m_startquery){
            result = udefrag_start_query(m_letter,m_qType,m_flags,m_mapSize,
                reinterpret_cast<udefrag_progress_callback>(ProgressCallback),
                reinterpret_cast<udefrag_terminator>(Terminator),m_qp,NULL
            );
            if(result < 0 && !g_mainFrame->m_stopped)
                etrace("Disk Processing Failure.");
            PostCommandEvent(g_mainFrame,ID_QueryCompletion);
            m_startquery = false;
        }
    }
    return NULL;
}

void QueryThread::ProgressCallback(udefrag_progress_info *pi, void *p)
{
    //Copied from Job.cpp function of same-name
    if (pi->completion_status > 0){
        dtrace("The Completion Status Was 0. Begin Populating The Query Tab Here.");
        //cannot access member variable m_qp from in here.
        //would rather call a new member function.
        g_mainFrame->m_queryThread->DisplayQueryResults();
    }
}

void QueryThread::DisplayQueryResults()
{
    ULONGLONG i;
    winx_blockmap *block;

    dtrace("The File's path is: %ws",m_qp->path);
    dtrace("The File has #clusters: %d",m_qp->filedisp.clusters);
    itrace("The File has #fragments: %d",m_qp->filedisp.fragments);
    
    for(block = m_qp->filedisp.blockmap, i = 0; block; block = block->next, i++){
        itrace("file part #%I64u start: %I64u, length: %I64u",i,block->lcn,block->length);
        if(block->next == m_qp->filedisp.blockmap) break;
    }

    gui_query_finished();
}

int QueryThread::Terminator(void *p)
{
    while(g_mainFrame->m_paused) ::Sleep(300);
    return g_mainFrame->m_stopped;
}

void MainFrame::OnQueryCompletion(wxCommandEvent& WXUNUSED(event))
{
    m_busy = false;
    m_queryThread->m_flags = 0;
    m_queryThread->singlefile = FALSE;
    wxUnsetEnv(L"UD_CUT_FILTER");    
}
