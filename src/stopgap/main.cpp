/* Any copyright is dedicated to the Public Domain.
   http://creativecommons.org/publicdomain/zero/1.0/ */
/* Written by Nils Maier in 2014. */
#include "prec.h"

void RedirectIOToConsole() {

    //Create a console for this application
    AllocConsole();

    // Get STDOUT handle
    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
    FILE *COutputHandle = _fdopen(SystemOutput, "w");

    // Get STDERR handle
    HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    FILE *CErrorHandle = _fdopen(SystemError, "w");

    // Get STDIN handle
    HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
    FILE *CInputHandle = _fdopen(SystemInput, "r");

    //make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    std::ios::sync_with_stdio(true);

    // Redirect the CRT standard input, output, and error handles to the console
    freopen_s(&CInputHandle, "CONIN$", "r", stdin);
    freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
    freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

    //Clear the error state for each of the C++ standard stream objects. We need to do this, as
    //attempts to access the standard streams before they refer to a valid target will cause the
    //iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
    //to always occur during startup regardless of whether anything has been read from or written to
    //the console or not.
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();
    // test CrtDbg output
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _RPT0(_CRT_WARN, "This is testing _CRT_WARN output\n");
    _RPT0(_CRT_ERROR, "This is testing _CRT_ERROR output\n");
    _ASSERT(0 && "testing _ASSERT");
    _ASSERTE(0 && "testing _ASSERTE");
}

void stopgap_init_run()
{
    zen::winx zw;
    Operation op;
    
    try
    {
        op.opts.volume = 'A';
        op.init();
        op.run();
    }
    catch (const Exit& ex)
    {
        _exit(ex.code_);
    }
    catch (const std::exception& ex)
    {
        std::wcerr << std::endl << L"Failed to process: " <<
            util::to_wstring(ex.what()) << std::endl;
        InterlockedExchange(&util::ConsoleHandler::gTerminated, 3);
    }
}
/*
int wmain(int argc, wchar_t **argv)
{
  // Any RAII stuff here is leaked on purpose!
  // If any RAII stuff needs to be actually destructed, move it into a block inside the
  // main try-catch block.

  // Set up console.
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);
	
	//std::wcout.imbue(std::locale(""));
	//std::wcerr.imbue(std::locale(""));
  //std::wcout = util::proxy();
  //std::wcerr = util::proxy();

  std::wcout << util::clear;

  // Set up CTRL-C handler
  util::ConsoleHandler ch;

  //zen::winx zw;

  Operation op;
  try {
    util::ConsoleIcon icon;
    op.opts.parseCmdLine(argc, argv);
    op.init();
    op.run();
  }
  catch (const Exit &ex) {
    std::wcout << util::clear;
    _exit(ex.code_);
  }
  catch (const std::exception &ex) {
    std::wcerr << std::endl << L"Failed to process: " << util::red <<
               util::to_wstring(ex.what()) << util::clear << std::endl;
    InterlockedExchange(&util::ConsoleHandler::gTerminated, 3);
  }

  std::wcout << util::clear;

  // Call exit, to avoid all the deallocation stuff!
  _exit(util::ConsoleHandler::gTerminated);
}
*/