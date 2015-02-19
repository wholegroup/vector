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
#include "jvm.h"
#include "base_render.h"
#include "cdr_render.h"

#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>
#include <libcdr/libcdr.h>

using namespace ::com::wholegroup::vector::jni;

CdrRender::CdrRender(Jvm* jvm)
	: BaseRender(jvm, L"Cdr")
{
}

void CdrRender::load(const ::std::wstring & pathFile)
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	//
	if (!::boost::filesystem::exists(pathFile))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"The file is not found."));
	}

	//
	::std::vector<wchar_t> shortPath(GetShortPathNameW(pathFile.c_str(), nullptr, 0), 0x01);

	if (0 < shortPath.size())
	{
		if ((shortPath.size() - 1) != GetShortPathNameW(pathFile.c_str(), 
			&shortPath[0], static_cast<DWORD>(shortPath.size())))
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An error getting the short path."));
		}
	}
	else // may be it is a network path
	{
		shortPath = ::std::vector<wchar_t>(pathFile.begin(), pathFile.end());
	}

	::std::wstring const usedFilePath = ::std::wstring(shortPath.begin(), shortPath.end());

	// 
	librevenge::RVNGFileStream input(::boost::locale::conv::utf_to_utf<char>(usedFilePath.c_str()).c_str());

	if (!libcdr::CDRDocument::isSupported(&input))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Unsupported file format!"));

		return;
	}

	//
	librevenge::RVNGStringVector output;
	librevenge::RVNGSVGDrawingGenerator generator(output, "svg");

	if (!libcdr::CDRDocument::parse(&input, &generator))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"SVG Generation failed!"));

		return;
	}

	//
	if (0 < output.size())
	{
		this->loadFromByteArray(output[0].len(), output[0].cstr());
	}
	else
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"There are no images!"));
	}
}
