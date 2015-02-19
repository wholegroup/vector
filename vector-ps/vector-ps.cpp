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
#include "suite.h"
#include "ui.h"
#include "vector-ps-prf.h"

using namespace ::com::wholegroup::vector;
using namespace ::com::wholegroup::vector::ps;

namespace {

// Preview side
#define PREVIEW_SIDE 128

// Image Side
#define IMAGE_SIDE 1024

// JVM
static jni::Jvm jvm;

// Filename
static ::std::wstring filename;

// Render
static ::boost::shared_ptr<jni::BaseRender> render;

// Image ratio
static double ratio;

/************************************************************************/
/* Prepare.                                                             */
/************************************************************************/
int16 readPrepare(FormatRecordPtr format)
{
	format->maxData = 0;
	
	// Load and create JVM
	try
	{
#if defined(_DEBUG) || defined(DEBUG)
		::std::wstring debugPathToCore;

		try
		{
			debugPathToCore = Ui::getPathToCore();
			
			if (!::boost::filesystem::exists(debugPathToCore + L"/lib"))
			{
				BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
					L"Path not found."));
			}
		}
		catch (exception::Exception const & ex)
		{
			UNREFERENCED_PARAMETER(ex);

#	ifdef WIN32
			debugPathToCore = L"D:\\Projects\\Vector\\build\\vector-core";
#	endif

#	ifdef __APPLE__
			debugPathToCore = L"/Users/admin/Projects/vector/build/vector-core";
#	endif
		}

		::std::wstring const pathToCore = debugPathToCore;
#else
		::std::wstring const pathToCore = Ui::getPathToCore();
#endif // DEBUG

		if (!jvm.isLoaded())
		{
			jvm.loadJvm(jni::JvmFinder().setHome(pathToCore).findJvm());
		}

		if (!jvm.isCreated())
		{
			jvm.createJvm(::boost::filesystem::system_complete(
				::boost::filesystem::path(pathToCore) / jni::VECTOR_CORE_JAR).wstring());
		}
	}
	catch (exception::Exception const & ex)
	{
		UNREFERENCED_PARAMETER(ex);

		return ioErr;
	}

	// Get current filename
	::std::wstring currentFilename;

	try
	{
#ifdef WIN32
		if (8 < getVersionMajor())
		{
			currentFilename = ::std::wstring(reinterpret_cast<wchar_t*>(
				format->fileSpec2->mReference));
		}
		else
		{
			currentFilename = Ui::getFilenameByHandle(
				format->dataFork);
		}
#elif __APPLE__
		::std::vector<UInt8> vfn(4096);
		
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

		if (noErr != FSRefMakePath(&format->fileSpec2->mReference,
			&vfn[0], static_cast<UInt32>(vfn.size())))
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An unknown error of getting filename path."));
		}

#pragma clang diagnostic pop
		
		currentFilename = ::boost::locale::conv::utf_to_utf<wchar_t>(
			::std::string(vfn.begin(), ::std::find(vfn.begin(), vfn.end(), '\0')).c_str());
#endif
	}
	catch (exception::Exception const & ex)
	{
		UNREFERENCED_PARAMETER(ex);

		return readErr;
	}

	// Set the dll's path
#ifdef WIN32
	try
	{
#	ifdef _DEBUG
		::std::wstring debugPathToCore;

		try
		{
			debugPathToCore = Ui::getPathToCore();
			
			if (!::boost::filesystem::exists(debugPathToCore + L"/zlib.dll"))
			{
				BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
					L"Path not found."));
			}
		}
		catch (exception::Exception const & ex)
		{
			UNREFERENCED_PARAMETER(ex);

#		ifdef _M_AMD64
			debugPathToCore = L"D:\\Projects\\Vector\\build\\64\\Debug";
#		else // _M_AMD64
			debugPathToCore = L"D:\\Projects\\Vector\\build\\32\\Debug";
#		endif // else _M_AMD64
		}

		::std::wstring const pathToDll = debugPathToCore;
#	else 		
#		ifdef _M_AMD64
		::std::wstring const pathToDll = Ui::getPathToCore() + L"/64";
#		else // _M_AMD64
		::std::wstring const pathToDll = Ui::getPathToCore() + L"/32";
#		endif // else _M_AMD64
#	endif // _DEBUG

	// check dlls 
#	ifdef BUILD_WPG
		if (!::boost::filesystem::exists(pathToDll + L"/libwpd.dll") || 
			!::boost::filesystem::exists(pathToDll + L"/libwpg.dll"))
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"Path not found."));
		}
#	endif // BUILD_WPG

#	ifdef BUILD_CDR
		if (!::boost::filesystem::exists(pathToDll + L"/libcdr.dll") || 
			!::boost::filesystem::exists(pathToDll + L"/libwpd.dll") ||
			!::boost::filesystem::exists(pathToDll + L"/icu_common.dll") || 
			!::boost::filesystem::exists(pathToDll + L"/icu_i18n.dll") ||
			!::boost::filesystem::exists(pathToDll + L"/icu_stubdata.dll") ||
			!::boost::filesystem::exists(pathToDll + L"/lcms2.dll") ||
			!::boost::filesystem::exists(pathToDll + L"/zlib.dll"))
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"Path not found."));
		}
#	endif // BUILD_CDR

		SetDllDirectoryW(pathToDll.c_str());
	}
	catch (exception::Exception const & ex)
	{
		UNREFERENCED_PARAMETER(ex);

		return nilHandleErr;
	}

#endif // WIN32

	// Create the render
	if (format->openForPreview || (nullptr == render) || 
		(currentFilename != filename))
	{
		filename = currentFilename;

#ifdef BUILD_SVG
		render.reset(new jni::SvgRender(&jvm));
#elif defined(BUILD_WMF)
		render.reset(new jni::WmfRender(&jvm));
#elif defined(BUILD_EMF)
		render.reset(new jni::EmfRender(&jvm));
#elif defined(BUILD_WPG)
		render.reset(new jni::WpgRender(&jvm));
#elif defined(BUILD_CDR)
		render.reset(new jni::CdrRender(&jvm));
#else
#	pragma message("The render is not specified.")
#endif

		try
		{
			// Init the render
			render->init();

			// Load the file
			render->load(filename);

			// Create
			render->create();

			// Normalize
			ratio = render->normalize();
		}
		catch (exception::Exception const & ex)
		{
			UNREFERENCED_PARAMETER(ex);

			render.reset();

			return readErr;
		}
	}

	// Set the progress message
	if (!format->openForPreview)
	{
		// Prepare
		StringSuite stringSuite(CreateStringSuite(format->sSPBasic));

		if ((kSPNoError != stringSuite.getError()) || (nullptr == stringSuite))
		{
			return nilHandleErr;
		}

		ASZString progressText;

		stringSuite->MakeFromCString("Rasterizing File", 16, &progressText);

		// Change
		ProgressSuite progressSuite(CreateProgressSuite(format->sSPBasic));

		if ((kSPNoError != progressSuite.getError()) || (nullptr == progressSuite))
		{
			return nilHandleErr;
		}

		progressSuite->ChangeProgressText(progressText);

		//
		stringSuite->Release(progressText);
	}

	return noErr;
}

/************************************************************************/
/* Start.                                                               */
/************************************************************************/
int16 readStart(FormatRecordPtr format)
{
	// Set canvas size
	if (format->openForPreview)
	{
		format->imageSize32.v = PREVIEW_SIDE;
		format->imageSize32.h = PREVIEW_SIDE;
	}
	else
	{
		int width  = Ui::DEFAULT_WIDTH;
		int height = Ui::DEFAULT_HEIGHT;

		if (ratio > 1)
		{
			height = static_cast<int>(floor(width / ratio + 0.5l));
		}
		else if (ratio < 1)
		{
			width = static_cast<int>(floor(height * ratio + 0.5l));
		}

		double resolution = Ui::DEFAULT_RESOLUTION;

		if (!Ui::doSettings(width, height, resolution))
		{
			return userCanceledErr;
		}

		format->imageSize32.h = width;
		format->imageSize32.v = height;
		format->imageHRes     = ::boost::numeric_cast<Fixed>(resolution * 0xFFFF);
		format->imageVRes     = format->imageHRes;
	}

	// Calcualate side size
	int const maxSide = std::max<int>(
		format->imageSize32.v, format->imageSize32.h);

	short side = IMAGE_SIDE;

	if (maxSide < side)
	{
		side = maxSide;
	}

	// Init
	format->PluginUsing32BitCoordinates = true;
	format->planes                      = 4; // RGBA
	format->imageMode                   = plugInModeRGBColor;
	format->depth                       = 8;
	format->planeBytes                  = format->depth >> 3;
	format->colBytes                    = format->planes * static_cast<int16>(format->planeBytes);
	format->rowBytes                    = side * format->colBytes;
	format->loPlane                     = 0;
	format->hiPlane                     = format->planes - 1;
	format->transparencyPlane           = format->hiPlane;

	return noErr;
}

/************************************************************************/
/* ICC Profile.                                                         */
/************************************************************************/
int16 DoReadICCProfile(FormatRecordPtr format)
{
	return noErr;
	
	// Check if can use
	if (!format->canUseICCProfiles)
	{
		return noErr;
	}

	// Get the handle suite
	HandleSuite handleSuite(CreateHandleSuite(format->sSPBasic));

	if ((kSPNoError != handleSuite.getError()) || (nullptr == handleSuite))
	{
		return nilHandleErr;
	}

	// Get ICC
	auto icc  = Ui::getIccProfile();
	auto size = static_cast<int32>(icc.size());

	if (0 == icc.size())
	{
		return paramErr;
	}

	// Set ICC
	Ptr data = nullptr;

	format->iCCprofileSize = size;
	format->iCCprofileData = handleSuite->New(size);

	handleSuite->SetLock(format->iCCprofileData, true, &data, NULL);

	if (nullptr != data)
	{
		::std::copy(&icc[0], &icc[0] + size, data);

		// Unlock
		handleSuite->SetLock(format->iCCprofileData, false, &data, NULL);
	}

	return noErr;
}

/************************************************************************/
/* Continue.                                                            */
/************************************************************************/
int16 readContinue(FormatRecordPtr format)
{
	// Calculate the side size
	uint32_t const side = format->rowBytes / format->colBytes;

	// Get the buffer suite
	BufferSuite bufferSuite(CreateBufferSuite(format->sSPBasic));

	if ((kSPNoError != bufferSuite.getError()) || (nullptr == bufferSuite))
	{
		return nilHandleErr;
	}

	// Allocate the pixel buffer
	unsigned32 bufferSize = format->rowBytes * side;

	Ptr pixelData = bufferSuite->New(&bufferSize, bufferSize);

	if (nullptr == pixelData)
	{
		return memFullErr;
	}

	format->data = pixelData;

	// Rasterize
	uint8 * const data8 = reinterpret_cast<uint8*>(pixelData);

	uint32_t const nSideH = static_cast<unsigned int>(
		ceil(format->imageSize32.h / static_cast<double>(side)));
	uint32_t const nSideV = static_cast<unsigned int>(
		ceil(format->imageSize32.v / static_cast<double>(side)));

	int16 result = noErr;

	for (uint32_t h = 0; h < nSideH; h++)
	{
		format->theRect32.left   = side * h;
		format->theRect32.right  = format->theRect32.left + side;

		for (uint32_t v = 0; v < nSideV; v++)
		{
			format->theRect32.top    = side * v;
			format->theRect32.bottom = format->theRect32.top + side;

			// Test solid fill
			/*
			for (uint32_t x = 0; x < side; x++)
			{
				for (uint32_t y = 0; y < side; y++)
				{
					data8[x * format->rowBytes + y * format->planes]     = 0x00;
					data8[x * format->rowBytes + y * format->planes + 1] = 0x00;
					data8[x * format->rowBytes + y * format->planes + 2] = 0xA0;
					data8[x * format->rowBytes + y * format->planes + 3] = 0xFF;
				}
			}
			*/

			// Render a block
			try
			{
				render->render(format->imageSize32.h, format->imageSize32.v,
					h * side, v * side, side, data8);
			}
			catch (exception::Exception const & ex)
			{
				UNREFERENCED_PARAMETER(ex);

				result = readErr;
			}

			if (noErr != result)
			{
				break;
			}
			
			// Show block
			result = format->advanceState();

			if (noErr != result)
			{
				break;
			}

			// Set progress index
			format->progressProc(h * nSideV + v, nSideH * nSideV);
		}

		// Stop if there was an error
		if (noErr != result)
		{
			break;
		}
	}

	// Free the allocated resources
	format->data = NULL;

	bufferSuite->Dispose(&pixelData);

	// Read the ICC profile
	if (noErr == result)
	{
		result = DoReadICCProfile(format);
	}

	return result;
}

/************************************************************************/
/* Finish.                                                              */
/************************************************************************/
int16 readFinish(FormatRecordPtr format)
{
	// Delete the render
	if (!format->openForPreview)
	{
		render.reset();
	}

	return noErr;
}

} // namespace

/************************************************************************/
/* Entry point.                                                         */
/************************************************************************/
DLLExport MACPASCAL void BOOST_SYMBOL_VISIBLE formatEntry(int16 const selector, 
	FormatRecordPtr format, intptr_t * data, int16 * result)
{
	// Show the about dialog
	if (formatSelectorAbout == selector)
	{
		Ui::doAbout();

		return;
	}

	// Operation
	*result = noErr;

	switch (selector)
	{
		case formatSelectorReadPrepare:
		{
			*result = readPrepare(format);
			
			break;
		}

		case formatSelectorReadStart:
		{
			*result = readStart(format);

			break;
		}

		case formatSelectorReadContinue:
		{
			*result = readContinue(format);
			
			break;
		}

		case formatSelectorReadFinish:
		{
			*result = readFinish(format);
			
			break;
		}

		default:
			break;
	}
}
