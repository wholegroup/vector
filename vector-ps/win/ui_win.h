#pragma once

#ifndef VECTOR_PS_UI_WIN_H
#define VECTOR_PS_UI_WIN_H

// com.wholegroup.vector.ps
namespace com {namespace wholegroup {namespace vector {namespace ps
{
	class UiWin
	{
		public:

			// Get current module handle.
			static HMODULE getCurrentModuleHandle();

			// Get the filename from the handle (old method).
			static ::std::wstring getFilenameByHandleOld(intptr_t const dataFork);

			// Get the filename from the handle (new method).
			static ::std::wstring getFilenameByHandleNew(intptr_t const dataFork);

			// Move the dialog to center.
			static void centerDialog(HWND const wnd);

			// Get window text.
			static ::std::wstring getWindowText(HWND const wnd);

			// Get the string by the specified registry path.
			static ::std::wstring getStringFromRegistry(
				::std::wstring const & location, ::std::wstring const & name);

			// Set the string to the specified registry path.
			static void setStringToRegistry(::std::wstring const & location,
				::std::wstring const & name, ::std::wstring const & value);

			// Return a string value from the resource.
			// ::std::wstring getStringFromResource(UINT const resourceId);

			// Return a integer value from the resource.
			// int getIntFromResource(HINSTANCE module, UINT const resourceId);

	};
}}}}

#endif // VECTOR_PS_UI_WIN_H