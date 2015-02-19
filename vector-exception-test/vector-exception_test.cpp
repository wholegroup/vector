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
#include "../vector-exception/vector-exception.h"

using namespace ::com::wholegroup::vector;

//
class ExceptionTest
	: public	::testing::Test
{
};

class ExceptionTestDeath: public ExceptionTest {};

//
TEST_F(ExceptionTest, throwException)
{
	EXPECT_THROW(throw exception::Exception(), 
		exception::Exception);
	EXPECT_THROW(throw exception::NullPointerException(), 
		exception::NullPointerException);
	EXPECT_THROW(throw exception::InvalidTypeException(), 
		exception::InvalidTypeException);
	EXPECT_THROW(throw exception::InvalidValueException(), 
		exception::InvalidValueException);
	EXPECT_THROW(throw exception::IOException(), 
		exception::IOException);
	EXPECT_THROW(throw exception::WinApiException(), 
		exception::WinApiException);
}

//
TEST_F(ExceptionTest, exceptionMessage)
{
	// what()
	try
	{
		throw exception::Exception() << 
			exception::ErrorMessage(L"Test Message");
	}
	catch (exception::Exception const & e)
	{
		EXPECT_EQ(::std::string("Test Message"), ::std::string(e.what()));
	}

	// wwhat()
	try
	{
		throw exception::Exception() << 
			exception::ErrorMessage(L"\u1F78");
	}
	catch (exception::Exception const & e)
	{
		::std::wstring wwhat = ::std::wstring(L"\u1F78");

		EXPECT_EQ(wwhat, e.wwhat());
	}
}
