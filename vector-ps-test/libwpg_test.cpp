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

#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>

//
class LibWpgTest
	: public ::testing::Test
{
};


TEST_F(LibWpgTest, WPG2SVG)
{
	//
	WPXFileStream input("D:\\Temp\\Test\\DOG.WPG");

	if (!libwpg::WPGraphics::isSupported(&input))
	{
		std::cerr << "ERROR: Unsupported file format!" << std::endl;

		return;
	}

	//
	::WPXString output;

	if (!libwpg::WPGraphics::generateSVG(&input, output))
	{
		std::cerr << "ERROR: SVG Generation failed!" << std::endl;

		return;
	}

	//
	std::cout << output.cstr() << std::endl;
}