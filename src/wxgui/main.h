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

#ifndef _UDEFRAG_GUI_MAIN_H_
#define _UDEFRAG_GUI_MAIN_H_

// =======================================================================
//                               Headers
// =======================================================================
#include <string>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#endif

#include <wx/artprov.h>
#include <wx/cmdline.h>
#include <wx/confbase.h>
#include <wx/dir.h>
#include <wx/display.h>
#include <wx/dynlib.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/gbsizer.h>
#include <wx/hyperlink.h>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/mstream.h>
#include <wx/splitter.h>
#include <wx/stdpaths.h>
#include <wx/sysopt.h>
#include <wx/taskbar.h>
#include <wx/textfile.h>
#include <wx/thread.h>
#include <wx/toolbar.h>
#include <wx/uri.h>

#include <wx/clipbrd.h>//genBTC (right-click menu) copy to clipboard
#include <wx/encconv.h> //genbtc for encodings
#include <wx/filepicker.h>//genBTC query tab 3
#include <wx/fontdlg.h>//genBTC (font-chooser)
#include <wx/grid.h>//genbtc
#include <wx/menu.h>//genbtc (right-click menu)
#include <wx/notebook.h>//genbtc
#include <wx/panel.h>//genbtc

#include <wx/sizer.h>//genbtc
#include <wx/stattext.h>//genBTC query tab 3

#if wxUSE_UNICODE
#define wxCharStringFmtSpec "%ls"
#else
#define wxCharStringFmtSpec "%hs"
#endif

/*
* Next definition is very important for mingw:
* _WIN32_IE must be no less than 0x0400
* to include some important constant definitions.
*/
#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif
#include <commctrl.h>

typedef enum {
    TBPF_NOPROGRESS	= 0,
    TBPF_INDETERMINATE	= 0x1,
    TBPF_NORMAL	= 0x2,
    TBPF_ERROR	= 0x4,
    TBPF_PAUSED	= 0x8
} TBPFLAG;

#include "../include/version.h"
#include "../dll/zenwinx/zenwinx.h"
#include "../dll/udefrag/udefrag.h"

// =======================================================================
//                              Constants
// =======================================================================

#define MAX_UTF8_PATH_LENGTH (256 * 1024)
#define LIST_COLUMNS 6 // number of columns in the list of volumes

enum {
    // file menu identifiers

    // NOTE: they share a single event handler
    ID_Analyze = 1,
    ID_Defrag,
    ID_QuickOpt,
    ID_FullOpt,
    ID_MoveToFront,
    ID_MoveToEnd,
    ID_MftOpt,

    ID_Pause,
    ID_Stop,

    ID_ShowReport,

    ID_Repeat,

    ID_SkipRem,
    ID_Rescan,

    ID_Repair,

    ID_WhenDoneNone,
    ID_WhenDoneExit,
    ID_WhenDoneStandby,
    ID_WhenDoneHibernate,
    ID_WhenDoneLogoff,
    ID_WhenDoneReboot,
    ID_WhenDoneShutdown,

    ID_Exit,

    // settings menu identifiers
    ID_LangTranslateOnline,
    ID_LangTranslateOffline,
    ID_LangOpenFolder,

    ID_GuiOptions,

    ID_BootEnable,
    ID_BootScript,

    ID_ChooseFont,

    ID_SortByPath,
    ID_SortBySize,
    ID_SortByCreationDate,
    ID_SortByModificationDate,
    ID_SortByLastAccessDate,

    ID_SortAscending,
    ID_SortDescending,

    // help menu identifiers
    ID_HelpContents,
    ID_HelpBestPractice,
    ID_HelpFaq,
    ID_HelpLegend,

    ID_DebugLog,
    ID_DebugSend,

    // NOTE: they share a single event handler
    ID_HelpUpgradeNone,
    ID_HelpUpgradeStable,
    ID_HelpUpgradeAll,
    ID_HelpUpgradeCheck,

    ID_HelpAbout,

    // event identifiers
    ID_AdjustListColumns,
    ID_AdjustListHeight,
    ID_AdjustFilesListColumns,      //genBTC fileslist.cpp
    ID_AdjustSystemTrayIcon,
    ID_AdjustTaskbarIconOverlay,
    ID_BootChange,
    ID_CacheJob,
    ID_DefaultAction,
    ID_DiskProcessingFailure,
    ID_JobCompletion,
    ID_QueryCompletion,             //genBTC query.cpp
    ID_PopulateList,
    ID_PopulateFilesList,           //genBTC fileslist.cpp
    ID_ReadUserPreferences,
    ID_RedrawMap,
    ID_SelectAll,
    ID_SetWindowTitle,
    ID_ShowUpgradeDialog,
    ID_Shutdown,
    ID_UpdateStatusBar,
    ID_UpdateVolumeInformation,
    ID_UpdateVolumeStatus,

    // tray icon menu identifiers
    ID_ShowHideMenu,
    ID_PauseMenu,
    ID_ExitMenu,
    ID_SelectProperDrive,
    ID_QueryClusters,
    
    ID_ANALYZE,
    ID_WXCOMBOBOX1,
    ID_WXFILEPICKERCTRL1,
    ID_WXTEXTCTRL1,
    ID_WXSTATICTEXT1,
    ID_PERFORMQUERY,

    // language selection menu item, must always be last in the list
    ID_LocaleChange
};

#define MAIN_WINDOW_DEFAULT_WIDTH  900
#define MAIN_WINDOW_DEFAULT_HEIGHT 600
#define MAIN_WINDOW_MIN_WIDTH      640
#define MAIN_WINDOW_MIN_HEIGHT     480
#define DEFAULT_LIST_HEIGHT        145
#define MIN_PANEL_HEIGHT            42

// dialog layout constants
#define SMALL_SPACING  DPI(5)
#define LARGE_SPACING  DPI(11)

#define DEFAULT_DRY_RUN          0
#define DEFAULT_FREE_COLOR_R   255
#define DEFAULT_FREE_COLOR_G   255
#define DEFAULT_FREE_COLOR_B   255
#define DEFAULT_GRID_COLOR_R     0
#define DEFAULT_GRID_COLOR_G     0
#define DEFAULT_GRID_COLOR_B     0
#define DEFAULT_GRID_LINE_WIDTH  1
#define DEFAULT_MAP_BLOCK_SIZE   4
#define DEFAULT_MINIMIZE_TO_SYSTEM_TRAY          0
#define DEFAULT_SECONDS_FOR_SHUTDOWN_REJECTION  60
#define DEFAULT_SHOW_MENU_ICONS                  1
#define DEFAULT_SHOW_PROGRESS_IN_TASKBAR         1
#define DEFAULT_SHOW_TASKBAR_ICON_OVERLAY        1

/* user defined language IDs
   Important: never change their order when adding new translations
   or the selection of the user will be broken */
enum {
    wxUD_LANGUAGE_BOSNIAN = wxLANGUAGE_USER_DEFINED+1,
    wxUD_LANGUAGE_ILOKO,
    wxUD_LANGUAGE_KAPAMPANGAN,
    wxUD_LANGUAGE_NORWEGIAN,
    wxUD_LANGUAGE_WARAY_WARAY,
    wxUD_LANGUAGE_ACOLI,
    wxUD_LANGUAGE_SINHALA_SRI_LANKA,
    wxUD_LANGUAGE_LAST          // must always be last in the list
};

// =======================================================================
//                          Macro definitions
// =======================================================================

/* converts pixels from 96 DPI to the current one */
#define DPI(x) ((int)((double)x * g_scaleFactor))

#define PostCommandEvent(window,id) { \
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,id); \
    wxPostEvent(window,event); \
}

#define ProcessCommandEvent(id) { \
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,id); \
    ProcessEvent(event); \
}

// =======================================================================
//                            Declarations
// =======================================================================

class StatThread: public wxThread {
public:
    StatThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~StatThread() { Wait(); }

    virtual void *Entry();
};

class Log: public wxLog {
public:
    Log()  { delete SetActiveTarget(this); };
    ~Log() { SetActiveTarget(NULL); };

    virtual void DoLog(wxLogLevel level,
        const wxChar *msg,time_t timestamp);
};

class App: public wxApp {
public:
    virtual bool OnInit();
    virtual int  OnExit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser) {
        parser.AddSwitch(wxT("s"),wxT("setup"),wxT("setup"));
    }

    static void InitLocale();
    static void SetLocale(int id);
    static void SaveReportTranslation();

private:
    void Cleanup();

    Log *m_log;
    StatThread *m_statThread;
};

class BtdThread: public wxThread {
public:
    BtdThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~BtdThread() { Wait(); }

    virtual void *Entry();
};

class ConfigThread: public wxThread {
public:
    ConfigThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~ConfigThread() { Wait(); }

    virtual void *Entry();
};

class CrashInfoThread: public wxThread {
public:
    CrashInfoThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~CrashInfoThread() { Wait(); }

    virtual void *Entry();
};

class JobThread: public wxThread {
public:
    JobThread() : wxThread(wxTHREAD_JOINABLE) {
        m_launch = false; Create(); Run();
    }
    ~JobThread() { Wait(); }

    virtual void *Entry();

    bool m_launch;
    wxArrayString *m_volumes;
    udefrag_job_type m_jobType;
    int m_mapSize;
    int m_flags;
    bool singlefile;

private:
    void ProcessVolume(int index);
    static void ProgressCallback(udefrag_progress_info *pi, void *p);
    static int Terminator(void *p);

    char m_letter;
};

class ListThread: public wxThread {
public:
    ListThread() : wxThread(wxTHREAD_JOINABLE) {
        m_rescan = true; Create(); Run();
    }
    ~ListThread() { Wait(); }

    virtual void *Entry();

    bool m_rescan;
};

//genBTC query.cpp Query Thread.
class QueryThread: public wxThread {
public:
    QueryThread() : wxThread(wxTHREAD_JOINABLE) {
        m_startquery = false; Create(); Run();
    }
    ~QueryThread() { Wait(); }

    virtual void *Entry();
    
    void DisplayQueryResults();
    
    bool m_startquery;
    wchar_t *m_querypath;
    int m_flags;
    bool singlefile;
    int m_mapSize;
    char m_letter;
    udefrag_query_type m_qType;
    udefrag_query_parameters *m_qp;

private:
    static void ProgressCallback(udefrag_progress_info *pi, void *p);
    static int Terminator(void *p);    
};

class UpgradeThread: public wxThread {
public:
    UpgradeThread(int level) : wxThread(wxTHREAD_JOINABLE) {
        m_level = level; m_check = true; Create(); Run();
    }
    ~UpgradeThread() { Wait(); }

    virtual void *Entry();

    bool m_check;
    int m_level;

private:
    int ParseVersionString(const char *version);
};

class SystemTrayIcon: public wxTaskBarIcon {
public:
    virtual wxMenu *CreatePopupMenu();

    void OnMenuShowHide(wxCommandEvent& event);
    void OnMenuPause(wxCommandEvent& event);
    void OnMenuExit(wxCommandEvent& event);

    void OnLeftButtonUp(wxTaskBarIconEvent& event);

    DECLARE_EVENT_TABLE()
};

class DrivesList: public wxListView {
public:
    DrivesList(wxWindow* parent, long style)
      : wxListView(parent,wxID_ANY,
        wxDefaultPosition,wxDefaultSize,style) {}
    ~DrivesList() {}

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnSelectionChange(wxListEvent& event);

    DECLARE_EVENT_TABLE()
};

class ListSortInfo{
public:
        ListSortInfo()
        {
            SortAscending = false;
            Column = -1;
        }

        bool SortAscending;
        int Column;
        class FilesList *ListCtrl;

};

//genBTC FilesList.cpp
class FilesList: public wxListCtrl {
public:
    FilesList(wxWindow* parent, long style)
      : wxListCtrl(parent,wxID_ANY,
        wxDefaultPosition,wxDefaultSize,style) {}
    ~FilesList() {}
    // -----------------------------------------------------------------------
    //BEGIN PROTOTYPES FROM wxListView
    // ---------------------
    // focus/selection stuff
    // ---------------------
    // [de]select an item
    void Select(long n, bool on = true){
        SetItemState(n, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);}
    // focus and show the given item
    void Focus(long index)
    {
        SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        EnsureVisible(index);
    }
    // get the currently focused item or -1 if none
    long GetFocusedItem() const{
        return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);}
    // get first and subsequent selected items, return -1 when no more
    long GetNextSelected(long item) const{
        return GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);}
    long GetFirstSelected() const{
        return GetNextSelected(-1);}
    // return true if the item is selected
    bool IsSelected(long index) const{
        return GetItemState(index, wxLIST_STATE_SELECTED) != 0;}
    //END PROTOTYPES FROM wxListView
    // -----------------------------------------------------------------------    
    void OnItemRClick(wxListEvent& event);
    void OnSelect(wxListEvent& event);
    void OnDeSelect(wxListEvent& event);

    void RClickOpenExplorer(wxCommandEvent& event);
    void RClickCopyClipboard(wxCommandEvent& event);
    void RClickSubMenuMoveFiletoDriveX(wxCommandEvent& event);
    void ReSelectProperDrive(wxCommandEvent& event);
    void RClickDefragMoveSingle(wxCommandEvent& event);

    wxListItem GetListItem(long index=-1,long col=-1);

    long currentlyselected;
    wxArrayString *currently_being_workedon_filenames;
    
    int n_lastItem;
    ListSortInfo m_sortinfo; 
    void OnColClick(wxListEvent& event );
    static bool DateCompare(wxString &lsDate1, wxString &lsDate2);

    DECLARE_EVENT_TABLE()
private:

};

class ClusterMap: public wxWindow {
public:
    ClusterMap(wxWindow* parent);
    ~ClusterMap();

    void OnEraseBackground(wxEraseEvent& event);
    void OnPaint(wxPaintEvent& event);

private:
    char *ScaleMap(int scaled_size);

    int m_width;
    int m_height;
    HDC m_cacheDC;
    HBITMAP m_cacheBmp;

    HBRUSH m_brushes[SPACE_STATES];

    DECLARE_EVENT_TABLE()
};

typedef struct _JobsCacheEntry {
    udefrag_job_type jobType;
    udefrag_progress_info pi;
    char *clusterMap;
    bool stopped;
} JobsCacheEntry;

WX_DECLARE_HASH_MAP(int, JobsCacheEntry*, \
    wxIntegerHash, wxIntegerEqual, JobsCache);

class MainFrame: public wxFrame {
public:
    MainFrame();
    ~MainFrame();

    WXLRESULT MSWWindowProc(WXUINT msg,WXWPARAM wParam,WXLPARAM lParam);

    bool CheckForTermination(int time);

    // action menu handlers
    void OnStartJob(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);

    void OnShowReport(wxCommandEvent& event);

    void OnRepeat(wxCommandEvent& event);
    
    void OnRepair(wxCommandEvent& event);

    void OnExit(wxCommandEvent& event);

    // settings menu handlers
    void OnLangTranslateOnline(wxCommandEvent& event);
    void OnLangTranslateOffline(wxCommandEvent& event);
    void OnLangOpenFolder(wxCommandEvent& event);

    void OnGuiOptions(wxCommandEvent& event);

    void OnBootEnable(wxCommandEvent& event);
    void OnBootScript(wxCommandEvent& event);

    void ChooseFont(wxCommandEvent& event);//genBTC fontpicker.

    // help menu handlers
    void OnHelpContents(wxCommandEvent& event);
    void OnHelpBestPractice(wxCommandEvent& event);
    void OnHelpFaq(wxCommandEvent& event);
    void OnHelpLegend(wxCommandEvent& event);

    void OnDebugLog(wxCommandEvent& event);
    void OnDebugSend(wxCommandEvent& event);

    void OnHelpUpgrade(wxCommandEvent& event);
    void OnHelpAbout(wxCommandEvent& event);

    // event handlers
    void OnActivate(wxActivateEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnSize(wxSizeEvent& event);

    void AdjustSystemTrayIcon(wxCommandEvent& event);
    void AdjustTaskbarIconOverlay(wxCommandEvent& event);
    void CacheJob(wxCommandEvent& event);
    void OnBootChange(wxCommandEvent& event);
    void OnDefaultAction(wxCommandEvent& event);
    void OnDiskProcessingFailure(wxCommandEvent& event);
    void OnJobCompletion(wxCommandEvent& event);
    void OnQueryCompletion(wxCommandEvent& event);  //genBTC query.cpp
    void OnLocaleChange(wxCommandEvent& event);
    void ReadUserPreferences(wxCommandEvent& event);
    void RedrawMap(wxCommandEvent& event);
    void SelectAll(wxCommandEvent& event);
    void SetWindowTitle(wxCommandEvent& event);
    void ShowUpgradeDialog(wxCommandEvent& event);
    void Shutdown(wxCommandEvent& event);
    void UpdateStatusBar(wxCommandEvent& event);

    //Volume List (mixed)
    void AdjustListColumns(wxCommandEvent& event);
    void AdjustListHeight(wxCommandEvent& event);
    void OnListSize(wxSizeEvent& event);
    void OnSplitChanged(wxSplitterEvent& event);
    void OnSkipRem(wxCommandEvent& event);
    void OnRescan(wxCommandEvent& event);
    void PopulateList(wxCommandEvent& event);
    void UpdateVolumeInformation(wxCommandEvent& event);
    void UpdateVolumeStatus(wxCommandEvent& event);

    // Files List (new) by genBTC
    void FilesAdjustListColumns(wxCommandEvent& event);
    void FilesOnListSize(wxSizeEvent& event);
    void FilesPopulateList(wxCommandEvent& event);
    void ReSelectProperDrive(wxCommandEvent& event);

    //for Query Menu (new) by genBTC
    void QueryClusters(wxCommandEvent& event);   //genBTC query.cpp

    // common routines
    int  CheckOption(const wxString& name);
    void SetSystemTrayIcon(const wxString& icon, const wxString& tooltip);
    void SetTaskbarProgressState(TBPFLAG flag);
    void SetTaskbarProgressValue(ULONGLONG completed, ULONGLONG total);

    bool m_repeat;
    bool m_skipRem;
    bool m_busy;
    bool m_paused;
    bool m_stopped;

    // overall progress counters
    int m_selected;
    int m_processed;

    SystemTrayIcon *m_systemTrayIcon;

    JobThread *m_jobThread;
    JobsCache m_jobsCache;
    JobsCacheEntry *m_currentJob;

    QueryThread *m_queryThread;

    wxMenu     *m_RClickPopupMenu1;     //genBTC Right Click Popup Menu
    wxMenu     *m_DriveSubMenu;         //genBTC Right Click Popup Menu
    wxTextCtrl *WxTextCtrl1;
private:
    int getmapsize();       //genBTC - used by job.cpp & query.cpp
    void InitQueryMenu();   //genBTC query.cpp
    void InitPopupMenus();              //genBTC Right Click Popup Menu
    void InitMenu();
    void InitToolbar();
    void InitStatusBar();
    void InitVolList();
    void InitFilesList();    //genBTC FilesList.cpp
    void ReadAppConfiguration();
    void ReadUserPreferences();
    void ReleasePause();
    void RemoveTaskbarIconOverlay();
    void SaveAppConfiguration();
    void SetPause();
    void SetTaskbarIconOverlay(
        const wxString& icon,
        const wxString& text);
    int ShowShutdownDialog(int action);

    int  m_x;
    int  m_y;
    int  m_width;
    int  m_height;
    bool m_saved;
    bool m_maximized;
    int  m_separatorPosition;

    // list column widths ratios:
    // 0.5 means that a column acquires
    // a half of the entire list width
    double m_r[LIST_COLUMNS];
    double m_fcolsr[LIST_COLUMNS]; //file-list column ratios //genbtc

    // list column widths:
    // used to check whether the user
    // has changed them or not
    int m_w[LIST_COLUMNS];
    int m_fcolsw[LIST_COLUMNS]; //file-list column widths //genbtc

    // list height
    int m_vListHeight;
    int m_filesListHeight;  //genBTC FilesList.cpp

    wxFont *m_vListFont;
    wxFont *m_filesListFont;  //genBTC FilesList.cpp

    wxString   *m_title;
    wxToolBar  *m_toolBar;
    wxMenuBar  *m_menuBar;
    wxMenuItem *m_subMenuWhenDone;
    wxMenuItem *m_subMenuLanguage;
    wxMenuItem *m_subMenuBootConfig;
    wxMenuItem *m_subMenuSortingConfig;
    wxMenuItem *m_subMenuDebug;
    wxMenuItem *m_subMenuUpgrade;
    wxMenu     *m_menuLanguage;

    wxNotebook* m_notebook1;        //genBTC New Tabs
    wxPanel* m_panel1;              //genBTC New Tabs
    wxSplitterWindow* m_splitter1;  //genBTC New Tabs
    wxPanel* m_panel2;              //genBTC New Tabs
//    wxGrid* m_grid1;                //genBTC New Tabs
    wxPanel* m_panel3;              //genBTC Tab3

    wxSplitterWindow *m_splitter;
    DrivesList       *m_vList;
    ClusterMap       *m_cMap;
    FilesList        *m_filesList;  //genBTC FilesList.cpp
    volume_info      m_volinfocache; //genBTC

    bool m_btdEnabled;
    BtdThread *m_btdThread;

    ConfigThread    *m_configThread;
    CrashInfoThread *m_crashInfoThread;
    ListThread      *m_listThread;
    UpgradeThread   *m_upgradeThread;

    //genBTC Query - tab 3.
    wxButton *PerformQuery;     
    wxStaticText *WxStaticText1;
    
    wxFilePickerCtrl *WxFilePickerCtrl1;
    wxComboBox *WxComboBox1;
    wxButton *Analyze;
    wxPanel *WxNoteBookPage2;
    wxPanel *WxNoteBookPage1;
    wxNotebook *WxNotebook1;
    wxBoxSizer *WxBoxSizer1;    

    DECLARE_EVENT_TABLE()
};

class Utils {
public:
    static bool CheckAdminRights(void);
    static bool DownloadFile(const wxString& url, const wxString& path);
    static void GaRequest(const wxString& path);
    static wxBitmap *LoadPngResource(const wchar_t *name);
    static int MessageDialog(wxFrame *parent,
        const wxString& caption, const wxArtID& icon,
        const wxString& text1, const wxString& text2,
        const wxChar* format, ...);
    static void OpenHandbook(const wxString& page,
        const wxString& anchor = wxEmptyString);
    static bool SetProcessPriority(int priority);
    static void ShellExec(const wxString& file,
        const wxString& action = wxT("open"),
        const wxString& parameters = wxEmptyString,
        int show = SW_SHOW, int flags = 0);
    static void ShowError(const wxChar* format, ...);
    static wxString ConvertChartoWxString(char* input);
    static char* wxStringToChar(wxString input);
    static void DrawSingleRectangleBorder(HDC m_cacheDC,int xblock,int yblock,int line_width,int cell_size,HBRUSH border,HBRUSH infill);
    static void createDirectoryRecursively(const std::wstring &directory);
    static void extendfiltertext(wxString itemtext,wxString *extfiltertext);
    static wxString makefiltertext(wxString itemtext);
};

/* flags for Utils::ShellExec */
#define SHELLEX_SILENT  0x1
#define SHELLEX_NOASYNC 0x2

// =======================================================================
//                     #Defined Functions
// =======================================================================
//from job.cpp
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

//from menu.cpp
#define UD_AppendCheckItem(id) AppendCheckItem(id, wxEmptyString)
#define UD_AppendRadioItem(id) AppendRadioItem(id, wxEmptyString)

#define UD_SetMenuIcon(id, icon) { \
    wxBitmap *pic; wxString string; \
    string.Printf(wxT("%hs%u"),#icon,g_iconSize); \
    pic = Utils::LoadPngResource(string.wc_str()); \
    if(pic) m_menuBar->FindItem(id)->SetBitmap(*pic); \
    delete pic; \
}

#define UD_SetMarginWidth(menu) { \
    wxMenuItemList list = menu->GetMenuItems(); \
    size_t count = list.GetCount(); \
    for(size_t i = 0; i < count; i++) \
        list.Item(i)->GetData()->SetMarginWidth(g_iconSize); \
}

// =======================================================================
//                           Global variables
// =======================================================================

extern MainFrame *g_mainFrame;
extern wxLocale *g_locale;
extern double g_scaleFactor;
extern int g_iconSize;
extern HANDLE g_synchEvent;

#endif /* _UDEFRAG_GUI_MAIN_H_ */
