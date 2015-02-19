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

#include "WPGBitmap.h"
#include "libwpg_utils.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define DUMP_BITMAP 0

#if DUMP_BITMAP
static unsigned bitmapId = 0;
#include <sstream>
#endif

namespace
{
void writeU16(unsigned char *buffer, unsigned &position, const int value)
{
	buffer[position++] = (unsigned char)(value & 0xFF);
	buffer[position++] = (unsigned char)((value >> 8) & 0xFF);
}

void writeU32(unsigned char *buffer, unsigned &position, const int value)
{
	buffer[position++] = (unsigned char)(value & 0xFF);
	buffer[position++] = (unsigned char)((value >> 8) & 0xFF);
	buffer[position++] = (unsigned char)((value >> 16) & 0xFF);
	buffer[position++] = (unsigned char)((value >> 24) & 0xFF);
}

void writeU8(unsigned char *buffer, unsigned &position, const int value)
{
	buffer[position++] = (unsigned char)(value & 0xFF);
}

}

class libwpg::WPGBitmap::Private
{
public:
	int width;
	int height;
	int vRes;
	int hRes;
	bool vFlip;
	bool hFlip;
	WPGColor *pixels;
	::WPXBinaryData dib;

	Private(int w, int h): width(w), height(h), vRes(72), hRes(72), vFlip(false), hFlip(false), pixels(0), dib() {}
private:
	// Unimplemented to prevent compiler from creating crasher ones
	Private(const Private &);
	Private &operator=(const Private &);
};

libwpg::WPGBitmap::WPGBitmap(int w, int h, int verticalResolution, int horizontalResolution, bool verticalFlip, bool horizontalFlip) :
	d(new Private(w, h))
{
	d->vRes = verticalResolution;
	d->hRes = horizontalResolution;
	d->vFlip = verticalFlip;
	d->hFlip = horizontalFlip;
	d->pixels = new WPGColor[w*h];
}

libwpg::WPGBitmap::~WPGBitmap()
{
	if (d)
	{
		if (d->pixels)
			delete [] d->pixels;
		delete d;
	}
}

libwpg::WPGBitmap::WPGBitmap(const WPGBitmap &bitmap): d(new Private(0,0))
{
	copyFrom(bitmap);
}

libwpg::WPGBitmap &libwpg::WPGBitmap::operator=(const WPGBitmap &bitmap)
{
	copyFrom(bitmap);
	return *this;
}

void libwpg::WPGBitmap::copyFrom(const WPGBitmap &bitmap)
{
	d->width = bitmap.d->width;
	d->height = bitmap.d->height;
	delete [] d->pixels;
	d->pixels = new WPGColor[d->width*d->height];
	for(int i=0; i < d->width*d->height; i++)
		d->pixels[i] = bitmap.d->pixels[i];
}

int libwpg::WPGBitmap::width() const
{
	return d->width;
}

int libwpg::WPGBitmap::height() const
{
	return d->height;
}

int libwpg::WPGBitmap::vres() const
{
	return d->vRes;
}

int libwpg::WPGBitmap::hres() const
{
	return d->hRes;
}

void libwpg::WPGBitmap::setPixel(int x, int y, const libwpg::WPGColor &color)
{
	if((x < 0) || (y <0) || (x >= d->width) || (y >= d->height))
		return;

	d->pixels[y *d->width + x] = color;
}

const ::WPXBinaryData &libwpg::WPGBitmap::getDIB() const
{
	if (d->dib.size() || d->height <= 0 || d->width <= 0)
		return d->dib;

#ifndef OUTPUT_DUMMY_BITMAPS
	unsigned tmpPixelSize = (unsigned)(d->height * d->width);
	if (tmpPixelSize < (unsigned)d->height) // overflow
		return d->dib;
#else
	unsigned tmpPixelSize = 1;
#endif

	unsigned tmpBufferPosition = 0;

	unsigned tmpDIBImageSize = tmpPixelSize * 4;
	if (tmpPixelSize > tmpDIBImageSize) // overflow !!!
		return d->dib;

	unsigned tmpDIBOffsetBits = 14 + 40;
	unsigned tmpDIBFileSize = tmpDIBOffsetBits + tmpDIBImageSize;
	if (tmpDIBImageSize > tmpDIBFileSize) // overflow !!!
		return d->dib;

	unsigned char *tmpDIBBuffer = new unsigned char[tmpDIBFileSize];

	// Create DIB file header
	writeU16(tmpDIBBuffer, tmpBufferPosition, 0x4D42);  // Type
	writeU32(tmpDIBBuffer, tmpBufferPosition, tmpDIBFileSize); // Size
	writeU16(tmpDIBBuffer, tmpBufferPosition, 0); // Reserved1
	writeU16(tmpDIBBuffer, tmpBufferPosition, 0); // Reserved2
	writeU32(tmpDIBBuffer, tmpBufferPosition, tmpDIBOffsetBits); // OffsetBits

	WPG_DEBUG_MSG(("WPGBitmap: DIB file header end = %i\n", tmpBufferPosition - 1));

	// Create DIB Info header
	writeU32(tmpDIBBuffer, tmpBufferPosition, 40); // Size
#ifndef OUTPUT_DUMMY_BITMAPS
	writeU32(tmpDIBBuffer, tmpBufferPosition, width());  // Width
	writeU32(tmpDIBBuffer, tmpBufferPosition, height()); // Height
#else
	writeU32(tmpDIBBuffer, tmpBufferPosition, 1);  // Width
	writeU32(tmpDIBBuffer, tmpBufferPosition, 1); // Height
#endif
	writeU16(tmpDIBBuffer, tmpBufferPosition, 1); // Planes
	writeU16(tmpDIBBuffer, tmpBufferPosition, 32); // BitCount
	writeU32(tmpDIBBuffer, tmpBufferPosition, 0); // Compression
	writeU32(tmpDIBBuffer, tmpBufferPosition, tmpDIBImageSize); // SizeImage
	writeU32(tmpDIBBuffer, tmpBufferPosition, (int)(hres()*100.0/2.54)); // XPelsPerMeter
	writeU32(tmpDIBBuffer, tmpBufferPosition, (int)(vres()*100.0/2.54)); // YPelsPerMeter
	writeU32(tmpDIBBuffer, tmpBufferPosition, 0); // ColorsUsed
	writeU32(tmpDIBBuffer, tmpBufferPosition, 0); // ColorsImportant

	WPG_DEBUG_MSG(("WPGBitmap: DIB info header end = %i\n", tmpBufferPosition - 1));

	// Write DIB Image data

#ifndef OUTPUT_DUMMY_BITMAPS
	int i = 0;
	int j = 0;
	if (d->vFlip)
		for	(i = 0; i < d->height && tmpBufferPosition < tmpDIBFileSize; i++)
		{
			if (d->hFlip)
				for (j = d->width - 1; j >= 0 && tmpBufferPosition < tmpDIBFileSize; j--)
				{
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].blue);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].green);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].red);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].alpha);
				}
			else
				for (j = 0; j < d->width && tmpBufferPosition < tmpDIBFileSize; j++)
				{
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].blue);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].green);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].red);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].alpha);
				}
		}
	else
		for (i = d->height - 1; i >= 0 && tmpBufferPosition < tmpDIBFileSize; i--)
		{
			if (d->hFlip)
				for (j = d->width - 1; j >= 0 && tmpBufferPosition < tmpDIBFileSize; j--)
				{
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].blue);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].green);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].red);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].alpha);
				}
			else
				for (j = 0; j < d->width && tmpBufferPosition < tmpDIBFileSize; j++)
				{
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].blue);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].green);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].red);
					writeU8(tmpDIBBuffer, tmpBufferPosition, d->pixels[i*d->width + j].alpha);
				}
		}

	WPG_DEBUG_MSG(("WPGBitmap: DIB file size = %i\n", tmpBufferPosition - 1));

#else
	writeU8(tmpDIBBuffer, tmpBufferPosition, 0x7F);
	writeU8(tmpDIBBuffer, tmpBufferPosition, 0x7F);
	writeU8(tmpDIBBuffer, tmpBufferPosition, 0x7F);
	writeU8(tmpDIBBuffer, tmpBufferPosition, 0x7F);
#endif

	d->dib.append(tmpDIBBuffer, tmpDIBFileSize);

	// temporary for debug - dump the binary bmp (need to have write access in the current directory
#if DUMP_BITMAP
	std::ostringstream filename;
	filename << "binarydump" << bitmapId++ << ".bmp";
	FILE *f = fopen(filename.str().c_str(), "wb");
	if (f)
	{
		for (unsigned k = 0; k < tmpDIBFileSize; k++)
			fprintf(f, "%c",tmpDIBBuffer[k]);
		fclose(f);
	}
#endif

	// Cleanup things before returning
	delete [] tmpDIBBuffer;

	return d->dib;
}
/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
