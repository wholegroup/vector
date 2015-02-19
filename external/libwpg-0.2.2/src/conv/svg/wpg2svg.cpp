/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* libwpg
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
 * Copyright (C) 2005 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 *
 * For further information visit http://libwpg.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by
 * Corel Corporation or Corel Corporation Limited."
 */

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "libwpg.h"
#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>

namespace
{

int printUsage()
{
	printf("Usage: wpg2svg [OPTION] <WordPerfect Graphics File>\n");
	printf("\n");
	printf("Options:\n");
	printf("--help                Shows this help message\n");
	printf("--version             Output wpg2svg version \n");
	return -1;
}

int printVersion()
{
	printf("wpg2svg %s\n", LIBWPG_VERSION_STRING);
	return 0;
}

} // anonymous namespace

int main(int argc, char *argv[])
{
	if (argc < 2)
		return printUsage();

	char *file = 0;

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--version"))
			return printVersion();
		else if (!file && strncmp(argv[i], "--", 2))
			file = argv[i];
		else
			return printUsage();
	}

	if (!file)
		return printUsage();

	WPXFileStream input(file);

	if (!libwpg::WPGraphics::isSupported(&input))
	{
		std::cerr << "ERROR: Unsupported file format (unsupported version) or file is encrypted!" << std::endl;
		return 1;
	}

	::WPXString output;
	if (!libwpg::WPGraphics::generateSVG(&input, output))
	{
		std::cerr << "ERROR: SVG Generation failed!" << std::endl;
		return 1;
	}

	std::cout << output.cstr() << std::endl;
	return 0;
}
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
