#pragma once

#ifdef _WIN32
// Exclude rarely-used stuff from Windows headers
#	define WIN32_LEAN_AND_MEAN
#	define WINVER 0x0501

#	include <windows.h>
#	include <commctrl.h>

#	pragma comment(lib, "comctl32.lib")
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif // WIN32

// Disable 4996 warnings
#define _SCL_SECURE_NO_WARNINGS

// Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/bimap.hpp>
#include <boost/exception/all.hpp>
#include <boost/range/as_array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

// Google Test/Mock
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Photoshop API
//#include "PIDefines.h"
//#include "PIFormat.h"

// Subprojects
#include "../vector-exception/vector-exception.h"
#include "../vector-ps/ui.h"
