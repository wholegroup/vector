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

#ifndef __WPGPAINTINTERFACE_H__
#define __WPGPAINTINTERFACE_H__

#include <libwpd/libwpd.h>

namespace libwpg
{

class WPGPaintInterface
{
public:
	virtual ~WPGPaintInterface() {}

	// none of the other callback functions will be called before this function is called
	virtual void startGraphics(const ::WPXPropertyList &propList) = 0;

	// none of the other callback functions will be called after this function is called
	virtual void endGraphics() = 0;

	virtual void setStyle(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &gradient) = 0;

	virtual void startLayer(const ::WPXPropertyList &propList) = 0;

	virtual void endLayer() = 0;

	virtual void startEmbeddedGraphics(const ::WPXPropertyList &propList) = 0;

	virtual void endEmbeddedGraphics() = 0;

	// Different primitive shapes
	virtual void drawRectangle(const ::WPXPropertyList &propList) = 0;

	virtual void drawEllipse(const ::WPXPropertyList &propList) = 0;

	virtual void drawPolygon(const ::WPXPropertyListVector &vertices) = 0;

	virtual void drawPolyline(const ::WPXPropertyListVector &vertices) = 0;

	virtual void drawPath(const ::WPXPropertyListVector &path) = 0;

	// Embedded binary/raster data
	virtual void drawGraphicObject(const ::WPXPropertyList &propList, const ::WPXBinaryData &binaryData) = 0;

	// Embedded text object
	virtual void startTextObject(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path) = 0;
	virtual void endTextObject() = 0;

	virtual void startTextLine(const ::WPXPropertyList &propList) = 0;
	virtual void endTextLine() = 0;

	virtual void startTextSpan(const ::WPXPropertyList &propList) = 0;
	virtual void endTextSpan() = 0;

	virtual void insertText(const ::WPXString &str) = 0;
};

} // namespace libwpg

#endif // __WPGPAINTINTERFACE_H__

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
