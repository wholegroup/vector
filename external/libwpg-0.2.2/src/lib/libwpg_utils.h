/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* libwpg
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
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

#ifndef __LIBWPG_UTILS_H__
#define __LIBWPG_UTILS_H__

#include <stdio.h>

//#define DEBUG // FIXME !

// debug message includes source file and line number
//#define VERBOSE_DEBUG 1

// do nothing with debug messages in a release compile
#ifdef DEBUG
#ifdef VERBOSE_DEBUG
#define WPG_DEBUG_MSG(M) printf("%15s:%5d: ", __FILE__, __LINE__); printf M
#define WPG_DEBUG(M) M
#else
#define WPG_DEBUG_MSG(M) printf M
#define WPG_DEBUG(M) M
#endif
#else
#define WPG_DEBUG_MSG(M)
#define WPG_DEBUG(M)
#endif

#endif // __LIBWPG_UTILS_H__
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
