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
 * @file config.cpp
 * @brief Configuration.
 * @addtogroup Configuration
 * @{
 */

// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.

// =======================================================================
//                            Declarations
// =======================================================================
#include "wx/wxprec.h"
#include "main.h"

extern "C" {
#define lua_c
#include "../lua5.1/lua.h"
#include "../lua5.1/lauxlib.h"
#include "../lua5.1/lualib.h"
}

// =======================================================================
//                      Application configuration
// =======================================================================
//reference these variables instead of magic-strings all over the source-files.
wxString OPTIONSDIR = (".\\options\\");
wxString OPTIONSFILE = OPTIONSDIR + ("options.lua");
//options file should be in its own directory because of the change-tracker
//change-tracker should not care about anything else being modified.

/**
 * @brief Reads application configuration.
 * @note Intended to be called once somewhere
 * in the main frame constructor.
 */
void MainFrame::ReadAppConfiguration()
{
    wxConfigBase *cfg = wxConfigBase::Get();

    // get window size and position
    m_saved = cfg->HasGroup(("MainFrame"));
    m_x = (int)cfg->Read(("/MainFrame/x"),0l);
    m_y = (int)cfg->Read(("/MainFrame/y"),0l);
    m_width = (int)cfg->Read(("/MainFrame/width"),
        DPI(MAIN_WINDOW_DEFAULT_WIDTH));
    m_height = (int)cfg->Read(("/MainFrame/height"),
        DPI(MAIN_WINDOW_DEFAULT_HEIGHT));

    // validate width and height
    wxDisplay display;
    if(m_width < DPI(MAIN_WINDOW_MIN_WIDTH)) m_width = DPI(MAIN_WINDOW_MIN_WIDTH);
    if(m_width > display.GetClientArea().width) m_width = DPI(MAIN_WINDOW_DEFAULT_WIDTH);
    if(m_height < DPI(MAIN_WINDOW_MIN_HEIGHT)) m_height = DPI(MAIN_WINDOW_MIN_HEIGHT);
    if(m_height > display.GetClientArea().height) m_height = DPI(MAIN_WINDOW_DEFAULT_HEIGHT);

    // validate x and y
    if(m_x < 0) m_x = 0; if(m_y < 0) m_y = 0;
    if(m_x > display.GetClientArea().width - 130)
        m_x = display.GetClientArea().width - 130;
    if(m_y > display.GetClientArea().height - 50)
        m_y = display.GetClientArea().height - 50;

    // now the window is surely inside of the screen

    cfg->Read(("/MainFrame/maximized"),&m_maximized,false);

    m_separatorPosition = (int)cfg->Read(
        ("/MainFrame/SeparatorPosition"),
        (long)DPI(DEFAULT_LIST_HEIGHT)
    );

    double r[LIST_COLUMNS] = {
        250.0/900, 280.0/900, 108.0/900,
        98.0/900, 97.0/900, 67.0/900
    };
    for(int i = 0; i < LIST_COLUMNS; i++){
        cfg->Read(wxString::Format(("/DrivesList/width%d"),i),
            &m_r[i], r[i]
        );
    }

    cfg->Read(("/Algorithm/RepeatAction"),&m_repeat,false);
    cfg->Read(("/Algorithm/SkipRemovableMedia"),&m_skipRem,true);
}

/**
 * @brief Saves application configuration.
 * @note Intended to be called in the main
 * frame destructor before termination
 * of threads.
 */
void MainFrame::SaveAppConfiguration()
{
    wxConfigBase *cfg = wxConfigBase::Get();
    cfg->Write(("/MainFrame/x"),(long)m_x);
    cfg->Write(("/MainFrame/y"),(long)m_y);
    cfg->Write(("/MainFrame/width"),(long)m_width);
    cfg->Write(("/MainFrame/height"),(long)m_height);
    cfg->Write(("/MainFrame/maximized"),(long)IsMaximized());
    cfg->Write(("/MainFrame/SeparatorPosition"),
        (long)m_splitter->GetSashPosition());

    int cwidth = 0;
    for(int i = 0; i < LIST_COLUMNS; i++)
        cwidth += m_vList->GetColumnWidth(i);

    for(int i = 0; i < LIST_COLUMNS; i++){
        cfg->Write(wxString::Format(("/DrivesList/width%d"),i),
            (double)m_vList->GetColumnWidth(i) / (double)cwidth
        );
    }

    cfg->Write(("/Language/Selected"),(long)g_locale->GetLanguage());

    cfg->Write(("/Algorithm/RepeatAction"),m_repeat);
    cfg->Write(("/Algorithm/SkipRemovableMedia"),m_skipRem);

    // save sorting parameters
    if(m_menuBar->FindItem(ID_SortByPath)->IsChecked()){
        cfg->Write(("/Algorithm/Sorting"),("path"));
    } else if(m_menuBar->FindItem(ID_SortBySize)->IsChecked()){
        cfg->Write(("/Algorithm/Sorting"),("size"));
    } else if(m_menuBar->FindItem(ID_SortByCreationDate)->IsChecked()){
        cfg->Write(("/Algorithm/Sorting"),("c_time"));
    } else if(m_menuBar->FindItem(ID_SortByModificationDate)->IsChecked()){
        cfg->Write(("/Algorithm/Sorting"),("m_time"));
    } else if(m_menuBar->FindItem(ID_SortByLastAccessDate)->IsChecked()){
        cfg->Write(("/Algorithm/Sorting"),("a_time"));
    }
    if(m_menuBar->FindItem(ID_SortAscending)->IsChecked()){
        cfg->Write(("/Algorithm/SortingOrder"),("asc"));
    } else {
        cfg->Write(("/Algorithm/SortingOrder"),("desc"));
    }

    cfg->Write(("/Upgrade/Level"),
        (long)m_upgradeThread->m_level);
}

// =======================================================================
//                          User preferences
// =======================================================================

#define UD_AdjustOption(name) { \
    if(!wxGetEnv(("UD_") (#name),NULL)) \
        wxSetEnv(("UD_") (#name), \
            wxString::Format(("%u"),DEFAULT_##name)); \
}

void MainFrame::ReadUserPreferences(wxCommandEvent& WXUNUSED(event))
{
    /*
    * The program should be configurable
    * through the options.lua file only.
    */
    wxUnsetEnv(("UD_DBGPRINT_LEVEL"));
    wxUnsetEnv(("UD_DISABLE_REPORTS"));
    wxUnsetEnv(("UD_DRY_RUN"));
    wxUnsetEnv(("UD_EX_FILTER"));
    wxUnsetEnv(("UD_FILE_SIZE_THRESHOLD"));
    wxUnsetEnv(("UD_FRAGMENT_SIZE_THRESHOLD"));
    wxUnsetEnv(("UD_FRAGMENTATION_THRESHOLD"));
    wxUnsetEnv(("UD_FRAGMENTS_THRESHOLD"));
    wxUnsetEnv(("UD_FREE_COLOR_R"));
    wxUnsetEnv(("UD_FREE_COLOR_G"));
    wxUnsetEnv(("UD_FREE_COLOR_B"));
    wxUnsetEnv(("UD_GRID_COLOR_R"));
    wxUnsetEnv(("UD_GRID_COLOR_G"));
    wxUnsetEnv(("UD_GRID_COLOR_B"));
    wxUnsetEnv(("UD_GRID_LINE_WIDTH"));
    wxUnsetEnv(("UD_IN_FILTER"));
    wxUnsetEnv(("UD_LOG_FILE_PATH"));
    wxUnsetEnv(("UD_MAP_BLOCK_SIZE"));
    wxUnsetEnv(("UD_MINIMIZE_TO_SYSTEM_TRAY"));
    wxUnsetEnv(("UD_OPTIMIZER_FILE_SIZE_THRESHOLD"));
    wxUnsetEnv(("UD_REFRESH_INTERVAL"));
    wxUnsetEnv(("UD_SECONDS_FOR_SHUTDOWN_REJECTION"));
    wxUnsetEnv(("UD_SHOW_MENU_ICONS"));
    wxUnsetEnv(("UD_SHOW_PROGRESS_IN_TASKBAR"));
    wxUnsetEnv(("UD_SHOW_TASKBAR_ICON_OVERLAY"));
    wxUnsetEnv(("UD_SORTING"));
    wxUnsetEnv(("UD_SORTING_ORDER"));
    wxUnsetEnv(("UD_TIME_LIMIT"));

    /* interpret options.lua file */
    lua_State *L; int status; wxString error = ("");
    wxFileName path(OPTIONSFILE);
    path.Normalize();
    if(!path.FileExists()){
        etrace("%ls file not found",
            path.GetFullPath().wc_str());
        goto done;
    }

    L = lua_open();
    if(!L){
        etrace("Lua initialization failed");
        goto done;
    }

    /* stop collector during initialization */
    lua_gc(L,LUA_GCSTOP,0);
    luaL_openlibs(L);
    lua_gc(L,LUA_GCRESTART,0);

    status = luaL_dofile(L,path.GetFullPath().char_str());
    if(status != 0){
        error += ("cannot interpret ") + path.GetFullPath();
        etrace("%ls",error.wc_str());
        if(!lua_isnil(L,-1)){
            const char *msg = lua_tostring(L,-1);
            if(!msg) msg = "(error object is not a string)";
            etrace("%hs",msg);
            error += wxString::Format(("\n%hs"),msg);
            lua_pop(L, 1);
        }
    }

    lua_close(L);

done:
    // ensure that GUI specific options are always set
    UD_AdjustOption(DRY_RUN);
    UD_AdjustOption(GRID_COLOR_R);
    UD_AdjustOption(GRID_COLOR_G);
    UD_AdjustOption(GRID_COLOR_B);
    UD_AdjustOption(GRID_LINE_WIDTH);
    UD_AdjustOption(FREE_COLOR_R);
    UD_AdjustOption(FREE_COLOR_G);
    UD_AdjustOption(FREE_COLOR_B);
    UD_AdjustOption(MAP_BLOCK_SIZE);
    UD_AdjustOption(MINIMIZE_TO_SYSTEM_TRAY);
    UD_AdjustOption(SECONDS_FOR_SHUTDOWN_REJECTION);
    UD_AdjustOption(SHOW_MENU_ICONS);
    UD_AdjustOption(SHOW_PROGRESS_IN_TASKBAR);
    UD_AdjustOption(SHOW_TASKBAR_ICON_OVERLAY);

    // reset log file path
    wxString v;
    if(wxGetEnv(("UD_LOG_FILE_PATH"),&v)){
        wxFileName logpath(v); logpath.Normalize();
        wxSetEnv(("UD_LOG_FILE_PATH"),logpath.GetFullPath());
    }
    ::udefrag_set_log_file_path();

    if(!error.IsEmpty()){
        wxMessageDialog dlg(this,error,("UltraDefrag"),
            wxOK | wxICON_ERROR/* | wxSTAY_ON_TOP*/);
        dlg.ShowModal();
    }
}

int MainFrame::CheckOption(const wxString& name)
{
    wxString value;
    if(wxGetEnv(name,&value)){
        unsigned long v;
        if(value.ToULong(&v))
            return (int)v;
    }
    return 0;
}

#undef UD_AdjustOption

// =======================================================================
//                      User preferences tracking
// =======================================================================

/**
 * @note This thread reloads the main configuration file
 * whenever something gets changed in the installation
 * directory, even when the last modification time
 * gets changed for its subdirectories.
 */
void *ConfigThread::Entry()
{
    ULONGLONG counter = 0;

    itrace("configuration tracking started");

    HANDLE h = FindFirstChangeNotification(OPTIONSDIR,
        FALSE,FILE_NOTIFY_CHANGE_LAST_WRITE);
    if(h == INVALID_HANDLE_VALUE){
        letrace("FindFirstChangeNotification failed");
        goto done;
    }

    while(!g_mainFrame->CheckForTermination(10)){
        DWORD status = WaitForSingleObject(h,100);
        if(status == WAIT_OBJECT_0){
            if(!(counter % 2)){
                /*
                * Do nothing, 'cause
                * "If a change occurs after a call
                * to FindFirstChangeNotification but
                * before a call to FindNextChangeNotification,
                * the operating system records the change.
                * When FindNextChangeNotification is executed,
                * the recorded change immediately satisfies
                * a wait for the change notification." (MSDN)
                * And so on... it happens between
                * FindNextChangeNotification calls too.
                *
                * However, everything mentioned above is false
                * when the program modifies the file itself.
                */
            } else {
                itrace("configuration has been changed");
                PostCommandEvent(g_mainFrame,ID_ReadUserPreferences);
                PostCommandEvent(g_mainFrame,ID_SetWindowTitle);
                PostCommandEvent(g_mainFrame,ID_AdjustSystemTrayIcon);
                PostCommandEvent(g_mainFrame,ID_AdjustTaskbarIconOverlay);
                PostCommandEvent(g_mainFrame,ID_RedrawMap);
            }
            counter ++;
            /* wait for the next notification */
            if(!FindNextChangeNotification(h)){
                letrace("FindNextChangeNotification failed");
                break;
            }
        }
    }

    FindCloseChangeNotification(h);

done:
    itrace("configuration tracking stopped");
    return nullptr;
}

// =======================================================================
//                            Event handlers
// =======================================================================

void MainFrame::OnGuiOptions(wxCommandEvent& WXUNUSED(event))
{
    if(m_title->Find(("Portable")) != wxNOT_FOUND)
        Utils::ShellExec(("notepad"),("open"),OPTIONSFILE);
    else
        Utils::ShellExec(OPTIONSFILE,("open"));
}

void MainFrame::OnBootScript(wxCommandEvent& WXUNUSED(event))
{
    wxFileName script(("%SystemRoot%\\system32\\ud-boot-time.cmd"));
    script.Normalize(); Utils::ShellExec(script.GetFullPath(),("edit"));
}

void MainFrame::ChooseFont(wxCommandEvent& WXUNUSED(event))
{
    wxFontDialog* dialog = new wxFontDialog(this);
    if (dialog->ShowModal() == wxID_OK){
        wxFontData fontDataOUT = dialog->GetFontData();  //Get "font data" from dialog.
        wxFont font = fontDataOUT.GetChosenFont();
        m_vList->SetFont(font);
        m_vList->Refresh();
        //ProcessCommandEvent(ID_AdjustListColumns);
        m_filesList->SetFont(font);
        m_filesList->Refresh();
        //ProcessCommandEvent(ID_AdjustFilesListColumns);
        dtrace("Chose new Font = %ws,%d", font.GetFaceName().wc_str(),font.GetPointSize());
    }
    dialog->Destroy();
}

/** @} */
