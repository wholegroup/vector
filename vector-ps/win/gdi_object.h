#pragma once

#ifndef VECTOR_PS_GDI_OBJECT_H
#define VECTOR_PS_GDI_OBJECT_H

// com.wholegroup.vector.ps
namespace com {namespace wholegroup {namespace vector {namespace ps
{
	// Deleter for the GdiObject type
	template<typename T>
	struct GdiObjectDeleter
	{
		// Delete the object.
		void operator()(T * gdiObject)
		{
			if (NULL != *gdiObject)
			{
				::DeleteObject(*gdiObject);
			}

			delete gdiObject;
		}
	};

	// Type safe wrapper for HGDIOBJ
	typedef ::std::unique_ptr<HBRUSH, GdiObjectDeleter<HBRUSH>>   BrushObject;
	typedef ::std::unique_ptr<HFONT, GdiObjectDeleter<HFONT>>     FontObject;
	typedef ::std::unique_ptr<HBITMAP, GdiObjectDeleter<HBITMAP>> BitmapObject;
	typedef ::std::unique_ptr<HCURSOR, GdiObjectDeleter<HCURSOR>> CursorObject;
}}}}

#endif // VECTOR_PS_GDI_OBJECT_H