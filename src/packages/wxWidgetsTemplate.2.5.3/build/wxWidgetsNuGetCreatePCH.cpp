/**
 * Synopsis:
 *  This file is built first in the compilation process. See readme/wiki for
 *  details. This file won't be built if you had set a different cpp file to
 *  create the PCH. The intention of this is to speed up any project that
 *  installs this NuGet package, as wxWidgets is large. If the project already
 *  had a PCH, this file wouldn't be built.
 *  This significantly speeds up complilation of a C++ project using wxWidgets.
 *  If you have a large project (which includes many more rarely changing
 *  header files), I suggest you set up your own PCH, and add those rarely
 *  changing header files to that (in addition to wx/wxprec.h).
 *  http://gamesfromwithin.com/the-care-and-feeding-of-pre-compiled-headers
 *
 * Instructions:
 *  Place at the top of all of your .cpp files:
 *   #include <wx/wxprec.h>
 *
 *  When including other wxWidgets header files, add following this example:
 *   #include <wx/wxprec.h>  //Place at top
 *   #include <wx/thread.h>  //Not included by wx/wxprec.h
 *   #include <wx/xml/xml.h> //Not included by wx/wxprec.h
 *   #include <wx/string.h>  //Included by wx/wxprec.h, this can be removed!
 *
 * DO NOT MODIFY THIS FILE! Create your own PCH and copy this if you want
 *  to make modifications. This file may be overwritten by NuGet on updates.
 */
#include <wx/wxprec.h>
#ifdef __WXMSW__ // See http://wiki.wxwidgets.org/Avoiding_Memory_Leaks
    #include <wx/msw/msvcrt.h>      // redefines the new() operator
#endif
#if !defined(_INC_CRTDBG) || !defined(_CRTDBG_MAP_ALLOC)
    #error Debug CRT functions have not been included!
#endif