#ifndef UTILS_WIN32_H_INCLUDED
#define UTILS_WIN32_H_INCLUDED

//#define _WIN32_IE 0x0400
////#define _WIN32_WINNT 0x0500
//
//#define __readableTo(extent)
//#define __nullterminated    __readableTo(sentinel(0))
//typedef __nullterminated const wchar_t *LPCWSTR, *PCWSTR;
#include <Windows.h>
#include <string>

class UtilsWin32 {
public:
    static void BrowseToFile(LPCWSTR filename);
    static void toClipboard(const std::string &s);
};

#endif // UTILS-WIN32_H_INCLUDED
