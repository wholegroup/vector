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

import org.apache.batik.dom.svg.SVGDOMImplementation
import org.apache.batik.dom.svg.SAXSVGDocumentFactory
import org.apache.batik.util.XMLResourceDescriptor

import org.w3c.dom.svg.SVGDocument

import spock.lang.Specification
import spock.lang.Unroll

import java.nio.channels.FileLock

class BaseRenderSpec extends Specification
{
	def "normalize if document is not created"()
	{
		when:
		new TestRender().normalize()

		then:
		thrown(IOException)
	}

	def "normalize if document is blank"()
	{
		given:
		SVGDocument document = (SVGDocument)SVGDOMImplementation.getDOMImplementation().createDocument(
			SVGDOMImplementation.SVG_NAMESPACE_URI,
			"svg",
			null
		)

		when:
		new TestRender(document).normalize()

		then: "Exception is called, because the bounds of the sensitive area can't be calculated."
		thrown(IOException)
	}

	@Unroll
	def "normalize with #width x #height"()
	{
		given: "Create a SVG document"
		SAXSVGDocumentFactory f = new SAXSVGDocumentFactory(
			XMLResourceDescriptor.getXMLParserClassName()
		);

		SVGDocument document = f.createSVGDocument(null,
			new ByteArrayInputStream(getSvg(startx, starty, width, height).toString().getBytes())
		);

		when:
		double ration = new TestRender((SVGDocument)document).normalize()

		then: "check the returned aspect ration"
		width / height == ration

		and: "check the calculated viewBox"
		"${startx} ${starty} ${width} ${height}" ==
			document.getRootElement().getAttribute("viewBox")

		and: "removed parameters"
		document.getRootElement().getAttribute("width").isEmpty()
		document.getRootElement().getAttribute("height").isEmpty()

		where:
		startx | starty | width | height
		0      | 0      | 1     | 1
		10     | 20     | 100   | 125
		20     | 10     | 150   | 100
	}

	def "render if the document is not created"()
	{
		when:
		new TestRender().render(100, 100, 0, 0, 100)

		then:
		thrown(IOException)
	}

	def "render if the document is not normalized"()
	{
		given:
		SAXSVGDocumentFactory f = new SAXSVGDocumentFactory(
			XMLResourceDescriptor.getXMLParserClassName()
		);

		SVGDocument document = f.createSVGDocument(null,
			new ByteArrayInputStream(getSvg(0, 0, 100, 100).toString().getBytes())
		);

		when:
		new TestRender((SVGDocument)document).render(100, 100, 0, 0, 100)

		then:
		thrown(IOException)
	}

	@Unroll
	def "render with #width x #height"()
	{
		given: "Create a SVG document"
		SAXSVGDocumentFactory f = new SAXSVGDocumentFactory(
			XMLResourceDescriptor.getXMLParserClassName()
		);

		SVGDocument document = f.createSVGDocument(null,
			new ByteArrayInputStream(getSvg(startx, starty, width, height).toString().getBytes())
		);

		and: "normalize the SVG document"
		TestRender testRender = new TestRender((SVGDocument)document)

		testRender.normalize()

		when:
		int side = width * 2

		byte[] image = testRender.render(width * 2, height * 2, -width, -height, side)

		then:
		def offset = 0

		(0..(side - 1)).each {y ->
			(0..(side - 1)).each {x ->
				if ((x < width) || (y < height))
				{
					assert 0 == image[offset++]
					assert 0 == image[offset++]
					assert 0 == image[offset++]
					assert 0 == image[offset++]
				}
				else
				{
					assert 63  == (int)(image[offset++] & 0xFF)
					assert 127 == (int)(image[offset++] & 0xFF)
					assert 255 == (int)(image[offset++] & 0xFF)
					assert 255 == (int)(image[offset++] & 0xFF)
				}
			}
		}

		where:
		startx | starty | width | height
		0      | 0      | 1     | 1
		10     | 20     | 100   | 125
		20     | 10     | 150   | 100
	}

	def "load if file is not found"()
	{
		when:
		new TestRender().loadFromFile(new File("/not.found").getAbsolutePath())

		then:
		thrown(FileNotFoundException)
	}

	def "load if the file has zero length"()
	{
		given: "create a file"
		File newFile = new File(getClass().getResource("/").getPath() + "zero.file")

		newFile.createNewFile()

		when:
		new TestRender().loadFromFile(newFile.getAbsolutePath())

		then:
		thrown(IOException)

		cleanup:
		newFile.delete()
	}

	def "load if file size is greater then maximum allowed size"()
	{
		given: "create a file"
		File newFile = new File(getClass().getResource("/").getPath() + "big.file")

		newFile.createNewFile()

		newFile << "0" * (BaseRender.MAX_FILE_SIZE + 1)

		when:
		new TestRender().loadFromFile(newFile.getAbsolutePath())

		then:
		thrown(IOException)

		cleanup:
		newFile.delete()
	}

	def "load if the file can't be read"()
	{
		given: "Open the file and lock"
		File exclusiveFile = new File(getClass().getResource("/tiger.svg").getPath())

		FileOutputStream fos = new FileOutputStream(exclusiveFile, true)
		FileLock fileLock = fos.channel.lock()

		when: "try to load the locked file"
		new TestRender().loadFromFile(exclusiveFile.getAbsolutePath())

		if (!System.properties['os.name'].toLowerCase().contains('win'))
		{
			throw new IOException("Locking is not working on linux/osx!")
		}

		then:
		thrown(IOException)

		cleanup:
		fileLock.release()
		fos.close()
	}

	def "load"()
	{
		given:
		File testFile = new File(getClass().getResource("/tiger.svg").getPath())

		when:
		TestRender testRender = new TestRender()

		testRender.loadFromFile(testFile.getAbsolutePath())

		then:
		testRender.getFileData() == testFile.getBytes()
	}

	def "load from zip"()
	{
		given:
		File testFile = new File(getClass().getResource("/svg.zip").getPath())

		when:
		TestRender testRender = new TestRender()

		testRender.loadFromFile(testFile.getAbsolutePath() + "!/SVG/tiger.svg")

		then:
		0 < testRender.getFileData().length
	}

	def "load from zip if not found"()
	{
		given:
		File testFile = new File(getClass().getResource("/svg.zip").getPath())

		when:
		TestRender testRender = new TestRender()

		testRender.loadFromFile(testFile.getAbsolutePath() + "!/SVG/notfound.svg")

		then:
		thrown(IOException)
	}

	def "loadFromByteArray null data"()
	{
		when:
		new TestRender().loadFromByteArray(null)

		then:
		thrown(IOException)
	}

	def "loadFromByteArray max data"()
	{
		given:
		TestRender testRender = new TestRender()

		when:
		testRender.loadFromByteArray(("0" * BaseRender.MAX_FILE_SIZE).bytes)

		then:
		null != testRender.getFileData()
	}

	def "loadFromByteArray big data"()
	{
		when:
		new TestRender().loadFromByteArray(("0" * (BaseRender.MAX_FILE_SIZE + 1)).bytes)

		then:
		thrown(IOException)
	}

	def "loadFromByteArray"()
	{
		given:
		String svg = getSvg(0, 0, 100, 100).toString()

		when:
		TestRender testRender = new TestRender()

		testRender.loadFromByteArray(svg.getBytes())

		then:
		testRender.getFileData() == svg.getBytes()
	}

	def getSvg = { startx, starty, width, height ->
		"""<?xml version="1.0" encoding="UTF-8"?>
		<svg xmlns:xlink="http://www.w3.org/1999/xlink" xmlns="http://www.w3.org/2000/svg" contentScriptType="text/ecmascript" contentStyleType="text/css" width="100" height="50" viewBox="1 2 3 4" version="1.0">
			<rect x="${startx}" y="${starty}" width="${width}" height="${height}" style="fill:rgb(63,127,255);" />
		</svg>
		"""
	}

	class TestRender extends BaseRender
	{
		TestRender()
		{
		}

		TestRender(SVGDocument document)
		{
			setDocument(document);
		}

		public byte[] getFileData()
		{
			super.getFileData()
		}

		public void create()
		{
		}
	}
}
