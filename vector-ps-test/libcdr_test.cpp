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

#include <boost/locale/encoding_utf.hpp>

#include <libcdr/libcdr.h>
#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>

//
class LibCdrTest
	: public ::testing::Test
{
};


TEST_F(LibCdrTest, CDR2SVG)
{
	//
	librevenge::RVNGFileStream input("D:\\Temp\\Test\\aflag.cdr");

	if (!libcdr::CDRDocument::isSupported(&input))
	{
		std::cerr << "ERROR: Unsupported file format!" << std::endl;

		return;
	}

	//
	librevenge::RVNGStringVector output;
	librevenge::RVNGSVGDrawingGenerator generator(output, "svg");

	if (!libcdr::CDRDocument::parse(&input, &generator))
	{
		std::cerr << "ERROR: SVG Generation failed!" << std::endl;

		return;
	}

	//
	for (unsigned k = 0; k < output.size(); ++k)
	{
		//std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
		//std::cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"";
		//std::cout << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";

		std::cout << output[k].cstr() << std::endl;

		// output the first image only 
		break;
	}
}

TEST_F(LibCdrTest, CMX2SVG)
{
	//
	librevenge::RVNGFileStream input("D:\\Temp\\Test\\miscs-0011.cmx");

	if (!libcdr::CMXDocument::isSupported(&input))
	{
		std::cerr << "ERROR: Unsupported file format!" << std::endl;

		return;
	}

	//
	librevenge::RVNGStringVector output;
	librevenge::RVNGSVGDrawingGenerator generator(output, "svg");

	if (!libcdr::CMXDocument::parse(&input, &generator))
	{
		std::cerr << "ERROR: SVG Generation failed!" << std::endl;

		return;
	}

	//
	for (unsigned k = 0; k < output.size(); ++k)
	{
		std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
		std::cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"";
		std::cout << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";

		std::cout << output[k].cstr() << std::endl;

		// output the first image only 
		//break;
	}
}

TEST_F(LibCdrTest, CdrFromUnc)
{
	::std::wcout << L"Test" << ::std::endl;

	::std::wstring filePath = L"\\\\?\\UNC\\ANDREY-Y560\\Public\\TSD5025.cdr";
	::std::string filePathA = ::boost::locale::conv::utf_to_utf<char>(filePath);

	//
	librevenge::RVNGFileStream input(filePathA.c_str());

	if (!libcdr::CDRDocument::isSupported(&input))
	{
		std::cerr << "ERROR: Unsupported file format!" << std::endl;

		return;
	}
}