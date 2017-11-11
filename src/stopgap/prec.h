#ifndef PREC_H
#define PREC_H

#define BOOST_DISABLE_THREADS 1

#if defined(BOOST_NO_CXX11_NULLPTR)
#define nullptr 0
#endif

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

#include <boost/program_options.hpp>
//#include <boost/pool/pool_alloc.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/config.hpp>
//#include <boost/dynamic_bitset.hpp>
#include <boost/multi_array.hpp>
#include <boost/regex.hpp>

#include <windows.h>

#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"version.lib")
#pragma comment(lib,"Advapi32.lib")

#include "stopgap.hpp"
#include "util.hpp"
#include "resource.h"
#include "zen.hpp"
#include "op.hpp"

#ifdef __clang__
//This is a known issue when compiling on Windows with Clang. Presumably, the problem is in the Win 8 "Windows.h" header.
// [http://clang-developers.42468.n3.nabble.com/CLang-compiling-windows-h-from-the-Windows-SDK-td1043938.html]
// GENBTC 
struct IUnknown;    //fix for CLANG
#endif

#endif //PREC_H