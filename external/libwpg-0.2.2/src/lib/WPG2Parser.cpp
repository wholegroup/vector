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
 * Copyright (C) 2005-2007 Fridrich Strba (fridrich.strba@bluewin.ch)
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

#define DUMP_BINARY_DATA 0

#include "WPG2Parser.h"
#include "WPGPaintInterface.h"
#include "libwpg_utils.h"

#include <math.h>
#include <vector>

// MSVC++ 6.0 does not have the macro defined, so we define it
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#if DUMP_BINARY_DATA
#include <sstream>
#endif

namespace
{

static double fixedPointToDouble(const unsigned int fixedPointNumber)
{
	short fixedPointNumberIntegerPart = (short)((fixedPointNumber & 0xFFFF0000) >> 16);
	double fixedPointNumberFractionalPart = (double)((double)(fixedPointNumber & 0x0000FFFF)/(double)0xFFFF);
	return ((fixedPointNumberIntegerPart >= 0) ?
	        ((double)fixedPointNumberIntegerPart + fixedPointNumberFractionalPart) :
	        ((double)fixedPointNumberIntegerPart - fixedPointNumberFractionalPart));
}
}

static const unsigned char defaultWPG2PaletteRed[] =
{
	0x00, 0xFF, 0x7F, 0xBF, 0x00, 0x00, 0x00, 0x7F,
	0x7F, 0x7F, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x3D, 0x48, 0x53, 0x5E, 0x69, 0x74, 0x7F, 0x8A,
	0x95, 0xA0, 0xAB, 0xB6, 0xC1, 0xCC, 0xD7, 0xE2,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x3D, 0x48, 0x53, 0x5E, 0x69, 0x74, 0x7F, 0x8A,
	0x95, 0xA0, 0xAB, 0xB6, 0xC1, 0xCC, 0xD7, 0xE2,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x33, 0x47, 0x61, 0x73, 0x87, 0x9C, 0xB0, 0xC7,
	0xCC, 0xD4, 0xDB, 0xE3, 0xE8, 0xF0, 0xF7, 0xFF,
};

static const unsigned char defaultWPG2PaletteGreen[] =
{
	0x00, 0xFF, 0x7F, 0xBF, 0x00, 0x7F, 0x7F, 0x00,
	0x00, 0x7F, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x3D, 0x48, 0x53, 0x5E, 0x69, 0x74, 0x7F, 0x8A,
	0x95, 0xA0, 0xAB, 0xB6, 0xC1, 0xCC, 0xD7, 0xE2,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x56, 0x64, 0x72, 0x80, 0x8E, 0x9C, 0xAA, 0xB1,
	0xB8, 0xBF, 0xC6, 0xCD, 0xD4, 0xDB, 0xE2, 0xE9,
	0x2B, 0x32, 0x39, 0x40, 0x47, 0x4E, 0x55, 0x63,
	0x71, 0x7F, 0x8D, 0x9B, 0xA9, 0xB7, 0xC5, 0xD3,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x29, 0x38, 0x45, 0x4F, 0x5C, 0x63, 0x69, 0xD4,
	0x87, 0x8F, 0x9C, 0xA8, 0xB3, 0xC4, 0xCF, 0xE0,
};

static const unsigned char defaultWPG2PaletteBlue[] =
{
	0x00, 0xFF, 0x7F, 0xBF, 0x7F, 0x00, 0x7F, 0x00,
	0x7F, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x2C, 0x42, 0x58, 0x6E, 0x84, 0x9A, 0xB0, 0xC6,
	0x3D, 0x48, 0x53, 0x5E, 0x69, 0x74, 0x7F, 0x8A,
	0x95, 0xA0, 0xAB, 0xB6, 0xC1, 0xCC, 0xD7, 0xE2,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE9, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xB0, 0xD3, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x7B, 0x91, 0xA7, 0xBD, 0xD3, 0xE4, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x3D, 0x48, 0x53, 0x5E, 0x69, 0x74, 0x7F, 0x8A,
	0x95, 0xA0, 0xAB, 0xB6, 0xC1, 0xCC, 0xD7, 0xE2,
	0x11, 0x17, 0x1C, 0x24, 0x29, 0x2B, 0x2B, 0x30,
	0x47, 0x57, 0x69, 0x78, 0x8C, 0x9C, 0xB0, 0xC7,
};

class WPG2Parser::ObjectCharacterization
{
public:
	bool taper;
	bool translate;
	bool skew;
	bool scale;
	bool rotate;
	bool hasObjectId;
	bool editLock;
	bool windingRule;
	bool filled;
	bool closed;
	bool framed;

	unsigned long objectId;
	unsigned long lockFlags;
	double rotationAngle;
	long sxcos;
	long sycos;
	long kxsin;
	long kysin;
	long txinteger;
	unsigned short txfraction;
	long tyinteger;
	unsigned short tyfraction;
	long px;
	long py;

	WPG2TransformMatrix matrix;

	ObjectCharacterization():
		taper(false),
		translate(false),
		skew(false),
		scale(false),
		rotate(false),
		hasObjectId(false),
		editLock(false),
		windingRule(false),
		filled(false),
		closed(false),
		framed(true),
		objectId(0),
		lockFlags(0),
		rotationAngle(0.0),
		sxcos(0),
		sycos(0),
		kxsin(0),
		kysin(0),
		txinteger(0),
		txfraction(0),
		tyinteger(0),
		tyfraction(0),
		px(0),
		py(0),
		matrix()
	{}
};

WPG2Parser::WPG2Parser(WPXInputStream *input, libwpg::WPGPaintInterface *painter, bool isEmbedded):
	WPGXParser(input, painter),
	m_recordLength(0),
	m_recordEnd(0),
	m_success(true), m_exit(false), m_graphicsStarted(isEmbedded),
	m_xres(1200), m_yres(1200),
	m_xofs(0), m_yofs(0),
	m_width(0), m_height(0),
	m_doublePrecision(false),
	m_style(),
	m_penForeColor(0,0,0),
	m_penBackColor(0xff,0xff,0xff),
	m_brushForeColor(0,0,0),
	m_brushBackColor(0xff,0xff,0xff),
	m_dashArray(),
	m_gradient(),
	m_dashArrayStyles(),
	m_layerOpened(false),
#ifdef DEBUG
	m_layerId(0),
#endif
	m_matrix(),
	m_gradientAngle(0.0),
	m_gradientRef(),
	m_groupStack(),
	m_compoundMatrix(),
	m_compoundWindingRule(false),
	m_compoundFilled(false),
	m_compoundFramed(true),
	m_compoundClosed(false),
	m_bitmap(),
	m_binaryData(),
	m_hFlipped(false),
	m_vFlipped(false),
	m_textData(),
	m_drawTextData(false)
#if DUMP_BINARY_DATA
	, m_binaryId(0)
#endif
{
	m_style.insert("draw:fill", "solid");
	// default style
	m_penForeColor = ::libwpg::WPGColor(0,0,0);
	m_penBackColor = ::libwpg::WPGColor(0xff,0xff,0xff);
	m_style.insert("svg:stroke-width", 0.0);
	m_style.insert("draw:stroke", "solid");
	m_dashArray = libwpg::WPGDashArray();
	m_brushForeColor = ::libwpg::WPGColor(0,0,0);
	m_brushBackColor = ::libwpg::WPGColor(0xff,0xff,0xff);

	m_style.insert("svg:stroke-color", m_penForeColor.getColorString());
	m_style.insert("svg:stroke-opacity", m_penForeColor.getOpacity(), WPX_PERCENT);
	m_style.insert("draw:fill-color", m_brushForeColor.getColorString());
	m_style.insert("draw:opacity", m_brushForeColor.getOpacity(), WPX_PERCENT);

	resetPalette();
	m_style.insert("draw:fill", "solid");
}

bool WPG2Parser::parse()
{
	typedef void (WPG2Parser::*Method)();

	struct RecordHandler
	{
		int type;
		const char *name;
		Method handler;
	};

	static const struct RecordHandler handlers[] =
	{
		{ 0x01, "Start WPG",            &WPG2Parser::handleStartWPG },
		{ 0x02, "End WPG",              &WPG2Parser::handleEndWPG },
		{ 0x03, "Form Settings",        &WPG2Parser::handleFormSettings },
		{ 0x04, "Ruler Settings",       0 },     // ignored
		{ 0x05, "Grid Settings",        0 },     // ignored
		{ 0x06, "Layer",                &WPG2Parser::handleLayer },
		{ 0x08, "Pen Style Definition", &WPG2Parser::handlePenStyleDefinition },
		{ 0x09, "Pattern Definition",   0 },
		{ 0x0a, "Comment",              0 },     // ignored
		{ 0x0b, "Color Transfer",       0 },
		{ 0x0c, "Color Palette",        &WPG2Parser::handleColorPalette },
		{ 0x0d, "DP Color Palette",     &WPG2Parser::handleDPColorPalette },
		{ 0x0e, "Bitmap Data",          &WPG2Parser::handleBitmapData },
		{ 0x0f, "Text Data",            &WPG2Parser::handleTextData },
		{ 0x10, "Chart Style",          0 },     // ignored
		{ 0x11, "Chart Data",           0 },     // ignored
		{ 0x12, "Object Image",         &WPG2Parser::handleObjectImage },
		{ 0x15, "Polyline",             &WPG2Parser::handlePolyline },
		{ 0x16, "Polyspline",           &WPG2Parser::handlePolyspline },
		{ 0x17, "Polycurve",            &WPG2Parser::handlePolycurve },
		{ 0x18, "Rectangle",            &WPG2Parser::handleRectangle },
		{ 0x19, "Arc",                  &WPG2Parser::handleArc },
		{ 0x1a, "Compound Polygon",     &WPG2Parser::handleCompoundPolygon },
		{ 0x1b, "Bitmap",               &WPG2Parser::handleBitmap },
		{ 0x1c, "Text Line",            &WPG2Parser::handleTextLine },
		{ 0x1d, "Text Block",           &WPG2Parser::handleTextBlock },
		{ 0x1e, "Text Path",            &WPG2Parser::handleTextPath },
		{ 0x1f, "Chart",                0 },
		{ 0x20, "Group",                0 },
		{ 0x21, "Object Capsule",       &WPG2Parser::handleObjectCapsule },
		{ 0x22, "Font Settings",        0 },
		{ 0x25, "Pen Fore Color",       &WPG2Parser::handlePenForeColor },
		{ 0x26, "DP Pen Fore Color",    &WPG2Parser::handleDPPenForeColor },
		{ 0x27, "Pen Back Color",       &WPG2Parser::handlePenBackColor },
		{ 0x28, "DP Pen Back Color",    &WPG2Parser::handleDPPenBackColor },
		{ 0x29, "Pen Style",            &WPG2Parser::handlePenStyle },
		{ 0x2a, "Pen Pattern",          0 },
		{ 0x2b, "Pen Size",             &WPG2Parser::handlePenSize },
		{ 0x2c, "DP Pen Size",          &WPG2Parser::handleDPPenSize  },
		{ 0x2d, "Line Cap",             &WPG2Parser::handleLineCap },
		{ 0x2e, "Line Join",            &WPG2Parser::handleLineJoin },
		{ 0x2f, "Brush Gradient",       &WPG2Parser::handleBrushGradient },
		{ 0x30, "DP Brush Gradient",    &WPG2Parser::handleDPBrushGradient },
		{ 0x31, "Brush Fore Color",     &WPG2Parser::handleBrushForeColor },
		{ 0x32, "DP Brush Fore Color",  &WPG2Parser::handleDPBrushForeColor },
		{ 0x33, "Brush Back Color",     &WPG2Parser::handleBrushBackColor },
		{ 0x34, "DP Brush Back Color",  &WPG2Parser::handleDPBrushBackColor },
		{ 0x35, "Brush Pattern",        &WPG2Parser::handleBrushPattern },
		{ 0x36, "Horizontal Line",      0 },
		{ 0x37, "Vertical Line",        0 },
		{ 0x38, "Poster Settings",      0 },
		{ 0x39, "Image State",          0 },
		{ 0x3a, "Envelope Definition",  0 },
		{ 0x3b, "Envelope",             0 },
		{ 0x3c, "Texture Definition",   0 },
		{ 0x3d, "Brush Texture",        0 },
		{ 0x3e, "Texture Alignment",    0 },
		{ 0x3f, "Pen Texture ",         0 },
		{ 0x00, 0, 0 } // end marker
	};

	while(!m_input->atEOS())
	{
#ifdef DEBUG
		long recordPos = m_input->tell();
#endif
		readU8();
		int recordType = readU8();
		if (recordType == 0 || recordType > (int)0x3f)
			break;
		int extension = readVariableLengthInteger();
		m_recordLength = readVariableLengthInteger();
		m_recordEnd = m_input->tell() + m_recordLength - 1;

		// inside a subgroup, one less sub record
		if(!m_groupStack.empty())
			m_groupStack.top().subIndex--;

		// search function to handler this record
		int index = -1;
		for(int i = 0; (index < 0) && handlers[i].name; i++)
			if(handlers[i].type == recordType)
				index = i;

		WPG_DEBUG_MSG(("\n"));
		if(index < 0)
		{
			WPG_DEBUG_MSG(("Unknown record type 0x%02x at %li  size %d  extension %d\n",
			               recordType, recordPos, m_recordLength, extension));
		}
		else
		{
			Method recordHandler = handlers[index].handler;

			if(!recordHandler)
				WPG_DEBUG_MSG(("Record '%s' (ignored) type 0x%02x at %li  size %d  extension %d\n",
				               handlers[index].name, recordType, recordPos, m_recordLength, extension));
			else
			{
				WPG_DEBUG_MSG(("Record '%s' type 0x%02x at %li  size %d  extension %d\n",
				               handlers[index].name, recordType, recordPos, m_recordLength, extension));

				// invoke the handler for this record
				(this->*recordHandler)();
			}
		}

		// the last subgroup
		if(!m_groupStack.empty())
		{
			WPGGroupContext &context = m_groupStack.top();
			if(context.subIndex == 0)
			{
				if(context.isCompoundPolygon())
					flushCompoundPolygon();
				m_groupStack.pop();
			}
		}

		// we enter another subgroup, save the context to stack
		if(extension > 0)
		{
			WPGGroupContext context;
			context.parentType = recordType;
			context.subIndex = extension;
			if(context.isCompoundPolygon())
			{
				context.compoundMatrix = m_compoundMatrix;
				context.compoundFilled = m_compoundFilled;
				context.compoundFramed = m_compoundFramed;
				context.compoundClosed = m_compoundClosed;
			}
			m_groupStack.push(context);
		}

		//if(m_input->tell() > m_recordEnd+1)
		{
			//WPG_DEBUG_MSG(("Record 0x%x consumes more bytes than necessary!\n", recordType));
			WPG_DEBUG_MSG(("Current stream position: %li\n", m_input->tell()));
		}

		if(m_exit) break;

		m_input->seek(m_recordEnd+1, WPX_SEEK_SET);
	}

	if (!m_exit)
		handleEndWPG();

	return m_success;
}

#ifdef DEBUG
static const char *describePrecision(unsigned char precision)
{
	const char *result = "Unknown";
	switch(precision)
	{
	case 0:
		result = "single";
		break;
	case 1:
		result = "double";
		break;
	default:
		break;
	}
	return result;
}

static const char *describeGradient(unsigned char gradientType)
{
	const char *result = "Unknown";
	switch(gradientType)
	{
	case 0:
		result = "None";
		break;
	case 1:
		result = "Linear";
		break;
	case 2:
		result = "Polygonal";
		break;
	case 3:
		result = "Concentric Circles";
		break;
	case 4:
		result = "Convergent Circles";
		break;
	case 5:
		result = "Concentric Ellipses";
		break;
	case 6:
		result = "Convergent Ellipses";
		break;
	case 7:
		result = "Concentric Squares";
		break;
	case 8:
		result = "Convergent Squares";
		break;
	case 9:
		result = "Concentric Rectangles";
		break;
	case 10:
		result = "Convergent Rectangles";
		break;
	default:
		break;
	}
	return result;
}
#endif

#define TO_DOUBLE(x) ( (m_doublePrecision) ? ((double)(x)/65536.0) : (double)(x) )
#define TRANSFORM_XY(x,y) { m_matrix.transform((x),(y)); (x)-= m_xofs; (y)-= m_yofs; (y)=m_height-(y); }

void WPG2Parser::handleStartWPG()
{
	if (m_graphicsStarted) // second start wpg in the document == possible corruption, bail out
	{
		handleEndWPG();
		return;
	}

	unsigned int horizontalUnit = readU16();
	unsigned int verticalUnit = readU16();
	unsigned char precision = readU8();

	// sanity check
	m_xres = horizontalUnit;
	m_yres = verticalUnit;
	if((horizontalUnit==0) || (verticalUnit==0))
	{
		m_xres = m_yres = 1200;
		WPG_DEBUG_MSG(("Warning ! Insane unit of measure"));
	}

	// danger if we do not recognize the precision code
	if(precision != 0 && precision != 1)
	{
		m_success = false;
		m_exit = true;
		return;
	}
	m_doublePrecision = (precision == 1);

#ifdef DEBUG
	long viewportX1 = (m_doublePrecision) ? readS32() : readS16();
	long viewportY1 = (m_doublePrecision) ? readS32() : readS16();
	long viewportX2 = (m_doublePrecision) ? readS32() : readS16();
	long viewportY2 = (m_doublePrecision) ? readS32() : readS16();
#else
	m_input->seek(((m_doublePrecision) ? 16 : 8), WPX_SEEK_CUR);
#endif
	long imageX1 = (m_doublePrecision) ? readS32() : readS16();
	long imageY1 = (m_doublePrecision) ? readS32() : readS16();
	long imageX2 = (m_doublePrecision) ? readS32() : readS16();
	long imageY2 = (m_doublePrecision) ? readS32() : readS16();

	// used to adjust coordinates
	m_xofs = (imageX1 < imageX2) ? imageX1 : imageX2;
	m_yofs = (imageY1 < imageY2) ? imageY1 : imageX2;
	m_width = (imageX2 > imageX1 ) ? imageX2-imageX1 : imageX1-imageX2;
	m_height = (imageY2 > imageY1) ? imageY2-imageY1 : imageY1-imageY2;

	WPG_DEBUG_MSG(("StartWPG 2\n"));
	WPG_DEBUG_MSG(("  Horizontal unit of measure : %d pixels/inch\n", horizontalUnit));
	WPG_DEBUG_MSG(("    Vertical unit of measure : %d pixels/inch\n", verticalUnit));
	WPG_DEBUG_MSG(("              Data precision : %d (%s)\n", precision, describePrecision(precision)));
	WPG_DEBUG_MSG(("                 Viewport X1 : %li\n", viewportX1));
	WPG_DEBUG_MSG(("                 Viewport Y1 : %li\n", viewportY1));
	WPG_DEBUG_MSG(("                 Viewport X2 : %li\n", viewportX2));
	WPG_DEBUG_MSG(("                 Viewport Y2 : %li\n", viewportY2));
	WPG_DEBUG_MSG(("                    Image X1 : %li\n", imageX1));
	WPG_DEBUG_MSG(("                    Image Y1 : %li\n", imageY1));
	WPG_DEBUG_MSG(("                    Image X2 : %li\n", imageX2));
	WPG_DEBUG_MSG(("                    Image Y2 : %li\n", imageY2));
	WPG_DEBUG_MSG(("                    X offset : %li\n", m_xofs));
	WPG_DEBUG_MSG(("                    Y offset : %li\n", m_yofs));
	WPG_DEBUG_MSG(("                       width : %li\n", m_width));
	WPG_DEBUG_MSG(("                      height : %li\n", m_height));

	::WPXPropertyList propList;
	propList.insert("svg:width", ((TO_DOUBLE(m_width)) / m_xres));
	propList.insert("svg:height", ((TO_DOUBLE(m_height)) / m_yres));

	m_painter->startGraphics(propList);

	static const int WPG2_defaultPenDashes[] =
	{
		1,   291,  0,                            // style #0 (actually solid)
		1,   218, 73,                            // style #1
		1,   145, 73,                            // style #2
		1,    73, 73,                            // style #3
		1,    36, 36,                            // style #4
		1,    18, 18,                            // style #5
		1,    18, 55,                            // style #6
		3,    18, 55,  18, 55, 18, 127,          // style #7
		2,   164, 55,  18, 55,                   // style #8
		3,   145, 36,  18, 36, 18,  36,          // style #9
		3,    91, 55,  91, 55, 18,  55,          // style #10
		4,    91, 36,  91, 36, 18,  36, 18, 36,  // style #11
		2,   182, 73,  73, 73,                   // style #12
		3,   182, 36,  55, 36, 55,  36,          // style #13
		3,   255, 73, 266, 73, 73,  73,          // style #14
		4,   273, 36, 273, 36, 55,  36, 55, 36,  // style #15
		0 // end marker
	};

	// create default pen styles
	int styleNo = 0;
	for(int i = 0; (long)i < (long)(sizeof(WPG2_defaultPenDashes)/sizeof(WPG2_defaultPenDashes[0]));)
	{
		int segments = 2 * WPG2_defaultPenDashes[i++];
		if(segments == 0) break;
		libwpg::WPGDashArray dashArray;
		for(int j = 0; j < segments; j++, i++)
			dashArray.add(WPG2_defaultPenDashes[i]*3.6/218.0);
		m_dashArrayStyles[styleNo] = dashArray;
		styleNo++;
	}
	m_graphicsStarted = true;
}

void WPG2Parser::handleEndWPG()
{
	if (!m_graphicsStarted)
		return;
	// sentinel
	if(m_layerOpened)
		m_painter->endLayer();

	m_painter->endGraphics();
	m_exit = true;
}

void WPG2Parser::handleFormSettings()
{
#ifdef DEBUG
	unsigned int w = (m_doublePrecision) ? readU32() : readU16();
	unsigned int h = (m_doublePrecision) ? readU32() : readU16();
	double width = (TO_DOUBLE(w)) / m_xres;
	double height = (TO_DOUBLE(h)) / m_yres;
	m_input->seek(((m_doublePrecision) ? 4 : 2), WPX_SEEK_CUR);
	unsigned int ml = (m_doublePrecision) ? readU32() : readU16();
	unsigned int mr = (m_doublePrecision) ? readU32() : readU16();
	unsigned int mt = (m_doublePrecision) ? readU32() : readU16();
	unsigned int mb = (m_doublePrecision) ? readU32() : readU16();
	double margL = (TO_DOUBLE(ml)) / m_xres;
	double margR = (TO_DOUBLE(mr)) / m_xres;
	double margT = (TO_DOUBLE(mt)) / m_xres;
	double margB = (TO_DOUBLE(mb)) / m_xres;

	WPG_DEBUG_MSG(("Form Settings: width: %f height : %f\n", width, height));
	WPG_DEBUG_MSG(("Form Margins:  left: %f right : %f top: %f bottom: %f\n", margL, margR, margT, margB));
#endif
}

void WPG2Parser::handleLayer()
{
	if (!m_graphicsStarted)
		return;
	WPXPropertyList propList;
	propList.insert("svg:id", (int)readU16());

	// close previous one
	if(m_layerOpened)
		m_painter->endLayer();

	m_painter->startLayer(propList);
	m_layerOpened = true;

	WPG_DEBUG_MSG(("  Layer Id: %d\n", m_layerId));
}

void WPG2Parser::handleCompoundPolygon()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);

	m_compoundWindingRule = objCh.windingRule;
	m_compoundMatrix = objCh.matrix;
	m_compoundFilled = objCh.filled;
	m_compoundFramed = objCh.framed;
	m_compoundClosed = objCh.closed;
}

void WPG2Parser::flushCompoundPolygon()
{
	if (!m_graphicsStarted)
		return;
	WPGGroupContext &context = m_groupStack.top();

	WPXPropertyList tmpStyle = m_style;

	if (!context.compoundFilled)
		tmpStyle.insert("draw:fill", "none");
	if (!context.compoundFramed)
		tmpStyle.insert("draw:stroke", "none");

	if(context.compoundWindingRule)
		tmpStyle.insert("svg:fill-rule", "nonzero");
	else
		tmpStyle.insert("svg:fill-rule", "evenodd");

	m_painter->setStyle( tmpStyle, context.compoundFilled ? m_gradient : ::WPXPropertyListVector() );

	if (context.compoundClosed)
	{
		WPXPropertyList element;
		element.insert("libwpg:path-action", "Z");
		context.compoundPath.append(element);
	}
	m_painter->drawPath(context.compoundPath);
}

void WPG2Parser::handlePenStyleDefinition()
{
	if (!m_graphicsStarted)
		return;
	unsigned int style = readU16();
	unsigned int segments = readU16();

	libwpg::WPGDashArray dashArray;
	for(unsigned i = 0; i < segments; i++)
	{
		unsigned int p = (m_doublePrecision) ? readU32() : readU16();
		unsigned int q = (m_doublePrecision) ? readU32() : readU16();
		dashArray.add(TO_DOUBLE(p)*3.6/218.0);
		dashArray.add(TO_DOUBLE(q)*3.6/218.0);
	}
	m_dashArrayStyles[style] = dashArray;

	WPG_DEBUG_MSG(("          Style : %d\n", style));
	WPG_DEBUG_MSG(("  Segment pairs : %d\n", segments));
}

#if 0
// TODO
void WPG2Parser::handlePatternDefinition()
{
	if (!m_graphicsStarted)
		return;
	WPG_DEBUG_MSG(("PatternDefinition\n"));
}
#endif

void WPG2Parser::handleColorPalette()
{
	if (!m_graphicsStarted)
		return;
	unsigned startIndex = readU16();
	unsigned numEntries = readU16();

	for(unsigned i = 0; i < numEntries; i++)
	{
		unsigned char red = readU8();
		unsigned char green = readU8();
		unsigned char blue = readU8();
		unsigned char alpha = 0xff - readU8();
		libwpg::WPGColor color(red, green, blue, alpha);
		m_colorPalette[startIndex+i] = color;
		WPG_DEBUG_MSG(("Index#%d: RGB %s\n", startIndex+i, color.getColorString().cstr()));
	}
}

void WPG2Parser::handleDPColorPalette()
{
	if (!m_graphicsStarted)
		return;
	unsigned startIndex = readU16();
	unsigned numEntries = readU16();

	for(unsigned int i = 0; i < numEntries; i++)
	{
		unsigned char red = readU16() >> 8 ;
		unsigned char green = readU16() >> 8 ;
		unsigned char blue = readU16() >> 8 ;
		unsigned char alpha = 0xff - (readU16() >> 8) ;
		libwpg::WPGColor color(red, green, blue, alpha);
		m_colorPalette[startIndex+i] = color;
		WPG_DEBUG_MSG(("Index#%d: RGB %s\n", startIndex+i, color.getColorString().cstr()));
	}
}

void WPG2Parser::handlePenForeColor()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned char red = readU8();
	unsigned char green = readU8();
	unsigned char blue = readU8();
	unsigned char alpha = 0xff - readU8();

	m_penForeColor = libwpg::WPGColor(red, green, blue, alpha);

	WPG_DEBUG_MSG(("   Foreground color (RGBA): %d %d %d %d\n", red, green, blue, alpha));
}

void WPG2Parser::handleDPPenForeColor()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	// we just ignore the least significant 8 bits
	unsigned char red = (m_doublePrecision)   ? readU16()>>8 : readU8();
	unsigned char green = (m_doublePrecision) ? readU16()>>8 : readU8();
	unsigned char blue = (m_doublePrecision)  ? readU16()>>8 : readU8();
	unsigned char alpha = 0xff - ((m_doublePrecision) ? readU16()>>8 : readU8());

	m_style.insert("svg:stroke-color", libwpg::WPGColor(red, green, blue, alpha).getColorString());
	m_style.insert("svg:stroke-opacity", libwpg::WPGColor(red, green, blue, alpha).getOpacity(), WPX_PERCENT);
	m_penForeColor = libwpg::WPGColor(red, green, blue, alpha);

	WPG_DEBUG_MSG(("   Foreground color (RGBA): %d %d %d %d\n", red, green, blue, alpha));
}

void WPG2Parser::handlePenBackColor()
{
	if (!m_graphicsStarted)
		return;
	if (!m_groupStack.empty() && m_groupStack.top().isCompoundPolygon())
		return;

	unsigned char red = readU8();
	unsigned char green = readU8();
	unsigned char blue = readU8();
	unsigned char alpha = 0xff - readU8();

	m_penBackColor = libwpg::WPGColor(red, green, blue, alpha);

	m_style.insert("svg:stroke-color", m_penForeColor.getColorString());
	m_style.insert("svg:stroke-opacity", m_penForeColor.getOpacity(), WPX_PERCENT);

	WPG_DEBUG_MSG(("   Background color (RGBA): %d %d %d %d\n", red, green, blue, alpha));
}

void WPG2Parser::handleDPPenBackColor()
{
	if (!m_graphicsStarted)
		return;
	if (!m_groupStack.empty() && m_groupStack.top().isCompoundPolygon())
		return;

	// we just ignore the least significant 8 bits
	unsigned int red = (m_doublePrecision)   ? readU16()>>8 : readU8();
	unsigned int green = (m_doublePrecision) ? readU16()>>8 : readU8();
	unsigned int blue = (m_doublePrecision)  ? readU16()>>8 : readU8();
	unsigned int alpha = 0xff - ((m_doublePrecision) ? readU16()>>8 : readU8());

	m_penBackColor = libwpg::WPGColor(red, green, blue, alpha);

	m_style.insert("svg:stroke-color", m_penForeColor.getColorString());
	m_style.insert("svg:stroke-opacity", m_penForeColor.getOpacity(), WPX_PERCENT);

	WPG_DEBUG_MSG(("   Background color (RGBA): %d %d %d %d\n", red, green, blue, alpha));
}

void WPG2Parser::setPenStyle()
{
	if (!m_style["draw:stroke"] || m_style["draw:stroke"]->getStr() != "dash")
		return;
	{
		double strokeWidth = m_style["svg:stroke-width"] ? m_style["svg:stroke-width"]->getDouble() : 0.0;
		m_style.insert("draw:dots1", m_dashArray.getDots1());
		m_style.insert("draw:dots1-length", 72.0*72.0*strokeWidth*(m_dashArray.getDots1Length()), WPX_POINT);
		m_style.insert("draw:dots2", m_dashArray.getDots2());
		m_style.insert("draw:dots2-length", 72.0*72.0*strokeWidth*(m_dashArray.getDots2Length()), WPX_POINT);
		m_style.insert("draw:distance", 72.0*72.0*strokeWidth*(m_dashArray.getDistance()), WPX_POINT);
	}
}

void WPG2Parser::handlePenStyle()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned int style = readU16();

	m_dashArray = m_dashArrayStyles[style];

	if (!m_dashArray.getDots1() || !m_dashArray.getDots2())
		m_style.insert("draw:stroke", "solid");
	else
		m_style.insert("draw:stroke", "dash");

	setPenStyle();

	WPG_DEBUG_MSG(("   Pen style : %d\n", style));
}

void WPG2Parser::handlePenSize()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned int width = readU16();

	m_style.insert("svg:stroke-width", (TO_DOUBLE(width) / m_xres));

	WPG_DEBUG_MSG(("   Width: %d\n", width));
}

void WPG2Parser::handleDPPenSize()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned long width = readU32();

	m_style.insert("svg:stroke-width", TO_DOUBLE(width) / m_xres / 256);
//	m_pen.height = TO_DOUBLE(height) / m_yres / 256;

	WPG_DEBUG_MSG(("   Width: %li\n", width));
}

void WPG2Parser::handleLineCap()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
//	WPG_DEBUG_MSG(("   Line cap : %d\n", style));
}

void WPG2Parser::handleLineJoin()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
//	WPG_DEBUG_MSG(("   Line join : %d\n", style));
}

void WPG2Parser::handleBrushGradient()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned angleFraction = readU16();
	unsigned angleInteger = readU16();
	unsigned xref = readU16();
	unsigned yref = readU16();
#ifdef DEBUG
	unsigned flag = readU16();
	bool granular = ((flag & (1<<6))>>6) == 1;
	bool anchor = ((flag & (1<<7))>>7) == 1;
#else
	readU16();
#endif

	// TODO: get gradient extent

	m_gradientAngle = angleInteger + (double)angleFraction/65536.0;
	m_gradientRef.insert("svg:cx", (double)xref);
	m_gradientRef.insert("svg:cy", (double)yref);

	WPG_DEBUG_MSG(("       Gradient angle : %d.%d\n", angleInteger, angleFraction));
	WPG_DEBUG_MSG(("   Gradient reference : %d.%d\n", xref, yref));
	WPG_DEBUG_MSG(("   Granular : %s\n", (granular ? "yes" : "no")));
	WPG_DEBUG_MSG(("   Anchored : %s\n", (anchor ? "yes" : "no")));
}

void WPG2Parser::handleDPBrushGradient()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned angleFraction = readU16();
	unsigned angleInteger = readU16();
	unsigned xref = readU16();
	unsigned yref = readU16();
#ifdef DEBUG
	unsigned flag = readU16();
	bool granular = (flag & (1<<6)) == 1;
	bool anchor = (flag & (1<<7)) == 1;
#else
	readU16();
#endif

	// TODO: get gradient extent (in double precision)

	m_gradientAngle = angleFraction + (double)angleInteger/65536.0;
	m_gradientRef.insert("svg:cx", (double)xref);
	m_gradientRef.insert("svg:cy", (double)yref);

	WPG_DEBUG_MSG(("       Gradient angle : %d.%d\n", angleInteger, angleFraction));
	WPG_DEBUG_MSG(("   Gradient reference : %d.%d\n", xref, yref));
	WPG_DEBUG_MSG(("   Granular : %s\n", (granular ? "yes" : "no")));
	WPG_DEBUG_MSG(("   Anchored : %s\n", (anchor ? "yes" : "no")));
}

void WPG2Parser::handleBrushForeColor()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned char gradientType = readU8();
	WPG_DEBUG_MSG(("   Gradient type : %d (%s)\n", gradientType, describeGradient(gradientType)));

	if(gradientType == 0)
	{
		unsigned char red = readU8();
		unsigned char green = readU8();
		unsigned char blue = readU8();
		unsigned char alpha = 0xff - readU8();
		WPG_DEBUG_MSG(("   Foreground color (RGBA): %d %d %d %d\n", red, green, blue, alpha));

		m_brushForeColor = libwpg::WPGColor(red, green, blue, alpha);

		m_style.insert("draw:fill-color", m_brushForeColor.getColorString());
		m_style.insert("draw:opacity", m_brushForeColor.getOpacity(), WPX_PERCENT);

		if(!m_style["draw:fill"] || m_style["draw:fill"]->getStr() != "gradient")
			m_style.insert("draw:fill", "solid");
	}
	else
	{
		unsigned count = readU16();
		std::vector<libwpg::WPGColor> colors;
		std::vector<double> positions;
		WPG_DEBUG_MSG(("  Gradient colors : %d\n", count));

		if (count > 0)
		{
			for(unsigned i = 0; i < count; i++)
			{
				unsigned char red = readU8();
				unsigned char green = readU8();
				unsigned char blue = readU8();
				unsigned char alpha = 0xff - readU8();
				libwpg::WPGColor color(red, green, blue, alpha);
				colors.push_back(color);
				WPG_DEBUG_MSG(("   Color #%d (RGBA): %d %d %d %d\n", i+1, red, green, blue, alpha));
			}

			for(unsigned j = 0; j < count-1; j++)
			{
				unsigned pos = readU16();
				positions.push_back(TO_DOUBLE(pos));
				WPG_DEBUG_MSG(("   Position #%d : %d\n", j+1, pos));
			}
		}

		// looks like Corel Presentations only create 2 colors gradient
		// and they are actually in reverse order
		if(count == 2)
		{
			double xref = m_gradientRef["svg:cx"]->getDouble()/65536.0;
			double yref = m_gradientRef["svg:cy"]->getDouble()/65536.0;
			double angle = m_gradientAngle*M_PI/180.0;
			double tanangle = tan(angle);
			double ref = (tanangle < 1e2 && tanangle > -1e2) ? (yref+xref*tanangle)/(1+tanangle) : xref;
			::WPXPropertyListVector gradient;
			m_style.insert("draw:angle", (int)(-m_gradientAngle)); // upside down
			WPXPropertyList propList;
			propList.insert("svg:offset", 0.0, WPX_PERCENT);
			propList.insert("svg:stop-color", colors[1].getColorString());
			propList.insert("svg:stop-opacity", colors[1].getOpacity(), WPX_PERCENT);
			gradient.append(propList);
			propList.clear();
			propList.insert("svg:offset", ref, WPX_PERCENT);
			propList.insert("svg:stop-color", colors[0].getColorString());
			propList.insert("svg:stop-opacity", colors[0].getOpacity(), WPX_PERCENT);
			gradient.append(propList);
			propList.clear();
			if((m_gradientRef["svg:cx"]->getInt() != 65535) && (m_gradientRef["svg:cy"]->getInt() != 65535))
			{
				propList.insert("svg:offset", 1.0, WPX_PERCENT);
				propList.insert("svg:stop-color", colors[1].getColorString());
				propList.insert("svg:stop-opacity", colors[1].getOpacity(), WPX_PERCENT);
				gradient.append(propList);
			}
			m_gradient = gradient;
			m_style.insert("draw:fill", "gradient");
		}
	}
}

void WPG2Parser::handleDPBrushForeColor()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
	unsigned char gradientType = readU8();
	WPG_DEBUG_MSG(("   Gradient type : %d (%s)\n", gradientType, describeGradient(gradientType)));

	if(gradientType == 0)
	{
		unsigned char red = (m_doublePrecision)   ? readU16()>>8 : readU8();
		unsigned char green = (m_doublePrecision)   ? readU16()>>8 : readU8();
		unsigned char blue = (m_doublePrecision)   ? readU16()>>8 : readU8();
		unsigned char alpha = 0xff - ((m_doublePrecision)   ? readU16()>>8 : readU8());
		WPG_DEBUG_MSG(("   Foreground color (RGBA): %d %d %d %d\n", red, green, blue, alpha));

		m_brushForeColor = libwpg::WPGColor(red, green, blue, alpha);

		m_style.insert("draw:fill-color", m_brushForeColor.getColorString());
		m_style.insert("draw:opacity", m_brushForeColor.getOpacity(), WPX_PERCENT);

		if(!m_style["draw:fill"] || m_style["draw:fill"]->getStr() != "none")
			m_style.insert("draw:fill", "solid");
	}
	else
	{
		unsigned count = readU16();
		std::vector<libwpg::WPGColor> colors;
		std::vector<double> positions;
		WPG_DEBUG_MSG(("  Gradient colors : %d\n", count));

		if (count > 0)
		{
			for(unsigned i = 0; i < count; i++)
			{
				unsigned char red = (m_doublePrecision)   ? readU16()>>8 : readU8();
				unsigned char green = (m_doublePrecision)   ? readU16()>>8 : readU8();
				unsigned char blue = (m_doublePrecision)   ? readU16()>>8 : readU8();
				unsigned char alpha = 0xff - ((m_doublePrecision)   ? readU16()>>8 : readU8());
				libwpg::WPGColor color(red, green, blue, alpha);
				colors.push_back(color);
				WPG_DEBUG_MSG(("   Color #%d (RGBA): %d %d %d %d\n", i+1, red, green, blue, alpha));
			}

			for(unsigned j = 0; j < count-1; j++)
			{
				unsigned pos = readU16();
				positions.push_back(TO_DOUBLE(pos));
				WPG_DEBUG_MSG(("   Position #%d : %d\n", j+1, pos));
			}
		}

		// looks like Corel Presentations only create 2 colors gradient
		// and they are actually in reverse order
		if(count == 2)
		{
			double xref = m_gradientRef["svg:cx"]->getDouble()/65536.0;
			double yref = m_gradientRef["svg:cy"]->getDouble()/65536.0;
			double angle = m_gradientAngle*M_PI/180.0;
			double tanangle = tan(angle);
			double ref = (tanangle<1e2) ? (yref+xref*tanangle)/(1+tanangle) : xref;
			::WPXPropertyListVector gradient;
			m_style.insert("draw:angle", (int)(-m_gradientAngle));

			WPXPropertyList propList;
			propList.insert("svg:offset", 0.0, WPX_PERCENT);
			propList.insert("svg:stop-color", colors[1].getColorString());
			propList.insert("svg:stop-opacity", colors[1].getOpacity(), WPX_PERCENT);
			gradient.append(propList);
			propList.clear();
			propList.insert("svg:offset", ref, WPX_PERCENT);
			propList.insert("svg:stop-color", colors[0].getColorString());
			propList.insert("svg:stop-opacity", colors[0].getOpacity(), WPX_PERCENT);
			gradient.append(propList);
			propList.clear();
			if((m_gradientRef["svg:cx"]->getInt() != 65535) && (m_gradientRef["svg:cy"]->getInt() != 65535))
			{
				propList.insert("svg:offset", 1.0, WPX_PERCENT);
				propList.insert("svg:stop-color", colors[1].getColorString());
				propList.insert("svg:stop-opacity", colors[1].getOpacity(), WPX_PERCENT);
				gradient.append(propList);
			}
			m_gradient = gradient;
		}
		m_style.insert("draw:fill", "gradient");
	}
}

void WPG2Parser::handleBrushBackColor()
{
	if (!m_graphicsStarted)
		return;
	if (!m_groupStack.empty() && m_groupStack.top().isCompoundPolygon())
		return;
#if 0
	unsigned char red = readU8();
	unsigned char green = readU8();
	unsigned char blue = readU8();
	unsigned char alpha = 0xff - readU8();

	m_brushBackColor = libwpg::WPGColor(red, green, blue, alpha);

	m_gradient.backColor = libwpg::WPGColor(red, green, blue, alpha);
	if(m_gradient.style == libwpg::WPGGradient::NoBrush)
		m_gradient.style = libwpg::WPGGradient::Solid;

	WPG_DEBUG_MSG(("   Backround color (RGBA): %d %d %d %d\n", red, green, blue, alpha));
#endif
}

void WPG2Parser::handleDPBrushBackColor()
{
	if (!m_graphicsStarted)
		return;
	if (!m_groupStack.empty() && m_groupStack.top().isCompoundPolygon())
		return;
#if 0
	// we just ignore the least significant 8 bits
	unsigned int red = (m_doublePrecision)   ? readU16()>>8 : readU8();
	unsigned int green = (m_doublePrecision) ? readU16()>>8 : readU8();
	unsigned int blue = (m_doublePrecision)  ? readU16()>>8 : readU8();
	unsigned int alpha = 0xff - (m_doublePrecision) ? readU16()>>8 : readU8();

	m_brushBackColor = libwpg::WPGColor(red, green, blue, alpha);

	m_style.insert("draw:fill-color", m_brushForeColor.getColorString());
	m_style.insert("draw:opacity", m_brushForeColor.getOpacity(), WPX_PERCENT);

	if(m_style["draw:fill"] && m_style["draw:fill"]->getStr() == "none")
		m_style.insert("draw:fill", "solid");

	WPG_DEBUG_MSG(("   Background color (RGBA): %d %d %d %d\n", red, green, blue, alpha));
#endif
}

void WPG2Parser::handleBrushPattern()
{
	if (!m_graphicsStarted)
		return;
	if(!m_groupStack.empty())
	{
		if (m_groupStack.top().isCompoundPolygon())
			return;
		if (m_groupStack.top().parentType == 0x01) // we don't handle Page Attributes for now
			return;
	}
#ifdef DEBUG
	unsigned int pattern = readU16();
#endif

	// TODO

	WPG_DEBUG_MSG(("   Pattern : %d\n", pattern));
}

void WPG2Parser::parseCharacterization(ObjectCharacterization *ch)
{
	// sanity check
	if(!ch) return;

	// identity
	ch->matrix = WPG2TransformMatrix();

	unsigned int flags = readU16();
	ch->taper = (flags & 0x01) != 0;
	ch->translate = (flags & 0x02) != 0;
	ch->skew = (flags & 0x04) != 0;
	ch->scale = (flags & 0x08) != 0;
	ch->rotate = (flags & 0x10) != 0;
	ch->hasObjectId = (flags & 0x20) != 0;
	ch->editLock = (flags & 0x80) != 0;
	ch->windingRule = (flags & (1<<12)) != 0;
	ch->filled = (flags & (1<<13)) != 0;
	ch->closed = (flags & (1<<14)) != 0;
	ch->framed = (flags & (1<<15)) != 0;

	if(ch->editLock) ch->lockFlags = readU32();

	// object ID can be 2 or 4 bytes
	if(ch->hasObjectId) ch->objectId = readU16();
	if(ch->objectId >> 15) ch->objectId = ((ch->objectId  & 0x7fff) << 16) | readU16();

	if(ch->rotate) ch->rotationAngle = fixedPointToDouble(readU32());

	if(ch->rotate || ch->scale)
	{
		ch->sxcos = readS32();
		ch->sycos = readS32();
		ch->matrix.element[0][0] = (double)(ch->sxcos)/65536;
		ch->matrix.element[1][1] = (double)(ch->sxcos)/65536;
	}

	if(ch->rotate || ch->skew)
	{
		ch->kxsin = readS32();
		ch->kysin = readS32();
		ch->matrix.element[1][0] = (double)(ch->kxsin)/65536;
		ch->matrix.element[0][1] = (double)(ch->kysin)/65536;
	}

	if(ch->translate)
	{
		ch->txfraction = readU16();
		ch->txinteger = readS32();
		ch->tyfraction = readU16();
		ch->tyinteger = readS32();
		ch->matrix.element[2][0] = (double)(ch->txinteger);
		ch->matrix.element[2][1] = (double)(ch->tyinteger);
	}

	if(ch->taper)
	{
		ch->px = readS32();
		ch->py = readS32();
		ch->matrix.element[0][2] = (double)(ch->px);
		ch->matrix.element[1][2] = (double)(ch->py);
	}

	WPG_DEBUG_MSG(("ObjectCharacterization\n"));
	WPG_DEBUG_MSG(("       taper : %s\n", (ch->taper ? "yes" : "no")));
	WPG_DEBUG_MSG(("   translate : %s\n", (ch->translate ? "yes" : "no")));
	WPG_DEBUG_MSG(("        skew : %s\n", (ch->skew ? "yes" : "no")));
	WPG_DEBUG_MSG(("       scale : %s\n", (ch->scale ? "yes" : "no")));
	WPG_DEBUG_MSG(("      rotate : %s\n", (ch->rotate ? "yes" : "no")));
	WPG_DEBUG_MSG((" hasObjectId : %s\n", (ch->hasObjectId ? "yes" : "no")));
	WPG_DEBUG_MSG(("    editLock : %s\n", (ch->editLock ? "yes" : "no")));
	WPG_DEBUG_MSG(("      closed : %s\n", (ch->closed ? "yes" : "no")));
	WPG_DEBUG_MSG(("      framed : %s\n", (ch->framed ? "yes" : "no")));
	WPG_DEBUG_MSG(("      filled : %s\n", (ch->filled ? "yes" : "no")));
#ifdef DEBUG
	if(ch->editLock) WPG_DEBUG_MSG(("  lock flags : 0x%x\n", (unsigned)ch->lockFlags));
	if(ch->hasObjectId) WPG_DEBUG_MSG(("   object ID : 0x%x\n", (unsigned)ch->objectId));
	if(ch->translate) WPG_DEBUG_MSG(("    tx : %li %d\n", ch->txinteger, ch->txfraction));
	if(ch->translate) WPG_DEBUG_MSG(("    ty : %li %d\n", ch->tyinteger, ch->tyfraction));
#endif
	WPG_DEBUG_MSG(("transform matrix:\n"));
	WPG_DEBUG_MSG(("%f %f %f\n", ch->matrix.element[0][0], ch->matrix.element[0][1],ch->matrix.element[0][2]));
	WPG_DEBUG_MSG(("%f %f %f\n", ch->matrix.element[1][0], ch->matrix.element[1][1],ch->matrix.element[1][2]));
	WPG_DEBUG_MSG(("%f %f %f\n", ch->matrix.element[2][0], ch->matrix.element[2][1],ch->matrix.element[2][2]));
}

void WPG2Parser::handlePolyline()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);
	m_matrix = objCh.matrix;

	WPXPropertyList tmpStyle = m_style;

	if (!objCh.filled)
		tmpStyle.insert("draw:fill", "none");
	if (!objCh.framed)
		tmpStyle.insert("draw:stroke", "none");

	bool insideCompound = m_groupStack.empty() ? false :
	                      m_groupStack.top().isCompoundPolygon();

	// inside a compound, so take the parent transformation into account
	if(insideCompound)
		m_matrix.transformBy(m_groupStack.top().compoundMatrix);

	unsigned long count = readU16();

	::WPXPropertyListVector points;
	::WPXPropertyList point;
	for(unsigned long i = 0; i < count; i++ )
	{
		point.clear();
		long x = (m_doublePrecision) ? readS32() : readS16();
		long y = (m_doublePrecision) ? readS32() : readS16();
		TRANSFORM_XY(x,y);
		point.insert("svg:x", (TO_DOUBLE(x)/m_xres));
		point.insert("svg:y", (TO_DOUBLE(y)/m_yres));
		points.append(point);
	}

	if(insideCompound)
	{
		if(count > 0)
		{
			// inside a compound ? convert it into path because for compound
			// we will only use paths
			::WPXPropertyListVector &path = m_groupStack.top().compoundPath;
			::WPXPropertyList element;
			element = points[0];
			element.insert("libwpg:path-action", "M");
			path.append(element);
			for(unsigned long ii = 1; ii < count; ii++)
			{
				element.clear();
				element = points[ii];
				element.insert("libwpg:path-action", "L");
				path.append(element);
			}
		}
	}
	else
	{
		// otherwise draw directly
		if (count > 2)
		{
			if(objCh.windingRule)
				tmpStyle.insert("svg:fill-rule", "nonzero");
			else
				tmpStyle.insert("svg:fill-rule", "evenodd");

			m_painter->setStyle( tmpStyle, objCh.filled ? m_gradient : ::WPXPropertyListVector() );

			if (objCh.filled || objCh.closed)
				m_painter->drawPolygon(points);
			else
				m_painter->drawPolyline(points);
		}
		else
		{

			m_painter->setStyle( tmpStyle, ::WPXPropertyListVector() );
			m_painter->drawPolyline(points);
		}
	}

	WPG_DEBUG_MSG(("   Vertices count : %li\n", count));
	for(unsigned int j = 0; j < count; j++ )
		WPG_DEBUG_MSG(("        Point #%d : %g,%g\n", j+1, points[j]["svg:x"]->getDouble(), points[j]["svg:x"]->getDouble()));
}

void WPG2Parser::handlePolyspline()
{
	// fprintf(stderr, "This is actually not an error, just a crash so that we know ");
	// fprintf(stderr, "that there is an actual document containing the Polyspline record\n");
	// crash here !!!
	// *((int *)NULL) = 0;
	if (!m_graphicsStarted)
		return;
	// We have never seen a polyspline in a WPG file and we are not even able to generate
	// one, so, we draw a polyline instead and wait for bug reports :)
	handlePolyline();
}

void WPG2Parser::handlePolycurve()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);
	m_matrix = objCh.matrix;

	WPXPropertyList tmpStyle = m_style;

	if (!objCh.filled)
		tmpStyle.insert("draw:fill", "none");
	if (!objCh.framed)
		tmpStyle.insert("draw:stroke", "none");

	bool insideCompound = m_groupStack.empty() ? false :
	                      m_groupStack.top().isCompoundPolygon();

	// inside a compound, so take the parent transformation into account
	if(insideCompound)
		m_matrix.transformBy(m_groupStack.top().compoundMatrix);

	unsigned int count = readU16();

	::WPXPropertyListVector path;
	::WPXPropertyList element;
	::WPXPropertyListVector vertices;
	::WPXPropertyListVector controlPoints;
	for(unsigned int i = 0; i < count; i++ )
	{
		long ix = (m_doublePrecision) ? readS32() : readS16();
		long iy = (m_doublePrecision) ? readS32() : readS16();
		TRANSFORM_XY(ix,iy);

		long ax = (m_doublePrecision) ? readS32() : readS16();
		long ay = (m_doublePrecision) ? readS32() : readS16();
		TRANSFORM_XY(ax,ay);

		long tx = (m_doublePrecision) ? readS32() : readS16();
		long ty = (m_doublePrecision) ? readS32() : readS16();
		TRANSFORM_XY(tx,ty);

		element.insert("svg:x", (TO_DOUBLE(ax)/m_xres));
		element.insert("svg:y", (TO_DOUBLE(ay)/m_yres));
		if (i == 0)
			element.insert("libwpg:path-action", "M");
		else
		{
			element.insert("svg:x2", (TO_DOUBLE(ix)/m_xres));
			element.insert("svg:y2", (TO_DOUBLE(iy)/m_yres));
			element.insert("libwpg:path-action", "C");
		}
		path.append(element);
		element.insert("svg:x1", (TO_DOUBLE(tx)/m_xres));
		element.insert("svg:y1", (TO_DOUBLE(ty)/m_yres));
	}

	element.clear();
	if (objCh.closed)
	{
		element.insert("libwpg:path-action", "Z");
		path.append(element);
	}

	if(insideCompound)
		// inside a compound ? just collect the path together
		m_groupStack.top().compoundPath.append(path);
	else
	{
		// otherwise draw directly
		if(objCh.windingRule)
			tmpStyle.insert("svg:fill-rule", "nonzero");
		else
			tmpStyle.insert("svg:fill-rule", "evenodd");
		m_painter->setStyle( tmpStyle, objCh.filled ? m_gradient : ::WPXPropertyListVector() );
		m_painter->drawPath(path);
	}
}

void WPG2Parser::handleRectangle()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);
	m_matrix = objCh.matrix;

	WPXPropertyList tmpStyle = m_style;

	if (!objCh.filled)
		tmpStyle.insert("draw:fill", "none");
	if (!objCh.framed)
		tmpStyle.insert("draw:stroke", "none");

	long x1 = (m_doublePrecision) ? readS32() : readS16();
	long y1 = (m_doublePrecision) ? readS32() : readS16();
	TRANSFORM_XY(x1,y1);

	long x2 = (m_doublePrecision) ? readS32() : readS16();
	long y2 = (m_doublePrecision) ? readS32() : readS16();
	TRANSFORM_XY(x2,y2);

	long xs1 = (x1 <= x2) ? x1 : x2;
	long xs2 = (x1 <= x2) ? x2 : x1;
	long ys1 = (y1 <= y2) ? y1 : y2;
	long ys2 = (y1 <= y2) ? y2 : y1;

	long rx = (m_doublePrecision) ? readS32() : readS16();
	long ry = (m_doublePrecision) ? readS32() : readS16();

	::WPXPropertyList propList;
	propList.insert("svg:x", (TO_DOUBLE(xs1) / m_xres));
	propList.insert("svg:width", (TO_DOUBLE(xs2-xs1) / m_xres));
	propList.insert("svg:y", (TO_DOUBLE(ys1) / m_yres));
	propList.insert("svg:height", (TO_DOUBLE(ys2-ys1) / m_yres));

	propList.insert("svg:rx", (TO_DOUBLE(rx)/m_xres));
	propList.insert("svg:ry", (TO_DOUBLE(ry)/m_yres));

	m_painter->setStyle( tmpStyle, objCh.filled ? m_gradient : ::WPXPropertyListVector() );
	m_painter->drawRectangle(propList);

	WPG_DEBUG_MSG(("      X1 : %li\n", x1));
	WPG_DEBUG_MSG(("      Y1 : %li\n", y1));
	WPG_DEBUG_MSG(("      X2 : %li\n", x2));
	WPG_DEBUG_MSG(("      Y2 : %li\n", y2));
	WPG_DEBUG_MSG((" Round X : %li\n", rx));
	WPG_DEBUG_MSG((" Round Y : %li\n", ry));
}

void WPG2Parser::handleArc()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);
	m_matrix = objCh.matrix;

	WPXPropertyList tmpStyle = m_style;

	if (!objCh.filled)
		tmpStyle.insert("draw:fill", "none");
	if (!objCh.framed)
		tmpStyle.insert("draw:stroke", "none");

	long cx = (m_doublePrecision) ? readS32() : readS16();
	long cy = (m_doublePrecision) ? readS32() : readS16();

	long radx = (m_doublePrecision) ? readS32() : readS16();
	long rady = (m_doublePrecision) ? readS32() : readS16();

	long ix = (m_doublePrecision) ? readS32() : readS16();
	ix += cx;
	long iy = (m_doublePrecision) ? readS32() : readS16();
	iy += cy;

	long ex = (m_doublePrecision) ? readS32() : readS16();
	ex += cx;
	long ey = (m_doublePrecision) ? readS32() : readS16();
	ey += cy;

	TRANSFORM_XY(cx,cy);
	TRANSFORM_XY(ix,iy);
	TRANSFORM_XY(ex,ey);

	m_painter->setStyle( tmpStyle, objCh.filled ? m_gradient : ::WPXPropertyListVector() );

	if (ix == ex && iy == ey)
	{
		::WPXPropertyList propList;
		propList.insert("svg:cx", (TO_DOUBLE(cx) / m_xres));
		propList.insert("svg:cy", (TO_DOUBLE(cy) / m_xres));
		propList.insert("svg:rx", (TO_DOUBLE(radx) / m_xres));
		propList.insert("svg:ry", (TO_DOUBLE(rady) / m_xres));
		if (objCh.rotate)
			propList.insert("libwpg:rotate", objCh.rotationAngle, WPX_GENERIC);

		m_painter->drawEllipse(propList);
	}
	else
	{
		::WPXPropertyList element;
		::WPXPropertyListVector path;

		element.insert("libwpg:path-action", "M");
		element.insert("svg:x", (TO_DOUBLE(ix)/m_xres));
		element.insert("svg:y", (TO_DOUBLE(iy)/m_yres));
		path.append(element);
		element.clear();

		element.insert("libwpg:path-action", "A");
		element.insert("svg:rx", (TO_DOUBLE(radx)/m_xres));
		element.insert("svg:ry", (TO_DOUBLE(rady)/m_yres));
		element.insert("svg:x", (TO_DOUBLE(ex)/m_xres));
		element.insert("svg:y", (TO_DOUBLE(ey)/m_yres));
		if (objCh.rotate)
			element.insert("libwpg:rotate", objCh.rotationAngle, WPX_GENERIC);
		path.append(element);

		m_painter->drawPath(path);
	}

	WPG_DEBUG_MSG(("   Center point x : %li\n", cx));
	WPG_DEBUG_MSG(("   Center point y : %li\n", cy));
	WPG_DEBUG_MSG(("         Radius x : %li\n", radx));
	WPG_DEBUG_MSG(("         Radius y : %li\n", rady));
	WPG_DEBUG_MSG(("  Initial point x : %li\n", ix));
	WPG_DEBUG_MSG(("  Initial point y : %li\n", iy));
	WPG_DEBUG_MSG(("      End point x : %li\n", ex));
	WPG_DEBUG_MSG(("      End point y : %li\n", ey));
}

void WPG2Parser::handleBitmap()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);

	if (objCh.scale)
	{
		if (objCh.sxcos < 0)
			m_hFlipped = true;
		if (objCh.sycos < 0)
			m_vFlipped = true;
	}
	m_matrix = objCh.matrix;

	long x1 = (m_doublePrecision) ? readS32() : readS16();
	long y1 = (m_doublePrecision) ? readS32() : readS16();
	long x2 = (m_doublePrecision) ? readS32() : readS16();
	long y2 = (m_doublePrecision) ? readS32() : readS16();
	TRANSFORM_XY(x1,y1);
	TRANSFORM_XY(x2,y2);

	long hres = (m_doublePrecision) ? readS32() : readS16();
	long vres = (m_doublePrecision) ? readS32() : readS16();

	long xs1 = (x1 <= x2) ? x1 : x2;
	long xs2 = (x1 <= x2) ? x2 : x1;
	long ys1 = (y1 <= y2) ? y1 : y2;
	long ys2 = (y1 <= y2) ? y2 : y1;

	m_bitmap.x1 = TO_DOUBLE(xs1) / m_xres;
	m_bitmap.y1 = TO_DOUBLE(ys1) / m_yres;
	m_bitmap.x2 = TO_DOUBLE(xs2) / m_xres;
	m_bitmap.y2 = TO_DOUBLE(ys2) / m_yres;
	if (hres == 0)
		hres = 72;
	m_bitmap.hres = hres;
	if (vres == 0)
		vres = 72;
	m_bitmap.vres = vres;

	WPG_DEBUG_MSG(("   x1 : %li\n", x1));
	WPG_DEBUG_MSG(("   y1 : %li\n", y1));
	WPG_DEBUG_MSG(("   x2 : %li\n", x2));
	WPG_DEBUG_MSG(("   y2 : %li\n", y2));
	WPG_DEBUG_MSG((" hres : %li (pixel per inch)\n", hres));
	WPG_DEBUG_MSG((" vres : %li (pixel per inch)\n", vres));
}

void WPG2Parser::handleBitmapData()
{
	if (!m_graphicsStarted)
		return;
	unsigned width = readU16();
	unsigned height = readU16();
	unsigned color_format = readU8();
	unsigned compression_format = readU8();

	WPG_DEBUG_MSG(("     dimension : %g, %g  %g, %g\n", m_bitmap.x1, m_bitmap.y1, m_bitmap.x2, m_bitmap.y2));
	WPG_DEBUG_MSG(("         width : %i pixels\n", width));
	WPG_DEBUG_MSG(("        height : %i pixels\n", height));
	WPG_DEBUG_MSG(("  color format : %d\n", color_format));
	WPG_DEBUG_MSG(("   compression : %d (%s)\n", compression_format,
	               (compression_format==0) ? "uncompressed":
	               (compression_format==1) ? "run-length encoding" : "unknown"));

	if (color_format > 12) // not documented and we are likely not knowing what to do with this
		return;
	unsigned tmpBufferSize;
	unsigned raster_len = width;
	if (color_format == 1)
	{
		tmpBufferSize = (width/8+1)*height;
		raster_len = width/8 + ((width % 8) ? 1 : 0);
	}
	else if (color_format == 2)
	{
		tmpBufferSize = (width/4+1)*height;
		raster_len = width/4 + ((width % 4) ? 1 : 0);
	}
	else if (color_format == 3)
	{
		tmpBufferSize = (width/2+1)*height;
		raster_len = width/2 + ((width % 2) ? 1 : 0);
	}
	else if (color_format == 5 || color_format == 6 || color_format == 7)
		tmpBufferSize = 2*width*height;
	else if (color_format == 8)
		tmpBufferSize = 3*width*height;
	else if (color_format == 9)
		tmpBufferSize = 4*width*height;
	else if (color_format == 10)
		tmpBufferSize = 6*width*height;
	else if (color_format == 11)
		tmpBufferSize = 8*width*height;
	else
		tmpBufferSize = width*height;


	std::vector<unsigned char> buffer;
	buffer.reserve(tmpBufferSize);

	// plain data, uncompression
	if(compression_format==0)
		for(unsigned ii=0; ii < tmpBufferSize && !m_input->atEOS() && m_input->tell() <= m_recordEnd; ii++)
			buffer.push_back( readU8() );
	// run-length encoding
	else if(compression_format==1)
	{
		unsigned char data[256];
		bool xor_raster = false;
		unsigned next_scanline = 0;
		unsigned data_size = 1;

		WPG_DEBUG_MSG(("Decoding RLE data\n"));

		// FIXME check for ptr, it should not go out of bound!!
		while (m_input->tell() <= m_recordEnd && !m_input->atEOS() && buffer.size() < tmpBufferSize)
		{
			unsigned char opcode = readU8();
			// specify data size
			if(opcode == 0x7d)
			{
				unsigned new_data_size = readU8();
				if(new_data_size != data_size)
				{
					data_size = new_data_size;
					if (tmpBufferSize < data_size*width*height)
					{
						tmpBufferSize = data_size*width*height;
						buffer.reserve(tmpBufferSize);
					}
					raster_len = data_size*width;
				}
			}

			// a run of black (index #0)
			else if(opcode == 0x7f)
			{
				unsigned count = 1 + readU8();
				for( ; count ; --count )
					for(unsigned j = 0; j < data_size && !m_input->atEOS(); j++)
						buffer.push_back( 0 );

			}

			// a run of white (index #255)
			else if(opcode == 0xff)
			{
				unsigned count = 1 + readU8();

				for( ; count ; --count )
					for(unsigned j = 0; j < data_size && !m_input->atEOS(); j++)
						buffer.push_back( 255 );
			}

			// extend previous run
			else if(opcode == 0xfd)
			{
				unsigned count = 1 + readU8();
				for( ; count; --count)
					for(unsigned j = 0; j < data_size && !m_input->atEOS(); j++)
						buffer.push_back( data[j] );
			}

			// repeat raster
			else if(opcode == 0xfe)
			{
				unsigned count = 1 + readU8();
				if ( buffer.size() < raster_len )
					break;
				unsigned raster_source = buffer.size() - raster_len;
				for( ; count; --count)
					for(unsigned long r = 0; r < raster_len; r++)
					{
						unsigned char pixel = buffer[raster_source + r];
						buffer.push_back( pixel );
					}
			}

			// XOR
			else if(opcode == 0x7e)
			{
				// Xor-ing will happen when about to enter next raster
				// see after the last if down below
				xor_raster = true;
				next_scanline = buffer.size() + raster_len;
			}

			// NOTE: the following two IFs here must be the last ones

			// a run of data
			else if(opcode >= 0x80)
			{
				unsigned count = 1 + (opcode & 0x7f);
				for(unsigned i = 0; i < data_size && !m_input->atEOS(); i++)
					data[i] = readU8();
				for( ; count; --count)
					for(unsigned j = 0; j < data_size; j++)
						buffer.push_back( data[j] );
			}

			// simple copy
			else if(opcode <= 0x7f)
			{
				unsigned count = opcode + 1;
				for( ; count; --count)
					for(unsigned j = 0; j < data_size && !m_input->atEOS(); j++)
						buffer.push_back( readU8() );
			}

			// unreachable: only sentinel
			else
			{
				WPG_DEBUG_MSG(("  ! unknown opcode %02x\n", opcode));
				break;
			}

			// new raster/scanline? if necessary do the XOR now
			if(xor_raster && buffer.size() >= next_scanline)
			{
				// reset, because XOR in one raster at a time
				xor_raster = false;

				// current raster must be XORed with previous raster
				unsigned current = next_scanline - raster_len;
				unsigned previous = current - raster_len;
				if (current >= buffer.size() || previous >= buffer.size())
					return;
				for( unsigned r = 0; r < raster_len; r++)
					buffer[current++] ^= buffer[previous++];

			}

		}

		WPG_DEBUG_MSG(("Finish decoding RLE data\n"));
	}

	// no buffer? format is unknown
	if(!buffer.size()) return;

	while (buffer.size() < tmpBufferSize)
		buffer.push_back(0);

	// prepare the bitmap structure for the listener
	libwpg::WPGBitmap bitmap(width, height, m_vFlipped, m_hFlipped);
	::WPXPropertyList propList;
	propList.insert("svg:x", (double)m_bitmap.x1);
	propList.insert("svg:y", (double)m_bitmap.y1);
	propList.insert("svg:width", (m_bitmap.x2 - m_bitmap.x1));
	propList.insert("svg:height", (m_bitmap.y2 - m_bitmap.y1));
	propList.insert("libwpg:mime-type", "image/bmp");

	// format 1: each byte represents 8 pixels, the color fetched from the palette
	if(color_format == 1)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{
				if ((x==0) && (i % 8 != 0))
					i = (i/8 + 1) * 8;
				unsigned index = ((buffer[i/8] & (0x01 << (7 - (i % 8)))) >> (7 - (i % 8)));
				const libwpg::WPGColor &color = m_colorPalette[index];
				bitmap.setPixel(x, y, color);
			}
		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	// format 2: each byte represents 4 pixels, the color fetched from the palette
	else if(color_format == 2)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{
				if ((x==0) && (i % 4 != 0))
					i = (i/4 + 1) * 4;
				unsigned index = ((buffer[i/4] & (0x03 << 2*(3 - (i % 4)))) >> 2*(3 - (i % 4)));
				const libwpg::WPGColor &color = m_colorPalette[index];
				bitmap.setPixel(x, y, color);
			}
		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	// format 3: each byte represents 2 pixels, the color fetched from the palette
	else if(color_format == 3)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{
				if ((x==0) && (i % 2 != 0))
					i = (i/2 + 1) * 2;
				unsigned index = ((buffer[i/2] & (0x0f << 4*(1 - (i % 2)))) >> 4*(1 - (i % 2)));
				const libwpg::WPGColor &color = m_colorPalette[index];
				bitmap.setPixel(x, y, color);
			}
		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	// format 4: each byte represents a pixel, the color fetched from the palette
	else if(color_format == 4)
	{
		int i = 0;
		for(unsigned long y = 0; y < height; y++)
			for(unsigned long x = 0; x < width; x++, i++)
			{
				const libwpg::WPGColor &color = m_colorPalette[buffer[i]];
				bitmap.setPixel(x, y, color);
			}

		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	// format 5: greyscale of 16 bits
	else if (color_format == 5)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{

				const libwpg::WPGColor color(buffer[2*i+1], buffer[2*i+1], buffer[2*i+1]);
				bitmap.setPixel(x, y, color);
			}

		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	// format 6: RGB, with 5 bits per colour
	else if (color_format == 6)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{
				unsigned short tmpColor = (unsigned short)(buffer[2*i] | (buffer[2*i+1] << 8));
				const libwpg::WPGColor color((tmpColor >> 10) & 0x1f, (tmpColor >> 5) & 0x1f, tmpColor & 0x1f);
				bitmap.setPixel(x, y, color);
			}

		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	// format 7:
	else if (color_format == 7)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{
				unsigned short tmpColor = (unsigned short)(buffer[2*i] | (buffer[2*i+1] << 8));
				const libwpg::WPGColor color(tmpColor & 0x0f, (tmpColor >> 4) & 0x0f, (tmpColor >> 8) & 0x0f, (tmpColor >> 12) & 0x0f);
				bitmap.setPixel(x, y, color);
			}

		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	else if (color_format == 8)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{
				const libwpg::WPGColor color(buffer[3*i+2] & 0xff, buffer[3*i+1] & 0xff, buffer[3*i] & 0xff);
				bitmap.setPixel(x, y, color);
			}

		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	else if (color_format == 9)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{
				const libwpg::WPGColor color(buffer[4*i+3], buffer[4*i+2] & 0xff, buffer[4*i+1] & 0xff, buffer[4*i] & 0xff);
				bitmap.setPixel(x, y, color);
			}

		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}
	else if (color_format == 12)
	{
		int i = 0;
		for (unsigned long y = 0; y < height; y++)
			for (unsigned long x = 0; x < width; x++, i++)
			{

				const libwpg::WPGColor color(buffer[i], buffer[i], buffer[i]);
				bitmap.setPixel(x, y, color);
			}

		m_painter->drawGraphicObject(propList, bitmap.getDIB());
	}

}

void WPG2Parser::handleObjectCapsule()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);
	m_matrix = objCh.matrix;

	long x1 = (m_doublePrecision) ? readS32() : readS16();
	long y1 = (m_doublePrecision) ? readS32() : readS16();
	long x2 = (m_doublePrecision) ? readS32() : readS16();
	long y2 = (m_doublePrecision) ? readS32() : readS16();
	TRANSFORM_XY(x1,y1);
	TRANSFORM_XY(x2,y2);

	long xs1 = (x1 <= x2) ? x1 : x2;
	long xs2 = (x1 <= x2) ? x2 : x1;
	long ys1 = (y1 <= y2) ? y1 : y2;
	long ys2 = (y1 <= y2) ? y2 : y1;

	m_binaryData.x1 = TO_DOUBLE(xs1) / m_xres;
	m_binaryData.y1 = TO_DOUBLE(ys1) / m_yres;
	m_binaryData.x2 = TO_DOUBLE(xs2) / m_xres;
	m_binaryData.y2 = TO_DOUBLE(ys2) / m_yres;

	unsigned long numDescriptions = readU16();

	m_binaryData.mimeTypes.clear();
	m_binaryData.mimeTypes.reserve(numDescriptions);

	static const char *mimeTypesMap[] =
	{
		"", // 0
		"", // "image/x-wpg",
		"", // "image/x-wpg",
		"",
		"",
		"", // 5
		"image/bmp",
		"image/cgm",
		"",
		"image/x-dxf",
		"image/x-eps", // 0xA
		"",
		"image/gif",
		"application/vnd.hp-hpgl",
		"",
		"", // 0xF
		"", // 0x10
		"",
		"",
		"image/x-pcx ",
		"image/x-lotus-pic",
		"", // Ox15
		"",
		"image/x-quicktime",
		"",
		"application/x-postscript",
		"", // 0x1A
		"",
		"image/x-targa",
		"image/tiff",
		"",
		"image/wmf", // 0x1f
		"", // 0x20
		"",
		"",
		"",
		"",
		"image/png",
		"image/gif" // 0x26
	};

	for (unsigned long i = 0; m_input->tell() <= m_recordEnd && !m_input->atEOS() && i < numDescriptions; i++)
	{
		unsigned char description = readU8();
		if (description < 0x27)
			m_binaryData.mimeTypes.push_back(WPXString(mimeTypesMap[description]));
		m_input->seek(7, WPX_SEEK_CUR);
	}

	m_binaryData.objectIndex = 0;

	WPG_DEBUG_MSG(("             x1 : %li\n", x1));
	WPG_DEBUG_MSG(("             y1 : %li\n", y1));
	WPG_DEBUG_MSG(("             x2 : %li\n", x2));
	WPG_DEBUG_MSG(("             y2 : %li\n", y2));
	WPG_DEBUG_MSG(("numDescriptions : %li\n", numDescriptions));
}

void WPG2Parser::handleObjectImage()
{
	if (!m_graphicsStarted)
		return;
	if ((unsigned long)m_binaryData.objectIndex >= m_binaryData.mimeTypes.size())
		return;
	unsigned accessoryDataLength = readU16();
	m_input->seek(accessoryDataLength, WPX_SEEK_CUR);

	::WPXPropertyList propList;
	propList.insert("svg:x", (double)m_binaryData.x1);
	propList.insert("svg:y", (double)m_binaryData.y1);
	propList.insert("svg:width", (m_binaryData.x2 - m_binaryData.x1));
	propList.insert("svg:height", (m_binaryData.y2 - m_binaryData.y1));
	propList.insert("libwpg:mime-type", m_binaryData.mimeTypes[m_binaryData.objectIndex]);

	WPG_DEBUG_MSG(("Image Object Mime Type : %s\n", propList["libwpg:mime-type"]->getStr().cstr()));

	::WPXBinaryData binaryData;
	while (!m_input->atEOS() && m_input->tell() <= m_recordEnd)
		binaryData.append((char)readU8());
	WPG_DEBUG_MSG(("     Image Object Size : %li\n", (unsigned long)binaryData.size()));

	// temporary for debug - dump the binary data (need to have write access in the current directory
#if DUMP_BINARY_DATA
	std::ostringstream filename;
	filename << "binarydump" << m_binaryId++ << ".bin";
	FILE *f = fopen(filename.str().c_str(), "wb");
	if (f)
	{
		const char *tmpBinaryBuffer = binaryData.getDataBuffer();
		unsigned long tmpBufferSize = binaryData.size();
		for (unsigned long k = 0; k < tmpBufferSize; k++)
			fprintf(f, "%c",tmpBinaryBuffer[k]);
		fclose(f);
	}
#endif


	m_painter->drawGraphicObject(propList, binaryData);
	m_binaryData.objectIndex++;
}

void WPG2Parser::handleTextLine()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);
	m_matrix = objCh.matrix;

	unsigned short textFlags = readU16();
	long x = (m_doublePrecision) ? readS32() : readS16();
	long y = (m_doublePrecision) ? readS32() : readS16();
	TRANSFORM_XY(x,y);

	unsigned char horizontalAlignment = readU8();
	unsigned char verticalAlignment = readU8();

	double baseLineAngle = fixedPointToDouble(readU32());

	WPG_DEBUG_MSG(("            text flags : 0x%.4x\n", textFlags));
	WPG_DEBUG_MSG(("                     x : %li\n", x));
	WPG_DEBUG_MSG(("                     y : %li\n", y));
	WPG_DEBUG_MSG(("  horizontal alignment : 0x%.2x\n", horizontalAlignment));
	WPG_DEBUG_MSG(("    vertical alignment : 0x%.2x\n", verticalAlignment));
	WPG_DEBUG_MSG(("        baseline angle : %.4f\n", baseLineAngle));

	m_textData.x1 = TO_DOUBLE(x)/m_xres;
	m_textData.y1 = TO_DOUBLE(y)/m_yres;
	m_textData.x2 = TO_DOUBLE(x)/m_xres;
	m_textData.y2 = TO_DOUBLE(y)/m_yres;
	m_textData.flags = textFlags;
	m_textData.horAlign = horizontalAlignment;
	m_textData.vertAlign = verticalAlignment;
	m_textData.baseLineAngle = baseLineAngle;

	m_drawTextData = true;
}

void WPG2Parser::handleTextBlock()
{
	if (!m_graphicsStarted)
		return;
	ObjectCharacterization objCh;
	parseCharacterization(&objCh);
	m_matrix = objCh.matrix;

	long x1 = (m_doublePrecision) ? readS32() : readS16();
	long y1 = (m_doublePrecision) ? readS32() : readS16();
	long x2 = (m_doublePrecision) ? readS32() : readS16();
	long y2 = (m_doublePrecision) ? readS32() : readS16();
	TRANSFORM_XY(x1,y1);
	TRANSFORM_XY(x2,y2);

	long xs1 = (x1 <= x2) ? x1 : x2;
	long xs2 = (x1 <= x2) ? x2 : x1;
	long ys1 = (y1 <= y2) ? y1 : y2;
	long ys2 = (y1 <= y2) ? y2 : y1;

	WPG_DEBUG_MSG(("             x1 : %li\n", xs1));
	WPG_DEBUG_MSG(("             y1 : %li\n", ys1));
	WPG_DEBUG_MSG(("             x2 : %li\n", xs2));
	WPG_DEBUG_MSG(("             y2 : %li\n", ys2));

	m_textData.x1 = TO_DOUBLE(xs1)/m_xres;
	m_textData.y1 = TO_DOUBLE(ys1)/m_yres;
	m_textData.x2 = TO_DOUBLE(xs2)/m_xres;
	m_textData.y2 = TO_DOUBLE(ys2)/m_yres;
	m_textData.flags = 0;
	m_textData.horAlign = 0;
	m_textData.vertAlign = 0;
	m_textData.baseLineAngle = 0.0;

	m_drawTextData = false;
}

void WPG2Parser::handleTextPath()
{
	if (!m_graphicsStarted)
		return;

	m_drawTextData = false;
}

void WPG2Parser::handleTextData()
{
	if (!m_graphicsStarted)
		return;
	if (!m_drawTextData)
		return;

	::WPXBinaryData textData;
	while (!m_input->atEOS() && m_input->tell() <= m_recordEnd)
		textData.append((char)readU8());
	WPGTextDataHandler handler(m_painter);
	::WPXPropertyList propList;

	propList.insert("svg:x", (double)m_textData.x1);
	propList.insert("svg:y", (double)m_textData.y1);
	if (m_textData.x1 != m_textData.x2 && m_textData.y1 != m_textData.y2)
	{
		propList.insert("svg:width", (m_textData.x2 - m_textData.x1));
		propList.insert("svg:height", (m_textData.y2 - m_textData.y1));
	}

	m_painter->startTextObject(propList, ::WPXPropertyListVector());

	WPDocument::parseSubDocument(const_cast<WPXInputStream *>(textData.getDataStream()), &handler, WPD_FILE_FORMAT_WP6);

	m_painter->endTextObject();

	m_drawTextData = false;
}

void WPG2Parser::resetPalette()
{
	m_colorPalette.clear();
	for (int i=0; i<256; i++)
	{
		unsigned char red = defaultWPG2PaletteRed[i];
		unsigned char green = defaultWPG2PaletteGreen[i];
		unsigned char blue = defaultWPG2PaletteBlue[i];
		libwpg::WPGColor color(red, green, blue);
		m_colorPalette[i] = color;
	}
}
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
