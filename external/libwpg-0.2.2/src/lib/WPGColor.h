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

#ifndef __WPGCOLOR_H__
#define __WPGCOLOR_H__

#include <libwpd/libwpd.h>

namespace libwpg
{

class WPGColor
{
public:
	int red, green, blue, alpha;

	WPGColor();

	WPGColor(int r, int g, int b);

	WPGColor(int r, int g, int b, int a);

	WPGColor(const WPGColor &color);

	WPGColor &operator=(const WPGColor &color);

	const ::WPXString getColorString() const;

	double getOpacity() const;
};

} // namespace libwpg

#endif // __WPGCOLOR_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
