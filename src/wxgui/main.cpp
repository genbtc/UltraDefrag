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
#include "wx/wxprec.h"
#include "main.h"
#pragma comment(lib, "user32")
#if !defined(__GNUC__)
#include <new.h> // for _set_new_handler
#endif

// Uncomment to test crash reporting facilities.
// NOTE: on Windows 7 you should reset Fault Tolerant
// Heap protection from time to time via the following
// command: rundll32 fthsvc.dll,FthSysprepSpecialize
// Otherwise some of crash tests will fail.
// #define CRASH_TESTS

// =======================================================================
//                            Global variables
// =======================================================================

MainFrame *g_mainFrame = nullptr;
double g_scaleFactor = 1.0f;   // DPI-aware scaling factor
int g_iconSize;                // small icon size
HANDLE g_synchEvent = nullptr;    // synchronization for threads
UINT g_TaskbarIconMsg;         // taskbar icon overlay setup on shell restart

// =======================================================================
//                             Web statistics
// =======================================================================

void *StatThread::Entry()
{
    bool enabled = true; wxString s;
    if(wxGetEnv("UD_DISABLE_USAGE_TRACKING",&s))
        if(s.Cmp("1") == 0) enabled = false;

    if(enabled){
        GA_REQUEST(USAGE_TRACKING);
#ifdef SEND_TEST_REPORTS
        GA_REQUEST(TEST_TRACKING);
#endif
    }

    return nullptr;
}

// =======================================================================
//                    Application startup and shutdown
// =======================================================================

/**
 * @brief Sets icon for system modal message boxes.
 */
BOOL CALLBACK DummyDlgProc(HWND hWnd,
    UINT msg,WPARAM wParam,LPARAM lParam)
{
    HINSTANCE hInst;

    switch(msg){
    case WM_INITDIALOG:
        // set icon for system modal message boxes
        hInst = (HINSTANCE)GetModuleHandle(NULL);
        if(hInst){
            HICON hIcon = LoadIcon(hInst,wxT("appicon"));
            if(hIcon) (void)SetClassLongPtr( \
                hWnd,GCLP_HICON,(LONG_PTR)hIcon);
        }
        // kill our window before showing it :)
        (void)EndDialog(hWnd,1);
        return FALSE;
    case WM_CLOSE:
        // handle it too, just for safety
        (void)EndDialog(hWnd,1);
        return TRUE;
    }
    return FALSE;
}

#if !defined(__GNUC__)
static int out_of_memory_handler(size_t n)
{
    int choice = MessageBox(
        g_mainFrame ? (HWND)g_mainFrame->GetHandle() : NULL,
		wxT("Try to release some memory by closing\n")
		wxT("other applications and click Retry then\n")
		wxT("or click Cancel to terminate the program."),
		wxT("UltraDefrag: out of memory!"),
        MB_RETRYCANCEL | MB_ICONHAND | MB_SYSTEMMODAL);
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
    SetAppName("UltraDefrag");
    wxInitAllImageHandlers();
    if(!wxApp::OnInit())
        return false;

    // set icon for system modal message boxes
    (void)DialogBox(
        (HINSTANCE)GetModuleHandle(NULL),
        wxT("dummy_dialog"),NULL,
        (DLGPROC)DummyDlgProc
    );

    // initialize udefrag library
    if(::udefrag_init_library() < 0){
        wxLogError("Initialization failed!");
        return false;
    }

    // set out of memory handler
#if !defined(__GNUC__)
    winx_set_killer(out_of_memory_handler);
    _set_new_handler(out_of_memory_handler);
    _set_new_mode(1);
#endif

    // enable crash handling
#ifdef ATTACH_DEBUGGER
    AttachDebugger();
#endif

    // uncomment to test out of memory condition
    /*for(int i = 0; i < 1000000000; i++)
        char *p = new char[1024];*/

#ifdef CRASH_TESTS
#ifndef _WIN64
    wchar_t *s1 = new wchar_t[1024];
    wcscpy(s1,wxT("hello"));
    delete s1;
    wcscpy(s1,wxT("world"));
    delete s1;
#else
    // the code above fails to crash
    // on Windows XP 64-bit edition
    void *p = NULL;
    *(char *)p = 0;
#endif
#endif

    // initialize debug log
    wxFileName logpath((".\\logs\\ultradefrag.log"));
    logpath.Normalize();
    wxSetEnv("UD_LOG_FILE_PATH",logpath.GetFullPath());
    ::udefrag_set_log_file_path();

    // initialize logging
    m_log = new Log();

    // use global config object for internal settings
    wxFileConfig *cfg = new wxFileConfig("","",
        "gui.ini","",wxCONFIG_USE_RELATIVE_PATH);
    wxConfigBase::Set(cfg);

	// get initial language selection
	int langid = 0;
	if (cfg->HasGroup("Language")) {
		langid = int(cfg->Read("/Language/Selected", langid));
	}
	else {
		langid = g_locale->GetSystemLanguage();
		//set to english if unknown.
		if (langid == wxLANGUAGE_UNKNOWN)
			langid = wxLANGUAGE_ENGLISH_US;
	}
	// enable i18n support (translations + defaults)
    SetLocale(langid);

    // save report translation on setup
    wxString cmdLine(GetCommandLine());
    if(cmdLine.Find("--setup") != wxNOT_FOUND){
        SaveReportTranslation();
        ::winx_flush_dbg_log(0);
        delete m_log;
        return false;
    }

    // start web statistics
    m_statThread = new StatThread();

    // check for administrative rights
    if(!Utils::CheckAdminRights()){
        wxMessageDialog dlg(nullptr,
            ("Administrative rights are needed to run the program!"),
            ("UltraDefrag"),wxOK | wxICON_ERROR
        );
        dlg.ShowModal(); Cleanup();
        return false;
    }

    // create synchronization event
    g_synchEvent = ::CreateEvent(nullptr,TRUE,FALSE, nullptr);
    if(!g_synchEvent){
        letrace("cannot create synchronization event");
        wxMessageDialog dlg(nullptr,
            ("Cannot create synchronization event!"),
            ("UltraDefrag"),wxOK | wxICON_ERROR
        );
        dlg.ShowModal(); Cleanup();
        return false;
    }

    // keep things DPI-aware
    HDC hdc = ::GetDC(nullptr);
    if(hdc){
        g_scaleFactor = (double)::GetDeviceCaps(hdc,LOGPIXELSX) / 96.0f;
        ::ReleaseDC(nullptr,hdc);
    }
    g_iconSize = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X);
    if(g_iconSize < 20) g_iconSize = 16;
    else if(g_iconSize < 24) g_iconSize = 20;
    else if(g_iconSize < 32) g_iconSize = 24;
    else g_iconSize = 32;

    // support taskbar icon overlay setup on shell restart
    g_TaskbarIconMsg = ::RegisterWindowMessageA("TaskbarButtonCreated");
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
    delete wxConfigBase::Set(nullptr);

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

//This is the Actual equivalent Int Main() Entry point. follow the trail.
IMPLEMENT_APP(App)

// =======================================================================
//                             Main window
// =======================================================================

/**
 * @brief Initializes main window.
 */
MainFrame::MainFrame()
    :wxFrame(nullptr,wxID_ANY,"UltraDefrag")
{
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );  
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );  
    g_mainFrame = this;
    m_vList = nullptr;
    m_cMap = nullptr;
    m_currentJob = nullptr;
    m_busy = false;
    m_paused = false;

    // set main window icon
    wxIconBundle icons;
    int sizes[] = {16,20,22,24,26,32,40,48,64};
    for(int i = 0; i < sizeof(sizes) / sizeof(int); i++){
        icons.AddIcon(wxIcon(wxT("appicon"),
            wxBITMAP_TYPE_ICO_RESOURCE,
            sizes[i],sizes[i])
        );
    }
    SetIcons(icons);

    // read configuration
    ReadAppConfiguration();
    ProcessCommandEvent(this,ID_ReadUserPreferences);

	// set main window title
    wxString instdir;
    if(wxGetEnv(wxT("UD_INSTALL_DIR"),&instdir)){
        wxFileName path(wxGetCwd()); path.Normalize();
        wxString cd = path.GetFullPath();
        itrace("current directory: %ls",ws(cd));
        itrace("installation directory: %ls",ws(instdir));
        if(cd.CmpNoCase(instdir) == 0){
			itrace("current directory matches "
				"installation location, so it isn't portable");
			m_title = new wxString(wxT(VERSIONINTITLE));
		}
		else {
			itrace("current directory differs from "
				"installation location, so it is portable");
			m_title = new wxString(wxT(VERSIONINTITLE_PORTABLE));
		}
	}
	else {
		m_title = new wxString(wxT(VERSIONINTITLE_PORTABLE));
	}
	ProcessCommandEvent(this, ID_SetWindowTitle);

    // set main window size and position
    SetSize(m_width,m_height);
    if(!m_saved){
        CenterOnScreen();
        GetPosition(&m_x,&m_y);
    }
    Move(m_x,m_y);
    if(m_maximized) wxTopLevelWindowMSW::Maximize(true);

	wxTopLevelWindowBase::SetMinSize(wxSize(DPI(MAIN_WINDOW_MIN_WIDTH),DPI(MAIN_WINDOW_MIN_HEIGHT)));

    // create menu, tool and status bars
    InitMenu(); InitToolbar(); InitStatusBar();

	//make sizer1 to hold the the tabbed "notebook". And make the notebook
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	//make a panel inside the notebook to hold the m_splitter
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    // create list of volumes and cluster map (with splitter as parent)
    m_splitter = new wxSplitterWindow(m_panel1,wxID_ANY, wxDefaultPosition,wxDefaultSize, 
                                      wxSP_3D | wxCLIP_CHILDREN);
    m_splitter->SetMinimumPaneSize(DPI(MIN_PANEL_HEIGHT));

    m_vList = new DrivesList(m_splitter,wxLC_REPORT | wxLC_NO_SORT_HEADER | 
                             wxLC_HRULES | wxLC_VRULES | wxBORDER_NONE);

    m_cMap = new ClusterMap(m_splitter);

    m_splitter->SplitHorizontally(m_vList,m_cMap);

	const int height = GetClientSize().GetHeight();
	const int maxPanelHeight = height - DPI(MIN_PANEL_HEIGHT) - m_splitter->GetSashSize();
    if(m_separatorPosition < DPI(MIN_PANEL_HEIGHT)) m_separatorPosition = DPI(MIN_PANEL_HEIGHT);
    else if(m_separatorPosition > maxPanelHeight) m_separatorPosition = maxPanelHeight;
    m_splitter->SetSashPosition(m_separatorPosition);

    // update frame layout so we'll be able to initialize
    // list of volumes and cluster map properly
    wxSizeEvent evt(wxSize(m_width,m_height));
    GetEventHandler()->ProcessEvent(evt);
    m_splitter->UpdateSize();

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
	m_notebook1->AddPage( m_panel1, "Drives", false );

	//make a 2nd panel inside the notebook to hold the 2nd page(a grid)
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    m_filesList = new FilesList(m_panel2,wxLC_REPORT /* | wxLC_SINGLE_SEL  | wxLC_NO_SORT_HEADER*/ \
                                        | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES | wxBORDER_NONE);
    InitFilesList();

	//make sizer3 to Fit the page2list, and initialize it.
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	bSizer3->Add( m_filesList, 1, wxEXPAND, 1 );
	m_panel2->SetSizer( bSizer3 );
    bSizer3->Fit( m_panel2 );

	//Finish Tab 2 - Add the Panel2(page2list+sizer3) to the notebook.
	m_notebook1->AddPage( m_panel2, "Files", false );

    //Finish Notebook & initialize
	bSizer1->Add( m_notebook1, 1, wxEXPAND, 1 );
	this->SetSizer( bSizer1 );

    // check the boot time defragmenter presence
    wxFileName btdFile(("%SystemRoot%\\system32\\defrag_native.exe"));
    btdFile.Normalize();
	const bool btd = btdFile.FileExists();
    m_menuBar->FindItem(ID_BootEnable)->Enable(btd);
    m_menuBar->FindItem(ID_BootScript)->Enable(btd);
    m_toolBar->EnableTool(ID_BootEnable,btd);
    m_toolBar->EnableTool(ID_BootScript,btd);
    if(btd && ::winx_bootex_check(L"defrag_native") > 0){
        m_menuBar->FindItem(ID_BootEnable)->Check(true);
        m_toolBar->ToggleTool(ID_BootEnable,true);
        m_btdEnabled = true;
    } else {
        m_btdEnabled = false;
    }

    // launch threads for time consuming operations
    if (btd) m_btdThread = new BtdThread();
    else m_btdThread = nullptr;
    m_configThread = new ConfigThread();

	const auto cfg = wxConfigBase::Get();
	const int ulevel = int(cfg->Read("/Upgrade/Level", 1));
	auto item = m_menuBar->FindItem(ID_HelpUpgradeNone + ulevel);
    if(item) item->Check();

    m_upgradeThread = new UpgradeThread(ulevel);

    // set system tray icon
    m_systemTrayIcon = new SystemTrayIcon();
    if(!m_systemTrayIcon->IsOk()){
        etrace("system tray icon initialization failed");
        wxSetEnv("UD_MINIMIZE_TO_SYSTEM_TRAY","0");
    }
    ProcessCommandEvent(this,ID_AdjustSystemTrayIcon);

	// set localized text
	ProcessCommandEvent(this, ID_LocaleChange + g_locale->GetLanguage());

    // allow disk processing
    m_jobThread = new JobThread();

    //create query thread to perform queries without blocking the GUI
    //(sort of like jobs) - may not be good to have both possibly running at once.
    //Create Query Tab, Tab #3.
    InitQueryMenu();
    
    UD_DisableTool(ID_Stop);    //change stop icon to be not always enabled.
}

/**
 * @brief Destructor for main window. Save Config. Free Resources. Close.
 */
MainFrame::~MainFrame()
{
    // terminate threads
    ProcessCommandEvent(this,ID_Stop);
    ::SetEvent(g_synchEvent);
    delete m_btdThread;
    delete m_configThread;
    delete m_jobThread;
    delete m_listThread;

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
 * @brief Returns true if the program is going to be terminated.
 * @param[in] time timeout interval, in milliseconds.
 */
bool MainFrame::CheckForTermination(int time)
{
	const DWORD result = ::WaitForSingleObject(g_synchEvent,DWORD(time));
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
    // action menu
    EVT_MENU_RANGE(ID_Analyze, ID_MftOpt,
                   MainFrame::OnStartJob)
    // includes jobs: ID_Analyze = 1,    ID_Defrag,
    //      ID_QuickOpt,    ID_FullOpt,    ID_MftOpt,
    // and now     ID_MoveToFront,     ID_MoveToEnd,
    // but we dont use those in this menu

    EVT_MENU(ID_Pause, MainFrame::OnPause)
    EVT_MENU(ID_Stop,  MainFrame::OnStop)

    EVT_MENU(ID_ShowReport, MainFrame::OnShowReport)

    EVT_MENU(ID_Repeat,  MainFrame::OnRepeat)

    EVT_MENU(ID_SkipRem, MainFrame::OnSkipRem)
    EVT_MENU(ID_Rescan,  MainFrame::OnRescan)

    EVT_MENU(ID_Repair,  MainFrame::OnRepair)

    EVT_MENU(ID_Exit, MainFrame::OnExit)

    // settings menu
    EVT_MENU(ID_LangTranslateOnline, MainFrame::OnLangTranslateOnline)
    EVT_MENU(ID_LangTranslateOffline, MainFrame::OnLangTranslateOffline)
    EVT_MENU(ID_LangOpenFolder, MainFrame::OnLangOpenFolder)

    EVT_MENU_RANGE(ID_LocaleChange, ID_LocaleChange \
        + wxUD_LANGUAGE_LAST, MainFrame::OnLocaleChange)

    EVT_MENU(ID_GuiOptions, MainFrame::OnGuiOptions)

    EVT_MENU(ID_BootEnable, MainFrame::OnBootEnable)
    EVT_MENU(ID_BootScript, MainFrame::OnBootScript)

    EVT_MENU(ID_ChooseFont, MainFrame::ChooseFontPickerDialog)          //genBTC
    // help menu
    EVT_MENU(ID_HelpContents,     MainFrame::OnHelpContents)
    EVT_MENU(ID_HelpBestPractice, MainFrame::OnHelpBestPractice)
    EVT_MENU(ID_HelpFaq,          MainFrame::OnHelpFaq)
    EVT_MENU(ID_HelpLegend,       MainFrame::OnHelpLegend)

    EVT_MENU(ID_DebugLog,  MainFrame::OnDebugLog)
    EVT_MENU(ID_DebugSend, MainFrame::OnDebugSend)

    EVT_MENU_RANGE(ID_HelpUpgradeNone,
                   ID_HelpUpgradeCheck,
                   MainFrame::OnHelpUpgrade)
    EVT_MENU(ID_HelpAbout, MainFrame::OnHelpAbout)

    // event handlers
    EVT_ACTIVATE(MainFrame::OnActivate)
    EVT_MOVE(MainFrame::OnMove)
    EVT_SIZE(MainFrame::OnSize)

    EVT_MENU(ID_AdjustListColumns, MainFrame::AdjustListColumns)
    EVT_MENU(ID_AdjustListHeight,  MainFrame::AdjustListHeight)
    EVT_MENU(ID_AdjustFilesListColumns, MainFrame::FilesAdjustListColumns)//genBTC
    EVT_MENU(ID_AdjustSystemTrayIcon,     MainFrame::AdjustSystemTrayIcon)
    EVT_MENU(ID_AdjustTaskbarIconOverlay, MainFrame::AdjustTaskbarIconOverlay)
    EVT_MENU(ID_BootChange,        MainFrame::OnBootChange)
    EVT_MENU(ID_CacheJob,          MainFrame::CacheJob)
    EVT_MENU(ID_DefaultAction,     MainFrame::OnDefaultAction)
    EVT_MENU(ID_DiskProcessingFailure, MainFrame::OnDiskProcessingFailure)
    EVT_MENU(ID_JobCompletion,     MainFrame::OnJobCompletion)
    EVT_MENU(ID_QueryCompletion,     MainFrame::OnQueryCompletion)  //genBTC query.cpp
    EVT_MENU(ID_PopulateList,      MainFrame::PopulateList)
    EVT_MENU(ID_PopulateFilesList,      MainFrame::FilesPopulateList) //genBTC
    EVT_MENU(ID_ReadUserPreferences,   MainFrame::ReadUserPreferences)
    EVT_MENU(ID_RedrawMap,         MainFrame::RedrawMap)
    EVT_MENU(ID_SelectAll,         MainFrame::SelectAll)
    EVT_MENU(ID_SetWindowTitle,    MainFrame::SetWindowTitle)
    EVT_MENU(ID_ShowUpgradeDialog, MainFrame::ShowUpgradeDialog)
    EVT_MENU(ID_Shutdown,          MainFrame::Shutdown)
    EVT_MENU(ID_UpdateStatusBar,   MainFrame::UpdateStatusBar)
    EVT_MENU(ID_UpdateVolumeInformation, MainFrame::UpdateVolumeInformation)
    EVT_MENU(ID_UpdateVolumeStatus,      MainFrame::UpdateVolumeStatus)
    EVT_MENU(ID_SelectProperDrive, MainFrame::ReSelectProperDrive)  //genBTC
    EVT_MENU(ID_QueryClusters, MainFrame::QueryClusters)    //genBTC query.cpp
    EVT_BUTTON(ID_PERFORMQUERY,MainFrame::QueryClusters)
END_EVENT_TABLE()

// =======================================================================
//                            Event handlers
// =======================================================================

WXLRESULT MainFrame::MSWWindowProc(WXUINT msg,WXWPARAM wParam,WXLPARAM lParam)
{
    if(msg == g_TaskbarIconMsg){
        // handle shell restart
        QueueCommandEvent(this,ID_AdjustTaskbarIconOverlay);
        return 0;
    }
    return wxFrame::MSWWindowProc(msg,wParam,lParam);
}

void MainFrame::SetWindowTitle(wxCommandEvent& event)
{
    if(event.GetString().IsEmpty()){
        if(CheckOption("UD_DRY_RUN")){
            SetTitle(*m_title + " (Dry Run)");
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
    if(event.GetActive() && m_vList)
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
    if(CheckOption("UD_MINIMIZE_TO_SYSTEM_TRAY") && IsIconized()) Hide();

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
    Utils::OpenHandbook("index.html");
}

void MainFrame::OnHelpBestPractice(wxCommandEvent& WXUNUSED(event))
{
    Utils::OpenHandbook("Tips.html");
}

void MainFrame::OnHelpFaq(wxCommandEvent& WXUNUSED(event))
{
    Utils::OpenHandbook("FAQ.html");
}

void MainFrame::OnHelpLegend(wxCommandEvent& WXUNUSED(event))
{
    Utils::OpenHandbook("GUI.html","cluster_map_legend");
}

/** @} */
