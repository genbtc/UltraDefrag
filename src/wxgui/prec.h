//////////////////////////////////////////////////////////////////////////
//
//  UltraDefrag - a powerful defragmentation tool for Windows NT.
//  Copyright (c) 2007-2017 Dmitri Arkhangelski (dmitriar@gmail.com).
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

#ifndef _UDEFRAG_GUI_PREC_H_
#define _UDEFRAG_GUI_PREC_H_
#include "wx/setup.h"
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>//genBTC (right-click menu) copy to clipboard
#include <wx/cmdline.h>
#include <wx/colordlg.h>
#include <wx/confbase.h>
#include <wx/dataview.h>//genbtc LCN Tab 4
#include <wx/datetime.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dir.h>
#include <wx/display.h>
#include <wx/dynlib.h>
#include <wx/encconv.h> //genbtc for encodings
#include <wx/event.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/filepicker.h>//genBTC query tab 3
#include <wx/fontdlg.h>//genBTC (font-chooser)
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/graphics.h>
#include <wx/grid.h>//genbtc
#include <wx/hashmap.h>
#include <wx/hyperlink.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/listbase.h>
#include <wx/listctrl.h>
#include <wx/menu.h>//genbtc (right-click menu)
#include <wx/metafile.h>
#include <wx/mstream.h>
#include <wx/process.h>
#include <wx/notebook.h>//genbtc
#include <wx/overlay.h>
#include <wx/panel.h>//genbtc
#include <wx/popupwin.h>    //genBTC ClusterMapLegend
#include <wx/settings.h>
#include <wx/sizer.h>//genbtc
#include <wx/spinctrl.h>    //genBTC ClusterMapLegend
#include <wx/splitter.h>
#include <wx/stattext.h>//genBTC query tab 3
#include <wx/string.h>
#include <wx/sysopt.h>
#include <wx/taskbar.h>
#include <wx/textctrl.h>//genBTC query tab
#include <wx/textfile.h>
#include <wx/thread.h>  //Not included by wx/wxprec.h
#include <wx/tglbtn.h> //genbtc LCN Tab 4
#include <wx/toolbar.h>
#include <wx/uri.h>
#include <wx/xlocale.h>
#include <wx/xml/xml.h> //Not included by wx/wxprec.h
//STOPGAP
#include "../stopgap/stopgap.hpp"
//STL
#include <algorithm>
#include <codecvt>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <iomanip>
#include <exception>
#include <string>
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <cstdio>
//UDEFRAG/ZenWinx
#include "../dll/udefrag/udefrag-internals.h"


//Windows:
#include <commctrl.h>
#include <signal.h>
//TaskBar Progress Flag.
typedef enum {
    TBPF_NOPROGRESS = 0,
    TBPF_INDETERMINATE = 0x1,
    TBPF_NORMAL = 0x2,
    TBPF_ERROR = 0x4,
    TBPF_PAUSED = 0x8
} TBPFLAG;

/*
* For MinGW: _WIN32_IE must be set to at least
* 0x0400 to include all the necessary stuff.
*/
#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif
//urlmon headers for ming
#if defined(__GNUC__)
extern "C" {
HRESULT WINAPI URLDownloadToFileW(
    /* LPUNKNOWN */ void *lpUnkcaller,
    LPCWSTR szURL,
    LPCWSTR szFileName,
    DWORD dwReserved,
    /*IBindStatusCallback*/ void *pBSC
);

HRESULT WINAPI URLDownloadToCacheFileW(
    /* LPUNKNOWN */ void *lpUnkcaller,
    LPCWSTR szURL,
    LPWSTR szFileName,
    DWORD cchFileName,
    DWORD dwReserved,
    /*IBindStatusCallback*/ void *pBSC
);
}
#endif


#endif /* _UDEFRAG_GUI_PREC_H_ */
