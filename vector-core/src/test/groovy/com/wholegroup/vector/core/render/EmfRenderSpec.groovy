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

class EmfRenderSpec extends Specification
{
	def "loadFromFile doesn't work"()
	{
		given:
		EmfRender emfRender = new EmfRender()

		when:
		emfRender.loadFromFile(new File(getClass().getResource("/butterfly.wmf").getPath()).getAbsolutePath())

		then:
		thrown(IOException)
	}

	def "loadFromByteArray incorrect data"()
	{
		given:
		EmfRender emfRender = new EmfRender()

		emfRender.loadFromByteArray("18728926368356715762903849879".bytes)

		when:
		emfRender.create()

		then:
		thrown(IOException)
	}

	def "loadFromByteArray wmf data"()
	{
		given:
		TestEmfRender emfRender = new TestEmfRender()

		emfRender.loadFromByteArray(new File(getClass().getResource("/butterfly.wmf").getPath()).bytes)

		when:
		emfRender.create()

		then:
		null != emfRender.getDocument()
	}

	class TestEmfRender extends EmfRender
	{
		public SVGDocument getDocument()
		{
			return super.getDocument();
		}
	}
}