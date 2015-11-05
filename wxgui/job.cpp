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
 * @file job.cpp
 * @brief Volume processing.
 * @addtogroup VolumeProcessing
 * @{
 */

// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.

// =======================================================================
//                            Declarations
// =======================================================================

#include "main.h"

#define UD_EnableTool(id) { \
    wxMenuItem *item = m_menuBar->FindItem(id); \
    if(item) item->Enable(true); \
    if(m_toolBar->FindById(id)) \
        m_toolBar->EnableTool(id,true); \
}

#define UD_DisableTool(id) { \
    wxMenuItem *item = m_menuBar->FindItem(id); \
    if(item) item->Enable(false); \
    if(m_toolBar->FindById(id)) \
        m_toolBar->EnableTool(id,false); \
}

// =======================================================================
//                              Jobs cache
// =======================================================================

void MainFrame::CacheJob(wxCommandEvent& event)
{

    int index = event.GetInt();
    JobsCacheEntry *cacheEntry = m_jobsCache[index];
    JobsCacheEntry *newEntry = (JobsCacheEntry *)event.GetClientData();

    if(!cacheEntry){
        m_jobsCache[index] = newEntry;
    } else {
        delete [] cacheEntry->clusterMap;
        //dtrace("Old Cache Entry updated.");
        memcpy(cacheEntry,newEntry,sizeof(JobsCacheEntry));
        delete newEntry;
    }

    m_currentJob = m_jobsCache[index];
}

// =======================================================================
//                          Job startup thread
// =======================================================================

void JobThread::ProgressCallback(udefrag_progress_info *pi, void *p)
{

    // update window title and tray icon tooltip
    char op = 'O';
    if(pi->current_operation == VOLUME_ANALYSIS) op = 'A';
    if(pi->current_operation == VOLUME_DEFRAGMENTATION) op = 'D';

    wxString title = wxString::Format(wxT("%c:  %c %6.2lf %%"),
        winx_toupper(g_mainFrame->m_jobThread->m_letter),op,pi->percentage
    );
    if(g_mainFrame->CheckOption(wxT("UD_DRY_RUN"))) title += wxT(" (dry run)");

    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,EventID_SetWindowTitle);
    event.SetString(title); wxPostEvent(g_mainFrame,event);

    g_mainFrame->SetSystemTrayIcon(g_mainFrame->m_paused ? \
        wxT("tray_paused") : wxT("tray_running"),title);

    // set overall progress
    if(g_mainFrame->m_jobThread->m_jobType == ANALYSIS_JOB \
      || pi->current_operation != VOLUME_ANALYSIS)
    {
        if(g_mainFrame->CheckOption(wxT("UD_SHOW_PROGRESS_IN_TASKBAR"))){
            g_mainFrame->SetTaskbarProgressState(TBPF_NORMAL);
            if(pi->clusters_to_process){
                g_mainFrame->SetTaskbarProgressValue(
                    (pi->clusters_to_process / g_mainFrame->m_selected) * \
                    g_mainFrame->m_processed + \
                    pi->processed_clusters / g_mainFrame->m_selected,
                    pi->clusters_to_process
                );
            } else {
                g_mainFrame->SetTaskbarProgressValue(0,1);
            }
        } else {
            g_mainFrame->SetTaskbarProgressState(TBPF_NOPROGRESS);
        }
    }

    // save progress information to the jobs cache
    int letter = (int)(g_mainFrame->m_jobThread->m_letter);
    JobsCacheEntry *cacheEntry = new JobsCacheEntry;

    cacheEntry->jobType = g_mainFrame->m_jobThread->m_jobType;

    memcpy(&cacheEntry->pi,pi,sizeof(udefrag_progress_info));

    cacheEntry->clusterMap = new char[pi->cluster_map_size];
    if(pi->cluster_map_size){
        memcpy(cacheEntry->clusterMap,
            pi->cluster_map,
            pi->cluster_map_size
        );
    }

    cacheEntry->stopped = g_mainFrame->m_stopped;
    event.SetId(EventID_CacheJob);
    event.SetInt(letter);
    event.SetClientData((void *)cacheEntry);
    wxPostEvent(g_mainFrame,event);

    if ((pi->completion_status > 0) && (pi->isfragfileslist = 1)){
        g_jpPtr = pi->jp;   //set Global Pointer back to JP. will be cleared soon.
        cacheEntry->pi.fragmented_files_prb = pi->fragmented_files_prb;
        //cacheEntry->pi.fragmented_files_prb = prb_copy(pi->fragmented_files_prb,NULL,NULL,NULL);
//        prb_destroy(pi->fragmented_files_prb,NULL);
        //populate the fragmented files-list tab.
        event.SetId(EventID_PopulateFilesList);
        event.SetInt(letter);
        //event.SetClientData((void *)cacheEntry);
        wxPostEvent(g_mainFrame,event);
        dtrace("Successfully sent Fragmented Files list over to MainFrame::FilesPopulateList()");
        event.SetId(EventID_UpdateVolumeStatus);
        wxPostEvent(g_mainFrame,event);
        return;
    }
    //dtrace("Updating Volume Status,Redrawing Map, and Updating StatusBar.");
    // update Volume status
    event.SetId(EventID_UpdateVolumeStatus);
    event.SetInt(letter);
    wxPostEvent(g_mainFrame,event);
    // Update Clustermap and Statusbar.
    PostCommandEvent(g_mainFrame,EventID_RedrawMap);
    PostCommandEvent(g_mainFrame,EventID_UpdateStatusBar);
    //after this, it finishes udefrag_start_job @ udefrag.c line 421.
    //after that, it goes back to line 182 of this file, and line 197 calls EventID_UpdateVolumeInformation.
}

int JobThread::Terminator(void *p)
{
    while(g_mainFrame->m_paused) ::Sleep(300);
    return g_mainFrame->m_stopped;
}

void JobThread::ProcessVolume(int index)
{
    // update volume capacity information
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,EventID_UpdateVolumeInformation);
    event.SetInt((int)m_letter); wxPostEvent(g_mainFrame,event);

    // process volume
    int result = udefrag_validate_volume(m_letter,FALSE);
    if(result == 0){
        m_flags |= g_mainFrame->m_repeat ? UD_JOB_REPEAT : 0;
        result = udefrag_start_job(m_letter,m_jobType,m_flags,m_mapSize,
            reinterpret_cast<udefrag_progress_callback>(ProgressCallback),
            reinterpret_cast<udefrag_terminator>(Terminator),NULL
        );
    }

    if(result < 0 && !g_mainFrame->m_stopped){
        etrace("Disk Processing Failure.");
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,EventID_DiskProcessingFailure);
        event.SetInt(result);
        event.SetString((*m_volumes)[index]);
        wxPostEvent(g_mainFrame,event);
    }
    // update volume dirty status
    wxPostEvent(g_mainFrame,event);

}

void *JobThread::Entry()
{
    while(!g_mainFrame->CheckForTermination(200)){
        if(m_launch){
            // do the job
            g_mainFrame->m_selected = (int)m_volumes->Count();
            g_mainFrame->m_processed = 0;

            for(int i = 0; i < g_mainFrame->m_selected; i++){
                if(g_mainFrame->m_stopped){
                  dtrace("JobThread stopping!!!!!!, because m_stopped.");
                  break;
                }

                m_letter = (char)((*m_volumes)[i][0]);
                //dtrace("About to process volume: %c",m_letter);
                ProcessVolume(i);

                /* advance overall progress to processed/selected */
                g_mainFrame->m_processed++;
                if(g_mainFrame->CheckOption(wxT("UD_SHOW_PROGRESS_IN_TASKBAR"))){

                    g_mainFrame->SetTaskbarProgressState(TBPF_NORMAL);

                    g_mainFrame->SetTaskbarProgressValue(g_mainFrame->m_processed, g_mainFrame->m_selected);
                } else {
                    g_mainFrame->SetTaskbarProgressState(TBPF_NOPROGRESS);
                }
            }
            // complete the job
            //TraceSource;
            PostCommandEvent(g_mainFrame,EventID_JobCompletion);
            delete m_volumes;
            m_launch = false;
        }
    }
    return NULL;
}

// =======================================================================
//                            Event handlers
// =======================================================================

void MainFrame::OnStartJob(wxCommandEvent& event)
{
    if(m_busy) return;

    // if nothing is selected in the list return
    m_jobThread->m_volumes = new wxArrayString;
    long i = m_vList->GetFirstSelected();
    while(i != -1){
        m_jobThread->m_volumes->Add(m_vList->GetItemText(i));
        i = m_vList->GetNextSelected(i);
    }
    if(!m_jobThread->m_volumes->Count()) return;

    // lock everything till the job completion
    m_busy = true; m_paused = false; m_stopped = false;
    UD_DisableTool(EventID_Analyze);
    UD_DisableTool(EventID_Defrag);
    UD_DisableTool(EventID_QuickOpt);
    UD_DisableTool(EventID_FullOpt);
    UD_DisableTool(EventID_MftOpt);
    UD_DisableTool(EventID_Repeat);
    UD_DisableTool(EventID_SkipRem);
    UD_DisableTool(EventID_Rescan);
    UD_DisableTool(EventID_Repair);
    UD_DisableTool(EventID_ShowReport);
    m_subMenuSortingConfig->Enable(false);

    ReleasePause();

    SetSystemTrayIcon(wxT("tray_running"),wxT("UltraDefrag"));
    ProcessCommandEvent(EventID_AdjustTaskbarIconOverlay);
    /* set overall progress: normal 0% */
    if(CheckOption(wxT("UD_SHOW_PROGRESS_IN_TASKBAR"))){
        SetTaskbarProgressValue(0,1);
        SetTaskbarProgressState(TBPF_NORMAL);
    }

    // set sorting parameters
    if(m_menuBar->FindItem(EventID_SortByPath)->IsChecked()){
        wxSetEnv(wxT("UD_SORTING"),wxT("path"));
    } else if(m_menuBar->FindItem(EventID_SortBySize)->IsChecked()){
        wxSetEnv(wxT("UD_SORTING"),wxT("size"));
    } else if(m_menuBar->FindItem(EventID_SortByCreationDate)->IsChecked()){
        wxSetEnv(wxT("UD_SORTING"),wxT("c_time"));
    } else if(m_menuBar->FindItem(EventID_SortByModificationDate)->IsChecked()){
        wxSetEnv(wxT("UD_SORTING"),wxT("m_time"));
    } else if(m_menuBar->FindItem(EventID_SortByLastAccessDate)->IsChecked()){
        wxSetEnv(wxT("UD_SORTING"),wxT("a_time"));
    }
    if(m_menuBar->FindItem(EventID_SortAscending)->IsChecked()){
        wxSetEnv(wxT("UD_SORTING_ORDER"),wxT("asc"));
    } else {
        wxSetEnv(wxT("UD_SORTING_ORDER"),wxT("desc"));
    }

    if (m_jobThread->singlefile){
        m_jobThread->m_flags |= UD_JOB_CONTEXT_MENU_HANDLER;
    }

    // launch the job
    switch(event.GetId()){
    case EventID_Analyze:
        m_jobThread->m_jobType = ANALYSIS_JOB;
        break;
    case EventID_Defrag:
        m_jobThread->m_jobType = DEFRAGMENTATION_JOB;
        break;
    case EventID_QuickOpt:
        m_jobThread->m_jobType = QUICK_OPTIMIZATION_JOB;
        break;
    case EventID_FullOpt:
        m_jobThread->m_jobType = FULL_OPTIMIZATION_JOB;
        break;
    default:
        m_jobThread->m_jobType = MFT_OPTIMIZATION_JOB;
        break;
    }
    int width, height; m_cMap->GetClientSize(&width,&height);
    int block_size = CheckOption(wxT("UD_MAP_BLOCK_SIZE"));
    int line_width = CheckOption(wxT("UD_GRID_LINE_WIDTH"));
    int cell_size = block_size + line_width;
    int blocks_per_line = (width - line_width) / cell_size;
    int lines = (height - line_width) / cell_size;
    m_jobThread->m_mapSize = blocks_per_line * lines;
    m_jobThread->m_launch = true;
}

void MainFrame::OnJobCompletion(wxCommandEvent& WXUNUSED(event))
{
    // unlock everything after the job completion
    UD_EnableTool(EventID_Analyze);
    UD_EnableTool(EventID_Defrag);
    UD_EnableTool(EventID_QuickOpt);
    UD_EnableTool(EventID_FullOpt);
    UD_EnableTool(EventID_MftOpt);
    UD_EnableTool(EventID_Repeat);
    UD_EnableTool(EventID_SkipRem);
    UD_EnableTool(EventID_Rescan);
    UD_EnableTool(EventID_Repair);
    UD_EnableTool(EventID_ShowReport);
    m_subMenuSortingConfig->Enable(true);
    m_busy = false;

    ReleasePause();

    SetSystemTrayIcon(wxT("tray"),wxT("UltraDefrag"));
    ProcessCommandEvent(EventID_SetWindowTitle);
    ProcessCommandEvent(EventID_AdjustTaskbarIconOverlay);
    SetTaskbarProgressState(TBPF_NOPROGRESS);
    // shutdown when requested
    if(!m_stopped)
        ProcessCommandEvent(EventID_Shutdown);
    dtrace("The Job Has Completed Fully.");
    m_jobThread->m_flags = 0;
    m_jobThread->singlefile = FALSE;
    wxUnsetEnv(L"UD_CUT_FILTER");
}

void MainFrame::SetPause()
{
    m_menuBar->FindItem(EventID_Pause)->Check(true);
    m_toolBar->ToggleTool(EventID_Pause,true);

    Utils::SetProcessPriority(IDLE_PRIORITY_CLASS);

    SetSystemTrayIcon(m_busy ? wxT("tray_paused") \
        : wxT("tray"),wxT("UltraDefrag"));
    ProcessCommandEvent(EventID_AdjustTaskbarIconOverlay);
}

void MainFrame::ReleasePause()
{
    m_menuBar->FindItem(EventID_Pause)->Check(false);
    m_toolBar->ToggleTool(EventID_Pause,false);

    Utils::SetProcessPriority(NORMAL_PRIORITY_CLASS);

    SetSystemTrayIcon(m_busy ? wxT("tray_running") \
        : wxT("tray"),wxT("UltraDefrag"));
    ProcessCommandEvent(EventID_AdjustTaskbarIconOverlay);
}

void MainFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
    m_paused = m_paused ? false : true;
    if(m_paused) SetPause(); else ReleasePause();
}

void MainFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    m_paused = false;
    ReleasePause();
    m_stopped = true;
}

void MainFrame::OnRepeat(wxCommandEvent& WXUNUSED(event))
{
    if(!m_busy){
        m_repeat = m_repeat ? false : true;
        m_menuBar->FindItem(EventID_Repeat)->Check(m_repeat);
        m_toolBar->ToggleTool(EventID_Repeat,m_repeat);
    }
}

void MainFrame::OnRepair(wxCommandEvent& WXUNUSED(event))
{
    if(m_busy) return;

    wxString args;
    long i = m_vList->GetFirstSelected();
    while(i != -1){
        char letter = (char)m_vList->GetItemText(i)[0];
        args << wxString::Format(wxT(" %c:"),letter);
        i = m_vList->GetNextSelected(i);
    }

    if(args.IsEmpty()) return;

    /*
    create command line to check disk for corruption:
    CHKDSK {drive} /F ................. check the drive and correct problems
    PING -n {seconds + 1} localhost ... pause for the specified seconds
    */
    wxFileName path(wxT("%windir%\\system32\\cmd.exe"));
    path.Normalize(); wxString cmd(path.GetFullPath());
    cmd << wxT(" /C ( ");
    cmd << wxT("for %D in ( ") << args << wxT(" ) do ");
    cmd << wxT("@echo. ");
    cmd << wxT("& echo chkdsk %D ");
    cmd << wxT("& echo. ");
    cmd << wxT("& chkdsk %D /F ");
    cmd << wxT("& echo. ");
    cmd << wxT("& echo ------------------------------------------------- ");
    cmd << wxT("& ping -n 11 localhost >nul ");
    cmd << wxT(") ");
    cmd << wxT("& echo. ");
    cmd << wxT("& pause");

    itrace("Command Line: %ls", cmd.wc_str());
    if(!wxExecute(cmd)) Utils::ShowError(wxT("Cannot execute cmd.exe program!"));
}

void MainFrame::OnDefaultAction(wxCommandEvent& WXUNUSED(event))
{
    long i = m_vList->GetFirstSelected();
    if(i != -1){
        volume_info v;
        char letter = (char)m_vList->GetItemText(i)[0];
        if(udefrag_get_volume_information(letter,&v) >= 0){
            if(v.is_dirty){
                ProcessCommandEvent(EventID_Repair);
                return;
            }
        }
        ProcessCommandEvent(EventID_Analyze);
    }
}

void MainFrame::OnDiskProcessingFailure(wxCommandEvent& event)
{
    wxString caption;
    switch(m_jobThread->m_jobType){
    case ANALYSIS_JOB:
        caption = wxString::Format(
            wxT("Analysis of %ls failed."),
            event.GetString().wc_str()
        );
        break;
    case DEFRAGMENTATION_JOB:
        caption = wxString::Format(
            wxT("Defragmentation of %ls failed."),
            event.GetString().wc_str()
        );
        break;
    default:
        caption = wxString::Format(
            wxT("Optimization of %ls failed."),
            event.GetString().wc_str()
        );
        break;
    }

    int error = event.GetInt();
    wxString msg = caption + wxT("\n") \
        + wxString::Format(wxT("%hs"),
        udefrag_get_error_description(error));

    Utils::ShowError(wxT("%ls"),msg.wc_str());
}

#undef UD_EnableTool
#undef UD_DisableTool

/** @} */
