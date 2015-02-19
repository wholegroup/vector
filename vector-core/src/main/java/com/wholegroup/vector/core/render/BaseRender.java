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

import org.apache.batik.transcoder.*;
import org.w3c.dom.svg.SVGDocument;
import org.apache.batik.bridge.BridgeContext;
import org.apache.batik.bridge.GVTBuilder;
import org.apache.batik.bridge.UserAgentAdapter;
import org.apache.batik.gvt.GraphicsNode;

import java.awt.geom.Rectangle2D;
import java.io.*;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

/**
 * The abstract <tt>BaseRender</tt> class provides common methods for process of rendering.
 */
abstract public class BaseRender implements ByteArrayRender, FileRender
{
	/** The created SVG document. */
	private SVGDocument document;

	/** The calculated bounds of the SVG document. */
	private Rectangle2D bounds;

	/** The output stream for process of rendering. */
	private ByteArrayOutputStream outputStream;

	/** The maximum vector file size is 16Mb. */
	public final static int MAX_FILE_SIZE = 16 * 1024 * 1024;

	/** The size of decompress buffer. */
	public final static int DECOMPRESS_BUFFER = 524288;

	/** The size of uncompressed data block. */
	public final static int UNCOMPRESSED_BLOCK = 8192;

	/** The data of a loaded file. */
	private byte[] fileData;

	/**
	 * Setter for a created document.
	 *
	 * @param document The created document.
	 */
	protected void setDocument(SVGDocument document)
	{
		this.document = document;
	}

	/**
	 * Getter for a created document.
	 *
	 * @return The created document.
	 */
	protected SVGDocument getDocument()
	{
		return this.document;
	}

	/**
	 * {@inheritDoc}
	 */
	public double normalize() throws IOException
	{
		if (null == this.document)
		{
			throw new IOException("The SVG document was not created.");
		}

		// Delete some attributes
		this.document.getRootElement().removeAttribute("width");
		this.document.getRootElement().removeAttribute("height");
		this.document.getRootElement().removeAttribute("viewBox");

		// Calculate the view box of the SVG document
		GraphicsNode gvtRoot = new GVTBuilder().build(
			new BridgeContext(new UserAgentAdapter()),
			this.document
		);

		this.bounds = gvtRoot.getSensitiveBounds();

		if (null == this.bounds)
		{
			throw new IOException("The bounds of the sensitive area can't be calculated.");
		}

		// Set the calculated view box into the SVG document
		StringBuilder viewBox = new StringBuilder();

		viewBox.append((int)this.bounds.getX());
		viewBox.append(" ");
		viewBox.append((int)this.bounds.getY());
		viewBox.append(" ");
		viewBox.append((int) this.bounds.getWidth());
		viewBox.append(" ");
		viewBox.append((int)this.bounds.getHeight());

		this.document.getRootElement().setAttribute("viewBox", viewBox.toString());

		// Return the aspect ratio
		return this.bounds.getWidth() / this.bounds.getHeight();
	}

	/**
	 * {@inheritDoc}
	 */
	public byte[] render(int width, int height, int left, int top, int side) throws IOException
	{
		// Check that the SVG document has been created.
		if (null == this.document)
		{
			throw new IOException("The SVG document wasn't created.");
		}

		// Check that the SVG document has been normalized.
		if (null == bounds)
		{
			throw new IOException("The SVG document wasn't normalized.");
		}

		// Create an ARGB Transcoder
		RGBATranscoder RGBATranscoder = new RGBATranscoder();

		RGBATranscoder.addTranscodingHint(SVGAbstractTranscoder.KEY_WIDTH, (float)side);
		RGBATranscoder.addTranscodingHint(SVGAbstractTranscoder.KEY_HEIGHT, (float)side);

		// Set area of interest.
		final double documentRatio = this.bounds.getWidth() / this.bounds.getHeight();
		final double imageRatio    = (double)(width / height);
		final double documentSide  = documentRatio < imageRatio ?
			this.bounds.getHeight() * side / height : this.bounds.getWidth() * side / width;

		RGBATranscoder.addTranscodingHint(SVGAbstractTranscoder.KEY_AOI,
			new Rectangle2D.Double(
				bounds.getX() + documentSide * left / side,
				bounds.getY() + documentSide * top / side,
				documentSide,
				documentSide
			)
		);

		// Create an output buffer if it is not exist
		if (null == this.outputStream)
		{
			outputStream = new ByteArrayOutputStream(side * side * 4);
		}

		outputStream.reset();

		// Rendering to the output buffer with the ARGB Transcoder
		try
		{
			RGBATranscoder.transcode(
				new TranscoderInput(this.document),
				new TranscoderOutput(outputStream)
			);
		}
		catch (TranscoderException e)
		{
			throw new IOException(e);
		}

		// Check size of the filled stream
		if ((side * side * 4) != outputStream.size())
		{
			throw new IOException("The rendered size is not equal to the expected.");
		}

		// Return the rendered byte array
		return outputStream.toByteArray();
	}

	/**
	 * {@inheritDoc}
	 */
	public void close()
	{
		this.outputStream = null;
		this.bounds       = null;
		this.document     = null;
		this.fileData     = null;
	}

	/**
	 * Getter for the data of a loaded file.
	 *
	 * @return The data.
	 */
	protected byte[] getFileData()
	{
		return fileData;
	}

	/**
	 * {@inheritDoc}
	 */
	public void loadFromFile(String pathFile) throws IOException
	{
		// Check is the pathFile a ZIP archive
		String[] fileParts = pathFile.split("!/", 2);

		// Check that it is a file
		File importedFile = new File(fileParts[0]);

		if (!importedFile.isFile())
		{
			throw new FileNotFoundException("The file '" + importedFile.getAbsolutePath() + "' doesn't exist.");
		}

		switch (fileParts.length)
		{
			case 1:
				rawLoad(importedFile);
				break;

			case 2:
				zipLoad(importedFile, fileParts[1]);
				break;

			default:
				throw new RuntimeException("An unknown error");
		}
	}

	/**
	 * Read data from a normal file.
	 *
	 * @param importedFile A normal file.
	 */
	private void rawLoad(File importedFile) throws IOException
	{
		FileInputStream fileInputStream = new FileInputStream(importedFile);

		try
		{
			// get the file size
			long fileLength = fileInputStream.getChannel().size();

			if (0 == fileLength)
			{
				throw new IOException("The file '" + importedFile.getAbsolutePath() + "' has zero length.");
			}

			// Check that the file size less the allowed max file size
			if (MAX_FILE_SIZE < importedFile.length())
			{
				throw new IOException("The file '" + importedFile.getAbsolutePath() + "' is too large. " +
					"The maximum file size is " + MAX_FILE_SIZE  / 1024 / 1024 + " MB.");
			}

			// Read the file into the data buffer
			this.fileData = new byte[(int)fileLength];

			int readBytes = fileInputStream.read(this.fileData);

			// Check number of readed bytes
			if (readBytes != fileLength)
			{
				throw new IOException("Number of readed bytes is not equal the file size.");
			}
		}
		finally
		{
			fileInputStream.close();
		}
	}

	/**
	 * Get data from a compressed file.
	 *
	 * @param zipFile      The zip file.
	 * @param importedFile The compressed filename.
	 *
	 * @throws IOException
	 */
	private void zipLoad(File zipFile, String importedFile) throws IOException
	{
		ZipFile zip = new ZipFile(zipFile);

		try
		{
			// Get the entry about the compressed file
			ZipEntry importedEntry = zip.getEntry(importedFile);

			if (null == importedEntry)
			{
				throw new FileNotFoundException("The specified filename '" + importedFile + "'  hasn't found in '" +
					zipFile.getAbsolutePath() + "'.");
			}

			// Check size of the uncompressed data
			long entrySize = importedEntry.getSize();

			if (0 > entrySize)
			{
				// If the size is unknown
				entrySize = DECOMPRESS_BUFFER;
			}

			// Check the size
			if (MAX_FILE_SIZE < entrySize)
			{
				throw new IOException("The specified filename '" + importedFile + "' in '" + zipFile.getAbsolutePath() +
					"' is too large. The maximum file size is " + MAX_FILE_SIZE  / 1024 / 1024 + " MB.");
			}

			// Read the uncompressed data
			InputStream importedEntryStream = zip.getInputStream(importedEntry);

			ByteArrayOutputStream decompressBuffer = new ByteArrayOutputStream((int)entrySize);

			byte[] buffer = new byte[UNCOMPRESSED_BLOCK];
			int    n;

			while (-1 < (n = importedEntryStream.read(buffer, 0, UNCOMPRESSED_BLOCK)))
			{
				decompressBuffer.write(buffer, 0, n);

				// Double check size
				if (MAX_FILE_SIZE < decompressBuffer.size())
				{
					throw new IOException("The specified filename '" + importedFile + "' in '" + zipFile.getAbsolutePath() + "' is too large. The maximum file size is " + MAX_FILE_SIZE  / 1024 / 1024 + " MB.");
				}
			}

			this.fileData = decompressBuffer.toByteArray();

		} finally
		{
			zip.close();
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public void loadFromByteArray(byte[] data) throws IOException
	{
		if (null == data)
		{
			throw new IOException("The data is null.");
		}

		if (MAX_FILE_SIZE < data.length)
		{
			throw new IOException("The data  is too large. The maximum size is " +
				MAX_FILE_SIZE  / 1024 / 1024 + " MB.");
		}

		this.fileData = data.clone();
	}
}
