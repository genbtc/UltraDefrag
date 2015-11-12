#include "utils_win32.h"
#include <ShlObj.h>
#include <Windows.h>
//#include "shlobj.h"
//#include <Shlobj.h>

void UtilsWin32::BrowseToFile(LPCWSTR filename)
{
    //Utils::ShellExec(wxT("explorer.exe"),wxT("open"),itemtext); //This opens the file itself in the default handler.
//    wxString xec;
//    xec.Printf(L"explorer.exe /select,%s",itemtext.wc_str());
//    system(xec.mb_str().data());
//
//    ShellExecute(0, _T("open"), _T("explorer.exe"), strArgs, 0, SW_NORMAL);
//    ITEMIDLIST *pidl = ILCreateFromPathW(filename);
//    if(pidl) {
//        SHOpenFolderAndSelectItems(pidl,0,0,0);
//        ILFree(pidl);
//    }
}

void UtilsWin32::toClipboard(const std::string &s)
{
    HWND hwnd = GetDesktopWindow();
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,s.size()+1);
	if (!hg){
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg),s.c_str(),s.size()+1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT,hg);
	CloseClipboard();
	GlobalFree(hg);
}
