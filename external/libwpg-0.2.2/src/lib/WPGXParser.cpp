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
 * Copyright (C) 2004 Marc Oude Kotte (marc@solcon.nl)
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

#include "WPGXParser.h"
#include "libwpg_utils.h"

WPGXParser::WPGXParser(WPXInputStream *input, libwpg::WPGPaintInterface *painter):
	m_input(input), m_painter(painter), m_colorPalette(std::map<int,libwpg::WPGColor>())
{
}

WPGXParser::WPGXParser(const WPGXParser &parser):
	m_input(parser.m_input), m_painter(parser.m_painter),
	m_colorPalette(parser.m_colorPalette)
{
}

unsigned char WPGXParser::readU8()
{
	if (!m_input || m_input->atEOS())
		return (unsigned char)0;
	unsigned long numBytesRead;
	unsigned char const *p = m_input->read(sizeof(unsigned char), numBytesRead);

	if (p && numBytesRead == 1)
		return *(unsigned char const *)(p);
	return (unsigned char)0;
}

unsigned short WPGXParser::readU16()
{
	unsigned short p0 = (unsigned short)readU8();
	unsigned short p1 = (unsigned short)readU8();
	return (unsigned short)(p0|(p1<<8));
}

unsigned int WPGXParser::readU32()
{
	unsigned int p0 = (unsigned int)readU8();
	unsigned int p1 = (unsigned int)readU8();
	unsigned int p2 = (unsigned int)readU8();
	unsigned int p3 = (unsigned int)readU8();
	return (unsigned long)(p0|(p1<<8)|(p2<<16)|(p3<<24));
}

short WPGXParser::readS16()
{
	return (short)readU16();
}

int WPGXParser::readS32()
{
	return (int)readU32();
}

unsigned int WPGXParser::readVariableLengthInteger()
{
	// read a byte
	unsigned char value8 = readU8();
	// if it's in the range 0-0xFE, then we have a 8-bit value
	if (value8<=0xFE)
	{
		return (unsigned int)value8;
	}
	else
	{
		// now read a 16 bit value
		unsigned short value16 = readU16();
		// if the MSB is 1, we have a 32 bit value
		if (value16>>15)
		{
			// read the next 16 bit value (LSB part, in value16 resides the MSB part)
			unsigned long lvalue16 = readU16();
			unsigned long value32 = value16 & 0x7fff;  // mask out the MSB
			return (value32<<16)+lvalue16;
		}
		else
		{
			// we have a 16 bit value, return it
			return (unsigned int)value16;
		}
	}
}

WPGXParser &WPGXParser::operator=(const WPGXParser &parser)
{
	if (this != &parser)
	{
		m_input = parser.m_input;
		m_painter = parser.m_painter;
		m_colorPalette = parser.m_colorPalette;
	}
	return *this;
}

void WPGTextDataHandler::endSubDocument()
{
	WPG_DEBUG_MSG(("WPGTextDataHandler::endSubDocument\n"));
}

void WPGTextDataHandler::openParagraph(const WPXPropertyList &propList, const WPXPropertyListVector & /* tabStops */)
{
	m_painter->startTextLine(propList);
}

void WPGTextDataHandler::closeParagraph()
{
	m_painter->endTextLine();
}

void WPGTextDataHandler::openSpan(const WPXPropertyList &propList)
{
	m_painter->startTextSpan(propList);
}

void WPGTextDataHandler::closeSpan()
{
	m_painter->endTextSpan();
}

void WPGTextDataHandler::insertTab()
{
	WPG_DEBUG_MSG(("WPGTextDataHandler::insertTab\n"));
}

void WPGTextDataHandler::insertSpace()
{
	m_painter->insertText(" ");
}

void WPGTextDataHandler::insertText(const WPXString &text)
{
	m_painter->insertText(text);
}

void WPGTextDataHandler::insertLineBreak()
{
	WPG_DEBUG_MSG(("WPGTextDataHandler::insertLineBreak\n"));
}

void WPGTextDataHandler::openListElement(const WPXPropertyList &propList, const WPXPropertyListVector &/*tabStops*/)
{
	m_painter->startTextLine(propList);
}

void WPGTextDataHandler::closeListElement()
{
	WPG_DEBUG_MSG(("WPGTextDataHandler::closeListElement\n"));
}
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
