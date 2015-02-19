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

package com.wholegroup.vector.core.render

import org.apache.batik.transcoder.TranscoderException
import org.apache.batik.transcoder.TranscoderOutput
import spock.lang.Specification

import java.awt.image.BufferedImage

import spock.lang.Unroll
import java.awt.image.DataBuffer

class RGBATranscoderSpec extends Specification
{
	@Unroll
	def "createImage with #width x #height"()
	{
		when:
		BufferedImage bufferedImage = new RGBATranscoder().createImage(width, height)

		then:
		BufferedImage.TYPE_INT_ARGB == bufferedImage.getType()

		width  == bufferedImage.getWidth()
		height == bufferedImage.getHeight()

		where:
		width | height
		1     | 1
		123   | 124
		124   | 123
		1024  | 1024
	}

	def "writeImage with an unsupported type"()
	{
		given:
		BufferedImage bufferedImage = new BufferedImage(64, 64, BufferedImage.TYPE_INT_RGB)

		when:
		new RGBATranscoder().writeImage(bufferedImage, null)

		then:
		thrown(TranscoderException)
	}

	@Unroll
	def "writeImage with #width x #height"()
	{
		given:
		BufferedImage bufferedImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB)
		DataBuffer    dataBuffer    = bufferedImage.getData().getDataBuffer()

		for(int n = 0; n < width * height; n++)
		{
			dataBuffer.setElem(n, 0x01020408)
		}

		when:
		ByteArrayOutputStream outputStream = new ByteArrayOutputStream();

		new RGBATranscoder().writeImage(bufferedImage, new TranscoderOutput(outputStream));

		then: "size"
		width * height * 4 == outputStream.size()

		and: "identity"
		byte[] checkingBuffer = outputStream.toByteArray();
		int    pos            = 0;

		while(pos < checkingBuffer.size())
		{
			[8, 1, 2, 4] == [
				checkingBuffer[pos++],
				checkingBuffer[pos++],
				checkingBuffer[pos++],
				checkingBuffer[pos++]
			]
		}

		where:
		width | height
		1     | 1
		123   | 124
		124   | 123
		1024  | 1024
	}
}
