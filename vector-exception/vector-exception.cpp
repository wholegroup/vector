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
#include "vector-exception.h"

using namespace ::com::wholegroup::vector::exception;

/************************************************************************/
/* Wide-string constructor.                                             */
/************************************************************************/
ExString::ExString(::std::wstring const & ws)
	: wmessage(ws), 
	message(ws.begin(), ws.end())
{
}

/************************************************************************/
/* Wide-char constructor.                                               */
/************************************************************************/
ExString::ExString(wchar_t const * const wc)
	: wmessage(wc),
	message(::std::string(wmessage.begin(), wmessage.end()))
{
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
::std::wstring const Exception::wwhat() const
{
	auto errorMessage = ::boost::get_error_info<ErrorMessage>(*this);

	if (nullptr == errorMessage)
	{
		::std::string const what(::std::exception::what());

		return std::wstring(what.begin(), what.end());
	}

	return errorMessage->getWMessage();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
char const * Exception::what() const BOOST_NOEXCEPT
{
	auto errorMessage = ::boost::get_error_info<ErrorMessage>(*this);

	if (nullptr == errorMessage)
	{
		return ::std::exception::what();
	}

	return errorMessage->getMessage();
}
