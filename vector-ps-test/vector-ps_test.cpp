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
#include "../vector-ps/ui.h"
#include <SPBasic.h>

using ::testing::Throw;

using namespace ::com::wholegroup::vector;

SPBasicSuite * sSPBasic = NULL;

//
class VectorPsTest
	: public ::testing::Test
{
};

// 
extern "C" BOOL APIENTRY DllMain(HANDLE, DWORD, LPVOID);

//
TEST_F(VectorPsTest, about)
{
	InitCommonControls();

	ps::Ui::doAbout();
}

//
TEST_F(VectorPsTest, settings)
{
	InitCommonControls();

	int    width;
	int    height;
	double resolution;

	ps::Ui::doSettings(width, height, resolution);
}

//
TEST_F(VectorPsTest, getFilenameByHandle_Null)
{
	EXPECT_THROW(ps::Ui::getFilenameByHandle(0), exception::WinApiException);
}

//
TEST_F(VectorPsTest, getFilenameByHandle)
{
	//
	::std::vector<char> moduleFilename(MAX_PATH, 0);
	
	EXPECT_NE(0, GetModuleFileNameA(NULL, &moduleFilename[0], 
		static_cast<DWORD>(moduleFilename.size())));

	::boost::trim_right_if(moduleFilename, 
		::boost::is_any_of(::boost::as_array(L"\0")));

	//
	OFSTRUCT ofStruct;

	HFILE file = ::OpenFile(&moduleFilename[0], &ofStruct, OF_READ);

	EXPECT_EQ(::std::wstring(moduleFilename.begin(), moduleFilename.end()), 
		ps::Ui::getFilenameByHandle((intptr_t)file));

	::CloseHandle((HANDLE)file);
}

//
TEST_F(VectorPsTest, getFilenameByHandle_Net_Disk)
{
	::std::string const filename("C:\\lion.svg");
	//
	OFSTRUCT ofStruct;

	HFILE asd = ::OpenFile(filename.c_str(), &ofStruct, OF_READ);

	if (HFILE_ERROR == asd)
	{
		::std::cout << "File " << filename << " not found." << ::std::endl;
		return;
	}

	try
	{
		::std::wcout << ps::Ui::getFilenameByHandle((intptr_t)asd) << ::std::endl;
	}
	catch (exception::Exception const & ex)
	{
		::std::wcout << ex.wwhat() << ::std::endl;
	}

	::CloseHandle((HANDLE)asd);

	/*
	HFILE file = ::OpenFile(filename.c_str(), &ofStruct, OF_READ);

	EXPECT_EQ(::std::wstring(filename.begin(), filename.end()), 
		ps::Ui::getFilenameByHandle((intptr_t)file));

	::CloseHandle((HANDLE)file);*/
}

//
TEST_F(VectorPsTest, getPathToCore)
{
	::std::wstring pathToCore = ps::Ui::getPathToCore();

	::std::wcout << pathToCore << ::std::endl;
}
