#pragma once

// Windows
#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#	define WINVER 0x0501

#	include <Windows.h>
#	include <Psapi.h>
#	include <Winternl.h>
#	include <ShellAPI.h>

#	define GDIPVER 0x110

#	include <objidl.h>
#	include <GdiPlus.h>

#	ifdef BUILD_EMF
#		ifdef _WIN64
#			pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.GdiPlus' version='1.1.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#		else
#			pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.GdiPlus' version='1.1.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#		endif
#	endif // BUILD_EMF

#endif // _WIN32

#ifndef UNREFERENCED_PARAMETER
#	define UNREFERENCED_PARAMETER(P) (void)P
#endif // UNREFERENCED_PARAMETER

// Disable 4996 warnings
#define _SCL_SECURE_NO_WARNINGS

// CRT
#include <math.h>

// STL
#include <vector>
#include <iterator>
#include <numeric>

// Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#include <boost/exception/all.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/range/as_array.hpp>
#include <boost/crc.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

// Photoshop API
#include "PIDefines.h"
#include "PIFormat.h"
#include "PIAbout.h"
#include "PITerminology.h"
#include "PIHandleSuite.h"
#include "PIBufferSuite.h"
#include "PIProgressSuite.h"
#include "ASZStringSuite.h"
#include "SPInterf.h"
#include "PIProperties.h"

// JDK
#include <jni.h>

// Subprojects
#include "../vector-exception/vector-exception.h"
#include "../vector-jni/jar.h"
#include "../vector-jni/jvm.h"
#include "../vector-jni/jvm_finder.h"
#include "../vector-jni/base_render.h"
#include "../vector-jni/svg_render.h"
#include "../vector-jni/wmf_render.h"
#include "../vector-jni/emf_render.h"
#include "../vector-jni/wpg_render.h"
#include "../vector-jni/cdr_render.h"
