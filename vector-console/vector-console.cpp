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

using namespace ::com::wholegroup::vector;

int wmain(int argc, wchar_t const * argv[])
{
	// Set utf-8 for ::std:wcout
	::std::ios::sync_with_stdio(false);
	::std::wcout.imbue(::std::locale(::std::locale(), 
		new ::boost::program_options::detail::utf8_codecvt_facet()));

	// Start
	console::Process process;

	try
	{
		process.start(argc, argv);
	}
	catch (::std::exception& ex)
	{
		::std::cout << "An unknown error :: " << ex.what() << ::std::endl;
	}
	catch (...)
	{
		::std::wcout << L"An unknown error." << ::std::endl;
	}

	return 0;
}

#ifndef _WIN32
int main(int argc, char const * argv[])
{
	// Initialize the parameters
	::std::vector< ::std::wstring> parameters(argc);

	for (int i = 0; i < argc; i++)
	{
		parameters[i] = ::boost::locale::conv::utf_to_utf<wchar_t>(argv[i]);
	}

	// Important: the second cycle is because parameters must be initialized before
	::std::vector<wchar_t const *> wargv(argc);

	for (int i = 0; i < argc; i++)
	{
		wargv[i] = parameters[i].c_str();
	}

	return wmain(argc, &wargv[0]);
}
#endif
