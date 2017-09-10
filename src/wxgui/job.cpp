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
#include "wx/wxprec.h"
#include "main.h"

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

    wxString title = wxString::Format("%c:  %c %6.2lf %%",
        winx_toupper(g_mainFrame->m_jobThread->m_letter),op,pi->percentage
    );
    if(g_mainFrame->CheckOption("UD_DRY_RUN")) title += " (Dry Run)";

    wxCommandEvent *event = new wxCommandEvent(
        wxEVT_COMMAND_MENU_SELECTED,ID_SetWindowTitle
    );
    event->SetString(title.c_str()); // make a deep copy
    g_mainFrame->GetEventHandler()->QueueEvent(event);

    event = new wxCommandEvent(
        wxEVT_COMMAND_MENU_SELECTED,ID_AdjustSystemTrayIcon
    );
    event->SetString(title.c_str()); // make another deep copy
    g_mainFrame->GetEventHandler()->QueueEvent(event);

    // set overall progress
    if(g_mainFrame->m_jobThread->m_jobType == ANALYSIS_JOB \
      || pi->current_operation != VOLUME_ANALYSIS)
    {
        if(g_mainFrame->CheckOption("UD_SHOW_PROGRESS_IN_TASKBAR")){
            g_mainFrame->SetTaskbarProgressState(TBPF_NORMAL);
            if(pi->clusters_to_process){
                g_mainFrame->SetTaskbarProgressValue(
                    pi->clusters_to_process / g_mainFrame->m_selected * \
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
    int letter = (int)g_mainFrame->m_jobThread->m_letter;
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
    event = new wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED,ID_CacheJob);
    event->SetInt(letter); event->SetClientData((void *)cacheEntry);
    g_mainFrame->GetEventHandler()->QueueEvent(event);

    if (pi->completion_status > 0) {
    event = new wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED,ID_UpdateVolumeStatus);
    event->SetInt(letter); g_mainFrame->GetEventHandler()->QueueEvent(event);
    QueueCommandEvent(g_mainFrame,ID_RedrawMap);
    QueueCommandEvent(g_mainFrame,ID_UpdateStatusBar);
        event.SetId(ID_PopulateFilesList); //populate the fragmented-files-list tab's listview.
        event.SetInt(letter);
        
        wxPostEvent(g_mainFrame,event);
        dtrace("Successfully sent Fragmented Files list over to MainFrame::FilesPopulateList()");
        event.SetId(ID_UpdateVolumeStatus);//updates status column with "Analyzed.", etc (on finished)
        wxPostEvent(g_mainFrame,event);
        return; //shortcut past a redundant redrawmap and updatestatusbar
    }
    //dtrace("Updating Volume Status,Redrawing Map, and Updating StatusBar.");
    // update Volume status
    wxPostEvent(g_mainFrame,event);
    // Update Clustermap and Statusbar.
    //after this, it finishes udefrag_start_job @ udefrag.c line 421.
    //after that, it goes back to line 182 of this file, and line 197 calls ID_UpdateVolumeInformation.
}

int JobThread::Terminator(void *p)
{
    while(g_mainFrame->m_paused) ::Sleep(300);
    return g_mainFrame->m_stopped;
}

void JobThread::ProcessVolume(int index)
{
    // update volume capacity information
    wxCommandEvent *event = new wxCommandEvent(
        wxEVT_COMMAND_MENU_SELECTED,ID_UpdateVolumeInformation
    );
    event->SetInt((int)m_letter);
    g_mainFrame->GetEventHandler()->QueueEvent(event);

    // process volume
    int result = udefrag_validate_volume(m_letter,FALSE);
    if(result == 0){
        //created a flags variable to hold custom flags on-the-fly such as ContextMenuHandler
        m_flags |= g_mainFrame->m_repeat ? UD_JOB_REPEAT : 0;
        result = udefrag_start_job(m_letter,m_jobType,m_flags,m_mapSize,
            reinterpret_cast<udefrag_progress_callback>(ProgressCallback),
            reinterpret_cast<udefrag_terminator>(Terminator), nullptr
        );
    }

    if(result < 0 && !g_mainFrame->m_stopped){
        wxCommandEvent *event = new wxCommandEvent(
            wxEVT_COMMAND_MENU_SELECTED,ID_DiskProcessingFailure
        );
        event->SetInt(result);
        event->SetString(((*m_volumes)[index]).c_str()); // make a deep copy
        g_mainFrame->GetEventHandler()->QueueEvent(event);
    }

    // update volume dirty status
    event = new wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED,ID_UpdateVolumeInformation);
    event->SetInt((int)m_letter); g_mainFrame->GetEventHandler()->QueueEvent(event);
}

void* JobThread::Entry()
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
                m_letter = (char)(*m_volumes)[i][0];
                //dtrace("About to process volume: %c",m_letter);
                ProcessVolume(i);

                /* advance overall progress to processed/selected */
                g_mainFrame->m_processed ++;
                if(g_mainFrame->CheckOption("UD_SHOW_PROGRESS_IN_TASKBAR")){
                    g_mainFrame->SetTaskbarProgressState(TBPF_NORMAL);
                    g_mainFrame->SetTaskbarProgressValue(
                        g_mainFrame->m_processed, g_mainFrame->m_selected
                    );
                } else {
                    g_mainFrame->SetTaskbarProgressState(TBPF_NOPROGRESS);
                }
            }
            // complete the job,very important.
            QueueCommandEvent(g_mainFrame,ID_JobCompletion);
            delete m_volumes;
            m_launch = false;
        }
    }

    return nullptr;
}

// =======================================================================
//                            Event handlers
// =======================================================================

/**
 * \brief Determine how large the Cluster Map should be (window size)
 * \return A total number of cells/blocks. 
 */
int MainFrame::GetMapSize(){
    int width, height; g_mainFrame->m_cMap->GetClientSize(&width,&height);
    int block_size = CheckOption("UD_MAP_BLOCK_SIZE");
    int line_width = CheckOption("UD_GRID_LINE_WIDTH");
    int cell_size = block_size + line_width;
    int blocks_per_line = (width - line_width) / cell_size;
    int lines = (height - line_width) / cell_size;
    return blocks_per_line * lines;
}

/**
 * \brief Event function. User starts the job, telling us what job it is.
 * Disable GUI elements, do program housekeeping, then calculate the job parameters.
 * Gets the job done.
 * \param event 
 */
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
    UD_EnableTool(ID_Stop);
    UD_DisableTool(ID_Analyze);
    UD_DisableTool(ID_Defrag);
    UD_DisableTool(ID_QuickOpt);
    UD_DisableTool(ID_FullOpt);
    UD_DisableTool(ID_MftOpt);
    UD_DisableTool(ID_Repeat);
    UD_DisableTool(ID_SkipRem);
    UD_DisableTool(ID_Rescan);
    UD_DisableTool(ID_Repair);
    UD_DisableTool(ID_ShowReport);
    m_subMenuSortingConfig->Enable(false);

    // XXX: force the repeat button to be
    // grayed out even when it's checked
    wxToolBarToolBase *btn = m_toolBar->FindById(ID_Repeat);
    if(btn){
        if(!m_repeatButtonBitmap.IsOk()){
            // save normal bitmap to restore it further
            m_repeatButtonBitmap = btn->GetNormalBitmap();
        }
        m_toolBar->SetToolNormalBitmap(
           ID_Repeat,btn->GetDisabledBitmap());
    }

    ReleasePause();

    ProcessCommandEvent(this,ID_AdjustSystemTrayIcon);
    ProcessCommandEvent(this,ID_AdjustTaskbarIconOverlay);
    /* set overall progress: normal 0% */
    if(CheckOption("UD_SHOW_PROGRESS_IN_TASKBAR")){
        SetTaskbarProgressValue(0,1);
        SetTaskbarProgressState(TBPF_NORMAL);
    }

    // set sorting parameters
    if(m_menuBar->FindItem(ID_SortByPath)->IsChecked()){
        wxSetEnv("UD_SORTING","path");
    } else if(m_menuBar->FindItem(ID_SortBySize)->IsChecked()){
        wxSetEnv("UD_SORTING","size");
    } else if(m_menuBar->FindItem(ID_SortByCreationDate)->IsChecked()){
        wxSetEnv("UD_SORTING","c_time");
    } else if(m_menuBar->FindItem(ID_SortByModificationDate)->IsChecked()){
        wxSetEnv("UD_SORTING","m_time");
    } else if(m_menuBar->FindItem(ID_SortByLastAccessDate)->IsChecked()){
        wxSetEnv("UD_SORTING","a_time");
    }
    if(m_menuBar->FindItem(ID_SortAscending)->IsChecked()){
        wxSetEnv("UD_SORTING_ORDER","asc");
    } else {
        wxSetEnv("UD_SORTING_ORDER","desc");
    }

    //handle single file defragmenting launched from the right click context menu
    // as if it was launched from the explorer shell context menu handler.
    if (m_jobThread->singlefile){
        m_jobThread->m_flags |= UD_JOB_CONTEXT_MENU_HANDLER;
    }

    // launch the job
    switch(event.GetId()){
    case ID_Analyze:
        m_jobThread->m_jobType = ANALYSIS_JOB;
        break;
    case ID_Defrag:
        m_jobThread->m_jobType = DEFRAGMENTATION_JOB;
        break;
    case ID_QuickOpt:
        m_jobThread->m_jobType = QUICK_OPTIMIZATION_JOB;
        break;
    case ID_FullOpt:
        m_jobThread->m_jobType = FULL_OPTIMIZATION_JOB;
        break;
    case ID_MoveToFront:
        m_jobThread->m_jobType = SINGLE_FILE_MOVE_FRONT_JOB;    //genBTC
        break;
    case ID_MoveToEnd:
        m_jobThread->m_jobType = SINGLE_FILE_MOVE_END_JOB;      //genBTC
        break;
    default:
        m_jobThread->m_jobType = MFT_OPTIMIZATION_JOB;
        break;
    }
    m_jobThread->m_mapSize = GetMapSize();
    m_jobThread->m_launch = true;
}


/**
 * \brief Event function. User stops the job. Stop Button.
 */
void MainFrame::OnJobCompletion(wxCommandEvent& WXUNUSED(event))
{
    // unlock everything after the job completion
    UD_DisableTool(ID_Stop);
    UD_EnableTool(ID_Analyze);
    UD_EnableTool(ID_Defrag);
    UD_EnableTool(ID_QuickOpt);
    UD_EnableTool(ID_FullOpt);
    UD_EnableTool(ID_MftOpt);
    UD_EnableTool(ID_Repeat);
    UD_EnableTool(ID_SkipRem);
    UD_EnableTool(ID_Rescan);
    UD_EnableTool(ID_Repair);
    UD_EnableTool(ID_ShowReport);
    m_subMenuSortingConfig->Enable(true);
    m_busy = false;

    // XXX: restore the repeat button bitmap
    if(m_repeatButtonBitmap.IsOk()){
        m_toolBar->SetToolNormalBitmap(
           ID_Repeat,m_repeatButtonBitmap);
    }

    ReleasePause();

    ProcessCommandEvent(this,ID_AdjustSystemTrayIcon);
    ProcessCommandEvent(this,ID_SetWindowTitle);
    ProcessCommandEvent(this,ID_AdjustTaskbarIconOverlay);
    SetTaskbarProgressState(TBPF_NOPROGRESS);

    // shutdown when requested
    if(!m_stopped) ProcessCommandEvent(this,ID_Shutdown);
        ProcessCommandEvent(ID_Shutdown);

    dtrace("The Job Has Completed Fully."); //Final Complete Message.
    //Handle cleanup of any single file defragmenting vars.
    m_jobThread->m_flags = 0;
    m_jobThread->singlefile = false;
    wxUnsetEnv(L"UD_CUT_FILTER");
}


/**
 * \brief Pause Button Start.
 */
void MainFrame::SetPause()
{
    m_menuBar->FindItem(ID_Pause)->Check(true);
    m_toolBar->ToggleTool(ID_Pause,true);

    Utils::SetProcessPriority(IDLE_PRIORITY_CLASS);

    ProcessCommandEvent(this,ID_AdjustSystemTrayIcon);
    ProcessCommandEvent(this,ID_AdjustTaskbarIconOverlay);
}


/**
 * \brief Pause Button Stop.
 */
void MainFrame::ReleasePause()
{
    m_menuBar->FindItem(ID_Pause)->Check(false);
    m_toolBar->ToggleTool(ID_Pause,false);

    Utils::SetProcessPriority(NORMAL_PRIORITY_CLASS);

    ProcessCommandEvent(this,ID_AdjustSystemTrayIcon);
    ProcessCommandEvent(this,ID_AdjustTaskbarIconOverlay);
}

/**
 * \brief Pause Button Handler. Event Function. 
 * Uses SetPause() & ReleasePause()
 */
void MainFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
    m_paused = m_paused ? false : true;
    if(m_paused) SetPause(); else ReleasePause();
}

/**
 * \brief Stop Button Handler. Event Function. 
 */
void MainFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    m_paused = false;
    ReleasePause();
    m_stopped = true;
}

/**
 * \brief Repeat Button Handler. Event Function. 
 */
void MainFrame::OnRepeat(wxCommandEvent& WXUNUSED(event))
{
    if(!m_busy){
        m_repeat = m_repeat ? false : true;
        m_menuBar->FindItem(ID_Repeat)->Check(m_repeat);
        m_toolBar->ToggleTool(ID_Repeat,m_repeat);
    }
}

/**
 * \brief Repair Button Handler. Event Function. 
 */
void MainFrame::OnRepair(wxCommandEvent& WXUNUSED(event))
{
    if(m_busy) return;

    wxString args;
    long i = m_vList->GetFirstSelected();
    while(i != -1){
        char letter = (char)m_vList->GetItemText(i)[0];
        args << wxString::Format(" %c:",letter);
        i = m_vList->GetNextSelected(i);
    }

    if(args.IsEmpty()) return;

    /*
    create command line to check disk for corruption:
    CHKDSK {drive} /F ................. check the drive and correct problems
    PING -n {seconds + 1} localhost ... trick to pause for n seconds afterwards
    */
    wxFileName path(("%windir%\\system32\\cmd.exe"));
    path.Normalize(); wxString cmd(path.GetFullPath());
    cmd << " /C ( ";
    cmd << "for %D in ( " << args << " ) do ";
    cmd << "@echo. ";
    cmd << "& echo chkdsk %D ";
    cmd << "& echo. ";
    cmd << "& chkdsk %D /F ";
    cmd << "& echo. ";
    cmd << "& echo ------------------------------------------------- ";
    //Pause for 11 seconds after the check completes, so you can actually read it:
    cmd << "& ping -n 11 localhost >nul ";
    cmd << ") ";
    cmd << "& echo. ";
    cmd << "& pause";

    itrace("Command Line: %ls", ws(cmd));
    if(!wxExecute(cmd)) Utils::ShowError(wxString::Format("Cannot execute cmd.exe program!"));
}

void MainFrame::OnDefaultAction(wxCommandEvent& WXUNUSED(event))
{
    long i = m_vList->GetFirstSelected();
    if(i != -1){
        volume_info v;
        char letter = (char)m_vList->GetItemText(i)[0];
        if(udefrag_get_volume_information(letter,&v) >= 0){
            if(v.is_dirty){
                ProcessCommandEvent(this,ID_Repair);
                return;
            }
        }
        ProcessCommandEvent(this,ID_Analyze);
    }
}

/**
 * \brief The Job Failed. Print an Error message 
 * \param event 
 */
void MainFrame::OnDiskProcessingFailure(wxCommandEvent& event)
{
    wxString caption;
    switch(m_jobThread->m_jobType){
    case ANALYSIS_JOB:
        caption = wxString::Format(
            "Analysis of %ls failed.",
            ws(event.GetString())
        );
        break;
    case DEFRAGMENTATION_JOB:
        caption = wxString::Format(
            "Defragmentation of %ls failed.",
            ws(event.GetString())
        );
        break;
    default:
        caption = wxString::Format(
            "Optimization of %ls failed.",
            ws(event.GetString())
        );
        break;
    }

    int error = event.GetInt();
    wxString msg = caption + "\n" \
        + wxString::Format("%hs",
        udefrag_get_error_description(error));

    Utils::ShowError(wxT("%ls"),ws(msg));
}

/** @} */
