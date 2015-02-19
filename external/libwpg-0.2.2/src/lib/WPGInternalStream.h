/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* libwpg
 * Version: MPL 2.0 / LGPLv2.1+
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2004-2005 William Lachance (wrlach@gmail.com)
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

#ifndef WPGINTERNALSTREAM_H
#define WPGINTERNALSTREAM_H
#include <libwpd-stream/WPXStream.h>

class WPGInternalInputStream : public WPXInputStream
{
public:
	WPGInternalInputStream(const unsigned char *data, unsigned long size);
	virtual ~WPGInternalInputStream();

	virtual bool isOLEStream()
	{
		return false;
	}
	virtual WPXInputStream *getDocumentOLEStream(const char * /*name*/)
	{
		return 0;
	}

	const virtual unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
	virtual int seek(long offset, WPX_SEEK_TYPE seekType);
	virtual long tell();
	virtual bool atEOS();

private:
	long m_offset;
	unsigned long m_size;
	const unsigned char *m_data;
	unsigned char *m_tmpBuf;
	WPGInternalInputStream(const WPGInternalInputStream &);
	WPGInternalInputStream &operator=(const WPGInternalInputStream &);
};

#endif
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
