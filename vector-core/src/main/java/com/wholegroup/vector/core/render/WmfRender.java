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

import net.arnx.wmf2svg.gdi.svg.SvgGdi;
import net.arnx.wmf2svg.gdi.svg.SvgGdiException;
import net.arnx.wmf2svg.gdi.wmf.WmfParseException;
import net.arnx.wmf2svg.gdi.wmf.WmfParser;
import org.apache.batik.dom.svg.SAXSVGDocumentFactory;
import org.apache.batik.dom.svg.SVGOMTransform;
import org.apache.batik.util.XMLResourceDescriptor;
import org.w3c.dom.NodeList;
import org.w3c.dom.svg.SVGDocument;
import org.w3c.dom.svg.SVGImageElement;

import javax.xml.transform.*;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.*;

/**
 * The <tt>WmfFileRender</tt> class provides method for creating a SVG document
 * from a WMF file.
 */
public class WmfRender extends BaseRender
{
	/**
	 * {@inheritDoc}
	 */
	public void create() throws IOException
	{
		// Check that the WMF file has been loaded.
		if (null == getFileData())
		{
			throw new IOException("The file was not loaded.");
		}

		// Create a SVG from the data of WMF file
		final ByteArrayOutputStream convertedSvg = new ByteArrayOutputStream();

		try
		{
			// Parse the data
			final SvgGdi gdi = new SvgGdi(false);

			new WmfParser().parse(new ByteArrayInputStream(getFileData()), gdi);

			// transform
			Transformer transformer = TransformerFactory.newInstance().newTransformer();

			transformer.setOutputProperty(OutputKeys.METHOD, "xml");
			transformer.setOutputProperty(OutputKeys.ENCODING, "UTF-8");
			transformer.setOutputProperty(OutputKeys.INDENT, "yes");
			transformer.setOutputProperty(OutputKeys.DOCTYPE_PUBLIC,
				"-//W3C//DTD SVG 1.0//EN");
			transformer.setOutputProperty(OutputKeys.DOCTYPE_SYSTEM,
				"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd");

			transformer.transform(
				new DOMSource(gdi.getDocument()),
				new StreamResult(convertedSvg)
			);

			convertedSvg.flush();
			convertedSvg.close();
		} catch (SvgGdiException e)
		{
			throw new IOException(e);
		} catch (WmfParseException e)
		{
			throw new IOException(e);
		} catch (TransformerException e)
		{
			throw new IOException(e);
		}

		// Create a SVG document
		SAXSVGDocumentFactory f = new SAXSVGDocumentFactory(
			XMLResourceDescriptor.getXMLParserClassName()
		);

		SVGDocument document = f.createSVGDocument(null,
			new ByteArrayInputStream(convertedSvg.toByteArray())
		);

		setDocument(document);
	}

	/**
	 * {@inheritDoc}
	 */
	public double normalize() throws IOException
	{
		fixImageBug(getDocument());

		return super.normalize();
	}

	/**
	 * To fix bug in wmf2svg.
	 * (image width/height can not be negative).
	 *
	 * @param document An unfixed document.
	 */
	public void fixImageBug(SVGDocument document)
	{
		NodeList imageNodes = document.getElementsByTagName("image");

		for (int i = 0; i < imageNodes.getLength(); i++)
		{
			SVGImageElement image = (SVGImageElement)imageNodes.item(i);

			if (image.hasAttribute("height"))
			{
				String height = image.getAttribute("height").trim();

				if (height.startsWith("-"))
				{
					image.setAttribute("height", height.substring(1));
					image.setAttribute("y", String.valueOf(-image.getY().getBaseVal().getValue()));

					// Flip
					SVGOMTransform trans = new SVGOMTransform();

					trans.setMatrix(trans.getMatrix().flipY());

					image.getTransform().getBaseVal().appendItem(trans);
				}
			}

			if (image.hasAttribute("width"))
			{
				String width = image.getAttribute("width").trim();

				if (width.startsWith("-"))
				{
					image.setAttribute("width", width.substring(1));
					image.setAttribute("x", String.valueOf(-image.getX().getBaseVal().getValue()));

					// Flip
					SVGOMTransform trans  = new SVGOMTransform();

					trans.setMatrix(trans.getMatrix().flipX());

					image.getTransform().getBaseVal().appendItem(trans);
				}
			}
		}
	}
}