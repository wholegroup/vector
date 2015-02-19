/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* libwpg
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2007 Ariya Hidayat (ariya@kde.org)
 * Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
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

#ifndef __WPGBITMAP_H__
#define __WPGBITMAP_H__

#include "WPGColor.h"
#include <libwpd/libwpd.h>

namespace libwpg
{

class WPGBitmap
{
public:
	WPGBitmap(int width, int height, int verticalResolution, int horizontalResolution, bool verticalFlip=false, bool horizontalFlip=false);

	WPGBitmap(const WPGBitmap &);

	WPGBitmap &operator=(const WPGBitmap &);

	void copyFrom(const WPGBitmap &);

	~WPGBitmap();

	// return width in pixel
	int width() const;

	// return height in pixel
	int height() const;

	// return vertical resolution in pixels per inch
	int vres() const;

	// return horizontal resolution in pixels per inch
	int hres() const;

	void setPixel(int x, int y, const WPGColor &color);

	const ::WPXBinaryData &getDIB() const;

private:
	class Private;
	Private *const d;
};

} // namespace libwpg

#endif // __WPGBITMAP_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
