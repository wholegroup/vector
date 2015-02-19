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
#include "ui_win.h"
#include "gdi_object.h"
#include "about_dialog.h"

using namespace ::com::wholegroup::vector::ps;

// Initializing the class constants
int const AboutDialog::HORIZ_OFFSET = 13;

//
int const AboutDialog::DIALOGFONT_HEIGHT = 10;

//
int const AboutDialog::CAPTIONFONT_HEIGHT = 12;

//
int const AboutDialog::VERTICAL_OFFSET = 193;

//
int const AboutDialog::BACKGROUND_COLOR = RGB(0xFF, 0xFF, 0xFF);

//
int const AboutDialog::LINK_COLOR = RGB(0x18, 0x3D, 0xAB);

//
int const AboutDialog::LINK_HOVER_COLOR = RGB(0x99, 0, 0);

//
::std::wstring const AboutDialog::SITE_URL(L"http://www.wholegroup.com");

//
::std::wstring const AboutDialog::VERSION(L"Version %s");

/************************************************************************/
/* Constructor.                                                         */
/************************************************************************/
AboutDialog::AboutDialog(HINSTANCE hModule)
	: hModule(hModule), 
	hoverLink(false)
{
}

/************************************************************************/
/* Destructor.                                                          */
/************************************************************************/
AboutDialog::~AboutDialog()
{
}

/************************************************************************/
/* Initialization.                                                      */
/************************************************************************/
void AboutDialog::init(HWND const wnd)
{
	// Get the window handles
	auto const ids = ::boost::assign::list_of
		(IDC_ABOUT_VERSION)
		(IDC_ABOUT_NAME)
		(IDB_ABOUT_LINK)
		(IDC_ABOUT_COPYRIGHT);
	
	for (auto id = ids.begin(); id != ids.end(); ++id)
	{
		HWND const wndItem = ::GetDlgItem(wnd, *id);

		if (NULL == wndItem)
		{
			BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
				L"GetDlgItem: error getting the window handle."));
		}

		this->windows[*id] = wndItem;
	}

	// Create a background brush
	this->backgroundBrush.reset(
		new HBRUSH(::CreateSolidBrush(BACKGROUND_COLOR)));

	if ((nullptr == this->backgroundBrush) || (NULL == *(this->backgroundBrush)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"CreateSolidBrush: error creating the background brush."));
	}

	// Load the background bitmap 
	this->background.reset(new HBITMAP(
		::LoadBitmap(this->hModule, MAKEINTRESOURCE(IDB_ABOUT_BG))));

	if ((nullptr == this->background) || (NULL == *(this->background)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"LoadBitmap: error loading the background bitmap."));
	}

	// Hand cursor
	this->handCursor = ::LoadCursorW(NULL, IDC_HAND);

	// Get the link text
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDB_ABOUT_LINK),
		"The window handle is not found.");

	this->linkText = UiWin::getWindowText(this->windows.at(IDB_ABOUT_LINK));

	// Font
	initFonts(wnd);

	// Set the plugin version
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_VERSION), 
		"The window handle is not found.");

	if (FALSE == ::SetWindowTextW(this->windows.at(IDC_ABOUT_VERSION), 
		::boost::replace_first_copy(VERSION, L"%s",
		::com::wholegroup::vector::ps::Version::VERSION()).c_str()))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetWindowTextW: error setting the plugin version."));
	}

	// Set the plugin name
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_NAME),
		"The window handle is not found.");

	if (FALSE == ::SetWindowTextW(this->windows.at(IDC_ABOUT_NAME),
		::com::wholegroup::vector::ps::Version::NAME().c_str()))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetWindowTextW: error setting the plugin name."));
	}

	// Size
	initSize(wnd);

	// Subclass the link
	subclassLink(IDB_ABOUT_LINK);
}

/************************************************************************/
/* Initializing the fonts.                                              */
/************************************************************************/
void AboutDialog::initFonts(HWND const wnd)
{
	// Get the current dialog font
	HFONT wndFont = reinterpret_cast<HFONT>(::SendMessage(wnd, WM_GETFONT, 0, 0));

	if (NULL == wndFont)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessage: error getting the current font."));
	}

	// Create a new dialog font
	LOGFONT lf;

	if (sizeof(lf) != ::GetObject(wndFont, sizeof(lf), &lf))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetObject: error getting attributes of the font."));
	}

	lf.lfHeight = -DIALOGFONT_HEIGHT;

	this->dialogFont.reset(
		new HFONT(::CreateFontIndirect(&lf)));

	if ((nullptr == this->dialogFont) || (NULL == *(this->dialogFont)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"CreateFontIndirect: error creating the new dialog font."));
	}

	// Set the new dialog font
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_VERSION), 
		"The window handle is not found.");

	::SendMessageW(this->windows.at(IDC_ABOUT_VERSION), WM_SETFONT, 
		reinterpret_cast<WPARAM>(*(this->dialogFont)), 0);

	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_COPYRIGHT),
		"The window handle is not found.");

	::SendMessageW(this->windows.at(IDC_ABOUT_COPYRIGHT), WM_SETFONT, 
		reinterpret_cast<WPARAM>(*(this->dialogFont)), 0);

	// Create a new caption font
	if (sizeof(lf) != ::GetObject(wndFont, sizeof(lf), &lf))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetObject: error getting attributes of the font."));
	}

	lf.lfHeight = -CAPTIONFONT_HEIGHT;
	lf.lfWeight = FW_BOLD;

	this->captionFont.reset(
		new HFONT(::CreateFontIndirect(&lf)));

	if ((nullptr == this->captionFont) || (NULL == *(this->captionFont)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"CreateFontIndirect: error creating a new caption font."));
	}

	// Set the new caption font
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_NAME),
		"The window handle is not found.");

	::SendMessageW(this->windows.at(IDC_ABOUT_NAME), WM_SETFONT, 
		reinterpret_cast<WPARAM>(*(this->captionFont)), 0);

	// Create a new link font
	if (sizeof(lf) != ::GetObject(wndFont, sizeof(lf), &lf))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetObject: error getting attributes of the font."));
	}

	lf.lfUnderline = TRUE;
	lf.lfWeight    = FW_BOLD;

	this->linkFont.reset(
		new HFONT(::CreateFontIndirect(&lf)));

	if ((nullptr == this->linkFont) || (NULL == *(this->linkFont)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"CreateFontIndirect: error creating a new caption font."));
	}
}

/************************************************************************/
/* Initializing sizes of the controls.                                  */
/************************************************************************/
void AboutDialog::initSize(HWND const wnd) const
{
	// Get the background size
	BITMAP backgroundBitmap;

	if (sizeof(backgroundBitmap) != ::GetObject(*(this->background),
		sizeof(backgroundBitmap), &backgroundBitmap))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetObject: error getting attributes of the background."));
	}

	// Set the new size of the dialog
	LONG const gwlStyle = ::GetWindowLong(wnd, GWL_STYLE);

	if (0 == gwlStyle)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowLong: error getting the window styles."));
	}

	LONG const gwlExtStyle = ::GetWindowLong(wnd, GWL_EXSTYLE);

	if (0 == gwlExtStyle)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowLong: error getting the extended window styles."));
	}

	RECT dialogRect = {0, 0, backgroundBitmap.bmWidth, backgroundBitmap.bmHeight};

	if (FALSE == ::AdjustWindowRectEx(&dialogRect, gwlStyle, 
		(nullptr != ::GetMenu(wnd)), gwlExtStyle))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"AdjustWindowRectEx: error calculating the required size of the dialog."));
	}

	if (FALSE == ::MoveWindow(wnd, 0, 0, dialogRect.right - dialogRect.left, 
		dialogRect.bottom - dialogRect.top, FALSE))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"MoveWindow: error changing the dialog size."));
	}

	// Parameters
	int horizontalOffset = HORIZ_OFFSET;
	int verticalOffset   = VERTICAL_OFFSET;

	int const dialogFontSize  = getFontHeightFromDlgItem(wnd, IDC_ABOUT_VERSION);
	int const captionFontSize = getFontHeightFromDlgItem(wnd, IDC_ABOUT_NAME);

	int const width = backgroundBitmap.bmWidth - horizontalOffset * 2;

	// Set the size of IDC_ABOUT_VERSION
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_VERSION),
		"The window handle is not found.");

	if (FALSE == ::MoveWindow(this->windows.at(IDC_ABOUT_VERSION),
		horizontalOffset + width / 2, verticalOffset, 
		width / 2, dialogFontSize, FALSE))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"MoveWindow: error changing the control size."));
	}

	// Set the size of IDC_ABOUT_NAME
	verticalOffset += dialogFontSize;
	verticalOffset += captionFontSize;

	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_NAME),
		"The window handle is not found.");

	if (FALSE == ::MoveWindow(this->windows.at(IDC_ABOUT_NAME),
		horizontalOffset, verticalOffset, 
		width, captionFontSize, FALSE))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"MoveWindow: error changing the control size."));
	}

	// Set the size of IDC_ABOUT_COPYRIGHT
	verticalOffset += captionFontSize;
	verticalOffset += dialogFontSize;

	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_ABOUT_COPYRIGHT), 
		"The window handle is not found.");

	if (FALSE == ::MoveWindow(this->windows.at(IDC_ABOUT_COPYRIGHT),
		horizontalOffset, verticalOffset, 
		width, dialogFontSize, FALSE))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"MoveWindow: error changing the control size."));
	}

	// Set the size of IDB_ABOUT_LINK
	verticalOffset += dialogFontSize;
	verticalOffset += dialogFontSize;

	RECT linkRect;

	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDB_ABOUT_LINK), 
		"The window handle is not found.");

	if (FALSE == ::GetWindowRect(this->windows.at(IDB_ABOUT_LINK), 
		&linkRect))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowRect: error getting the control size."));
	}

	if (FALSE == ::MoveWindow(this->windows.at(IDB_ABOUT_LINK),
		horizontalOffset + width / 2 - (linkRect.right - linkRect.left) / 2, verticalOffset, 
		linkRect.right - linkRect.left, linkRect.bottom - linkRect.top, FALSE))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"MoveWindow: error changing the control size."));
	}
}


/************************************************************************/
/* Show the dialog.                                                     */
/************************************************************************/
void AboutDialog::show()
{
	::DialogBoxParamW(
		this->hModule, MAKEINTRESOURCE(IDD_ABOUT), ::GetActiveWindow(), 
		reinterpret_cast<DLGPROC>(&AboutDialog::messageProc), (LPARAM)this);
}

/************************************************************************/
/* Message processing.                                                  */
/************************************************************************/
LRESULT CALLBACK AboutDialog::messageProc(HWND wnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	// Initializing of the dialog
	if (WM_INITDIALOG == uiMsg)
	{
		try
		{
			// Save the pointer to the dialog
			AboutDialog * const dialog = reinterpret_cast<AboutDialog *>(lParam);

			if (nullptr == dialog)
			{
				BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
					L"The pointer to the dialog is null."));
			}

			::SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(dialog));

			// Init
			dialog->init(wnd);

			// Center
			UiWin::centerDialog(wnd);
		}
		catch (::std::exception & ex)
		{
			::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
			::EndDialog(wnd, IDABORT);
		}

		return TRUE;
	}

	// The pointer to the dialog
	AboutDialog * const dialog = reinterpret_cast<AboutDialog *>(
		::GetWindowLongPtr(wnd, GWLP_USERDATA));

	// Process the command
	switch (uiMsg)
	{
		// Fill the background of the dialog
		case WM_ERASEBKGND:
			try
			{
				BOOST_ASSERT_MSG(nullptr != dialog, 
					"The dialog is not initialized.");

				dialog->fillBackground(wnd, reinterpret_cast<HDC>(wParam));

				return TRUE;
			}
			catch (::std::exception & ex)
			{
				::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
				::EndDialog(wnd, IDABORT);
			}

			break;

		// Background and text color of the controls
		case WM_CTLCOLORSTATIC:
			BOOST_ASSERT_MSG(nullptr != dialog, 
				"The dialog is not initialized.");

			return reinterpret_cast<LRESULT>(*(dialog->backgroundBrush));

		// Close dialog by clicking a mouse button
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			::EndDialog(wnd, IDOK);

			return 0;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					::EndDialog(wnd, static_cast<INT_PTR>(LOWORD(wParam)));

					return 0;

				// Open the site link and close the dialog
				case IDB_ABOUT_LINK:
					::ShellExecuteW(0, L"open", SITE_URL.c_str(), 
						0, 0, SW_SHOWNORMAL);
					::EndDialog(wnd, IDOK);

					return 0;

				default:
					break;
			}

			break;

		case WM_DRAWITEM:
			if (IDB_ABOUT_LINK == wParam)
			{
				try
				{
					BOOST_ASSERT_MSG(nullptr != dialog, 
						"The dialog is not initialized.");

					dialog->paintLink(reinterpret_cast<PDRAWITEMSTRUCT>(lParam));

					return TRUE;
				}
				catch (::std::exception & ex)
				{
					::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
					::EndDialog(wnd, IDABORT);
				}
			}

			break;

		default:
			break;
	}

	return FALSE;
}

/************************************************************************/
/* Fill the background.                                                 */
/************************************************************************/
void AboutDialog::fillBackground(HWND const wnd, HDC const dc) const
{
	// Get the coordinates of the dialog
	RECT rect;

	if (FALSE == ::GetClientRect(wnd, &rect))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetClientRect: error getting the coordinates of the dialog."));
	}

	// Copy the background to the device context
	HDC buffer = ::CreateCompatibleDC(NULL);

	if (NULL == buffer)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"CreateCompatibleDC: error creating the device context."));
	}

	if (NULL == ::SelectObject(buffer, *(this->background)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SelectObject: error setting the background as a "
			L"bitmap on the device context."));
	}

	if (FALSE == ::BitBlt(dc, 0, 0, rect.right, rect.bottom, buffer, 0, 0, SRCCOPY))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"BitBlt: error copying to the device context of the dialog."));
	}

	if (FALSE == ::DeleteDC(buffer))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"DeleteDC: error deleting the device context."));
	}
}

/************************************************************************/
/* Paint the site link.                                                 */
/************************************************************************/
void AboutDialog::paintLink(PDRAWITEMSTRUCT const item) const
{
	// Set the link color
	COLORREF const linkColor = this->hoverLink ? 
		LINK_HOVER_COLOR : LINK_COLOR;

	if (CLR_INVALID == ::SetTextColor(item->hDC, linkColor))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetTextColor: error setting the link color."));
	}

	// Set the background mode
	if (FALSE == ::SetBkMode(item->hDC, TRANSPARENT))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetBkMode: error setting the transparent background."));
	}

	// Set the font
	if (NULL == ::SelectObject(item->hDC, *(this->linkFont)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SelectObject: error setting the link font."));
	}

	// Calculate the link position
	SIZE size;

	if (FALSE == ::GetTextExtentPoint32(item->hDC, this->linkText.c_str(), 
		static_cast<int>(this->linkText.length()), &size))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetTextExtentPoint32: error getting the size of the link text."));
	}

	RECT rect(item->rcItem);

	rect.top  += ((rect.bottom - rect.top) - size.cy) / 2;
	rect.left += ((rect.right - rect.left) - size.cx) / 2;

	// Draw the link on the center
	if (FALSE == ::FillRect(item->hDC, &item->rcItem, *(this->backgroundBrush)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"FillRect: error filling the background."));
	}

	if (FALSE == ::DrawTextW(item->hDC, this->linkText.c_str(), 
		static_cast<int>(this->linkText.length()), &rect, 0))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"DrawTextW: error drawing the link text."));
	}
}

/************************************************************************/
/* Return the font height from the specified dialog item.               */
/************************************************************************/
int AboutDialog::getFontHeightFromDlgItem(HWND const wnd, int const itemId) const
{
	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(itemId), 
		"The window handle is not found.");

	HWND const itemWnd = this->windows.at(itemId);

	// Get the device context
	HDC dc = ::GetDC(itemWnd);

	if (NULL == dc)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetDC: error getting the device context."));
	}

	// Get the text metrics
	TEXTMETRIC tm;

	if (FALSE == ::GetTextMetrics(dc, &tm))
	{
		::ReleaseDC(itemWnd, dc);

		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetDC: error getting the text metrics."));
	}

	// Release
	if (0 == ::ReleaseDC(itemWnd, dc))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"ReleaseDC: error releasing the device context."));
	}

	return static_cast<int>(tm.tmHeight);
}

/************************************************************************/
/* Message processing of link controls.                                 */
/************************************************************************/
LRESULT CALLBACK AboutDialog::linkProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Get the dialog pointer
	AboutDialog * const dialog = reinterpret_cast<AboutDialog * const>(
		::GetWindowLongPtrW(wnd, GWLP_USERDATA));

	BOOST_ASSERT_MSG(nullptr != dialog, 
		"The pointer to the dialog is null.");

	// Process the message
	switch(msg)
	{
		// Set the hand cursor
		case WM_SETCURSOR:
			::SetCursor(dialog->handCursor);
			return TRUE;

		// Run tracking mouse
		case WM_MOUSEMOVE:
			if (!dialog->hoverLink)
			{
				TRACKMOUSEEVENT tme = { sizeof(tme) };

				tme.dwFlags   = TME_LEAVE;
				tme.hwndTrack = wnd;

				if (::TrackMouseEvent(&tme))
				{
					dialog->hoverLink = true;

					::InvalidateRect(wnd, NULL, TRUE);
					::UpdateWindow(wnd);
				}
			}

			break;

		// Notice of leaving mouse
		case WM_MOUSELEAVE:
			dialog->hoverLink = false;

			::InvalidateRect(wnd, NULL, TRUE);
			::UpdateWindow(wnd);
			break;

		default:
			break;
	}

	// Call the original message procedure
	BOOST_ASSERT_MSG(dialog->linkOriginalProcs.end() != dialog->linkOriginalProcs.find(wnd),
		"The original function is is null.");

	return CallWindowProc(
		dialog->linkOriginalProcs.at(wnd), wnd, msg, wParam, lParam);
}

/************************************************************************/
/* Subclassing the specified link control.                              */
/************************************************************************/
void AboutDialog::subclassLink(INT const controlId)
{
	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(controlId),
		"The window handle is not found.");

	HWND const hwndLink = this->windows.at(controlId);

	// Get the original message procedure
	WNDPROC const wndProc = reinterpret_cast<WNDPROC const>(
		::GetWindowLongPtrW(hwndLink, GWLP_WNDPROC));

	if (nullptr == wndProc)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowLongPtrW: error getting the original message procedure."));
	}

	// And save
	this->linkOriginalProcs[hwndLink] = wndProc;

	// Subclassing
	::SetWindowLongPtrW(hwndLink, GWLP_WNDPROC, 
		reinterpret_cast<LONG_PTR>(&AboutDialog::linkProc));
	::SetWindowLongPtrW(hwndLink, GWLP_USERDATA, 
		reinterpret_cast<LONG_PTR>(this));
}
