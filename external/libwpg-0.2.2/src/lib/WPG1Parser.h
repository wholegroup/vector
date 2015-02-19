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
 * Copyright (C) 2004 Marc Oude Kotte (marc@solcon.nl)
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

#ifndef __WPG1PARSER_H__
#define __WPG1PARSER_H__

#include "WPGXParser.h"
#include "WPGDashArray.h"
#include "WPGBitmap.h"
#include <libwpd/libwpd.h>
#include <vector>

class WPG1Parser : public WPGXParser
{
public:
	WPG1Parser(WPXInputStream *input, libwpg::WPGPaintInterface *painter);
	bool parse();

private:
	void handleStartWPG();
	void handleEndWPG();

	void handleFillAttributes();
	void handleLineAttributes();
	void handleColormap();

	void handleLine();
	void handlePolyline();
	void handleRectangle();
	void handlePolygon();
	void handleEllipse();

	void handleCurvedPolyline();

	void decodeRLE(std::vector<unsigned char> &buffer, unsigned width, unsigned height, unsigned depth);
	void fillPixels(libwpg::WPGBitmap &bitmap, const unsigned char *buffer, unsigned width, unsigned height, unsigned depth);
	void handleBitmapTypeOne();
	void handleBitmapTypeTwo();
	void handlePostscriptTypeOne();
	void handlePostscriptTypeTwo();

	void handleGraphicsTextAttributes();
	void handleGraphicsTextTypeOne();
	void handleGraphicsTextTypeTwo();
	void handleGraphicsTextTypeThree();

	void resetPalette();

	// parsing context
	int m_recordLength;
	long m_recordEnd;
	bool m_success;
	bool m_exit;
	bool m_graphicsStarted;
	int m_width;
	int m_height;
	::WPXPropertyList m_style;
	libwpg::WPGColor m_penForeColor;
	libwpg::WPGColor m_penBackColor;
	libwpg::WPGColor m_brushForeColor;
	libwpg::WPGColor m_brushBackColor;
	libwpg::WPGDashArray m_dashArray;
	::WPXPropertyListVector m_gradient;
};

#endif // __WPG1PARSER_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
