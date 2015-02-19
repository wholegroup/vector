/*
 * Copyright (C) 2015 Andrey Rychkov <wholegroup@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package com.wholegroup.vector.core.render;

import org.apache.batik.dom.svg.SAXSVGDocumentFactory;
import org.apache.batik.util.XMLResourceDescriptor;
import org.w3c.dom.svg.SVGDocument;

import java.io.*;
import java.util.zip.*;

/**
 * The <tt>SvgFileRender</tt> class provides method for creating a SVG document
 * from a SVG file.
 */
public class SvgRender extends BaseRender
{
	/** The size of decompress buffer. */
	public final static int DECOMPRESS_BUFFER = 524288;

	/** The size of uncompressed data block. */
	public final static int UNCOMPRESSED_BLOCK = 8192;

	/**
	 * {@inheritDoc}
	 */
	public void create() throws IOException
	{
		// Check that the SVG file has been loaded.
		if (null == getFileData())
		{
			throw new IOException("The file was not loaded.");
		}

		// Create a data stream
		InputStream inputStream = null;

		if (isGzip(getFileData()))
		{
			inputStream = new GZIPInputStream(new ByteArrayInputStream(getFileData()));
		}
		else if (isZip(getFileData()))
		{
			ZipInputStream zip = new ZipInputStream(new BufferedInputStream(new ByteArrayInputStream(getFileData())));

			// Read the first file
			ZipEntry firstEntry = zip.getNextEntry();

			if (null != firstEntry)
			{
				// Check size of the uncompressed data
				long entrySize = firstEntry.getSize();

				if (0 > entrySize)
				{
					// If the size is unknown
					entrySize = DECOMPRESS_BUFFER;
				}

				// Check the size
				if (MAX_FILE_SIZE < entrySize)
				{
					throw new IOException("The file '" + firstEntry.getName() + "' is too large. The maximum file size is " + MAX_FILE_SIZE  / 1024 / 1024 + " MB.");
				}

				// Read the uncompressed data
				ByteArrayOutputStream decompressBuffer = new ByteArrayOutputStream((int)entrySize);

				byte[] buffer = new byte[UNCOMPRESSED_BLOCK];
				int    n;

				while (-1 < (n = zip.read(buffer, 0, UNCOMPRESSED_BLOCK)))
				{
					decompressBuffer.write(buffer, 0, n);

					// Double check size
					if (MAX_FILE_SIZE < decompressBuffer.size())
					{
						throw new IOException("The file '" + firstEntry.getName() + "' is too large. The maximum file size is " + MAX_FILE_SIZE  / 1024 / 1024 + " MB.");
					}
				}

				// Create a stream
				inputStream = new ByteArrayInputStream(decompressBuffer.toByteArray());
			}

			zip.close();
		}
		else
		{
			inputStream = new ByteArrayInputStream(getFileData());
		}

		// create a SVG document
		SAXSVGDocumentFactory f = new SAXSVGDocumentFactory(
			XMLResourceDescriptor.getXMLParserClassName()
		);

		SVGDocument document = f.createSVGDocument(null, inputStream);

		setDocument(document);
	}

	/**
	 * Determines if a byte array is compressed. The java.util.zip GZip
	 * implementaiton does not expose the GZip header so it is difficult to determine
	 * if a string is compressed.
	 *
	 * @param bytes An array of bytes.
	 *
	 * @return True if the array is compressed or false otherwise.
	 *
	 * @throws java.io.IOException If the byte array couldn't be read.
	 */
	private boolean isGzip(byte[] bytes) throws IOException
	{
		if (null == bytes)
		{
			return false;
		}

		if (2 > bytes.length)
		{
			return false;
		}

		return ((bytes[0] == (byte) (GZIPInputStream.GZIP_MAGIC)) &&
			(bytes[1] == (byte) (GZIPInputStream.GZIP_MAGIC >> 8))
		);
	}

	/**
	 *
	 * @param bytes An array of bytes.
	 *
	 * @return True if the array is compressed or false otherwise.
	 *
	 * @throws java.io.IOException If the byte array couldn't be read.
	 */
	private boolean isZip(byte[] bytes) throws IOException
	{
		if (null == bytes)
		{
			return false;
		}

		if (4 > bytes.length)
		{
			return false;
		}

		return ((bytes[0] == (byte) (ZipEntry.LOCSIG)) &&
			(bytes[1] == (byte) (ZipEntry.LOCSIG >> 8)) &&
			(bytes[2] == (byte) (ZipEntry.LOCSIG >> 16)) &&
			(bytes[3] == (byte) (ZipEntry.LOCSIG >> 24))
		);
	}
}
