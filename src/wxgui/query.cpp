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
 * @{
 */
/**=========================================================================**
***                        Declarations                                     **
***=========================================================================**/
#include "main.h"
#include "udefrag-internals_flags.h"
#include "../dll/udefrag/query.h"
/**=========================================================================**
***                    Functions in the Query Menu.                         **
***=========================================================================**/
// Instead of making a query Menu, make a Query Tab
// This will cause a "query mode" to happen.
// First button will be "Analyze" (since we are in query mode, it will cache the result in RAM)
// Second Item should be a dropdown menu containing a list of all the queries.
// Below that there should be a Caption field that has descriptive help text about the query selected.
// Off to the right there should be a button that says "Go!" or "Perform Query"
// Below all of that we would have some kind of a universal results box, maybe just an open ended text box.
// The best thing would be to have the query functions return the original structs, and then have the GUI deal with it.
// We should make the udefrag-internals struct's available to the GUI. (such as blockmap, regions) 
// We will have to create query.c for a passthrough. 

void MainFrame::InitQueryMenu()
{

}

void MainFrame::QueryClusters(wxCommandEvent& event){

}
/**=========================================================================**
***                  Dedicated Query Thread.                                **
***=========================================================================**/
//should create some kind of a struct to pass any variables we need IN.
// drive_letter. query_type. File Path. 
void *QueryThread::Entry()
{
    while(!g_mainFrame->CheckForTermination(200)){
        if(m_startquery){
//            result = udefrag_start_query(m_letter,m_jobType,m_flags,m_mapSize,
//                reinterpret_cast<udefrag_progress_callback>(ProgressCallback),
//                reinterpret_cast<udefrag_terminator>(Terminator),NULL
//            );
        }
    }
    return NULL;
}

