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
 * @file main.cpp
 * @brief Main window.
 * @addtogroup MainWindow
 * @{
 */

// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.

// =======================================================================
//                            Declarations
// =======================================================================

#include "main.h"

#if !defined(__GNUC__)
#include <new.h> // for _set_new_handler
#endif

// =======================================================================
//                            Global variables
// =======================================================================

MainFrame *g_mainFrame = NULL;
double g_scaleFactor = 1.0f;   // DPI-aware scaling factor
int g_iconSize;                // small icon size
HANDLE g_synchEvent = NULL;    // synchronization for threads
UINT g_TaskbarIconMsg;         // taskbar icon overlay setup on shell restart
PVOID g_jpPtr = NULL;       //pointer back to the udefrag-internals jp-> variable.

// =======================================================================
//                             Web statistics
// =======================================================================

void *StatThread::Entry()
{
    bool enabled = true; wxString s;
    if(wxGetEnv(wxT("UD_DISABLE_USAGE_TRACKING"),&s))
        if(s.Cmp(wxT("1")) == 0) enabled = false;

    if(enabled){
#ifndef _WIN64
        Utils::GaRequest(wxT("/appstat/gui-x86.html"));
#else
    #if defined(_IA64_)
        Utils::GaRequest(wxT("/appstat/gui-ia64.html"));
    #else
        Utils::GaRequest(wxT("/appstat/gui-x64.html"));
    #endif
#endif
    }

    return NULL;
}

// =======================================================================
//                    Application startup and shutdown
// =======================================================================

#if !defined(__GNUC__)
static int out_of_memory_handler(size_t n)
{
    int choice = MessageBox(
        g_mainFrame ? (HWND)g_mainFrame->GetHandle() : NULL,
        wxT("Try to release some memory by closing\n")
        wxT("other applications and click Retry then\n")
        wxT("or click Cancel to terminate the program."),
        wxT("UltraDefrag: out of memory!"),
        MB_RETRYCANCEL | MB_ICONHAND);
    if(choice == IDCANCEL){
        winx_flush_dbg_log(FLUSH_IN_OUT_OF_MEMORY);
        if(g_mainFrame) // remove system tray icon
            delete g_mainFrame->m_systemTrayIcon;
        exit(3); return 0;
    }
    return 1;
}
#endif

/**
 * @brief Initializes the application.
 */
bool App::OnInit()
{
    // initialize wxWidgets
    SetAppName(wxT("UltraDefrag"));
    wxInitAllImageHandlers();
    if(!wxApp::OnInit())
        return false;

    // initialize udefrag library
    if(::udefrag_init_library() < 0){
        wxLogError(wxT("Initialization failed!"));
        return false;
    }

    // set out of memory handler
#if !defined(__GNUC__)
    winx_set_killer(out_of_memory_handler);
    _set_new_handler(out_of_memory_handler);
    _set_new_mode(1);
#endif

    // initialize debug log
    wxFileName logpath(wxT(".\\logs\\ultradefrag.log"));
    logpath.Normalize();
    wxSetEnv(wxT("UD_LOG_FILE_PATH"),logpath.GetFullPath());
    ::udefrag_set_log_file_path();

    // initialize logging
    m_log = new Log();

    // use global config object for internal settings
    wxFileConfig *cfg = new wxFileConfig(wxT(""),wxT(""),
        wxT("gui.ini"),wxT(""),wxCONFIG_USE_RELATIVE_PATH);
    wxConfigBase::Set(cfg);

    // enable i18n support
    InitLocale();

    // save report translation on setup
    wxString cmdLine(GetCommandLine());
    if(cmdLine.Find(wxT("--setup")) != wxNOT_FOUND){
        SaveReportTranslation();
        ::winx_flush_dbg_log(0);
        delete m_log;
        return false;
    }

    // start web statistics
    m_statThread = new StatThread();

    // check for administrative rights
    if(!Utils::CheckAdminRights()){
        wxMessageDialog dlg(NULL,
            wxT("Administrative rights are needed to run the program!"),
            wxT("UltraDefrag"),wxOK | wxICON_ERROR
        );
        dlg.ShowModal(); Cleanup();
        return false;
    }

    // create synchronization event
    g_synchEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL);
    if(!g_synchEvent){
        letrace("cannot create synchronization event");
        wxMessageDialog dlg(NULL,
            wxT("Cannot create synchronization event!"),
            wxT("UltraDefrag"),wxOK | wxICON_ERROR
        );
        dlg.ShowModal(); Cleanup();
        return false;
    }

    // keep things DPI-aware
    HDC hdc = ::GetDC(NULL);
    if(hdc){
        g_scaleFactor = (double)::GetDeviceCaps(hdc,LOGPIXELSX) / 96.0f;
        ::ReleaseDC(NULL,hdc);
    }
    g_iconSize = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
    if(g_iconSize < 20) g_iconSize = 16;
    else if(g_iconSize < 24) g_iconSize = 20;
    else if(g_iconSize < 32) g_iconSize = 24;
    else g_iconSize = 32;

    // support taskbar icon overlay setup on shell restart
    g_TaskbarIconMsg = ::RegisterWindowMessage(wxT("TaskbarButtonCreated"));
    if(!g_TaskbarIconMsg) letrace("cannot register TaskbarButtonCreated message");

    // create main window
    g_mainFrame = new MainFrame();
    g_mainFrame->Show(true);
    SetTopWindow(g_mainFrame);
    return true;
}

/**
 * @brief Frees application resources.
 */
void App::Cleanup()
{
    // flush configuration to disk
    delete wxConfigBase::Set(NULL);

    // stop web statistics
    delete m_statThread;

    // deinitialize logging
    ::winx_flush_dbg_log(0);
    delete m_log;
}

/**
 * @brief Deinitializes the application.
 */
int App::OnExit()
{
    Cleanup();
    return wxApp::OnExit();
}

IMPLEMENT_APP(App)

// =======================================================================
//                             Main window
// =======================================================================

/**
 * @brief Initializes main window.
 */
MainFrame::MainFrame()
    :wxFrame(NULL,wxID_ANY,wxT("UltraDefrag"))
{
    g_mainFrame = this;
    m_cMap = NULL;
    m_currentJob = NULL;
    m_busy = false;
    m_paused = false;

    // set main window icon
    SetIcons(wxICON(appicon));

    // read configuration
    ReadAppConfiguration();
    ProcessCommandEvent(EventID_ReadUserPreferences);

    // set main window title
    wxString *instdir = new wxString();
    //genBTC re-arranged the below, A LOT.
    wxStandardPaths stdpaths;
    wxFileName exepath(stdpaths.GetExecutablePath());
    wxString cd = exepath.GetPath();
    if((wxGetEnv(wxT("UD_INSTALL_DIR"),instdir))&&(cd.CmpNoCase(*instdir) == 0)) {
        itrace("current directory matches "
            "installation location, so it isn't portable");
        itrace("installation location: %ls",instdir->wc_str());
        m_title = new wxString(wxT(VERSIONINTITLE));
    } else {
        itrace("current directory differs from "
            "installation location, so it is portable");
        itrace("current directory: %ls",cd.wc_str());
        wxSetEnv(wxT("UD_IS_PORTABLE"),wxT("1"));
        m_title = new wxString(wxT(VERSIONINTITLE_PORTABLE));
    }
    //genBTC re-arranged the above, A LOT.
    ProcessCommandEvent(EventID_SetWindowTitle);
    delete instdir;

    // set main window size and position
    SetSize(m_width,m_height);
    if(!m_saved){
        CenterOnScreen();
        GetPosition(&m_x,&m_y);
    }
    Move(m_x,m_y);
    if(m_maximized) Maximize(true);

    SetMinSize(wxSize(DPI(MAIN_WINDOW_MIN_WIDTH),DPI(MAIN_WINDOW_MIN_HEIGHT)));

    // create menu, tool and status bars
    InitMenu(); InitToolbar(); InitStatusBar();

	//make sizer1 to hold the the tabbed "notebook". And make the notebook
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	//make a panel inside the notebook to hold the m_splitter
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    // create list of volumes and cluster map
    // - don't - use live update style to avoid horizontal scrollbar appearance on list resizing
    m_splitter = new wxSplitterWindow(m_panel1,wxID_ANY,
        wxDefaultPosition,wxDefaultSize,
        wxSP_3D/* | wxSP_LIVE_UPDATE*/ | wxCLIP_CHILDREN);
    m_splitter->SetMinimumPaneSize(DPI(MIN_PANEL_HEIGHT));

    m_vList = new DrivesList(m_splitter,wxLC_REPORT | \
        wxLC_NO_SORT_HEADER | wxLC_HRULES | wxLC_VRULES | wxBORDER_NONE);
    //LONG_PTR style = ::GetWindowLongPtr((HWND)m_vList->GetHandle(),GWL_STYLE);
    //style |= LVS_SHOWSELALWAYS; ::SetWindowLongPtr((HWND)m_vList->GetHandle(),GWL_STYLE,style);
    m_cMap = new ClusterMap(m_splitter);

    m_splitter->SplitHorizontally(m_vList,m_cMap);

    int height = GetClientSize().GetHeight();
    int maxPanelHeight = height - DPI(MIN_PANEL_HEIGHT) - m_splitter->GetSashSize();
    if(m_separatorPosition < DPI(MIN_PANEL_HEIGHT)) m_separatorPosition = DPI(MIN_PANEL_HEIGHT);
    else if(m_separatorPosition > maxPanelHeight) m_separatorPosition = maxPanelHeight;
    m_splitter->SetSashPosition(m_separatorPosition);

    // update frame layout so we'll be able to initialize
    // list of volumes and cluster map properly
    wxSizeEvent evt(wxSize(m_width,m_height));
    ProcessEvent(evt); m_splitter->UpdateSize();

    InitVolList();
    m_vList->SetFocus();
    // populate list of volumes
    m_listThread = new ListThread();

    //make sizer2 to Fit the splitter, and initialize it.
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	bSizer2->Add( m_splitter, 1, wxEXPAND, 1 );
	m_panel1->SetSizer( bSizer2 );

	//Finish Tab1 - Add the Panel1(Splitter+sizer2) to the notebook.
	m_notebook1->AddPage( m_panel1, wxT("Drives"), false );

	//make a 2nd panel inside the notebook to hold the 2nd page(a grid)
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    m_filesList = new FilesList(m_panel2,wxLC_REPORT | \
        wxLC_HRULES | wxLC_VRULES | wxBORDER_NONE);
    InitFilesList();
    //m_listfilesThread = new ListFilesThread();//genBTC

	//make sizer3 to Fit the page2list, and initialize it.
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	bSizer3->Add( m_filesList, 1, wxEXPAND, 1 );
	m_panel2->SetSizer( bSizer3 );
    bSizer3->Fit( m_panel2 );

	//Finish Tab 2 - Add the Panel2(page2list+sizer3) to the notebook.
	m_notebook1->AddPage( m_panel2, wxT("Files"), false );

    //Finish Notebook & initialize
	bSizer1->Add( m_notebook1, 1, wxEXPAND, 1 );
	this->SetSizer( bSizer1 );

    // check the boot time defragmenter presence
    wxFileName btdFile(wxT("%SystemRoot%\\system32\\defrag_native.exe"));
    btdFile.Normalize();
    bool btd = btdFile.FileExists();
    m_menuBar->FindItem(EventID_BootEnable)->Enable(btd);
    m_menuBar->FindItem(EventID_BootScript)->Enable(btd);
    m_toolBar->EnableTool(EventID_BootEnable,btd);
    m_toolBar->EnableTool(EventID_BootScript,btd);
    if(btd && ::winx_bootex_check(L"defrag_native") > 0){
        m_menuBar->FindItem(EventID_BootEnable)->Check(true);
        m_toolBar->ToggleTool(EventID_BootEnable,true);
        m_btdEnabled = true;
    } else {
        m_btdEnabled = false;
    }

    // launch threads for time consuming operations
    m_btdThread = btd ? new BtdThread() : NULL;
    m_configThread = new ConfigThread();
    //m_crashInfoThread = new CrashInfoThread();    //genBTC disabled crash-info-thread. was causing debugging issues.

    wxConfigBase *cfg = wxConfigBase::Get();
    int ulevel = (int)cfg->Read(wxT("/Upgrade/Level"),1);
    wxMenuItem *item = m_menuBar->FindItem(EventID_HelpUpgradeNone + ulevel);
    if(item) item->Check();

    m_upgradeThread = new UpgradeThread(ulevel);

    // set system tray icon
    m_systemTrayIcon = new SystemTrayIcon();
    if(!m_systemTrayIcon->IsOk()){
        etrace("system tray icon initialization failed");
        wxSetEnv(wxT("UD_MINIMIZE_TO_SYSTEM_TRAY"),wxT("0"));
    }
    SetSystemTrayIcon(wxT("tray"),wxT("UltraDefrag"));

    // set localized text
    ProcessCommandEvent(EventID_LocaleChange \
        + g_locale->GetLanguage());

    // allow disk processing
    m_jobThread = new JobThread();
}

/**
 * @brief Deinitializes main window.
 */
MainFrame::~MainFrame()
{
    // terminate threads
    ProcessCommandEvent(EventID_Stop);
    ::SetEvent(g_synchEvent);
    delete m_btdThread;
    delete m_configThread;
    //delete m_crashInfoThread;//genBTC stopped the crashinfo thread.
    delete m_jobThread;
    delete m_listThread;
    //delete m_listfilesThread;//genbtc

    // save configuration
    SaveAppConfiguration();
    delete m_upgradeThread;

    // remove system tray icon
    delete m_systemTrayIcon;

    // free resources
    ::CloseHandle(g_synchEvent);
    delete m_title;
}

/**
 * @brief Returns true if the program
 * is going to be terminated.
 * @param[in] time timeout interval,
 * in milliseconds.
 */
bool MainFrame::CheckForTermination(int time)
{
    DWORD result = ::WaitForSingleObject(g_synchEvent,(DWORD)time);
    if(result == WAIT_FAILED){
        letrace("synchronization failed");
        return true;
    }
    return result == WAIT_OBJECT_0 ? true : false;
}

// =======================================================================
//                             Event table
// =======================================================================

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    // file menu
    EVT_MENU_RANGE(EventID_Analyze, EventID_MftOpt,
                   MainFrame::OnStartJob)
    EVT_MENU(EventID_Pause, MainFrame::OnPause)
    EVT_MENU(EventID_Stop,  MainFrame::OnStop)

    EVT_MENU(EventID_Repeat,  MainFrame::OnRepeat)

    EVT_MENU(EventID_SkipRem, MainFrame::OnSkipRem)
    EVT_MENU(EventID_Rescan,  MainFrame::OnRescan)

    EVT_MENU(EventID_Repair,  MainFrame::OnRepair)

    EVT_MENU(EventID_Exit, MainFrame::OnExit)

    // report menu
    EVT_MENU(EventID_ShowReport, MainFrame::OnShowReport)

    // settings menu
    EVT_MENU_RANGE(EventID_LangShowLog, EventID_LangSubmit,
                   MainFrame::OnLangOpenTransifex)
    EVT_MENU(EventID_LangOpenFolder, MainFrame::OnLangOpenFolder)

    EVT_MENU_RANGE(EventID_LocaleChange, EventID_LocaleChange \
        + wxUD_LANGUAGE_LAST, MainFrame::OnLocaleChange)

    EVT_MENU(EventID_GuiOptions, MainFrame::OnGuiOptions)

    EVT_MENU(EventID_BootEnable, MainFrame::OnBootEnable)
    EVT_MENU(EventID_BootScript, MainFrame::OnBootScript)

    EVT_MENU(EventID_ReportOptions, MainFrame::OnReportOptions)

    // help menu
    EVT_MENU(EventID_HelpContents,     MainFrame::OnHelpContents)
    EVT_MENU(EventID_HelpBestPractice, MainFrame::OnHelpBestPractice)
    EVT_MENU(EventID_HelpFaq,          MainFrame::OnHelpFaq)
    EVT_MENU(EventID_HelpLegend,       MainFrame::OnHelpLegend)

    EVT_MENU(EventID_DebugLog,  MainFrame::OnDebugLog)
    EVT_MENU(EventID_DebugSend, MainFrame::OnDebugSend)

    EVT_MENU_RANGE(EventID_HelpUpgradeNone,
                   EventID_HelpUpgradeCheck,
                   MainFrame::OnHelpUpgrade)
    EVT_MENU(EventID_HelpAbout, MainFrame::OnHelpAbout)

    // event handlers
    EVT_ACTIVATE(MainFrame::OnActivate)
    EVT_MOVE(MainFrame::OnMove)
    EVT_SIZE(MainFrame::OnSize)

    EVT_MENU(EventID_AdjustListColumns, MainFrame::AdjustListColumns)
    EVT_MENU(EventID_AdjustListHeight,  MainFrame::AdjustListHeight)
    EVT_MENU(EventID_AdjustFilesListColumns, MainFrame::FilesAdjustListColumns)//genBTC
    EVT_MENU(EventID_AdjustFilesListHeight,  MainFrame::FilesAdjustListHeight)//genBTC
    EVT_MENU(EventID_AdjustSystemTrayIcon,     MainFrame::AdjustSystemTrayIcon)
    EVT_MENU(EventID_AdjustTaskbarIconOverlay, MainFrame::AdjustTaskbarIconOverlay)
    EVT_MENU(EventID_BootChange,        MainFrame::OnBootChange)
    EVT_MENU(EventID_CacheJob,          MainFrame::CacheJob)
    EVT_MENU(EventID_DefaultAction,     MainFrame::OnDefaultAction)
    EVT_MENU(EventID_DiskProcessingFailure, MainFrame::OnDiskProcessingFailure)
    EVT_MENU(EventID_JobCompletion,     MainFrame::OnJobCompletion)
    EVT_MENU(EventID_PopulateList,      MainFrame::PopulateList)
    EVT_MENU(EventID_PopulateFilesList,      MainFrame::FilesPopulateList) //genBTC
    EVT_MENU(EventID_ReadUserPreferences,   MainFrame::ReadUserPreferences)
    EVT_MENU(EventID_RedrawMap,         MainFrame::RedrawMap)
    EVT_MENU(EventID_SelectAll,         MainFrame::SelectAll)
    EVT_MENU(EventID_SetWindowTitle,    MainFrame::SetWindowTitle)
    EVT_MENU(EventID_ShowUpgradeDialog, MainFrame::ShowUpgradeDialog)
    EVT_MENU(EventID_Shutdown,          MainFrame::Shutdown)
    EVT_MENU(EventID_UpdateStatusBar,   MainFrame::UpdateStatusBar)
    EVT_MENU(EventID_UpdateVolumeInformation, MainFrame::UpdateVolumeInformation)
    EVT_MENU(EventID_UpdateVolumeStatus,      MainFrame::UpdateVolumeStatus)
    EVT_MENU(EventID_FilesAnalyzedUpdateFilesList, MainFrame::FilesAnalyzedUpdateFilesList)//genBTC
END_EVENT_TABLE()

// =======================================================================
//                            Event handlers
// =======================================================================

WXLRESULT MainFrame::MSWWindowProc(WXUINT msg,WXWPARAM wParam,WXLPARAM lParam)
{
    if(msg == g_TaskbarIconMsg){
        // handle shell restart
        PostCommandEvent(this,EventID_AdjustTaskbarIconOverlay);
        return 0;
    }
    return wxFrame::MSWWindowProc(msg,wParam,lParam);
}

void MainFrame::SetWindowTitle(wxCommandEvent& event)
{
    if(event.GetString().IsEmpty()){
        if(CheckOption(wxT("UD_DRY_RUN"))){
            SetTitle(*m_title + wxT(" (dry run)"));
        } else {
            SetTitle(*m_title);
        }
    } else {
        SetTitle(event.GetString());
    }
}

void MainFrame::OnActivate(wxActivateEvent& event)
{
    /* suggested by Brian Gaff */
    if(event.GetActive())
        m_vList->SetFocus();
    event.Skip();
}

void MainFrame::OnMove(wxMoveEvent& event)
{
    if(!IsMaximized() && !IsIconized()){
        GetPosition(&m_x,&m_y);
        GetSize(&m_width,&m_height);
    }

    // hide window on minimization if system tray icon is turned on
    if(CheckOption(wxT("UD_MINIMIZE_TO_SYSTEM_TRAY")) && IsIconized()) Hide();

    event.Skip();
}

void MainFrame::OnSize(wxSizeEvent& event)
{
    if(!IsMaximized() && !IsIconized())
        GetSize(&m_width,&m_height);
    if(m_cMap) m_cMap->Refresh();
    event.Skip();
}

// =======================================================================
//                            Menu handlers
// =======================================================================

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

// help menu handlers
void MainFrame::OnHelpContents(wxCommandEvent& WXUNUSED(event))
{
    Utils::OpenHandbook(wxT("index.html"));
}

void MainFrame::OnHelpBestPractice(wxCommandEvent& WXUNUSED(event))
{
    Utils::OpenHandbook(wxT("Tips.html"));
}

void MainFrame::OnHelpFaq(wxCommandEvent& WXUNUSED(event))
{
    Utils::OpenHandbook(wxT("FAQ.html"));
}

void MainFrame::OnHelpLegend(wxCommandEvent& WXUNUSED(event))
{
    Utils::OpenHandbook(wxT("GUI.html"),wxT("cluster_map_legend"));
}

void MainFrame::SelectAll(wxCommandEvent& WXUNUSED(event))
{
    for(int i = 0; i < m_filesList->GetItemCount(); i++)
        m_filesList->Select(i);
    m_filesList->Focus(0);
    for(int i = 0; i < m_vList->GetItemCount(); i++)
        m_vList->Select(i);
    m_vList->Focus(0);
}

/** @} */
unsigned WindowsTickToUnixSeconds(ULONGLONG windowsTicks)
{
   ULONGLONG secs;
   time_t t;

   secs = (windowsTicks / WINDOWS_TICK - SEC_TO_UNIX_EPOCH);
   t = (time_t) secs;
   if (secs != (ULONGLONG) t)    // checks for truncation/overflow/underflow
      return (time_t) -1;   // value not representable as a POSIX time
   return t;
}
