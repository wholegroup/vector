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

/**
 * The <tt>Render</tt> interface provides creating and rendering SVG vector images.
 */
public interface Render
{
	/**
	 * Create a SVG document from the buffer.
	 *
	 * @throws java.io.IOException
	 */
	public void create() throws java.io.IOException;

	/**
	 * Normalize the created document.
	 *
	 * @return The aspect ratio of the normalized document.
	 *
	 * @throws java.io.IOException
	 */
	public double normalize() throws java.io.IOException;

	/**
	 * Render piece of the normalized SVG document to an output buffer.
	 *
	 * @param width  The total width of the final image.
	 * @param height The total height of the final image.
	 * @param left   The left position of the needed piece of the final image
	 * @param top    The top position of the needed piece of the final image
	 * @param side   The side of the square.
	 *
	 * @return The buffer with the rendered piece of the final image.
	 *
	 * @throws java.io.IOException
	 */
	public byte[] render(int width, int height, int left, int top, int side) throws java.io.IOException;

	/**
	 * Release everything used resources.
	 *
	 * @throws java.io.IOException
	 */
	public void close() throws java.io.IOException;
}
