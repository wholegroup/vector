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
#include "jvm.h"
#include "base_render.h"
#include "emf_render.h"

namespace {

	typedef ::Gdiplus::GpStatus (WINGDIPAPI * GdipConvertToEmfPlusPtr)(
		const ::Gdiplus::GpGraphics* refGraphics, 
		::Gdiplus::GpMetafile* metafile, 
		BOOL* conversionSuccess, 
		::Gdiplus::EmfType emfType, 
		const WCHAR* description, 
		::Gdiplus::GpMetafile** out_metafile);

	//
	static GdipConvertToEmfPlusPtr convertToEmfPlusPtr = nullptr;

	class MyGraphics : public ::Gdiplus::Graphics
	{
		public:

			MyGraphics(::Gdiplus::Bitmap * bmpRef)
				: ::Gdiplus::Graphics(bmpRef)
			{
			}

			::Gdiplus::GpGraphics * GetNativeGraphics() const
			{
				return this->nativeGraphics;
			}
	};

	class MyMetafile : public ::Gdiplus::Metafile
	{
		public:

			MyMetafile(HENHMETAFILE hEmf)
				: ::Gdiplus::Metafile(hEmf)
			{
			}

			::Gdiplus::Status ConvertToEmfPlus(IN const MyGraphics * refGraphics)
			{
				::Gdiplus::GpMetafile * metafile = nullptr;

				::Gdiplus::GpStatus status = convertToEmfPlusPtr(
					refGraphics->GetNativeGraphics(),
					(::Gdiplus::GpMetafile *)this->nativeImage,
					nullptr,
					::Gdiplus::EmfTypeEmfPlusOnly, 
					nullptr, 
					&metafile);

				if (metafile != NULL)
				{
					if (status == ::Gdiplus::Ok)
					{
						::Gdiplus::DllExports::GdipDisposeImage(nativeImage);

						SetNativeImage(metafile);
					}
					else
					{
						::Gdiplus::DllExports::GdipDisposeImage(metafile);
					}
				}

				return status;
			}
	};
};

using namespace ::com::wholegroup::vector::jni;

EmfRender::EmfRender(Jvm* jvm)
	: BaseRender(jvm, L"Emf"), gdipToken(0)
{
}

// Destructor.
EmfRender::~EmfRender()
{
	this->emfplus.reset();

	if (0 != this->gdipToken)
	{
		::Gdiplus::GdiplusShutdown(gdipToken);
	}
}

// Initialize this render.
void EmfRender::init()
{
	BOOST_ASSERT_MSG(!isInit(), "The render has been already initialized.");

	//
	::Gdiplus::GdiplusStartupInput gdipStartupInput;

	if (::Gdiplus::Ok != ::Gdiplus::GdiplusStartup(&gdipToken, &gdipStartupInput, nullptr))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error starting gdi+."));
	}

	//
	if (!convertToEmfPlusPtr)
	{
		HMODULE gpModule;
		
		::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCWSTR)&::Gdiplus::GdiplusStartup, &gpModule);

		if (!gpModule)
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An error getting gdi+ handle."));
		}

		convertToEmfPlusPtr = (GdipConvertToEmfPlusPtr)GetProcAddress(gpModule, "GdipConvertToEmfPlus");

		if (!convertToEmfPlusPtr)
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An error getting pointer to GdipConvertToEmfPlus."));
		}
	}
}

// Check that the render is initialized.
bool EmfRender::isInit() const
{
	return (0 != this->gdipToken);
}

// Load the file by the specified path.
void EmfRender::load(const ::std::wstring & pathFile)
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	//
	if (!::boost::filesystem::exists(pathFile) || 
		!::boost::filesystem::is_regular_file(pathFile))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"File doesn't exist."));
	}

	//
	uintmax_t fileSize = ::boost::filesystem::file_size(pathFile);

	if (fileSize > (1024 * 1024 * 30))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"File is too big."));
	}

	this->fileData.resize(static_cast<size_t>(fileSize), 0);

	//
	::std::ifstream emfFile(pathFile, std::ios::binary);

	if (!emfFile.is_open())
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"File can't be opened."));
	}

	this->fileData.assign(::std::istreambuf_iterator<char>(emfFile), 
		::std::istreambuf_iterator<char>());
}

// Load the file by the specified path.
void EmfRender::loadFromByteArray(int const length, char const * const data)
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
		L"Unsupported"));
}

// Create a document.
void EmfRender::create()
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");
	BOOST_ASSERT_MSG(this->fileData.size(), "The file data is empty.");

	// create
	::boost::shared_ptr<HENHMETAFILE__> emf(::SetEnhMetaFileBits(static_cast<UINT>(this->fileData.size()), 
		&this->fileData[0]), [](HENHMETAFILE hEmf){DeleteEnhMetaFile(hEmf);});

	if (!emf)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error creating metafile."));
	}

	this->emfplus.reset(new MyMetafile(emf.get()));

	// convert
	::Gdiplus::Bitmap bmp(1, 1, PixelFormat32bppARGB);
	MyGraphics graphics(&bmp);

	graphics.SetSmoothingMode(::Gdiplus::SmoothingModeAntiAlias8x8);
	graphics.SetTextRenderingHint(::Gdiplus::TextRenderingHintAntiAlias);

	if (::Gdiplus::Ok != ((MyMetafile *)this->emfplus.get())->ConvertToEmfPlus(&graphics))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error converting metafile."));
	}
}

// Normalize the document.
double const EmfRender::normalize()
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	return static_cast<double>((*this->emfplus).GetWidth()) / (*this->emfplus).GetHeight();
}

// Render a piece of the document to the specified buffer.
void EmfRender::render(int width, int height, int left, int top, int side, unsigned char* buffer)
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");
	BOOST_ASSERT_MSG(0 < width, "The width should be greater then zero.");
	BOOST_ASSERT_MSG(0 < height, "The height should be greater then zero.");
	BOOST_ASSERT_MSG(0 <= left, "The left position should be greater then or equal zero.");
	BOOST_ASSERT_MSG(0 <= top, "The top position should be greater then or equal zero.");
	BOOST_ASSERT_MSG(0 < side, "The side should be greater then zero.");
	BOOST_ASSERT_MSG(nullptr != buffer, "The buffer is null.");

	//
	if (this->quad.size() != (side * side * 4))
	{
		this->quad.resize(side * side * 4, 0);
	}
	else
	{
		std::fill(this->quad.begin(), this->quad.end(), 0);
	}

	//
	::Gdiplus::Bitmap bmp(side, side, 4 * side, PixelFormat32bppARGB, &this->quad[0]);
	::Gdiplus::Graphics graphics(&bmp);

	graphics.DrawImage(this->emfplus.get(), -left, -top, width, height);

	for(int y = 0; y < side; y++)
	{
		for(int x = 0; x < side; x++)
		{
			buffer[y * (side << 2) + (x << 2) + 0] = this->quad[y * (side << 2) + (x << 2) + 2];
			buffer[y * (side << 2) + (x << 2) + 1] = this->quad[y * (side << 2) + (x << 2) + 1];
			buffer[y * (side << 2) + (x << 2) + 2] = this->quad[y * (side << 2) + (x << 2) + 0];
			buffer[y * (side << 2) + (x << 2) + 3] = this->quad[y * (side << 2) + (x << 2) + 3];
		}
	}
}

// Close the document.
void EmfRender::close()
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	this->fileData.clear();
	this->quad.clear();
}

/*
// EMF2WMF
void EmfRender::load(const ::std::wstring & pathFile) const
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	//
	HDC hDc = GetDC(GetDesktopWindow());
	HDC hDcComp = CreateCompatibleDC(hDc);
	ReleaseDC(GetDesktopWindow(), hDc);
	SetMapMode(hDcComp, MM_ANISOTROPIC);

	HENHMETAFILE hEmf = GetEnhMetaFileW(pathFile.c_str());

	if (!hEmf)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error getting the specified metafile."));
	}

	UINT uSize = GetWinMetaFileBits(hEmf, 0, NULL, MM_ANISOTROPIC, hDcComp);

	BYTE * pBuffer = (BYTE *) GlobalAlloc(GPTR, uSize);

	GetWinMetaFileBits(hEmf, uSize, pBuffer, MM_ANISOTROPIC, hDcComp);

	DeleteEnhMetaFile(hEmf);
	DeleteDC(hDcComp);

	// load 
	this->loadFromByteArray(uSize, (char *)pBuffer);

	GlobalFree(pBuffer);
}
*/