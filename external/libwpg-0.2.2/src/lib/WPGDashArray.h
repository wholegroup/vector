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

#ifndef __WPGDASHARRAY_H__
#define __WPGDASHARRAY_H__

namespace libwpg
{

class WPGDashArrayPrivate;

class WPGDashArray
{
public:
	WPGDashArray();
	~WPGDashArray();
	WPGDashArray(const WPGDashArray &);
	WPGDashArray &operator=(const WPGDashArray &);
	void add(double p);
	int getDots1() const;
	double getDots1Length() const;
	int getDots2() const;
	double getDots2Length() const;
	double getDistance() const;

private:
	WPGDashArrayPrivate *d;
};

} // namespace libwpg


#endif // __WPGDASHARRAY_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
