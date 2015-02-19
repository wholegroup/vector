#pragma once

// Exclude rarely-used stuff from Windows headers
#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	define WINVER 0x0501

#	include <windows.h>

#	define GDIPVER 0x110

#	include <objidl.h>
#	include <GdiPlus.h>

#else
#	include <dlfcn.h>
#endif

#ifndef UNREFERENCED_PARAMETER
	#define UNREFERENCED_PARAMETER(P) (void)P
#endif

// Disable 4996 warnings
#define _SCL_SECURE_NO_WARNINGS

// STL
#include <cstddef>
#include <string>
#include <memory>
#include <vector>

// Boost
#define BOOST_FILESYSTEM_NO_DEPRECATED

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

#include <boost/assert.hpp>
#include <boost/exception/all.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include <boost/locale/encoding_utf.hpp>

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

// JDK
#include <jni.h>

// Other
#include <sys/stat.h>

// Subprojects
#include "../vector-exception/vector-exception.h"
