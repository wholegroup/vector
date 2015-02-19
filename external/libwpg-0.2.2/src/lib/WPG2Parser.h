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

#ifndef __WPG2PARSER_H__
#define __WPG2PARSER_H__

#include "WPGXParser.h"
#include "WPGDashArray.h"
#include "WPGBitmap.h"
#include <libwpd/libwpd.h>

#include <map>
#include <stack>
#include <vector>

class WPG2TransformMatrix
{
public:
	double element[3][3];

	WPG2TransformMatrix()
	{
		// identity transformation
		element[0][0] = element[1][1] = 1;
		element[2][2] = 1;
		element[0][1] = element[0][2] = 0;
		element[1][0] = element[1][2] = 0;
		element[2][0] = element[2][1] = 0;
	}

	void transform(long &x, long &y) const
	{
		long rx = (long)(element[0][0]*x + element[1][0]*y + element[2][0]);
		long ry = (long)(element[0][1]*x + element[1][1]*y + element[2][1]);
		x = rx;
		y = ry;
	}

	::WPXPropertyList transformPoint(const ::WPXPropertyList &p) const
	{
		::WPXPropertyList propList;
		propList.insert("svg:x", (element[0][0]*p["svg:x"]->getDouble() + element[1][0]*p["svg:y"]->getDouble() + element[2][0]));
		propList.insert("svg:y", (element[0][1]*p["svg:x"]->getDouble() + element[1][1]*p["svg:y"]->getDouble() + element[2][1]));
		return propList;
	}

	::WPXPropertyList transformRect(const ::WPXPropertyList &r) const
	{
		::WPXPropertyList propList;
		double oldx1 = r["svg:x"]->getDouble();
		double oldy1 = r["svg:y"]->getDouble();
		double oldx2 = r["svg:x"]->getDouble() + r["svg:width"]->getDouble();
		double oldy2 = r["svg:y"]->getDouble() + r["svg:height"]->getDouble();

		double newx1 = element[0][0]*oldx1 + element[1][0]*oldy1 + element[2][0];
		double newy1 = element[0][1]*oldx1 + element[1][1]*oldy1 + element[2][1];
		double newx2 = element[0][0]*oldx2 + element[1][0]*oldy2 + element[2][0];
		double newy2 = element[0][1]*oldx2 + element[1][1]*oldy2 + element[2][1];

		propList.insert("svg:x", (double)newx1);
		propList.insert("svg:y", (double)newy1);
		propList.insert("svg:width", (newx2-newx1));
		propList.insert("svg:height", (newy2-newy1));
		return propList;
	}

	WPG2TransformMatrix &transformBy(const WPG2TransformMatrix &m)
	{
		double result[3][3];

		for(int i = 0; i < 3; i++)
			for(int j = 0; j < 3; j++)
			{
				result[i][j] = 0;
				for(int k = 0; k < 3; k++)
					result[i][j] += m.element[i][k]*element[k][j];
			}

		for(int x = 0; x < 3; x++)
			for(int y = 0; y < 3; y++)
				element[x][y] = result[x][y];

		return *this;
	}
};

class WPGCompoundPolygon
{
public:
	WPG2TransformMatrix matrix;
	bool isFilled;
	bool isFramed;
	bool isClosed;

	WPGCompoundPolygon(): matrix(), isFilled(true), isFramed(true), isClosed(true) {}
};

class WPGGroupContext
{
public:
	unsigned subIndex;
	int parentType;
	::WPXPropertyListVector compoundPath;
	WPG2TransformMatrix compoundMatrix;
	bool compoundWindingRule;
	bool compoundFilled;
	bool compoundFramed;
	bool compoundClosed;

	WPGGroupContext(): subIndex(0), parentType(0),
		compoundPath(), compoundMatrix(), compoundWindingRule(false),
		compoundFilled(false), compoundFramed(true), compoundClosed(false)	{}

	bool isCompoundPolygon() const
	{
		return parentType == 0x1a;
	}
};

class WPGBitmapContext
{
public:
	double x1, y1, x2, y2;
	long hres, vres;
	WPGBitmapContext(): x1(0), y1(0), x2(0), y2(0), hres(100), vres(100) {}
};

class WPGBinaryDataContext
{
public:
	double x1, y1, x2, y2;
	int numObjects, objectIndex;
	std::vector<WPXString> mimeTypes;
	WPGBinaryDataContext(): x1(0), y1(0), x2(0), y2(0), numObjects(0), objectIndex(0), mimeTypes() {}
};

class WPGTextDataContext
{
public:
	double x1, y1, x2, y2;
	unsigned short flags;
	unsigned char vertAlign;
	unsigned char horAlign;
	double baseLineAngle;
	WPGTextDataContext(): x1(0), y1(0), x2(0), y2(0), flags(), vertAlign(), horAlign(), baseLineAngle(0.0) {}
};

class WPG2Parser : public WPGXParser
{
public:
	WPG2Parser(WPXInputStream *input, libwpg::WPGPaintInterface *painter, bool isEmbedded = false);
	bool parse();

private:
	void handleStartWPG();
	void handleEndWPG();
	void handleFormSettings();
	void handleLayer();
	void handleCompoundPolygon();

	void handlePenStyleDefinition();
//	void handlePatternDefinition();
	void handleColorPalette();
	void handleDPColorPalette();
	void handlePenForeColor();
	void handleDPPenForeColor();
	void handlePenBackColor();
	void handleDPPenBackColor();
	void handlePenStyle();
	void handlePenSize();
	void handleDPPenSize();
	void handleLineCap();
	void handleLineJoin();
	void handleBrushGradient();
	void handleDPBrushGradient();
	void handleBrushForeColor();
	void handleDPBrushForeColor();
	void handleBrushBackColor();
	void handleDPBrushBackColor();
	void handleBrushPattern();

	void handlePolyline();
	void handlePolyspline();
	void handlePolycurve();
	void handleRectangle();
	void handleArc();

	void handleBitmap();
	void handleBitmapData();

	void handleTextData();
	void handleTextLine();
	void handleTextBlock();
	void handleTextPath();

	void handleObjectCapsule();
	void handleObjectImage();

	void resetPalette();
	void flushCompoundPolygon();
	void setPenStyle();

	// parsing context
	int m_recordLength;
	long m_recordEnd;
	bool m_success;
	bool m_exit;
	bool m_graphicsStarted;
	unsigned int m_xres;
	unsigned int m_yres;
	long m_xofs;
	long m_yofs;
	long m_width;
	long m_height;
	bool m_doublePrecision;
	::WPXPropertyList m_style;
	libwpg::WPGColor m_penForeColor;
	libwpg::WPGColor m_penBackColor;
	libwpg::WPGColor m_brushForeColor;
	libwpg::WPGColor m_brushBackColor;
	libwpg::WPGDashArray m_dashArray;
	::WPXPropertyListVector m_gradient;
	std::map<unsigned int,libwpg::WPGDashArray> m_dashArrayStyles;
	bool m_layerOpened;
#ifdef DEBUG
	unsigned int m_layerId;
#endif
	WPG2TransformMatrix m_matrix;
	double m_gradientAngle;
	::WPXPropertyList m_gradientRef;
	std::stack<WPGGroupContext> m_groupStack;
	WPG2TransformMatrix m_compoundMatrix;
	bool m_compoundWindingRule;
	bool m_compoundFilled;
	bool m_compoundFramed;
	bool m_compoundClosed;
	WPGBitmapContext m_bitmap;
	WPGBinaryDataContext m_binaryData;
	bool m_hFlipped, m_vFlipped;
	WPGTextDataContext m_textData;
	bool m_drawTextData;

	class ObjectCharacterization;
	void parseCharacterization(ObjectCharacterization *);
#if DUMP_BINARY_DATA
	unsigned m_binaryId;
#endif
};

#endif // __WPG2PARSER_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
