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
 * Copyright (C) 2006 Fridrich Strba (fridrich.strba@bluewin.ch)
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

#include "WPGInternalStream.h"

WPGInternalInputStream::WPGInternalInputStream(const unsigned char *data, unsigned long size) :
	WPXInputStream(),
	m_offset(0),
	m_size(size),
	m_data(data),
	m_tmpBuf(0)
{
}

WPGInternalInputStream::~WPGInternalInputStream()
{
	if (m_tmpBuf)
		delete [] m_tmpBuf;
}

const unsigned char *WPGInternalInputStream::read(unsigned long numBytes, unsigned long &numBytesRead)
{
	numBytesRead = 0;

	if (0 == numBytes)
		return 0;

	if (m_tmpBuf)
		delete [] m_tmpBuf;
	m_tmpBuf = 0;

	int numBytesToRead;

	if ((m_offset+numBytes) < m_size)
		numBytesToRead = numBytes;
	else
		numBytesToRead = m_size - m_offset;

	numBytesRead = numBytesToRead; // about as paranoid as we can be..

	if (numBytesToRead == 0)
		return 0;

	m_tmpBuf = new unsigned char[numBytesToRead];
	for (unsigned long i=0; (long)i<(long)numBytesToRead; i++)
	{
		m_tmpBuf[i] = m_data[m_offset];
		m_offset++;
	}

	return m_tmpBuf;
}

int WPGInternalInputStream::seek(long offset, WPX_SEEK_TYPE seekType)
{
	if (seekType == WPX_SEEK_CUR)
		m_offset += offset;
	else if (seekType == WPX_SEEK_SET)
		m_offset = offset;

	if (m_offset < 0)
	{
		m_offset = 0;
		return 1;
	}
	if ((long)m_offset > (long)m_size)
	{
		m_offset = m_size;
		return 1;
	}

	return 0;
}

long WPGInternalInputStream::tell()
{
	return m_offset;
}

bool WPGInternalInputStream::atEOS()
{
	if ((long)m_offset == (long)m_size)
		return true;

	return false;
}
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
