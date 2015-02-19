/*
 * Copyright (C) 2015 Andrey Rychkov <wholegroup@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "version.h"

using namespace ::com::wholegroup::vector::ps;

#define STRINGIFY(x) L ## #x
#define TOSTRING(x) STRINGIFY(x)

namespace com {namespace wholegroup {namespace vector {namespace ps
{

// Manufacturer
::std::wstring const & Version::MANUFACTURER()
{
	static const ::std::wstring _MANUFACTURER(L"Whole Group");

	return _MANUFACTURER;
}

// The plugin version
::std::wstring const & Version::VERSION()
{
	static const ::std::wstring _VERSION(TOSTRING(PROJECT_VERSION));

	return _VERSION;
}

// The product name
#ifdef __APPLE__
::std::wstring const & Version::PRODUCT()
{
	static const ::std::wstring _PRODUCT(TOSTRING(PRODUCT_NAME));

	return _PRODUCT;
}
#endif


// The plugin name
::std::wstring const & Version::NAME()
{
#ifdef BUILD_SVG
	static const ::std::wstring _NAME(L"SVG Import Plugin");
#elif defined(BUILD_WMF)
	static const ::std::wstring _NAME(L"WMF Import Plugin");
#elif defined(BUILD_EMF)
	static const ::std::wstring _NAME(L"EMF Import Plugin");
#elif defined(BUILD_WPG)
	static const ::std::wstring _NAME(L"WPG Import Plugin");
#elif defined(BUILD_CDR)
	static const ::std::wstring _NAME(L"CDR Import Plugin");
#else
#	pragma message("ERROR: You should set the build mode.")
	static const ::std::wstring _NAME(L"");
#endif

	return _NAME;
}

// The format name
::std::wstring const & Version::FORMAT()
{
#ifdef BUILD_SVG
	static const ::std::wstring _FORMAT(L"SVG");
#elif defined(BUILD_WMF)
	static const ::std::wstring _FORMAT(L"WMF");
#elif defined(BUILD_EMF)
	static const ::std::wstring _FORMAT(L"EMF");
#elif defined(BUILD_WPG)
	static const ::std::wstring _FORMAT(L"WPG");
#elif defined(BUILD_CDR)
	static const ::std::wstring _FORMAT(L"CDR");
#else
#	pragma message("ERROR: You should set the format.")
	static const ::std::wstring _FORMAT(L"");
#endif
	return _FORMAT;
}

};};};};

#undef STRINGIFY
#undef TOSTRING
