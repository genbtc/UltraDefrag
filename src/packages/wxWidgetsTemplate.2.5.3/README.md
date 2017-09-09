# wxWidgetsTemplateNuGet
The license for this package and its contents are that of the wxWidgets project: http://www.wxwidgets.org/about/licence/
Copyright (c) 2013-2015 Andrew Smart

This NuGet package assists MSVC projects that depend on wxWidgets.

Guide to installing and configuring this package is here:
https://wiki.wxwidgets.org/Microsoft_Visual_C++_NuGet

There is no need to configure this package if the WXWIN environmental
variable defined, and the wxWidgets DLLs exist there.

This package modifies the project in many ways, including:
* Defining wxWidgets preprocessor directives on the project level, where
  appropriate:
  * _WINDOWS, __WXMSW__, WXUSINGDLL, __WXDEBUG__, WIN32, __WXUNIVERSAL__,
    wxTOOLKIT_PREFIX=mswuniv
* Adds the wxWidgets include folder to the compiler search paths.
* Adds the wxWidgets library folder to the compiler search paths.
* Links against all appropriate wxWidgets libraries.
  * Implemented via the PCH including wx/wxprec.h, which includes
    msvc/wx/setup.h
  * See msvc/wx/setup.h for details.
* Allows any developer on the project to link to any wxWidgets build they want.
  * e.g. link to static/dynamic build
  * e.g. link to monolithic build
  * Their decision, without this NuGet package, would otherwise modify the
    .vcxproj file, forcing their choice on every developer on the project.
  * There is no need to define the WXWIN environmental variable; though it will
    use it as the default if it is defined.
* In project output folder (where .exe is placed), hardlinks to all relevant
  wxWidgets .dll, .manifest, .pdb are written.
  * This is to help you know what .dll files your .exe depends on; these may be
    redistributed with the .exe.
  * Also this allows the .exe to be run locally within this folder, as it will
    be able to find the .dll files it depends on.
  * Microsoft Explorer is incorrect in the display of hardlink file sizes, it
    reports the file size of what the hardlink links to.
  * Read more about hardlinks (and MSBuild's creation of them) if interested.
  * I'm not sure if anyone even needs the .manifest/.pdb files here, but I
    hardlinked them here just in case someone does.
* Creates a PCH to more than halve compilation time.
  * It won't override any other PCH set up in the project (in that PCH I
    suggest including wx/wxprec.h).
  * If your project is large I suggest not relying on this package's PCH, as it
    only includes wx/wxprec.h, and cannot include any other rarely changing
    header files your project has.
  * You must place #include <wx/wxprec.h> at the top of all the project's
    source (.cpp) files, otherwise there will be a build error.
  * If you don't want a source (.cpp) file to use the wxWidgets PCH (i.e. it
    doesn't use wxWidgets), right click on that source file, and set it to not
    use a PCH.
* Sets up project to use MSVC memory leak detection facilities:
  https://wiki.wxwidgets.org/Avoiding_Memory_Leaks#Automatic_leak_detection_with_Microsoft_VisualC.2B.2B
  * Defines _CRTDBG_MAP_ALLOC preprocessor directive.
  * Employs project wide usage of memory leak detection via directives in the
    packaged PCH
  * When in debug mode of a debug configuration, and application is exited,
    any memory leaks will be printed to the output pane of Visual Studio with
    a message like so:
      Detected memory leaks!
      Dumping objects ->

Modern wxWidgets is built using the Unicode character set; starting around
Visual Studio 2012 new VC++ projects use the Unicode character set by default.
* You may need to manually set this Unicode setting in order for your
  executable to link against the wxWidgets library without problems.
* This setting is in the project property pages, under the General category.
* Here is some helpful information if you run into issues here (e.g. some
  libraries are ANSI, some are Unicode, and what to set your project to compile
  with):
  http://stackoverflow.com/questions/10241540/implications-of-using-non-unicode-library-with-a-unicode-built-application

NuGet/C++ technology described here:
http://blogs.msdn.com/b/vcblog/archive/2013/06/18/deep-dive-into-nuget-native-part-one.aspx