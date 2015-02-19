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
#include "resource.h"
#include "../version.h"
#include "../ui.h"
#include "../vector-ps-prf.h"
#include "ui_win.h"
#include "gdi_object.h"
#include "about_dialog.h"
#include "settings_dialog.h"

using namespace ::com::wholegroup::vector::ps;

// 
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

//
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)

//
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)

// NtQueryObject (ntdll.dll)
NTSTATUS (__stdcall *NtQueryObjectRef)(
	_In_opt_   HANDLE Handle,
	_In_       int ObjectInformationClass,
	_Out_opt_  PVOID ObjectInformation,
	_In_       ULONG ObjectInformationLength,
	_Out_opt_  PULONG ReturnLength) = nullptr;

// GetFinalPathNameByHandleW (kernel32.dll)
DWORD (WINAPI *GetFinalPathNameByHandleWRef)(
	_In_   HANDLE hFile,
	_Out_  LPWSTR lpszFilePath,
	_In_   DWORD cchFilePath,
	_In_   DWORD dwFlags) = nullptr;

/************************************************************************/
/* Show the about dialog.                                               */
/************************************************************************/
void Ui::doAbout()
{
	AboutDialog(UiWin::getCurrentModuleHandle()).show();
}

/************************************************************************/
/* Show the settings dialog.                                            */
/************************************************************************/
bool Ui::doSettings(int & width, int & height, double & resolution)
{
	// Create a dialog
	SettingsDialog settings(UiWin::getCurrentModuleHandle());

	settings.setOriginalSize(width, height);
	settings.setOriginalResolution(resolution);

	// Init the dialog
	double columnWidth;
	double gutterWidth;
	double pointSize;

	// Set the preferences
	if (getPreferences(columnWidth, gutterWidth, pointSize))
	{
		settings.setColumnWidth(columnWidth);
		settings.setGutterWidth(gutterWidth);
		settings.setPointSize(pointSize);
	}
	
	// Show the dialog
	bool const returnValue = settings.show();

	if (returnValue)
	{
		width      = settings.getCurrentWidth();
		height     = settings.getCurrentHeight();
		resolution = settings.getCurrentResolution();
	}

	return returnValue;
}

/************************************************************************/
/* Return the ICC profile (sRGB) from the resources.                    */
/************************************************************************/
std::vector<char> Ui::getIccProfile(void)
{
	auto buffer = ::std::vector<char>();

	while (true, true)
	{
		HMODULE hModule = UiWin::getCurrentModuleHandle();

		// Find
		HRSRC const hrSrc = FindResource(hModule, 
			MAKEINTRESOURCE(IDR_ICC), RT_RCDATA);

		if (NULL == hrSrc)
		{
			break;
		}

		// Load
		HGLOBAL const hgLR = LoadResource(hModule, hrSrc);

		if (NULL == hgLR)
		{
			break;
		}

		// Lock
		PCHAR const pcRes = (PCHAR)LockResource(hgLR);

		if (NULL == pcRes)
		{
			break;
		}

		// Copy
		DWORD const resourceSize = SizeofResource(hModule, hrSrc);

		buffer.resize(resourceSize);

		::std::copy(pcRes, pcRes + resourceSize, &buffer[0]);

		break;
	}

	return buffer;
}

/************************************************************************/
/* Get the path to the vector core.                                     */
/************************************************************************/
::std::wstring Ui::getPathToCore()
{
	::std::wstring registryPath(L"SOFTWARE\\");

#ifdef _M_X64
	registryPath.append(L"Wow6432Node\\");
#endif

	registryPath.append(Version::MANUFACTURER());
	registryPath.append(L"\\");
	registryPath.append(Version::NAME());

	return UiWin::getStringFromRegistry(registryPath, L"Path");
}

/************************************************************************/
/* Get the filename from the handle (old method).                       */
/************************************************************************/
::std::wstring UiWin::getFilenameByHandleOld(intptr_t const dataFork)
{
	//
	if (nullptr == NtQueryObjectRef)
	{
		//
		HMODULE hNtdll = ::GetModuleHandleW(L"ntdll.dll");

		if (nullptr == hNtdll)
		{
			BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
				L"GetModuleHandleW: an error getting a handle of ntdll.dll."));
		}

		//
		NtQueryObjectRef = (decltype(NtQueryObjectRef))::GetProcAddress(
			hNtdll, "NtQueryObject");

		if (nullptr == NtQueryObjectRef)
		{
			BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
				L"GetProcAddress: an error getting a pointer of NtQueryObject."));
		}
	}

	//
	::std::vector<int8_t> buffer(sizeof(UNICODE_STRING), 0);

	ULONG bufferLength = 0;
	NTSTATUS status;

	status = NtQueryObjectRef(reinterpret_cast<HANDLE>(dataFork), 
		1, &buffer[0], static_cast<ULONG>(buffer.size()), &bufferLength);

	if ((STATUS_INFO_LENGTH_MISMATCH != status) && (STATUS_BUFFER_OVERFLOW != status))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"NtQueryObjectRef: an error getting the buffer size."));
	}

	if (0 == bufferLength)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"NtQueryObjectRef: the buffer size is zero."));
	}

	if (UINT16_MAX < bufferLength)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"NtQueryObjectRef: the buffer size is too big."));
	}

	//
	buffer.resize(bufferLength, 0);

	status = NtQueryObjectRef(reinterpret_cast<HANDLE>(dataFork), 
		1, &buffer[0], static_cast<ULONG>(buffer.size()), &bufferLength);

	if (STATUS_SUCCESS != status)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"NtQueryObjectRef: an error getting the object information."));
	}

	if (0 == reinterpret_cast<PUNICODE_STRING>(&buffer[0])->Length)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"NtQueryObjectRef: the object length is zero."));
	}

	return ::std::wstring(L"\\\\.\\GLOBALROOT") + ::std::wstring(
		reinterpret_cast<PUNICODE_STRING>(&buffer[0])->Buffer);
}

/************************************************************************/
/* Get the filename from the handle (new method).                       */
/************************************************************************/
::std::wstring UiWin::getFilenameByHandleNew(intptr_t const dataFork)
{
	// 
	if (nullptr == GetFinalPathNameByHandleWRef)
	{
		//
		HMODULE hKernel32dll = ::GetModuleHandleW(L"kernel32.dll");

		if (nullptr == hKernel32dll)
		{
			BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
				L"GetModuleHandleW: an error getting a handle of kernel32.dll."));
		}

		//
		GetFinalPathNameByHandleWRef = (decltype(GetFinalPathNameByHandleWRef))::GetProcAddress(
			hKernel32dll, "GetFinalPathNameByHandleW");

		if (nullptr == GetFinalPathNameByHandleWRef)
		{
			BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
				L"GetProcAddress: an error getting a pointer of GetFinalPathNameByHandleWRef."));
		}
	}

	// length
	DWORD length = ::GetFinalPathNameByHandleWRef(reinterpret_cast<HANDLE>(dataFork),
		nullptr, 0, 0);

	if (0 == length)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetFinalPathNameByHandleW: an error getting the size."));
	}

	// filename
	::std::vector<wchar_t> rawFilename(length, 0);

	if (0 == ::GetFinalPathNameByHandleWRef(reinterpret_cast<HANDLE>(dataFork), 
		reinterpret_cast<LPWSTR>(&rawFilename[0]), static_cast<DWORD>(rawFilename.size()), 0))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetFinalPathNameByHandleW: an error getting the filename."));
	}

	::boost::trim_right_if(rawFilename, 
		::boost::is_any_of(::boost::as_array(L"\0")));

	return ::std::wstring(rawFilename.begin(), rawFilename.end());
}

/************************************************************************/
/* Get the filename from the handle.                                    */
/************************************************************************/
::std::wstring Ui::getFilenameByHandle(intptr_t const dataFork)
{
	if (nullptr == NtQueryObjectRef)
	{
		try
		{
			return UiWin::getFilenameByHandleNew(dataFork);
		}
		catch (exception::Exception const & ex)
		{
			UNREFERENCED_PARAMETER(ex);
		}
	}

	return UiWin::getFilenameByHandleOld(dataFork);
}


/************************************************************************/
/* Get current module handle.                                           */
/************************************************************************/
HMODULE UiWin::getCurrentModuleHandle()
{
	HMODULE hModule = NULL;

	if (FALSE == ::GetModuleHandleExW(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCWSTR>(&UiWin::getCurrentModuleHandle), &hModule))
	{
		return NULL;
	}

	return hModule;
}


/************************************************************************/
/* Move the dialog to center.                                           */
/************************************************************************/
void UiWin::centerDialog(HWND const dlg)
{
	int  nHeight;
	int  nWidth;
	int  nTitleBits;
	RECT rcDialog;
	RECT rcParent;
	int  xOrigin;
	int  yOrigin;
	int  xScreen;
	int  yScreen;
	HWND hParent = GetParent(dlg);

	if  (NULL == hParent)
	{
		hParent = GetDesktopWindow();
	}

	GetClientRect(hParent, &rcParent);

	ClientToScreen(hParent, (LPPOINT)&rcParent.left);
	ClientToScreen(hParent, (LPPOINT)&rcParent.right);

	nTitleBits = GetSystemMetrics(SM_CXSIZE);

	// If dialog has no sys menu compensate for odd# bitmaps by sub 1 bitwidth
	if  (!(GetWindowLong(dlg, GWL_STYLE) & WS_SYSMENU))
	{
		nTitleBits -= nTitleBits / 3;
	}

	GetWindowRect(dlg, &rcDialog);

	nWidth  = rcDialog.right  - rcDialog.left;
	nHeight = rcDialog.bottom - rcDialog.top;

	xOrigin = max(rcParent.right - rcParent.left - nWidth, 0) / 2
		+ rcParent.left - nTitleBits;

	xScreen = GetSystemMetrics(SM_CXSCREEN);

	if  (xOrigin + nWidth > xScreen)
	{
		xOrigin = max(0, xScreen - nWidth);
	}

	yOrigin = max(rcParent.bottom - rcParent.top - nHeight, 0) / 3
		+ rcParent.top;

	yScreen = GetSystemMetrics(SM_CYSCREEN);

	if  (yOrigin + nHeight > yScreen)
	{
		yOrigin = max(0 , yScreen - nHeight);
	}

	SetWindowPos(dlg, NULL, xOrigin, yOrigin, nWidth, nHeight, SWP_NOZORDER);
}

/************************************************************************/
/* Return a string value from the resource.                             */
/************************************************************************/
/*
::std::wstring getStringFromResource(UINT const resourceId)
{
	wchar_t *buffer = 0;

	std::size_t length = ::LoadStringW(UiWin::getCurrentModuleHandle(), resourceId, 
		reinterpret_cast<wchar_t*>(&buffer), 0);

	return std::wstring(buffer, length);
}

// Return a integer value from the resource.
int getIntFromResource(HINSTANCE module, UINT const resourceId)
{
	return ::boost::lexical_cast<int>(getStringFromResource(module, resourceId));
}*/


/************************************************************************/
/* Get window text.                                                     */
/************************************************************************/
::std::wstring UiWin::getWindowText(HWND const wnd)
{
	// Length
	int const textLength = ::GetWindowTextLengthW(wnd);

	if (0 >= textLength)
	{
		return ::std::wstring(L"");
	}

	// Text
	::std::vector<wchar_t> textVector(textLength + 1, 0);

	if (textLength != ::GetWindowTextW(wnd, &textVector[0], 
		static_cast<int>(textVector.size())))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowTextW: error getting text of the window."));
	}

	return ::std::wstring(textVector.begin(), --textVector.end());
}

/************************************************************************/
/* Get the string by the specified registry path.                       */
/************************************************************************/
::std::wstring UiWin::getStringFromRegistry(
	::std::wstring const & location, ::std::wstring const & name)
{
	// Open the key
	HKEY hKey;

	if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, 
		location.c_str(), 0, KEY_QUERY_VALUE, &hKey))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"RegOpenKeyExW: error opening a registry key."));
	}

	::std::unique_ptr<HKEY__, ::std::function<void(HKEY)>> const key(hKey, 
		[](HKEY hKey){ ::RegCloseKey(hKey); });

	// Get the length
	DWORD regKeyType = 0;
	DWORD regKeySize = 0;

	if (ERROR_SUCCESS != ::RegQueryValueExW(key.get(), name.c_str(), 
		nullptr, &regKeyType, nullptr, &regKeySize))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"RegQueryValueExW: error getting length of value."));
	}

	if (REG_SZ != regKeyType)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"RegQueryValueExW: error type."));
	}

	// Read the value
	::std::vector<wchar_t> bufferValue(regKeySize / sizeof(wchar_t), 0);
	
	if (ERROR_SUCCESS != ::RegQueryValueExW(key.get(), name.c_str(), 
		nullptr, &regKeyType, (LPBYTE)&bufferValue[0], &regKeySize))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"RegQueryValueExW: error reading value."));
	}

	::boost::trim_right_if(bufferValue, 
		::boost::is_any_of(::boost::as_array(L"\0")));

	return ::std::wstring(bufferValue.cbegin(), bufferValue.cend());
}

/************************************************************************/
/* Set the string to the specified registry path.                       */
/************************************************************************/
void UiWin::setStringToRegistry(::std::wstring const & location, 
	::std::wstring const & name, ::std::wstring const & value)
{
	// Open the key
	HKEY hKey;

	if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, 
		location.c_str(), 0, KEY_SET_VALUE, &hKey))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"RegOpenKeyExW: error opening a registry key."));
	}

	::std::unique_ptr<HKEY__, ::std::function<void(HKEY)>> const key(hKey, 
		[](HKEY hKey){ ::RegCloseKey(hKey); });

	// Set the string
	if (ERROR_SUCCESS != ::RegSetValueExW(key.get(), name.c_str(), 0, REG_SZ, 
		(LPBYTE)value.c_str(), static_cast<DWORD>(value.length() * 
		sizeof(::std::wstring::value_type))))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"RegSetValueExW: an error setting value."));
	}
}
