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

#include "WPG1Parser.h"
#include "WPGPaintInterface.h"
#include "libwpg_utils.h"
#include <libwpd/libwpd.h>
#include <libwpd/WPXPropertyList.h>

static const unsigned char defaultWPG1PaletteRed[] =
{
	0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F,
	0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x14, 0x20, 0x2C, 0x38, 0x45, 0x51, 0x61,
	0x71, 0x82, 0x92, 0xA2, 0xB6, 0xCB, 0xE3, 0xFF,
	0x00, 0x41, 0x7D, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0x7D, 0x41,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7D, 0x9E, 0xBE, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xBE, 0x9E,
	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
	0xB6, 0xC7, 0xDB, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEB, 0xDB, 0xC7,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
	0x00, 0x1C, 0x38, 0x55, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x55, 0x38, 0x1C,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x38, 0x45, 0x55, 0x61, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x61, 0x55, 0x45,
	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x51, 0x59, 0x61, 0x69, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x69, 0x61, 0x59,
	0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51,
	0x00, 0x10, 0x20, 0x30, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x30, 0x20, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x20, 0x28, 0x30, 0x38, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x38, 0x30, 0x28,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x3C, 0x30, 0x34, 0x3C, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x3C, 0x34, 0x30,
	0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C, 0x2C,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char defaultWPG1PaletteGreen[] =
{
	0x00, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x3F, 0x7F,
	0xC0, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF,
	0x00, 0x14, 0x20, 0x2C, 0x38, 0x45, 0x51, 0x61,
	0x71, 0x82, 0x92, 0xA2, 0xB6, 0xCB, 0xE3, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x41, 0x7D, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0x7D, 0x41,
	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
	0x7D, 0x9E, 0xBE, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xBE, 0x9E,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
	0xB6, 0xC7, 0xDB, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF,
	0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEB, 0xDB, 0xC7,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1C, 0x38, 0x55, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x55, 0x38, 0x1C,
	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x45, 0x55, 0x61, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x61, 0x55, 0x45,
	0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51,
	0x51, 0x59, 0x61, 0x69, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x69, 0x61, 0x59,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x20, 0x30, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x30, 0x20, 0x10,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x28, 0x30, 0x38, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x38, 0x30, 0x28,
	0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
	0x3C, 0x30, 0x34, 0x3C, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x3C, 0x34, 0x30,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char defaultWPG1PaletteBlue[] =
{
	0x00, 0x7F, 0x00, 0x7F, 0x00, 0x7F, 0x00, 0x7F,
	0xC0, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x14, 0x20, 0x2C, 0x38, 0x45, 0x51, 0x61,
	0x71, 0x82, 0x92, 0xA2, 0xB6, 0xCB, 0xE3, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0x7D, 0x41,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x41, 0x7D, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xBE, 0x9E,
	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
	0x7D, 0x9E, 0xBE, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEB, 0xDB, 0xC7,
	0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
	0xB6, 0xC7, 0xDB, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x55, 0x38, 0x1C,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1C, 0x38, 0x55, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x61, 0x55, 0x45,
	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x45, 0x55, 0x61, 0x71, 0x71, 0x71, 0x71,
	0x71, 0x71, 0x71, 0x71, 0x71, 0x69, 0x61, 0x59,
	0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51,
	0x51, 0x59, 0x61, 0x69, 0x71, 0x71, 0x71, 0x71,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x30, 0x20, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x20, 0x30, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x38, 0x30, 0x28,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x28, 0x30, 0x38, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x3C, 0x34, 0x30,
	0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
	0x2C, 0x30, 0x34, 0x3C, 0x41, 0x41, 0x41, 0x41,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


WPG1Parser::WPG1Parser(WPXInputStream *input, libwpg::WPGPaintInterface *painter):
	WPGXParser(input, painter),
	m_recordLength(0), m_recordEnd(0),
	m_success(true), m_exit(false), m_graphicsStarted(false),
	m_width(0), m_height(0), m_style(),
	m_penForeColor(0,0,0),
	m_penBackColor(0xff,0xff,0xff),
	m_brushForeColor(0,0,0),
	m_brushBackColor(0xff,0xff,0xff),
	m_dashArray(), m_gradient()
{
	m_style.insert("draw:fill", "solid");
	m_style.insert("svg:stroke-color", m_penForeColor.getColorString());
	m_style.insert("svg:stroke-opacity", m_penForeColor.getOpacity(), WPX_PERCENT);
	m_style.insert("draw:fill-color", m_brushForeColor.getColorString());
	m_style.insert("draw:opacity", m_brushForeColor.getOpacity(), WPX_PERCENT);
}

bool WPG1Parser::parse()
{
	typedef void (WPG1Parser::*Method)();

	struct RecordHandler
	{
		int type;
		const char *name;
		Method handler;
	};

	static const struct RecordHandler handlers[] =
	{
		{ 0x01, "Fill Attributes",           &WPG1Parser::handleFillAttributes },
		{ 0x02, "Line Attributes",           &WPG1Parser::handleLineAttributes },
		{ 0x03, "Marker Atttibutes",         0 },
		{ 0x04, "Polymarker",                0 },
		{ 0x05, "Line",                      &WPG1Parser::handleLine },
		{ 0x06, "Polyline",                  &WPG1Parser::handlePolyline },
		{ 0x07, "Rectangle",                 &WPG1Parser::handleRectangle },
		{ 0x08, "Polygon",                   &WPG1Parser::handlePolygon },
		{ 0x09, "Ellipse",                   &WPG1Parser::handleEllipse },
		{ 0x0a, "Reserved",                  0 },
		{ 0x0b, "Bitmap (Type 1)",           &WPG1Parser::handleBitmapTypeOne },
		{ 0x0c, "Graphics Text (Type 1)",    &WPG1Parser::handleGraphicsTextTypeOne },
		{ 0x0d, "Graphics Text Attributes",  &WPG1Parser::handleGraphicsTextAttributes },
		{ 0x0e, "Colormap",                  &WPG1Parser::handleColormap },
		{ 0x0f, "Start WPG",                 &WPG1Parser::handleStartWPG },
		{ 0x10, "End WPG",                   &WPG1Parser::handleEndWPG },
		{ 0x11, "Postscript Data (Type 1)",  &WPG1Parser::handlePostscriptTypeOne },
		{ 0x12, "Output Attributes",         0 },
		{ 0x13, "Curved Polyline",           &WPG1Parser::handleCurvedPolyline },
		{ 0x14, "Bitmap (Type 2)",           &WPG1Parser::handleBitmapTypeTwo },
		{ 0x15, "Start Figure",              0 },
		{ 0x16, "Start Chart",               0 },
		{ 0x17, "Planperfect Data",          0 },
		{ 0x18, "Graphics Text (Type 2)",    &WPG1Parser::handleGraphicsTextTypeTwo },
		{ 0x19, "Start WPG (Type 2)",        0 },
		{ 0x1a, "Graphics Text (Type 3)",    0 },
		{ 0x1b, "Postscript Data (Type 2)",  &WPG1Parser::handlePostscriptTypeTwo },
		{ 0x00, 0, 0 } // end marker
	};

	// initialization
	m_recordLength = 0;
	m_recordEnd = 0;
	m_success = true;
	m_exit = false;
	m_graphicsStarted = false;

	// default style
	m_penForeColor = libwpg::WPGColor(0,0,0);
	m_penBackColor = libwpg::WPGColor(0,0,0);
	m_style.insert("svg:stroke-width", 0.0);
	m_style.insert("draw:stroke", "solid");
	m_dashArray = libwpg::WPGDashArray();
	m_brushForeColor = libwpg::WPGColor(0,0,0);
	m_brushBackColor = libwpg::WPGColor(0,0,0);
	m_style.insert("svg:stroke-color", m_penForeColor.getColorString());
	m_style.insert("svg:stroke-opacity", m_penForeColor.getOpacity(), WPX_PERCENT);
	m_style.insert("draw:fill-color", m_brushForeColor.getColorString());
	m_style.insert("draw:opacity", m_brushForeColor.getOpacity(), WPX_PERCENT);
	resetPalette();

	while(!m_input->atEOS())
	{
#ifdef DEBUG
		long recordPos = m_input->tell();
#endif
		int recordType = readU8();
		if (recordType == 0)
			break;
		m_recordLength = readVariableLengthInteger();
		m_recordEnd = m_input->tell() + m_recordLength - 1;

		// search function to handler this record
		int index = -1;
		for(int i = 0; (index < 0) && handlers[i].name; i++)
			if(handlers[i].type == recordType)
				index = i;

		WPG_DEBUG_MSG(("\n"));
		if(index < 0)
			WPG_DEBUG_MSG(("Unknown record type 0x%02x at %li  size %d\n",
			               recordType, recordPos, m_recordLength));
		else
		{
			Method recordHandler = handlers[index].handler;
			if(!recordHandler)
				WPG_DEBUG_MSG(("Record '%s' (ignored) type 0x%02x at %li  size %d\n",
				               handlers[index].name, recordType, recordPos, m_recordLength));
			else
			{
				WPG_DEBUG_MSG(("Record '%s' type 0x%02x at %li  size %d\n",
				               handlers[index].name, recordType, recordPos, m_recordLength));

				// invoke the handler for this record
				(this->*recordHandler)();
			}
		}

		WPG_DEBUG_MSG(("Current stream position: %li\n", m_input->tell()));

		if(m_exit) break;

		m_input->seek(m_recordEnd+1, WPX_SEEK_SET);
	}

	if (!m_exit)
		handleEndWPG();

	return m_success;
}

void WPG1Parser::handleStartWPG()
{
	if (m_graphicsStarted)
	{
		handleEndWPG();
		return;
	}
	m_input->seek(2, WPX_SEEK_CUR);
	m_width = readU16();
	m_height = readU16();

	::WPXPropertyList propList;
	propList.insert("svg:width", (double)m_width/1200.0);
	propList.insert("svg:height", (double)m_height/1200.0);
	m_painter->startGraphics(propList);
	m_graphicsStarted = true;

	WPG_DEBUG_MSG(("StartWPG\n"));
}

void WPG1Parser::handleEndWPG()
{
	if (!m_graphicsStarted)
		return;
	m_painter->endGraphics();
	m_exit = true;

	WPG_DEBUG_MSG(("EndWPG\n"));
}

void WPG1Parser::handleColormap()
{
	if (!m_graphicsStarted)
		return;
	unsigned startIndex = readU16();
	unsigned numEntries = readU16();
	if (startIndex > 255 || numEntries > 256 || startIndex + numEntries > 256)
		return;

	WPG_DEBUG_MSG(("Colormap\n"));
	for(unsigned int i = 0; i < numEntries; i++)
	{
		unsigned char red = readU8();
		unsigned char green = readU8();
		unsigned char blue = readU8();
		libwpg::WPGColor color(red, green, blue);
		m_colorPalette[startIndex+i] = color;
		WPG_DEBUG_MSG(("Index#%d: RGB %s\n", startIndex+i, color.getColorString().cstr()));
	}
}

void WPG1Parser::handleFillAttributes()
{
	if (!m_graphicsStarted)
		return;
	unsigned char style = readU8();
	unsigned char color = readU8();

	if(style == 0)
		m_style.insert("draw:fill", "none");
	if(style == 1)
		m_style.insert("draw:fill", "solid");

	m_brushForeColor = m_colorPalette[color];
	m_style.insert("draw:fill-color", m_brushForeColor.getColorString());
	m_style.insert("draw:opacity", m_brushForeColor.getOpacity(), WPX_PERCENT);

	WPG_DEBUG_MSG(("Fill Attributes\n"));
	WPG_DEBUG_MSG(("         Fill style: %d\n", style));
	WPG_DEBUG_MSG(("   Fill color index: %d\n", color));
}

void WPG1Parser::handleLineAttributes()
{
	if (!m_graphicsStarted)
		return;
	unsigned char style = readU8();
	unsigned char color = readU8();
	unsigned int width = readU16();

	if (!style)
		m_style.insert("draw:stroke", "none");
	else
		m_style.insert("draw:stroke", "solid");
	m_penForeColor = m_colorPalette[color];
	m_style.insert("svg:stroke-color", m_penForeColor.getColorString());
	m_style.insert("svg:stroke-opacity", m_penForeColor.getOpacity(), WPX_PERCENT);

	if (!width && style)
		m_style.insert("svg:stroke-width", 0.0);
	else
		m_style.insert("svg:stroke-width", (double)width / 1200.0);

	WPG_DEBUG_MSG(("Line Attributes\n"));
	WPG_DEBUG_MSG(("         Line style: %d\n", style));
	WPG_DEBUG_MSG(("   Line color index: %d\n", color));
	WPG_DEBUG_MSG(("         Line width: %d\n", width));
}

void WPG1Parser::handleLine()
{
	if (!m_graphicsStarted)
		return;
	int sx = readS16();
	int sy = readS16();
	int ex = readS16();
	int ey = readS16();

	::WPXPropertyListVector points;
	::WPXPropertyList point;
	point.insert("svg:x", (double)sx/1200.0);
	point.insert("svg:y", (double)(m_height-sy)/1200.0);
	points.append(point);
	point.clear();
	point.insert("svg:x", (double)ex/1200.0);
	point.insert("svg:y", (double)(m_height-ey)/1200.0);
	points.append(point);

	m_painter->setStyle(m_style, m_gradient);

	m_painter->drawPolyline(points);

	WPG_DEBUG_MSG(("Line\n"));
	WPG_DEBUG_MSG(("    Starting point: %d,%d\n", sx, sy));
	WPG_DEBUG_MSG(("         End point: %d,%d\n", ex, ey));
}

void WPG1Parser::handlePolyline()
{
	if (!m_graphicsStarted)
		return;
	unsigned int count = readU16();

	::WPXPropertyListVector points;
	::WPXPropertyList point;
	for(unsigned int i = 0; i < count; i++ )
	{
		point.clear();
		long x = readS16();
		long y = readS16();
		point.insert("svg:x", (double)x/1200.0);
		point.insert("svg:y", (double)(m_height-y)/1200.0);
		points.append(point);
	}

	m_painter->setStyle(m_style, ::WPXPropertyListVector());

	m_painter->drawPolyline(points);

	WPG_DEBUG_MSG(("Polyline\n"));
}

void WPG1Parser::handleRectangle()
{
	if (!m_graphicsStarted)
		return;
	int x = readS16();
	int y = readS16();
	int w = readS16();
	int h = readS16();

	::WPXPropertyList propList;
	propList.insert("svg:x", (double)x/1200.0);
	// in WPG, we have the coordinate of lower left point, in SVG-ish coordinates we have to get upper left
	propList.insert("svg:y", (double)(m_height - h - y)/1200.0);
	propList.insert("svg:width", (double)w/1200.0);
	propList.insert("svg:height",(double)h/1200.0);

	m_painter->setStyle(m_style, m_gradient);

	m_painter->drawRectangle(propList);

	WPG_DEBUG_MSG(("Line\n"));
	WPG_DEBUG_MSG(("    Corner point: %d,%d\n", x, y));
	WPG_DEBUG_MSG(("           Width: %d\n", w));
	WPG_DEBUG_MSG(("          Height: %d\n", h));
}

void WPG1Parser::handlePolygon()
{
	if (!m_graphicsStarted)
		return;
	unsigned int count = readU16();

	::WPXPropertyListVector points;
	::WPXPropertyList point;
	for(unsigned int i = 0; i < count; i++ )
	{
		point.clear();
		long x = readS16();
		long y = readS16();
		point.insert("svg:x", (double)x/1200.0);
		point.insert("svg:y", (double)(m_height-y)/1200.0);
		points.append(point);
	}

	m_painter->setStyle(m_style, m_gradient);

	m_painter->drawPolygon(points);

	WPG_DEBUG_MSG(("Polygon\n"));
}

void WPG1Parser::handleEllipse()
{
	if (!m_graphicsStarted)
		return;

	::WPXPropertyList propList;
	propList.insert("svg:cx", (double)readS16()/1200.0);
	propList.insert("svg:cy", (double)(m_height-readS16())/1200.0);
	propList.insert("svg:rx", (double)readS16()/1200.0);
	propList.insert("svg:ry", (double)readS16()/1200.0);
	propList.insert("libwpg:rotate", (double)readS16());

#if 0
	int beginAngle = readS16();
	int endAngle = readS16();
	unsigned flags = readU16();
#endif
	m_painter->setStyle(m_style, m_gradient);

	m_painter->drawEllipse(propList);

	WPG_DEBUG_MSG(("Ellipse\n"));
	WPG_DEBUG_MSG(("    Center point: %s,%s\n", propList["svg:cx"]->getStr().cstr(), propList["svg:cy"]->getStr().cstr()));
	WPG_DEBUG_MSG(("        Radius x: %s\n", propList["svg:rx"]->getStr().cstr()));
	WPG_DEBUG_MSG(("        Radius y: %s\n", propList["svg:ry"]->getStr().cstr()));
}

void WPG1Parser::handleCurvedPolyline()
{
	if (!m_graphicsStarted)
		return;
	readU32();
	unsigned int count = readU16();
	if (!count)
		return;
	::WPXPropertyListVector path;
	::WPXPropertyList element;

	long xInitial = readS16();
	long yInitial = readS16();
	element.insert("libwpg:path-action", "M");
	element.insert("svg:x", (double)xInitial/1200.0);
	element.insert("svg:y", (double)(m_height-yInitial)/1200.0);
	path.append(element);
	for (unsigned i = 1; i < (count-1)/3; i++)
	{
		long xControl1 = readS16();
		long yControl1 = readS16();
		long xControl2 = readS16();
		long yControl2 = readS16();
		long xCoordinate = readS16();
		long yCoordinate = readS16();

		element.clear();
		element.insert("libwpg:path-action", "C");
		element.insert("svg:x1", (double)xControl1/1200.0);
		element.insert("svg:y1", (double)(m_height-yControl1)/1200.0);
		element.insert("svg:x2", (double)xControl2/1200.0);
		element.insert("svg:y2", (double)(m_height-yControl2)/1200.0);
		element.insert("svg:x", (double)xCoordinate/1200.0);
		element.insert("svg:y", (double)(m_height-yCoordinate)/1200.0);
		path.append(element);

	}

	m_painter->setStyle(m_style, m_gradient);

	m_painter->drawPath(path);

	WPG_DEBUG_MSG(("Curved Polyline\n"));
}

void WPG1Parser::decodeRLE(std::vector<unsigned char> &buffer, unsigned width, unsigned height, unsigned depth)
{
	buffer.clear();

	// This are the known depth values for WPG1, no point to try to decode others since they are likely to indicate corruption
	if (depth != 8 && depth != 4 && depth != 2 && depth != 1)
		return;

	// round to the next byte
	unsigned scanline_width = (width * depth + 7)/8;
	unsigned tmpBufferSize = scanline_width * height;
	WPG_DEBUG_MSG(("Scanline width: %d\n", scanline_width));
	WPG_DEBUG_MSG(("Output size: %d\n", scanline_width * height));

	WPG_DEBUG_MSG(("Decoding RLE data\n"));

	buffer.reserve(tmpBufferSize);
	while (m_input->tell() < m_recordEnd && !m_input->atEOS() && buffer.size() < tmpBufferSize)
	{
		unsigned char opcode = readU8();

		if(opcode & 0x80)
		{
			// run of byte
			int count = (int)(opcode & 0x7f);
			unsigned char pixel = (count > 0) ? readU8() : 0xff;
			if(count == 0)
				count = (int)readU8();
			for( ; count ; --count)
				buffer.push_back( pixel );
		}
		else
		{
			int count = (int)(opcode & 0x7f);
			if(count > 0)
			{
				// literal byte copy
				for( ; count ; --count)
					buffer.push_back( readU8() );
			}
			else
			{
				// copy entire scan line
				count = (int)readU8();
				if (buffer.size() < scanline_width )
				{
					WPG_DEBUG_MSG(("Cannot copy the scanline, not enough data %li\n", (long)buffer.size()));
					break;
				}
				unsigned raster_source = buffer.size() - scanline_width;
				for( ; count; --count)
					for(unsigned r = 0; r < scanline_width ; r++)
					{
						unsigned char pixel = buffer[raster_source + r];
						buffer.push_back( pixel );
					}
			}
		}
	}
	WPG_DEBUG_MSG(("Finish decoding RLE data\n"));
	WPG_DEBUG_MSG(("Buffer length: %li\n", (long)buffer.size()));

	while (buffer.size() < tmpBufferSize)
		buffer.push_back(0);
}

void WPG1Parser::fillPixels(libwpg::WPGBitmap &bitmap, const unsigned char *buffer, unsigned width, unsigned height, unsigned depth)
{
	// sanity
	if(!buffer)
		return;

	if (depth != 8 && depth != 4 && depth != 2 && depth != 1)
		return;

	// round to the next byte
	unsigned scanline_width = (width * depth + 7)/8;

	// 1-bit image: black and white
	if(depth == 1)
	{
		libwpg::WPGColor black(0, 0, 0);
		libwpg::WPGColor white(255, 255, 255);
		for(unsigned y = 0; y < height; y++)
		{
			const unsigned char *buf = buffer + y * scanline_width;
			for(unsigned x = 0; x < width; x++)
				if(buf[x/8] & (0x80 >> (x & 7)))
					bitmap.setPixel(x, y, white);
				else
					bitmap.setPixel(x, y, black);
		}
	}
	// 2-bit image: 4-color bitmap (indexed)
	else if(depth == 2)
	{
		unsigned i = 0;
		for (unsigned y = 0; y < height; y++)
			for (unsigned x = 0; x < width; x++, i++)
			{
				if ((x==0) && (i % 4 != 0))
					i = (i/4 + 1) * 4;
				unsigned index = ((buffer[i/4] & (0x03 << 2*(3 - (i % 4)))) >> 2*(3 - (i % 4)));
				const libwpg::WPGColor &color = m_colorPalette[index];
				bitmap.setPixel(x, y, color);
			}
	}
	// 4 -bit image: 16-colour bitmap (indexed)
	else if(depth == 4)
	{
		unsigned i = 0;
		for (unsigned y = 0; y < height; y++)
			for (unsigned x = 0; x < width; x++, i++)
			{
				if ((x==0) && (i % 2 != 0))
					i = (i/2 + 1) * 2;
				unsigned index = ((buffer[i/2] & (0x0f << 4*(1 - (i % 2)))) >> 4*(1 - (i % 2)));
				const libwpg::WPGColor &color = m_colorPalette[index];
				bitmap.setPixel(x, y, color);
			}
	}
	// 8-bit image: 256-colour image (indexed)
	else if(depth == 8)
	{
		for(unsigned y = 0; y < height; y++)
		{
			const unsigned char *buf = buffer + y * scanline_width;
			for(unsigned x = 0; x < width; x++)
			{
				const libwpg::WPGColor &color = m_colorPalette[buf[x]];
				bitmap.setPixel(x, y, color);
			}
		}

	}
}

void WPG1Parser::handleBitmapTypeOne()
{
	if (!m_graphicsStarted)
		return;
	int width = readS16();
	int height = readS16();
	int depth = readS16();
	int hres = readS16();
	int vres = readS16();

	WPG_DEBUG_MSG(("Bitmap\n"));
	WPG_DEBUG_MSG(("                Width: %d\n", width));
	WPG_DEBUG_MSG(("               Height: %d\n", height));
	WPG_DEBUG_MSG(("                Depth: %d\n", depth));
	WPG_DEBUG_MSG(("Horizontal resolution: %d\n", hres));
	WPG_DEBUG_MSG(("  Vertical resolution: %d\n", vres));

	// if this happens, likely corruption, bail out.
	if (depth != 1 && depth != 2 && depth != 4 && depth != 8)
		return;

	// Sanity checks
	if(hres <= 0)
		hres = 72;
	if(vres <= 0)
		vres = 72;
	if (width < 0)
		width = 0;
	if (height < 0)
		height = 0;

	// Bitmap Type 1 does not specify position
	// Assume on the corner (0,0)

	libwpg::WPGBitmap bitmap(width, height, vres, hres);
	::WPXPropertyList propList;
	propList.insert("svg:x", 0.0);
	propList.insert("svg:y", 0.0);
	propList.insert("svg:width", (double)width/(double)hres);
	propList.insert("svg:height", (double)height/(double)vres);
	propList.insert("libwpg:mime-type", "image/bmp");

	std::vector<unsigned char> buffer;
	decodeRLE(buffer, width, height, depth);
	if (buffer.size() && buffer.size() == (unsigned long)((width*depth + 7)/8)*height)
	{
		fillPixels(bitmap, &buffer[0], width, height, depth);
		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
}

void WPG1Parser::handleBitmapTypeTwo()
{
	if (!m_graphicsStarted)
		return;
	int rotation = readS16();
	int x1 = readS16();
	int y1 = readS16();
	int x2 = readS16();
	int y2 = readS16();
	int width = readS16();
	int height = readS16();
	int depth = readS16();
	int hres = readS16();
	int vres = readS16();

	WPG_DEBUG_MSG(("Bitmap\n"));
	WPG_DEBUG_MSG(("       Rotation Angle: %d\n", rotation));
	WPG_DEBUG_MSG(("      Top left corner: %d,%d\n", x1, y1));
	WPG_DEBUG_MSG(("  Bottom right corner: %d,%d\n", x2, y2));
	WPG_DEBUG_MSG(("                Width: %d\n", width));
	WPG_DEBUG_MSG(("               Height: %d\n", height));
	WPG_DEBUG_MSG(("                Depth: %d\n", depth));
	WPG_DEBUG_MSG(("Horizontal resolution: %d\n", hres));
	WPG_DEBUG_MSG(("  Vertical resolution: %d\n", vres));

	// if this happens, likely corruption, bail out.
	if (rotation < 0 || rotation > 359)
		return;
	if (depth != 1 && depth != 2 && depth != 4 && depth != 8)
		return;

	// Sanity checks
	if(hres <= 0)
		hres = 72;
	if(vres <= 0)
		vres = 72;
	if (width < 0)
		width = 0;
	if (height < 0)
		height = 0;

	y1 = m_height - y1;
	y2 = m_height - y2;

	long xs1 = (x1 <= x2) ? x1 : x2;
	long xs2 = (x1 <= x2) ? x2 : x1;
	long ys1 = (y1 <= y2) ? y1 : y2;
	long ys2 = (y1 <= y2) ? y2 : y1;
	WPG_DEBUG_MSG(("%li %li   %li %li\n", xs1, ys1, xs2, ys2));

	libwpg::WPGBitmap bitmap(width, height, vres, hres);
	::WPXPropertyList propList;
	propList.insert("svg:x", (double)xs1/(double)hres);
	propList.insert("svg:y", (double)(ys1)/(double)vres);
	propList.insert("svg:width", (double)(xs2-xs1)/(double)hres);
	propList.insert("svg:height", (double)(ys2-ys1)/(double)vres);
	propList.insert("libwpg:mime-type", "image/bmp");

	std::vector<unsigned char> buffer;
	decodeRLE(buffer, width, height, depth);
	if (buffer.size()  && buffer.size() == (unsigned long)((width*depth + 7)/8)*height)
	{
		fillPixels(bitmap, &buffer[0], width, height, depth);
		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
}

void WPG1Parser::handlePostscriptTypeOne()
{
	if (!m_graphicsStarted)
		return;
	long x1 = readS16();
	long y1 = readS16();
	long x2 = readS16();
	long y2 = readS16();

	::WPXPropertyList propList;

	propList.insert("svg:x", (double)x1/72.0);
	propList.insert("svg:y", (double)m_height/1200.0 - (double)y1/72.0);
	propList.insert("svg:width", ((double)x2 - (double)x1)/72.0);
	propList.insert("svg:height", ((double)y1 - (double)y2)/72.0);
	propList.insert("libwpg:mime-type", "application/x-postscript");

	::WPXBinaryData data;
	data.clear();
	while (!m_input->atEOS() && m_input->tell() <= m_recordEnd)
		data.append((char)readU8());
	if (data.size())
		m_painter->drawGraphicObject(propList, data);
}

void WPG1Parser::handlePostscriptTypeTwo()
{
	if (!m_graphicsStarted)
		return;
#ifdef DEBUG
	unsigned lengthOfData = readU32();
	int rotation = readS16();
#else
	readU32();
	readS16();
#endif
	int x1 = readS16();
	int y1 = readS16();
	int x2 = readS16();
	int y2 = readS16();

	WPG_DEBUG_MSG(("Postscript (Type 2)\n"));
	WPG_DEBUG_MSG(("       Length of Data: %d\n", lengthOfData));
	WPG_DEBUG_MSG(("       Rotation Angle: %d\n", rotation));
	WPG_DEBUG_MSG(("   Bottom left corner: %d,%d\n", x1, y1));
	WPG_DEBUG_MSG(("     Top right corner: %d,%d\n", x2, y2));

	y1 = m_height - y1;
	y2 = m_height - y2;

	long xs1 = (x1 <= x2) ? x1 : x2;
	long xs2 = (x1 <= x2) ? x2 : x1;
	long ys1 = (y1 <= y2) ? y1 : y2;
	long ys2 = (y1 <= y2) ? y2 : y1;
	WPG_DEBUG_MSG(("%li %li   %li %li\n", xs1, ys1, xs2, ys2));

	::WPXPropertyList propList;

	propList.insert("svg:x", (double)xs1/1200.0);
	propList.insert("svg:y", (double)(ys1)/1200.0);

	propList.insert("svg:width", ((double)xs2 - (double)xs1)/1200.0);
	propList.insert("svg:height", ((double)ys2 -(double)ys1)/1200.0);

	propList.insert("libwpg:mime-type", "image/x-eps");

	m_input->seek(48, WPX_SEEK_CUR);

	::WPXBinaryData data;
	data.clear();
	while (!m_input->atEOS() && m_input->tell() <= m_recordEnd)
		data.append((char)readU8());
	if (data.size())
		m_painter->drawGraphicObject(propList, data);
}

void WPG1Parser::handleGraphicsTextAttributes()
{
	if (!m_graphicsStarted)
		return;
}

void WPG1Parser::handleGraphicsTextTypeOne()
{
	if (!m_graphicsStarted)
		return;
	unsigned short textLength = readU16();
	int x = readS16();
	int y = readS16();
	WPXString textString;
	for (unsigned short i=0; i < textLength; i++)
		textString.append(readU8());

	WPG_DEBUG_MSG(("Graphics Text (Type 1)\n"));
	WPG_DEBUG_MSG(("  Length of String: %d\n", textLength));
	WPG_DEBUG_MSG(("   String position: %d,%d\n", x, y));
	WPG_DEBUG_MSG(("            String: %s\n", textString.cstr()));

	y = m_height - y;

	::WPXPropertyList propList;

	propList.insert("svg:x", (double)x/1200.0);
	propList.insert("svg:y", (double)(y)/1200.0);

	m_painter->startTextObject(propList, ::WPXPropertyListVector());

	m_painter->insertText(textString);

	m_painter->endTextObject();
}

void WPG1Parser::handleGraphicsTextTypeTwo()
{
	if (!m_graphicsStarted)
		return;
#ifdef DEBUG
	int sizeEquivalentData = readU32();
	short rotationAngle = readS16();
#endif
	unsigned short textLength = readU16();
#ifdef DEBUG
	int x1 = readS16();
	int y1 = readS16();
	int x2 = readS16();
	int y2 = readS16();
	int sx = readS16();
	int sy = readS16();
	unsigned char type = readU8();
#endif
	WPXBinaryData textString;
	for (unsigned short i=0; i < textLength; i++)
		textString.append(readU8());

	WPG_DEBUG_MSG(("Graphics Text (Type 1)\n"));
	WPG_DEBUG_MSG(("  Size of Equivalent data: %d\n", sizeEquivalentData));
	WPG_DEBUG_MSG(("           Rotation Angle: %d\n", rotationAngle));
	WPG_DEBUG_MSG(("         Length of String: %d\n", textLength));
	WPG_DEBUG_MSG(("           Start position: %d,%d\n", x1, y1));
	WPG_DEBUG_MSG(("             End position: %d,%d\n", x2, y2));
	WPG_DEBUG_MSG(("             Scale factor: %d,%d\n", sx, sy));
	WPG_DEBUG_MSG(("                     Type: %d\n", type));

//	WPGTextDataHandler handler(m_painter);
//	WPDocument::parseSubDocument(const_cast<WPXInputStream *>(textString.getDataStream()), &handler, WPD_FILE_FORMAT_WP5);
}

void WPG1Parser::resetPalette()
{
	m_colorPalette.clear();
	for (int i=0; i<256; i++)
	{
		unsigned char red = defaultWPG1PaletteRed[i];
		unsigned char green = defaultWPG1PaletteGreen[i];
		unsigned char blue = defaultWPG1PaletteBlue[i];
		libwpg::WPGColor color(red, green, blue);
		m_colorPalette[i] = color;
	}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
