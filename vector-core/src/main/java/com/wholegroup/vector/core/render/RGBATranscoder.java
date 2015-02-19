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

import org.apache.batik.transcoder.TranscoderException;
import org.apache.batik.transcoder.TranscoderOutput;
import org.apache.batik.transcoder.image.ImageTranscoder;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;

import java.io.OutputStream;
import java.io.IOException;

/**
 * This class is an <tt>ImageTranscoder</tt> that produces an array of ARGB bytes.
 */
public class RGBATranscoder extends ImageTranscoder
{
	/**
	 * {@inheritDoc}
	 */
	public BufferedImage createImage(int width, int height)
	{
		return new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
	}

	/**
	 * {@inheritDoc}
	 */
	public void writeImage(BufferedImage bufferedImage, TranscoderOutput transcoderOutput)
		throws TranscoderException
	{
		if (BufferedImage.TYPE_INT_ARGB != bufferedImage.getType())
		{
			throw new TranscoderException("An unsupported type of the buffered image.");
		}

		DataBufferInt dataBuffer   = (DataBufferInt)bufferedImage.getData().getDataBuffer();
		OutputStream  outputStream = transcoderOutput.getOutputStream();

		int    npixel = 0;
		byte[] line   = new byte[4 * bufferedImage.getWidth()];

		for (int y = 0; y < bufferedImage.getHeight(); y++)
		{
			for(int x = 0; x < 4 * bufferedImage.getWidth();)
			{
				final int pixel = dataBuffer.getData()[npixel++];

				line[x++] = (byte) (pixel >> 16);
				line[x++] = (byte) (pixel >> 8);
				line[x++] = (byte) (pixel);
				line[x++] = (byte) (pixel >> 24);
			}

			try
			{
				outputStream.write(line);
			} catch (IOException ex)
			{
				throw new TranscoderException(ex);
			}
		}
	}
}
