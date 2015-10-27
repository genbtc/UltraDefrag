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

#include "main.h"

#define UD_AppendCheckItem(id) AppendCheckItem(id, wxEmptyString)
#define UD_AppendRadioItem(id) AppendRadioItem(id, wxEmptyString)

#define UD_SetMenuIcon(id, icon) \
    string.Printf(wxT("%hs%u"),#icon,g_iconSize); \
    pic = Utils::LoadPngResource(string.wc_str()); \
    if(pic) m_menuBar->FindItem(id)->SetBitmap(*pic); \
    delete pic;

#define UD_SetMarginWidth(menu) \
    list = menu->GetMenuItems(); \
    count = list.GetCount(); \
    for(index = 0; index < count; index++) \
        list.Item(index)->GetData()->SetMarginWidth(g_iconSize);

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
    menuWhenDone->UD_AppendRadioItem(EventID_WhenDoneNone);
    menuWhenDone->UD_AppendRadioItem(EventID_WhenDoneExit);
    menuWhenDone->UD_AppendRadioItem(EventID_WhenDoneStandby);
    menuWhenDone->UD_AppendRadioItem(EventID_WhenDoneHibernate);
    menuWhenDone->UD_AppendRadioItem(EventID_WhenDoneLogoff);
    menuWhenDone->UD_AppendRadioItem(EventID_WhenDoneReboot);
    menuWhenDone->UD_AppendRadioItem(EventID_WhenDoneShutdown);

    // create action menu
    wxMenu *m_menuAction = new wxMenu;
    m_menuAction->Append(EventID_Analyze);
    m_menuAction->Append(EventID_Defrag);
    m_menuAction->Append(EventID_QuickOpt);
    m_menuAction->Append(EventID_FullOpt);
    m_menuAction->Append(EventID_MftOpt);
    m_menuAction->UD_AppendCheckItem(EventID_Pause);
    m_menuAction->Append(EventID_Stop);
    m_menuAction->AppendSeparator();
    m_menuAction->UD_AppendCheckItem(EventID_Repeat);
    m_menuAction->AppendSeparator();
    m_menuAction->UD_AppendCheckItem(EventID_SkipRem);
    m_menuAction->Append(EventID_Rescan);
    m_menuAction->AppendSeparator();
    m_menuAction->Append(EventID_Repair);
    m_menuAction->AppendSeparator();
    m_subMenuWhenDone = \
        m_menuAction->AppendSubMenu(
            menuWhenDone, wxEmptyString
        );
    m_menuAction->AppendSeparator();
    m_menuAction->Append(EventID_Exit);

    // create report menu
    wxMenu *menuReport = new wxMenu;
    menuReport->Append(EventID_ShowReport);

    // create language menu
    m_menuLanguage = new wxMenu;
    m_menuLanguage->Append(EventID_LangShowLog);
    m_menuLanguage->Append(EventID_LangShowReport);
    m_menuLanguage->Append(EventID_LangOpenFolder);
    m_menuLanguage->Append(EventID_LangSubmit);
    m_menuLanguage->AppendSeparator();

    wxString AppLocaleDir(wxGetCwd());
    AppLocaleDir.Append(wxT("/locale"));
    if(!wxDirExists(AppLocaleDir)){
        itrace("lang dir not found: %ls",AppLocaleDir.wc_str());
        AppLocaleDir = wxGetCwd() + wxT("/../wxgui/locale");
    }
    if(!wxDirExists(AppLocaleDir)){
        etrace("lang dir not found: %ls",AppLocaleDir.wc_str());
        AppLocaleDir = wxGetCwd() + wxT("/../../wxgui/locale");
    }

    wxDir dir(AppLocaleDir);
    const wxLanguageInfo *info;

    if(!dir.IsOpened()){
        etrace("can't open lang dir: %ls",AppLocaleDir.wc_str());
        info = g_locale->FindLanguageInfo(wxT("en_US"));
        m_menuLanguage->AppendRadioItem(EventID_LocaleChange \
            + info->Language, info->Description);
    } else {
        wxString folder;
        wxArrayString langArray;

        bool cont = dir.GetFirst(&folder, wxT("*"), wxDIR_DIRS);

        while(cont){
            info = g_locale->FindLanguageInfo(folder);
            if(info){
                if(info->Description == wxT("Chinese")){
                    langArray.Add(wxT("Chinese (Traditional)"));
                } else {
                    if(info->Description == wxT("English")){
                        langArray.Add(wxT("English (U.K.)"));
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
        unsigned int breakDelta = (unsigned int)ceil((double) \
            (langArray.Count() + langArray.Count() % 2 + 4) / 3);
        unsigned int breakCnt = breakDelta - 4;
        itrace("languages: %d, break count: %d, delta: %d",
            langArray.Count(), breakCnt, breakDelta);
        for(unsigned int i = 0;i < langArray.Count();i++){
            info = g_locale->FindLanguageInfo(langArray[i]);
            m_menuLanguage->AppendRadioItem(EventID_LocaleChange \
                + info->Language, info->Description);
            if((i+1) % breakCnt == 0){
                m_menuLanguage->Break();
                breakCnt += breakDelta;
            }
        }
    }

    // create boot configuration menu
    wxMenu *menuBootConfig = new wxMenu;
    menuBootConfig->UD_AppendCheckItem(EventID_BootEnable);
    menuBootConfig->Append(EventID_BootScript);

    // create sorting configuration menu
    wxMenu *menuSortingConfig = new wxMenu;
    menuSortingConfig->UD_AppendRadioItem(EventID_SortByPath);
    menuSortingConfig->UD_AppendRadioItem(EventID_SortBySize);
    menuSortingConfig->UD_AppendRadioItem(EventID_SortByCreationDate);
    menuSortingConfig->UD_AppendRadioItem(EventID_SortByModificationDate);
    menuSortingConfig->UD_AppendRadioItem(EventID_SortByLastAccessDate);
    menuSortingConfig->AppendSeparator();
    menuSortingConfig->UD_AppendRadioItem(EventID_SortAscending);
    menuSortingConfig->UD_AppendRadioItem(EventID_SortDescending);

    // create settings menu
    wxMenu *menuSettings = new wxMenu;
    m_subMenuLanguage = \
        menuSettings->AppendSubMenu(
            m_menuLanguage, wxEmptyString
        );
    menuSettings->Append(EventID_GuiOptions);
    m_subMenuSortingConfig = \
        menuSettings->AppendSubMenu(
            menuSortingConfig, wxEmptyString
        );
    m_subMenuBootConfig = \
        menuSettings->AppendSubMenu(
            menuBootConfig, wxEmptyString
        );
    menuSettings->Append(EventID_ReportOptions);

    // create debug menu
    wxMenu *menuDebug = new wxMenu;
    menuDebug->Append(EventID_DebugLog);
    menuDebug->Append(EventID_DebugSend);

    // create upgrade menu
    wxMenu *menuUpgrade = new wxMenu;
    menuUpgrade->UD_AppendRadioItem(EventID_HelpUpgradeNone);
    menuUpgrade->UD_AppendRadioItem(EventID_HelpUpgradeStable);
    menuUpgrade->UD_AppendRadioItem(EventID_HelpUpgradeAll);
    menuUpgrade->AppendSeparator();
    menuUpgrade->Append(EventID_HelpUpgradeCheck);

    // create help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(EventID_HelpContents);
    menuHelp->AppendSeparator();
    menuHelp->Append(EventID_HelpBestPractice);
    menuHelp->Append(EventID_HelpFaq);
    menuHelp->Append(EventID_HelpLegend);
    menuHelp->AppendSeparator();
    m_subMenuDebug = \
        menuHelp->AppendSubMenu(
            menuDebug, wxEmptyString
        );
    menuHelp->AppendSeparator();
    m_subMenuUpgrade = \
        menuHelp->AppendSubMenu(
            menuUpgrade, wxEmptyString
        );
    menuHelp->AppendSeparator();
    menuHelp->Append(EventID_HelpAbout);

    // create main menu
    m_menuBar = new wxMenuBar;
    m_menuBar->Append(m_menuAction, wxEmptyString);
    m_menuBar->Append(menuReport  , wxEmptyString);
    m_menuBar->Append(menuSettings, wxEmptyString);
    m_menuBar->Append(menuHelp    , wxEmptyString);

    SetMenuBar(m_menuBar);

    // set menu icons
    if(CheckOption(wxT("UD_SHOW_MENU_ICONS"))){
        wxBitmap *pic; wxString string;
        UD_SetMenuIcon(EventID_Analyze         , glass )
        UD_SetMenuIcon(EventID_Defrag          , defrag)
        UD_SetMenuIcon(EventID_QuickOpt        , quick )
        UD_SetMenuIcon(EventID_FullOpt         , full  )
        UD_SetMenuIcon(EventID_MftOpt          , mft   )
        UD_SetMenuIcon(EventID_Stop            , stop  )
        UD_SetMenuIcon(EventID_ShowReport      , report)
        UD_SetMenuIcon(EventID_GuiOptions      , gear  )
        UD_SetMenuIcon(EventID_BootScript      , script)
        UD_SetMenuIcon(EventID_HelpContents    , help  )
        UD_SetMenuIcon(EventID_HelpBestPractice, light )
        UD_SetMenuIcon(EventID_HelpAbout       , star  )

        wxMenuItemList list; size_t count, index;
        UD_SetMarginWidth(m_menuBar->GetMenu(0))
        UD_SetMarginWidth(m_menuBar->GetMenu(1))
        UD_SetMarginWidth(m_menuBar->GetMenu(2))
        UD_SetMarginWidth(m_menuBar->GetMenu(3))
        UD_SetMarginWidth(menuBootConfig)
    }

    // initial settings
    m_menuBar->FindItem(EventID_Repeat)->Check(m_repeat);
    m_menuBar->FindItem(EventID_SkipRem)->Check(m_skipRem);

    int id = g_locale->GetLanguage();
    wxMenuItem *item = m_menuBar->FindItem(EventID_LocaleChange + id);
    if(item) item->Check(true);

    wxConfigBase *cfg = wxConfigBase::Get();
    wxString sorting = cfg->Read(wxT("/Algorithm/Sorting"),wxT("path"));
    if(sorting == wxT("path")){
        m_menuBar->FindItem(EventID_SortByPath)->Check();
    } else if(sorting == wxT("size")){
        m_menuBar->FindItem(EventID_SortBySize)->Check();
    } else if(sorting == wxT("c_time")){
        m_menuBar->FindItem(EventID_SortByCreationDate)->Check();
    } else if(sorting == wxT("m_time")){
        m_menuBar->FindItem(EventID_SortByModificationDate)->Check();
    } else if(sorting == wxT("a_time")){
        m_menuBar->FindItem(EventID_SortByLastAccessDate)->Check();
    }
    wxString order = cfg->Read(wxT("/Algorithm/SortingOrder"),wxT("asc"));
    if(order == wxT("asc")){
        m_menuBar->FindItem(EventID_SortAscending)->Check();
    } else {
        m_menuBar->FindItem(EventID_SortDescending)->Check();
    }
}

#undef UD_AppendCheckItem
#undef UD_AppendRadioItem
#undef UD_SetMenuIcon
#undef UD_SetMarginWidth

/** @} */
