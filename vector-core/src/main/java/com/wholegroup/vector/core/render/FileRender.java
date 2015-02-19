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
 * The <tt>FileRender</tt> interface provides loading vector files.
 */
public interface FileRender extends Render
{
	/**
	 * Open a file and save into a buffer.
	 *
	 * @param pathFile The path to a file.
	 *
	 * @throws java.io.IOException
	 */
	public void loadFromFile(String pathFile) throws java.io.IOException;
}
