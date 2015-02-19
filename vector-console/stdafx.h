#pragma once

#ifdef _WIN32
// Exclude rarely-used stuff from Windows headers
#	define WIN32_LEAN_AND_MEAN
#	define WINVER 0x0501

#	include <windows.h>

#	ifdef _WIN64
#		pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.GdiPlus' version='1.1.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#	else
#		pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.GdiPlus' version='1.1.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#	endif
#endif

// Disable 4996 warnings
#define _SCL_SECURE_NO_WARNINGS

// STL

// Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

#include <boost/assert.hpp>
#include <boost/exception/all.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include <boost/scope_exit.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/timer.hpp>
#include <boost/locale/encoding_utf.hpp>

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

// PNG
#include "png.h"

// JDK
#include <jni.h>

// Subprojects
#include "../vector-jni/jar.h"
#include "../vector-jni/jvm.h"
#include "../vector-jni/jvm_finder.h"
#include "../vector-jni/base_render.h"
#include "../vector-jni/svg_render.h"
#include "../vector-jni/wmf_render.h"
#include "../vector-console-core/vector-console_core.h"
