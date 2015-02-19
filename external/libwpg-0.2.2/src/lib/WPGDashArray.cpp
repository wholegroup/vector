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

#include "WPGDashArray.h"

#include <vector>

namespace libwpg
{
class WPGDashArrayPrivate
{
public:
	WPGDashArrayPrivate() : dashes(), dots1(0), dots2(0),
		dots1len(0.0), dots2len(0.0), gap(0.0) {};
	void _recalculateDots();
	std::vector<double> dashes;
	int dots1;
	int dots2;
	double dots1len;
	double dots2len;
	double gap;
};
}

void libwpg::WPGDashArrayPrivate::_recalculateDots()
{
	dots1 = dots2 = 0,
	dots1len = dots2len = gap = 0.0;

	if (dashes.size() >= 2)
	{
		dots1len = dashes[0];
		gap = dashes[1];
	}

	unsigned count = dashes.size() / 2;
	unsigned i = 0;
	for (; i < count;)
	{
		if (dots1len == dashes[2*i])
			dots1++;
		else
			break;
		gap = gap < dashes[2*i+1] ?  dashes[2*i+1] : gap;
		i++;
	}
	if (i < count)
	{
		dots2len = dashes[2*i];
		gap = gap < dashes[2*i+1] ? dashes[2*i+1] : gap;
	}
	for (; i < count;)
	{
		if (dots2len == dashes[2*i])
			dots2++;
		else
			break;
		gap = gap < dashes[2*i+1] ? dashes[2*i+1] : gap;
		i++;
	}
	if (!dots2)
	{
		dots2 = dots1;
		dots2len = dots1len;
	}
}


libwpg::WPGDashArray::WPGDashArray() : d(new libwpg::WPGDashArrayPrivate())
{
	d->_recalculateDots();
}

libwpg::WPGDashArray::~WPGDashArray()
{
	delete d;
}

libwpg::WPGDashArray::WPGDashArray(const libwpg::WPGDashArray &dash):
	d(new libwpg::WPGDashArrayPrivate())
{
	d->dashes = dash.d->dashes;
	d->_recalculateDots();
}

libwpg::WPGDashArray &libwpg::WPGDashArray::operator=(const libwpg::WPGDashArray &dash)
{
	d->dashes = dash.d->dashes;
	d->_recalculateDots();
	return *this;
}

int libwpg::WPGDashArray::getDots1() const
{
	return d->dots1;
}

double libwpg::WPGDashArray::getDots1Length() const
{
	return d->dots1len;
}

int libwpg::WPGDashArray::getDots2() const
{
	return d->dots2;
}

double libwpg::WPGDashArray::getDots2Length() const
{
	return d->dots2len;
}

double libwpg::WPGDashArray::getDistance() const
{
	return d->gap;
}

void libwpg::WPGDashArray::add(double p)
{
	d->dashes.push_back(p);
	d->_recalculateDots();
}
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
