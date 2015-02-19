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

#ifndef __WPGXPARSER_H__
#define __WPGXPARSER_H__

#include "WPGPaintInterface.h"
#include <libwpd-stream/WPXStream.h>
#include <libwpd/libwpd.h>
#include "WPGColor.h"

#include <map>

class WPGXParser
{
public:
	WPGXParser(WPXInputStream *input, libwpg::WPGPaintInterface *painter);
	WPGXParser(const WPGXParser &parser);
	virtual ~WPGXParser() {};
	virtual bool parse() = 0;

	unsigned char readU8();
	unsigned short readU16();
	unsigned int readU32();
	short readS16();
	int readS32();
	unsigned int readVariableLengthInteger();
	WPGXParser &operator=(const WPGXParser &parser);

protected:
	WPXInputStream *m_input;
	libwpg::WPGPaintInterface *m_painter;
	std::map<int,libwpg::WPGColor> m_colorPalette;
};

class WPGTextDataHandler : public ::WPXDocumentInterface
{
public:
	WPGTextDataHandler(libwpg::WPGPaintInterface *painter) :
		m_painter(painter),
		m_fontName("Times New Roman"),
		m_paragraphStyle(),
		m_textStyle() {}

	~WPGTextDataHandler() {}
	void setDocumentMetaData(const WPXPropertyList & /* propList */) {}

	void startDocument() {}
	void endDocument() {}

	void startSubDocument() {}
	void endSubDocument();

	void definePageStyle(const WPXPropertyList & /* propList */) {}
	void openPageSpan(const WPXPropertyList & /* propList */) {}
	void closePageSpan() {}
	void openHeader(const WPXPropertyList & /* propList */) {}
	void closeHeader() {}
	void openFooter(const WPXPropertyList & /* propList */) {}
	void closeFooter() {}

	void defineParagraphStyle(const WPXPropertyList & /* propList */, const WPXPropertyListVector & /* tabStops */) {}
	void openParagraph(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops);
	void closeParagraph();

	void defineCharacterStyle(const WPXPropertyList & /* propList */) {}
	void openSpan(const WPXPropertyList &propList);
	void closeSpan();

	void defineSectionStyle(const WPXPropertyList & /* propList */, const WPXPropertyListVector & /* columns */) {}
	void openSection(const WPXPropertyList & /* propList */, const WPXPropertyListVector & /* columns */) {}
	void closeSection() {}

	void insertTab();
	void insertSpace();
	void insertText(const WPXString &text);
	void insertLineBreak();
	void insertField(const WPXString & /* type */, const WPXPropertyList & /* propList */) {}

	void defineOrderedListLevel(const WPXPropertyList & /* propList */) {}
	void defineUnorderedListLevel(const WPXPropertyList & /* propList */) {}
	void openOrderedListLevel(const WPXPropertyList & /* propList */) {}
	void openUnorderedListLevel(const WPXPropertyList & /* propList */) {}
	void closeOrderedListLevel() {}
	void closeUnorderedListLevel() {}
	void openListElement(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops);
	void closeListElement();

	void openFootnote(const WPXPropertyList & /* propList */) {}
	void closeFootnote() {}
	void openEndnote(const WPXPropertyList & /* propList */) {}
	void closeEndnote() {}
	void openComment(const WPXPropertyList & /* propList */) {}
	void closeComment() {}
	void openTextBox(const WPXPropertyList & /* propList */) {}
	void closeTextBox() {}

	void openTable(const WPXPropertyList & /* propList */, const WPXPropertyListVector & /* columns */) {}
	void openTableRow(const WPXPropertyList & /* propList */) {}
	void closeTableRow() {}
	void openTableCell(const WPXPropertyList & /* propList */) {}
	void closeTableCell() {}
	void insertCoveredTableCell(const WPXPropertyList & /* propList */) {}
	void closeTable() {}

	void openFrame(const WPXPropertyList & /* propList */) {}
	void closeFrame() {}

	void insertBinaryObject(const WPXPropertyList & /* propList */, const WPXBinaryData & /* data */) {}
	void insertEquation(const WPXPropertyList & /* propList */, const WPXString & /* data */) {}

private:
	libwpg::WPGPaintInterface *m_painter;
	::WPXString m_fontName;
	::WPXPropertyList m_paragraphStyle, m_textStyle;
	// Unimplemented to prevent compiler from creating crasher ones
	WPGTextDataHandler(const WPGTextDataHandler &);
	WPGTextDataHandler &operator=(const WPGTextDataHandler &);
};


#endif // __WPGXPARSER_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
