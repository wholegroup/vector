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
import spock.lang.Unroll

class WmfRenderSpec extends Specification
{
	def "create if data is not loaded"()
	{
		when:
		new WmfRender().create()

		then:
		thrown(IOException)
	}

	def "create if the loaded file is not valid"()
	{
		given:
		WmfRender wmfRender = new WmfRender()

		wmfRender.loadFromFile(new File(getClass().getResource("/tiger.svg").getPath()).getAbsolutePath())

		when:
		wmfRender.create()

		then:
		thrown(IOException)
	}

	def "create"()
	{
		given:
		TestWmfRender wmfRender = new TestWmfRender()

		wmfRender.loadFromFile(new File(getClass().getResource("/butterfly.wmf").getPath()).getAbsolutePath())

		when:
		wmfRender.create()

		then:
		null != wmfRender.getDocument()
	}

	@Unroll
	def "render #filename"()
	{
		given:
		TestWmfRender wmfRender = new TestWmfRender()

		wmfRender.loadFromFile(new File(getClass().getResource("/wmf.zip").getPath()).getAbsolutePath() + "!/" + filename)

		when:
		wmfRender.create()
		wmfRender.normalize()
		def image = wmfRender.render(128, 128, 0, 0, 128)
		wmfRender.close()

		then:
		null != image

		where:
		filename << ["DS2059.WMF", "F09609.WMF"]
	}

	class TestWmfRender extends WmfRender
	{
		public SVGDocument getDocument()
		{
			return super.getDocument();
		}
	}
}
