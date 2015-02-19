/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* libwpd
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2002 William Lachance (wrlach@gmail.com)
 * Copyright (C) 2002 Marc Maurer (uwog@uwog.net)
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 *
 * For further information visit http://libwpd.sourceforge.net
 */

/* "This product is not manufactured, approved, or supported by
 * Corel Corporation or Corel Corporation Limited."
 */

#ifndef WP5TABLEEOLGROUP_H
#define WP5TABLEEOLGROUP_H

#include "WP5VariableLengthGroup.h"

class WP5TableEOLGroup : public WP5VariableLengthGroup
{
public:
	WP5TableEOLGroup(WPXInputStream *input, WPXEncryption *encryption);
	~WP5TableEOLGroup();
	void _readContents(WPXInputStream *input, WPXEncryption *encryption);
	void parse(WP5Listener *listener);

private:
	// Beginning column sub-function
	uint8_t m_cellVerticalAlignment;
	bool m_useCellAttributes;
	bool m_useCellJustification;
	uint8_t m_columnNumber;
	uint8_t m_colSpan;
	uint8_t m_rowSpan;
	bool m_spannedFromAbove;
	uint16_t m_cellAttributes;
	uint8_t m_cellJustification;
};

#endif /* WP5TABLEEOLGROUP_H */
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
