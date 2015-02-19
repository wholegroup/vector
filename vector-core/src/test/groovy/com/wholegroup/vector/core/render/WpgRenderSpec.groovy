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

class WpgRenderSpec extends Specification
{
	def "loadFromFile doesn't work"()
	{
		given:
		WpgRender wpgRender = new WpgRender()

		when:
		wpgRender.loadFromFile(new File(getClass().getResource("/tiger.svg").getPath()).getAbsolutePath())

		then:
		thrown(IOException)
	}

	def "loadFromByteArray incorrect data"()
	{
		given:
		WpgRender wpgRender = new WpgRender()

		wpgRender.loadFromByteArray("incorrect data".bytes)

		when:
		wpgRender.create()

		then:
		thrown(IOException)
	}

	def "loadFromByteArray svg data"()
	{
		given:
		TestWpgRender wpgRender = new TestWpgRender()

		wpgRender.loadFromByteArray("""
<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<rect x="10" y="10" height="100" width="100" style="stroke:#ff0000; fill: #0000ff"/></svg>""".bytes)

		when:
		wpgRender.create()

		then:
		null != wpgRender.getDocument()
	}

	class TestWpgRender extends WpgRender
	{
		public SVGDocument getDocument()
		{
			return super.getDocument();
		}
	}
}