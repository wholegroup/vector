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

import org.w3c.dom.svg.SVGDocument
import spock.lang.Specification

class SvgRenderSpec extends Specification
{
	def "create if file is not loaded"()
	{
		when:
		new SvgRender().create()

		then:
		thrown(IOException)
	}

	def "create if the loaded file is not valid"()
	{
		given:
		SvgRender svgFileRender = new SvgRender()

		svgFileRender.loadFromFile(new File(getClass().getResource("/butterfly.wmf").getPath()).getAbsolutePath())

		when:
		svgFileRender.create()

		then:
		thrown(IOException)
	}

	def "create"()
	{
		given:
		TestSvgRender svgFileRender = new TestSvgRender()

		svgFileRender.loadFromFile(new File(getClass().getResource("/tiger.svg").getPath()).getAbsolutePath())

		when:
		svgFileRender.create()

		then:
		null != svgFileRender.getDocument()
	}

	def "create from string"()
	{
		given:
		TestSvgRender svgFileRender = new TestSvgRender()

		svgFileRender.loadFromFile(new File(getClass().getResource("/tiger.svg").getPath()).getAbsolutePath())

		when:
		svgFileRender.create()

		then:
		null != svgFileRender.getDocument()
	}

	def "create from a GZIP file"()
	{
		given:
		TestSvgRender svgFileRender = new TestSvgRender()

		svgFileRender.loadFromFile(new File(getClass().getResource("/tiger.svg.gz").getPath()).getAbsolutePath())

		when:
		svgFileRender.create()

		then:
		null != svgFileRender.getDocument()
	}

	def "create from a ZIP file"()
	{
		given:
		TestSvgRender svgFileRender = new TestSvgRender()

		svgFileRender.loadFromFile(new File(getClass().getResource("/tiger.svg.zip").getPath()).getAbsolutePath())

		when:
		svgFileRender.create()

		then:
		null != svgFileRender.getDocument()
	}

	def "create from a GZIP file in archive"()
	{
		given:
		TestSvgRender svgFileRender = new TestSvgRender()

		svgFileRender.loadFromFile(new File(getClass().getResource("/svg.zip").getPath()).getAbsolutePath() + "!/SVG/tiger.svg.gz")

		when:
		svgFileRender.create()

		then:
		null != svgFileRender.getDocument()
	}

	class TestSvgRender extends SvgRender
	{
		public SVGDocument getDocument()
		{
			return super.getDocument();
		}
	}
}
