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
 * @file menu.cpp
 * @brief Menu.
 * @addtogroup Menu
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
//                        Menu for main window
// =======================================================================

/**
 * @brief Initializes main menu. Uses i18n.cpp for localization strings
 */
void MainFrame::InitMenu()
{
    // create when done menu
    wxMenu *menuWhenDone = new wxMenu;
    menuWhenDone->UD_AppendRadioItem(ID_WhenDoneNone,L"None");
    menuWhenDone->UD_AppendRadioItem(ID_WhenDoneExit,L"Exit");
    menuWhenDone->UD_AppendRadioItem(ID_WhenDoneStandby,L"Standby");
    menuWhenDone->UD_AppendRadioItem(ID_WhenDoneHibernate,L"Hibernate");
    menuWhenDone->UD_AppendRadioItem(ID_WhenDoneLogoff,L"Logoff");
    menuWhenDone->UD_AppendRadioItem(ID_WhenDoneReboot,L"Reboot");
    menuWhenDone->UD_AppendRadioItem(ID_WhenDoneShutdown,L"Shutdown");

    // create action menu
    wxMenu *m_menuAction = new wxMenu;
    m_menuAction->Append(ID_Analyze,L"Analyze");
    m_menuAction->Append(ID_Defrag,L"Defragment");
    m_menuAction->Append(ID_QuickOpt,L"Quick Optimize");
    m_menuAction->Append(ID_FullOpt,L"Full Optimize");
    m_menuAction->Append(ID_MftOpt,L"MFT Optimize");
    m_menuAction->UD_AppendCheckItem(ID_Pause,L"Pause");
    m_menuAction->Append(ID_Stop,L"Stop");
    m_menuAction->AppendSeparator();
    m_menuAction->UD_AppendCheckItem(ID_Repeat,L"Repeat");
    m_menuAction->AppendSeparator();
    m_menuAction->Append(ID_ShowReport,L"Show Report");
    m_menuAction->AppendSeparator();
    m_menuAction->UD_AppendCheckItem(ID_SkipRem,L"Skip Removable");
    m_menuAction->Append(ID_Rescan,L"Rescan");
    m_menuAction->AppendSeparator();
    m_menuAction->Append(ID_Repair,L"Repair");
    m_menuAction->AppendSeparator();
    m_subMenuWhenDone = \
        m_menuAction->AppendSubMenu(
            menuWhenDone, "SubMenu WhenDone"
        );
    m_menuAction->AppendSeparator();
    m_menuAction->Append(ID_Exit,L"Exit");

    // create Query menu by genBTC) - Query.cpp
    //title is set @ Line 246, and added to the menubar.
    wxMenu *menuQuery = new wxMenu;
    menuQuery->Append(ID_QueryClusters, L"&Show File's Clusters" , L"");    //query the internals and ask what clusters a file uses.

    // create language menu
    m_menuLanguage = new wxMenu;
    m_menuLanguage->Append(ID_LangTranslateOnline,L"Translate Langs Online");
    m_menuLanguage->Append(ID_LangTranslateOffline,L"Translate Langs Offline");
    m_menuLanguage->Append(ID_LangOpenFolder,L"Open Langs Folder");
    m_menuLanguage->AppendSeparator();

    wxString AppLocaleDir(wxGetCwd());
    AppLocaleDir.Append("/locale");
    if(!wxDirExists(AppLocaleDir)){
        itrace("lang dir not found: %ls",AppLocaleDir.wc_str());
        AppLocaleDir = wxGetCwd() + "/../wxgui/locale";
    }
    if(!wxDirExists(AppLocaleDir)){
        etrace("lang dir not found: %ls",AppLocaleDir.wc_str());
        AppLocaleDir = wxGetCwd() + "/../../wxgui/locale";
    }

    wxDir dir(AppLocaleDir);
    const wxLanguageInfo *info;

    if(!dir.IsOpened()){
        etrace("can't open lang dir: %ls",AppLocaleDir.wc_str());
        info = g_locale->FindLanguageInfo("en_US");
        m_menuLanguage->AppendRadioItem(ID_LocaleChange \
            + info->Language, info->Description);
    } else {
        wxString folder;
        wxArrayString langArray;

        bool cont = dir.GetFirst(&folder, "*", wxDIR_DIRS);

        while(cont){
            info = g_locale->FindLanguageInfo(folder);
            if(info){
                if(info->Description == "Chinese"){
                    langArray.Add("Chinese (Traditional)");
                } else {
                    if(info->Description == "English"){
                        langArray.Add("English (U.K.)");
                    } else {
                        langArray.Add(info->Description);
                    }
                }
            } else {
                etrace("can't find locale info for %ls",folder.wc_str());
            }
            cont = dir.GetNext(&folder);
        }

        langArray.Sort();

        // divide list of languages to three columns
        unsigned int breakDelta = static_cast<unsigned int>( \
			ceil(double(langArray.Count() + langArray.Count() % 2 + 4) / 3));
        unsigned int breakCnt = breakDelta - 4;
        itrace("languages: %d, break count: %d, delta: %d",
            langArray.Count(), breakCnt, breakDelta);
        for(unsigned int i = 0;i < langArray.Count();i++){
            info = g_locale->FindLanguageInfo(langArray[i]);
            m_menuLanguage->AppendRadioItem(ID_LocaleChange \
                + info->Language, info->Description);
            if((i+1) % breakCnt == 0){
                m_menuLanguage->Break();
                breakCnt += breakDelta;
            }
        }
    }

    // create boot configuration menu
    wxMenu *menuBootConfig = new wxMenu;
    menuBootConfig->UD_AppendCheckItem(ID_BootEnable,L"Boot D Enabled");
    menuBootConfig->Append(ID_BootScript,L"BootScript");

    // create sorting configuration menu
    wxMenu *menuSortingConfig = new wxMenu;
    menuSortingConfig->UD_AppendRadioItem(ID_SortByPath,L"Sort By Path");
    menuSortingConfig->UD_AppendRadioItem(ID_SortBySize,L"Sort By Size");
    menuSortingConfig->UD_AppendRadioItem(ID_SortByCreationDate,L"Sort By Creation Date");
    menuSortingConfig->UD_AppendRadioItem(ID_SortByModificationDate,L"Sort By Modification Date");
    menuSortingConfig->UD_AppendRadioItem(ID_SortByLastAccessDate,L"Sort By Access Date");
    menuSortingConfig->AppendSeparator();
    menuSortingConfig->UD_AppendRadioItem(ID_SortAscending,L"Sort Ascending");
    menuSortingConfig->UD_AppendRadioItem(ID_SortDescending,L"Sort Descending");

    // create settings menu
    wxMenu *menuSettings = new wxMenu;
    m_subMenuLanguage = \
        menuSettings->AppendSubMenu(
            m_menuLanguage, "SubMenu Language"
        );
    menuSettings->Append(ID_GuiOptions,L"GUI Options");
    m_subMenuSortingConfig = \
        menuSettings->AppendSubMenu(
            menuSortingConfig, "SubMenu SortingConfig"
        );
    m_subMenuBootConfig = \
        menuSettings->AppendSubMenu(
            menuBootConfig, "SubMenu BootConfig"
        );

    menuSettings->Append(ID_ChooseFont,L"Choose Font");

    // create debug menu
    wxMenu *menuDebug = new wxMenu;
    menuDebug->Append(ID_DebugLog,L"Debug Log");
    menuDebug->Append(ID_DebugSend,L"Debug Send");

    // create upgrade menu
    wxMenu *menuUpgrade = new wxMenu;
    menuUpgrade->UD_AppendRadioItem(ID_HelpUpgradeNone,L"Help Upgrade None");
    menuUpgrade->UD_AppendRadioItem(ID_HelpUpgradeStable,L"Help Upgrade Stable");
    menuUpgrade->UD_AppendRadioItem(ID_HelpUpgradeAll,L"Help Upgrade All");
    menuUpgrade->AppendSeparator();
    menuUpgrade->Append(ID_HelpUpgradeCheck,L"Help Upgrade Check");

    // create help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(ID_HelpContents,L"Help Contents");
    menuHelp->AppendSeparator();
    menuHelp->Append(ID_HelpBestPractice,L"Help Best Practices");
    menuHelp->Append(ID_HelpFaq,L"Help FAQ");
    menuHelp->Append(ID_HelpLegend,L"Help Legend");
    menuHelp->AppendSeparator();
    m_subMenuDebug = \
        menuHelp->AppendSubMenu(
            menuDebug, "SubMenu Debug"
        );
    menuHelp->AppendSeparator();
    m_subMenuUpgrade = \
        menuHelp->AppendSubMenu(
            menuUpgrade, "SubMenu Upgrade"
        );
    menuHelp->AppendSeparator();
    menuHelp->Append(ID_HelpAbout,L"Help About");

    // create main menu
    m_menuBar = new wxMenuBar;
    m_menuBar->Append(m_menuAction, "Menu Action");
    m_menuBar->Append(menuQuery   , "&Query");
    m_menuBar->Append(menuSettings, "Menu Settings");
    m_menuBar->Append(menuHelp    , "Menu Help");

    SetMenuBar(m_menuBar);

    // set menu icons
    if(CheckOption("UD_SHOW_MENU_ICONS")){
        UD_SetMenuIcon(ID_Analyze         , glass );
        UD_SetMenuIcon(ID_Defrag          , defrag);
        UD_SetMenuIcon(ID_QuickOpt        , quick );
        UD_SetMenuIcon(ID_FullOpt         , full  );
        UD_SetMenuIcon(ID_MftOpt          , mft   );
        UD_SetMenuIcon(ID_Stop            , stop  );
        UD_SetMenuIcon(ID_ShowReport      , report);
        UD_SetMenuIcon(ID_GuiOptions      , gear  );
        UD_SetMenuIcon(ID_BootScript      , script);
        UD_SetMenuIcon(ID_HelpContents    , help  );
        UD_SetMenuIcon(ID_HelpBestPractice, light );
        UD_SetMenuIcon(ID_HelpAbout       , star  );

        UD_SetMarginWidth(m_menuBar->GetMenu(0));
        UD_SetMarginWidth(m_menuBar->GetMenu(1));
        UD_SetMarginWidth(m_menuBar->GetMenu(2));
        UD_SetMarginWidth(menuBootConfig);
    }

    // initial settings
    m_menuBar->FindItem(ID_Repeat)->Check(m_repeat);
    m_menuBar->FindItem(ID_SkipRem)->Check(m_skipRem);

	const int id = g_locale->GetLanguage();
    wxMenuItem *item = m_menuBar->FindItem(ID_LocaleChange + id);
    if(item) item->Check(true);

    wxConfigBase *cfg = wxConfigBase::Get();
	const wxString sorting = cfg->Read("/Algorithm/Sorting","path");
    if(sorting == "path"){
        m_menuBar->FindItem(ID_SortByPath)->Check();
    } else if(sorting == "size"){
        m_menuBar->FindItem(ID_SortBySize)->Check();
    } else if(sorting == "c_time"){
        m_menuBar->FindItem(ID_SortByCreationDate)->Check();
    } else if(sorting == "m_time"){
        m_menuBar->FindItem(ID_SortByModificationDate)->Check();
    } else if(sorting == "a_time"){
        m_menuBar->FindItem(ID_SortByLastAccessDate)->Check();
    }
	const wxString order = cfg->Read("/Algorithm/SortingOrder","asc");
    if(order == "asc"){
        m_menuBar->FindItem(ID_SortAscending)->Check();
    } else {
        m_menuBar->FindItem(ID_SortDescending)->Check();
    }
}

/** @} */
