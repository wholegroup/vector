#pragma once

#ifdef _WIN32
// Exclude rarely-used stuff from Windows headers
#	define WIN32_LEAN_AND_MEAN
#	define WINVER 0x0501

#	include <windows.h>

#	define GDIPVER 0x110

#	include <objidl.h>
#	include <GdiPlus.h>

#	ifdef _WIN64
#		pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.GdiPlus' version='1.1.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#	else
#		pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.GdiPlus' version='1.1.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#	endif
#endif // _WIN32

#ifndef UNREFERENCED_PARAMETER
#	define UNREFERENCED_PARAMETER(P) (void)P
#endif

// Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

#include <boost/exception/all.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include <boost/make_shared.hpp>

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

// Google Test/Mock
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// The Invocation API
#include <jni.h>

// Subprojects
#include "../vector-exception/vector-exception.h"


