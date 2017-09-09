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
 * @file utils.cpp
 * @brief Auxiliary utilities.
 * @addtogroup Utils
 * @{
 */

// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.

// =======================================================================
//                            Declarations
// =======================================================================
#include "wx/wxprec.h"
#include "main.h"
#include <stdexcept>

typedef HRESULT (__stdcall *URLMON_PROCEDURE)(
    /* LPUNKNOWN */ void *lpUnkcaller,
    LPCWSTR szURL,
    LPCWSTR szFileName,
    DWORD dwReserved,
    /*IBindStatusCallback*/ void *pBSC
);

// =======================================================================
//                         Auxiliary utilities
// =======================================================================

/**
 * @brief Defines whether the user
 * has administrative rights or not.
 */
bool Utils::CheckAdminRights(void)
{
    PSID psid = nullptr;
    SID_IDENTIFIER_AUTHORITY SystemSidAuthority = {SECURITY_NT_AUTHORITY};
    if(!::AllocateAndInitializeSid(&SystemSidAuthority,2,
      SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_ADMINS,
      0,0,0,0,0,0,&psid)){
        letrace("cannot create the security identifier");
        return false;
    }

    BOOL is_member = false;
    if(!::CheckTokenMembership(nullptr,psid,&is_member)){
        letrace("cannot check token membership");
        if(psid) ::FreeSid(psid);
        return false;
    }

    if(!is_member) itrace("the user is not a member of administrators group");
    if(psid) ::FreeSid(psid);
    return is_member == 0 ? false : true;
}

/**
 * @brief Downloads a file from the web.
 * @return Path to the downloaded file.
 * @note If the program terminates before
 * the file download completion it crashes.
 */
bool Utils::DownloadFile(const wxString& url, const wxString& path)
{
    itrace("downloading %ls",url.wc_str());

    /*
    * URLDownloadToCacheFileW cannot be used
    * here because it may immediately delete
    * the file after its creation.
    */
    wxDynamicLibrary lib(("urlmon"));
    wxDYNLIB_FUNCTION(URLMON_PROCEDURE,
        URLDownloadToFileW, lib);

    if(!pfnURLDownloadToFileW)
        return false;

    HRESULT result = pfnURLDownloadToFileW(
        nullptr,url.wc_str(),path.wc_str(),0, nullptr);
    if(result != S_OK){
        etrace("URLDownloadToFile failed "
            "with code 0x%x",(UINT)result);
        return false;
    }

    return true;
}

/**
 * @brief Sends a request to Google Analytics
 * service gathering statistics of the use
 * of the program.
 * @details Based on http://code.google.com/apis/analytics/docs/
 * and http://www.vdgraaf.info/google-analytics-without-javascript.html
 */
void Utils::GaRequest(const wxString& path)
{
    srand((unsigned int)time(nullptr));
    int utmn = (rand() << 16) + rand();
    int utmhid = (rand() << 16) + rand();
    int cookie = (rand() << 16) + rand();
    int random = (rand() << 16) + rand();
    __int64 today = (__int64)time(nullptr);

    wxString url;

    url << "http://www.google-analytics.com/__utm.gif?utmwv=4.6.5";
    url << wxString::Format("&utmn=%u",utmn);
    url << "&utmhn=ultradefrag.sourceforge.net";
    url << wxString::Format("&utmhid=%u&utmr=-",utmhid);
    url << "&utmp=" << path;
    url << "&utmac=";
    url << "UA-15890458-1";
    url << wxString::Format("&utmcc=__utma%%3D%u.%u.%I64u.%I64u.%I64u." \
        ("50%%3B%%2B__utmz%%3D%u.%I64u.27.2.utmcsr%%3Dgoogle.com%%7Cutmccn%%3D") \
        ("(referral)%%7Cutmcmd%%3Dreferral%%7Cutmcct%%3D%%2F%%3B"),
        cookie,random,today,today,today,cookie,today);

    wxFileName target((".\\tmp"));
    target.Normalize();
    wxString dir(target.GetFullPath());
    if(!wxDirExists(dir)) wxMkdir(dir);

    /*
    * Use a subfolder to prevent configuration files
    * reload (see ConfigThread::Entry() for details).
    */
    dir << "\\data";
    if(!wxDirExists(dir)) wxMkdir(dir);

    wxString file(dir);
    file << "\\__utm.gif";
    if(DownloadFile(url,file))
        wxRemoveFile(file);
}

/**
 * @brief Creates a bitmap
 * from a png resource.
 */
wxBitmap * Utils::LoadPngResource(const wchar_t *name)
{
    HRSRC resource = ::FindResource(nullptr,name,RT_RCDATA);
    if(!resource){
        letrace("cannot find %ls resource",name);
        return nullptr;
    }

    HGLOBAL handle = ::LoadResource(nullptr,resource);
    if(!handle){
        letrace("cannot load %ls resource",name);
        return nullptr;
    }

    char *data = (char *)::LockResource(handle);
    if(!data){
        letrace("cannot lock %ls resource",name);
        return nullptr;
    }

    DWORD size = ::SizeofResource(nullptr,resource);
    if(!size){
        letrace("cannot get size of %ls resource",name);
        return nullptr;
    }

    wxMemoryInputStream is(data,size);
    return new wxBitmap(wxImage(is,wxBITMAP_TYPE_PNG,-1),-1);
}

/**
 * @brief Opens the UltraDefrag Handbook,
 * either its local copy or from the web.
 */
void Utils::OpenHandbook(const wxString& page, const wxString& anchor)
{
    wxString path;
    path = "./handbook/" + page;

    if(wxFileExists(path)){
        path = wxGetCwd();
        path.Replace("\\","/");
        if(!anchor.IsEmpty()){
            /*
            * wxLaunchDefaultBrowser
            * is unable to open a local
            * page with anchor appended.
            * So, we're making a redirector
            * and opening it instead.
            */
            wxString redirector(("./handbook/"));
            redirector << page << "." << anchor << ".html";
            if(!wxFileExists(redirector)){
                wxTextFile file;
                file.Create(redirector);
                file.AddLine("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">");
                file.AddLine("<html><head><meta http-equiv=\"Refresh\" content=\"0; URL=" \
                    + page + "#" + anchor + "\">");
                file.AddLine("</head><body>");
                file.AddLine("Redirecting... if the page has not been redirected automatically click ");
                file.AddLine("<a href=\"" + page + "#" + anchor + "\">here</a>.");
                file.AddLine("</body></html>");
                file.Write();
                file.Close();
            }
            path << "/" << redirector;
        } else {
            path << "/handbook/" << page;
        }
    } else {
        path = "http://ultradefrag.sourceforge.net";
        path << "/handbook/" << page;
        if(!anchor.IsEmpty())
            path << "#" << anchor;
    }

    itrace("%ls",path.wc_str());
    if(path.Left(4) == "http") {
        if(!wxLaunchDefaultBrowser(path))
            ShowError("Cannot open %ls!",path.wc_str());
    } else {
        ShellExec(path,"open");
    }
}

/**
 * @brief Sets priority for the current process.
 * @param[in] priority process priority class.
 * Read MSDN article on SetPriorityClass for details.
 */
bool Utils::SetProcessPriority(int priority)
{
    HANDLE hProcess = ::OpenProcess(PROCESS_SET_INFORMATION,
        FALSE,::GetCurrentProcessId());
    if(!hProcess){
        letrace("cannot open current process");
        return false;
    }

    BOOL result = ::SetPriorityClass(hProcess,(DWORD)priority);
    if(!result) letrace("cannot set process priority");

    ::CloseHandle(hProcess);
    return result != 0 ? true : false;
}

/**
 * @brief Windows ShellExecute analog.
 */
void Utils::ShellExec(
    const wxString& file,
    const wxString& action,
    const wxString& parameters,
    int show, int flags)
{
    SHELLEXECUTEINFO se;
    memset(&se,0,sizeof se);
    se.cbSize = sizeof se;

    se.fMask = SEE_MASK_FLAG_NO_UI;
    if(flags & SHELLEX_NOASYNC)
        se.fMask |= SEE_MASK_FLAG_DDEWAIT;

    se.lpVerb = action.wc_str();
    se.lpFile = file.wc_str();
    if(!parameters.IsEmpty())
        se.lpParameters = parameters.wc_str();

    se.nShow = show;

    if(!ShellExecuteEx(&se)){
        letrace("cannot %ls %ls %ls",
            action.wc_str(), file.wc_str(),
            parameters.wc_str());
        if(!(flags & SHELLEX_SILENT)){
            ShowError("Cannot %ls %ls %ls",
                action.wc_str(), file.wc_str(),
                parameters.wc_str());
        }
    }
}

/**
 * @brief wxMessageDialog analog,
 * but with custom button labels
 * and with ability to center dialog
 * over the parent window.
 * @param[in] parent the parent window.
 * @param[in] caption the dialog caption.
 * @param[in] icon one of the wxART_xxx constants.
 * @param[in] text1 the OK button label.
 * @param[in] text2 the Cancel button label.
 * @param[in] format the format specification.
 * @param[in] ... the parameters.
 */
int Utils::MessageDialog(wxFrame *parent,
    const wxString& caption, const wxArtID& icon,
    const wxString& text1, const wxString& text2,
    const wxChar* format, ...)
{
    wxString message;
    va_list args;
    va_start(args,format);
    message.PrintfV(format,args);
    va_end(args);

    wxDialog dlg(parent,wxID_ANY,caption);

    // wxAppProvider performs double icon conversion:
    // once from icon to bitmap and then back to icon;
    // since it causes the icon to look untidy we're using
    // direct icon loading here
    LPCWSTR id = nullptr;
    if(icon == wxART_QUESTION) id = IDI_QUESTION;
    else if(icon == wxART_WARNING) id = IDI_EXCLAMATION;
    else if(icon == wxART_ERROR) id = IDI_HAND;
    else if(icon == wxART_INFORMATION) id = IDI_ASTERISK;

    HICON hIcon = nullptr;
    if(id){
        hIcon = ::LoadIcon(nullptr,id);
        if(!hIcon) letrace("cannot load icon for \"%ls\"",icon.wc_str());
    }

    wxIcon messageIcon;
    if(hIcon){
        wxSize size = wxGetHiconSize(hIcon);
        messageIcon.SetSize(size.x, size.y);
        messageIcon.SetHICON((WXHICON)hIcon);
    } else {
        messageIcon = wxArtProvider::GetIcon(icon,wxART_MESSAGE_BOX);
    }

    wxStaticBitmap *pic = new wxStaticBitmap(&dlg,wxID_ANY,messageIcon);
    wxStaticText *msg = new wxStaticText(&dlg,wxID_ANY,message,
        wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE);

    wxGridBagSizer* contents = new wxGridBagSizer(0, 0);

    contents->Add(pic, wxGBPosition(0, 0), wxDefaultSpan,
        wxBOTTOM | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL,
        LARGE_SPACING);
    contents->Add(msg, wxGBPosition(0, 1), wxDefaultSpan,
        wxALL & ~wxTOP | wxALIGN_CENTER_HORIZONTAL | \
        wxALIGN_CENTER_VERTICAL,LARGE_SPACING);

    wxButton *ok = new wxButton(&dlg,wxID_OK,text1);
    wxButton *cancel = new wxButton(&dlg,wxID_CANCEL,text2);

    // Burmese needs Padauk font for display
    if(g_locale->GetCanonicalName().Left(2) == "my"){
        wxFont textFont = msg->GetFont();
        if(!textFont.SetFaceName("Padauk")){
            etrace("Padauk font needed for correct Burmese text display not found");
        } else {
            textFont.SetPointSize(textFont.GetPointSize() + 2);
            msg->SetFont(textFont);
            ok->SetFont(textFont);
            cancel->SetFont(textFont);
        }
    }

    wxBoxSizer *buttons = new wxBoxSizer(wxHORIZONTAL);
    buttons->Add(ok,wxSizerFlags(1));
    buttons->AddSpacer(LARGE_SPACING);
    buttons->Add(cancel,wxSizerFlags(1));

    contents->Add(buttons, wxGBPosition(1, 0), wxGBSpan(1, 2),
        wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->AddSpacer(LARGE_SPACING);
    hbox->Add(contents,wxSizerFlags());
    hbox->AddSpacer(LARGE_SPACING);

    wxBoxSizer *space = new wxBoxSizer(wxVERTICAL);
    space->AddSpacer(LARGE_SPACING);
    space->Add(hbox,wxSizerFlags());
    space->AddSpacer(LARGE_SPACING);

    dlg.SetSizerAndFit(space);
    space->SetSizeHints(&dlg);

    if(!parent->IsIconized()) dlg.Center();
    else dlg.CenterOnScreen();

    return dlg.ShowModal();
}

/**
 * @brief Shows an error and
 * invites to open log file.
 */
void Utils::ShowError(const wxChar* format, ...)
{
    wxString message;
    va_list args;
    va_start(args,format);
    message.PrintfV(format,args);
    va_end(args);

    wxString log = _("Open &log");
    log.Replace("&","");

    if(MessageDialog(g_mainFrame,_("Error!"),
      wxART_ERROR,log,_("&Cancel"),message) == wxID_OK)
    {
        PostCommandEvent(g_mainFrame,ID_DebugLog);
    }
}

wxString Utils::ConvertChartoWxString(char* input)
{
    #if wxUSE_UNICODE
        int size = sizeof input + 1;
        wchar_t *buffer = new wchar_t[size*4];  // 32bit chars?
        wxEncodingConverter wxec;
        wxec.Init(wxFONTENCODING_ISO8859_1, wxFONTENCODING_UNICODE, wxCONVERT_SUBSTITUTE);
        wxec.Convert(input, buffer);
        winx_free(input);
        wxString temp(buffer);
        return temp;
    #else
        return wxString(input.c_str());
    #endif
}
char* Utils::wxStringToChar(wxString input)
{
#if (wxUSE_UNICODE)
   size_t size = input.size() + 1;
   char *buffer = new char[size];//No need to multiply by 4, converting to 1 byte char only.
   memset(buffer, 0, size); //Good Practice, Can use buffer[0] = '\0' also.
   wxEncodingConverter wxec;
   wxec.Init(wxFONTENCODING_ISO8859_1, wxFONTENCODING_ISO8859_1, wxCONVERT_SUBSTITUTE);
   wxec.Convert(input.mb_str(), buffer);
   return buffer; //To free this buffer memory is user responsibility.
#else
   return (char *)(input.c_str());
#endif
//Other way:
// convert wxString to const char *
//wxString eh = huh->GetValue();
//const wxCharBuffer eheheh = eh.ToAscii();
}
void Utils::DrawSingleRectangleBorder(HDC m_cacheDC,int xblock,int yblock,int line_width,int cell_size,HBRUSH brush,HBRUSH infill){
    int x = xblock*cell_size;
    int y = yblock*cell_size;
    int w,r;
    w = r = line_width;
    for (int q=0;q <=1; q++,r--){
        RECT rc={x+q*w,y+q*w,x+cell_size+w*r,y+cell_size+w*r};
        ::FillRect(m_cacheDC,&rc,brush);
        brush = infill;
    }
}

void Utils::createDirectoryRecursively(const std::wstring &directory) {
  static const std::wstring separators(L"\\/");

  // If the specified directory name doesn't exist, do our thing
  DWORD fileAttributes = ::GetFileAttributesW(directory.c_str());
  if(fileAttributes == INVALID_FILE_ATTRIBUTES) {

    // Recursively do it all again for the parent directory, if any
    std::size_t slashIndex = directory.find_last_of(separators);
    if(slashIndex != std::wstring::npos) {
      createDirectoryRecursively(directory.substr(0, slashIndex));
    }

    // Create the last directory on the path (the recursive calls will have taken
    // care of the parent directories by now)
    BOOL result = ::CreateDirectoryW(directory.c_str(), nullptr);
    if(result == FALSE) {
      throw std::runtime_error("Could not create directory");
    }

  } else { // Specified directory name already exists as a file or directory

    bool isDirectoryOrJunction =
      (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ||
      (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

    if(!isDirectoryOrJunction) {
      throw std::runtime_error(
        "Could not create directory because a file with the same name exists"
      );
    }

  }
}

/** @} */
