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

#include "stdafx.h"

#define GDIPVER 0x0110

#include <objidl.h>
#include <GdiPlus.h>

#pragma comment (lib, "GdiPlus.lib")

//
class EmfWmfTest
	: public ::testing::Test
{
};

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ::Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   ::Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (::Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   ::Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

TEST_F(EmfWmfTest, EMF2WMF)
{
	HDC hDc = GetDC(GetDesktopWindow());
	HDC hDcComp = CreateCompatibleDC(hDc);
	ReleaseDC(GetDesktopWindow(), hDc);
	SetMapMode(hDcComp,MM_ANISOTROPIC);
	HENHMETAFILE hEmf = GetEnhMetaFileW(L"D:\\in.emf");
	UINT uSize = GetWinMetaFileBits(hEmf, 0, NULL, MM_ANISOTROPIC, hDcComp);
	BYTE *pBuffer = (BYTE *) GlobalAlloc(GPTR, uSize);
	GetWinMetaFileBits(hEmf, uSize, pBuffer, MM_ANISOTROPIC, hDcComp);
	HMETAFILE hWmf = SetMetaFileBitsEx(uSize, pBuffer);
	PlayMetaFile(hDcComp, hWmf);
	HMETAFILE hWmfNew = CopyMetaFileW(hWmf, L"D:\\out.wmf");
	DeleteMetaFile(hWmfNew);
	DeleteMetaFile(hWmf);
	DeleteEnhMetaFile(hEmf);
	GlobalFree(pBuffer);
	DeleteDC(hDcComp);
}

TEST_F(EmfWmfTest, EMF2GDIP)
{
	::Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken1;
   ULONG_PTR           gdiplusToken2;
	auto a = ::Gdiplus::GdiplusStartup(&gdiplusToken1, &gdiplusStartupInput, NULL);
	auto b = ::Gdiplus::GdiplusStartup(&gdiplusToken2, &gdiplusStartupInput, NULL);

	{
		::Gdiplus::Metafile mf(L"D:\\Temp\\Test\\lion.emf");

		::Gdiplus::Bitmap bmp(2000, 2000, PixelFormat32bppARGB);

		{
			::Gdiplus::Graphics graphics (&bmp);

			graphics.SetSmoothingMode(::Gdiplus::SmoothingModeAntiAlias);

			mf.ConvertToEmfPlus(&graphics);
			graphics.DrawImage(&mf, 0, 0, 2000, 2000);
		}

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);

		bmp.Save(L"d:\\test2.png", &pngClsid, NULL);
	}

	::Gdiplus::GdiplusShutdown(gdiplusToken2);
	::Gdiplus::GdiplusShutdown(gdiplusToken1);
}
