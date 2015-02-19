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
#include "settings_dialog.h"

using namespace ::com::wholegroup::vector::ps;

// Maximum possible width
int32_t const SettingsDialog::MAX_WIDTH = 131072;

// Maximum possible height
int32_t const SettingsDialog::MAX_HEIGHT = 131072;

// Default column width
double const SettingsDialog::DEFAULT_COLUMN_WIDTH = 180.0l;

// Default gutter width
double const SettingsDialog::DEFAULT_GUTTER_WIDTH = 12.0l;

// Default point size
double const SettingsDialog::DEFAULT_POINT_SIZE = 72.0l;

// Default value of the contrain proportions flag 
bool const SettingsDialog::DEFAULT_CONSTRAIN_PROPORTIONS = true;

// Default size unit
SettingsDialog::Unit const SettingsDialog::DEFAULT_SIZE_UNIT(
	SettingsDialog::POINTS);

// Default resolution unit
SettingsDialog::Unit const SettingsDialog::DEFAULT_RESOLUTION_UNIT(
	SettingsDialog::PIXELS_INCH);

// List of value units
::std::map<SettingsDialog::Unit, ::std::wstring> const SettingsDialog::VALUE_UNITS(
	::boost::assign::map_list_of(SettingsDialog::PERCENT, L"percent")
	(SettingsDialog::PIXELS, L"pixels")
	(SettingsDialog::INCHES, L"inches")
	(SettingsDialog::CM, L"cm")
	(SettingsDialog::MM, L"mm")
	(SettingsDialog::POINTS, L"points")
	(SettingsDialog::PICAS, L"picas")
	(SettingsDialog::COLUMNS, L"columns"));

// List of resolution units
::std::map<SettingsDialog::Unit, ::std::wstring> const SettingsDialog::RESOLUTION_UNITS(
	::boost::assign::map_list_of(SettingsDialog::PIXELS_INCH, L"pixels/inch")
	(SettingsDialog::PIXELS_CM, L"pixels/cm"));

// Unit precisions
::std::map<SettingsDialog::Unit, short> const SettingsDialog::UNIT_PRECISIONS(
	::boost::assign::map_list_of(SettingsDialog::PERCENT, 2)
	(SettingsDialog::PIXELS, 0)
	(SettingsDialog::INCHES, 3)
	(SettingsDialog::CM, 2)
	(SettingsDialog::MM, 2)
	(SettingsDialog::POINTS, 1)
	(SettingsDialog::PICAS, 2)
	(SettingsDialog::COLUMNS, 3)
	(SettingsDialog::PIXELS_INCH, 3)
	(SettingsDialog::PIXELS_CM, 3));

// Links of value and static
::boost::bimap<int, int> const SettingsDialog::VALUE_STATIC_LINKS(
	::boost::assign::list_of<::boost::bimap<int, int>::relation>
	(IDC_SETTINGS_WIDTH, IDC_SETTINGS_WIDTH_TITLE)
	(IDC_SETTINGS_HEIGHT, IDC_SETTINGS_HEIGHT_TITLE)
	(IDC_SETTINGS_RESOLUTION, IDC_SETTINGS_RESOLUTION_TITLE));

// Links of value and unit
::boost::bimap<int, int> const SettingsDialog::VALUE_UNIT_LINKS(
	::boost::assign::list_of<::boost::bimap<int, int>::relation>
	(IDC_SETTINGS_WIDTH, IDC_SETTINGS_WIDTH_UNIT)
	(IDC_SETTINGS_HEIGHT, IDC_SETTINGS_HEIGHT_UNIT)
	(IDC_SETTINGS_RESOLUTION, IDC_SETTINGS_RESOLUTION_UNIT));

// Value filter
::std::vector<wchar_t> const SettingsDialog::VALUE_FILTER(
	::boost::assign::list_of(0x08)('.')
	('0')('1')('2')('3')('4')('5')('6')('7')('8')('9')
	('X' - 'A' + 1)('C' - 'A' + 1)('V' - 'A' + 1)); // CTRL+X,C,V

// Dialog title
::std::wstring const SettingsDialog::TITLE = L"Rasterize %s Format";

// Image size text
::std::wstring const SettingsDialog::IMAGE_SIZE = L"Image size: %s";

// Sizes
::std::vector<::std::wstring> const SettingsDialog::SIZES(
	::boost::assign::list_of(L" bytes")(L"K")(L"M")(L"G"));

// Link colors
::std::map<int, ::std::pair<COLORREF, COLORREF>> const SettingsDialog::LINK_COLORS;

/************************************************************************/
/* Constructor.                                                         */
/************************************************************************/
SettingsDialog::SettingsDialog(HINSTANCE module)
	: module(module), 
	originalWidth(Ui::DEFAULT_WIDTH), 
	originalHeight(Ui::DEFAULT_HEIGHT),
	originalResolution(Ui::DEFAULT_RESOLUTION),
	columnWidth(DEFAULT_COLUMN_WIDTH),
	gutterWidth(DEFAULT_GUTTER_WIDTH),
	pointSize(DEFAULT_POINT_SIZE),
	constrainProportions(DEFAULT_CONSTRAIN_PROPORTIONS)
{
}

/************************************************************************/
/* Destructor.                                                          */
/************************************************************************/
SettingsDialog::~SettingsDialog()
{
}

/************************************************************************/
/* Message processing.                                                  */
/************************************************************************/
LRESULT CALLBACK SettingsDialog::dialogProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Initializing of the dialog
	if (WM_INITDIALOG == msg)
	{
		try
		{
			// Save the pointer to the dialog
			SettingsDialog * const dialog = reinterpret_cast<SettingsDialog *>(lParam);

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

	// Reference to AboutDialog
	SettingsDialog * const dialog = reinterpret_cast<SettingsDialog *>(
		::GetWindowLongPtr(wnd, GWLP_USERDATA));

	// Process a command
	switch (msg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					try
					{
						BOOST_ASSERT_MSG(nullptr != dialog, 
							"The dialog is not initialized.");

						// Validate all of the values
						for (auto item = VALUE_UNIT_LINKS.begin(); item != VALUE_UNIT_LINKS.end(); ++item)
						{
							if (!dialog->validateValue(item->left))
							{
								return 0;
							}
						}
					}
					catch (::std::exception & ex)
					{
						::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
						::EndDialog(wnd, IDABORT);
					}

					::EndDialog(wnd, IDOK);

					return 0;

				case IDCANCEL:
					::EndDialog(wnd, IDCANCEL);

					return 0;

				// Set the constrain proportions
				case IDC_SETTINGS_PROPORTIONS:
					if (BN_CLICKED == HIWORD(wParam))
					{
						try
						{
							BOOST_ASSERT_MSG(nullptr != dialog, 
								"The dialog is not initialized.");

							BOOST_ASSERT_MSG(dialog->windows.end() != 
								dialog->windows.find(IDC_SETTINGS_PROPORTIONS),
								"The window handle is not found.");

							dialog->enableProportions(BST_CHECKED == ::SendMessageW(
								dialog->windows.at(IDC_SETTINGS_PROPORTIONS), BM_GETCHECK, 0, 0));

							return 0;
						}
						catch (::std::exception & ex)
						{
							::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
							::EndDialog(wnd, IDABORT);
						}
					}
					break;

				// Change the unit
				case IDC_SETTINGS_WIDTH_UNIT:
				case IDC_SETTINGS_HEIGHT_UNIT:
				case IDC_SETTINGS_RESOLUTION_UNIT:
					if (CBN_SELCHANGE == HIWORD(wParam))
					{
						try
						{
							BOOST_ASSERT_MSG(nullptr != dialog, 
								"The dialog is not initialized.");

							dialog->changeUnit(wnd, LOWORD(wParam));
						}
						catch (::std::exception & ex)
						{
							::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
							::EndDialog(wnd, IDABORT);
						}
					}
					break;

				// Change the value
				case IDC_SETTINGS_WIDTH:
				case IDC_SETTINGS_HEIGHT:
				case IDC_SETTINGS_RESOLUTION:
					if (EN_CHANGE == HIWORD(wParam))
					{
						BOOST_ASSERT_MSG(nullptr != dialog, 
							"The dialog is not initialized.");

						if (ES_READONLY != (::GetWindowLong(reinterpret_cast<HWND>(lParam), GWL_STYLE) & ES_READONLY))
						{
							try
							{
								dialog->changeValue(LOWORD(wParam));
							}
							catch (::std::exception & ex)
							{
								::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
								::EndDialog(wnd, IDABORT);
							}
						}
					}
					break;

				default:
					break;
			}

			break;

		case WM_MEASUREITEM:
			try {
				// Get the value window
				int const unitId = static_cast<int>(wParam);

				BOOST_ASSERT_MSG(VALUE_UNIT_LINKS.right.end() != 
					VALUE_UNIT_LINKS.right.find(static_cast<int>(wParam)),
					"The unknown unit id.");

				HWND const valueWnd = ::GetDlgItem(wnd, 
					VALUE_UNIT_LINKS.right.at(static_cast<int>(wParam)));

				if (NULL == valueWnd)
				{
					BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
						L"GetDlgItem: error getting the window handle."));
				}

				// Get the rectangle of the value window
				RECT rect;

				if (FALSE == ::GetClientRect(valueWnd, &rect))
				{
					BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
						L"GetClientRect: error getting the rectangle."));
				}

				// Set the height
				reinterpret_cast<PMEASUREITEMSTRUCT>(lParam)->itemHeight = 
					(rect.bottom - rect.top) - 2;

				return TRUE;
			}
			catch (::std::exception & ex)
			{
				::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
				::EndDialog(wnd, IDABORT);
			}

			break;

		case WM_DRAWITEM:
			switch (static_cast<int>(wParam))
			{
				case IDC_SETTINGS_WIDTH_UNIT:
				case IDC_SETTINGS_HEIGHT_UNIT:
				case IDC_SETTINGS_RESOLUTION_UNIT:
					try
					{
						BOOST_ASSERT_MSG(nullptr != dialog, 
							"The dialog is not initialized.");

						dialog->paintList(static_cast<int>(wParam),
							reinterpret_cast<PDRAWITEMSTRUCT>(lParam));

						return TRUE;
					}
					catch (::std::exception & ex)
					{
						::MessageBoxA(wnd, ex.what(), "Error", MB_OK | MB_ICONERROR);
						::EndDialog(wnd, IDABORT);
					}

					break;

				default:
					break;
			}

			break;

		default:
			break;
	}

	return FALSE;
}

/************************************************************************/
/* Initialization.                                                      */
/************************************************************************/
void SettingsDialog::init(HWND const wnd)
{
	// Initialize the image size
	this->currentWidth = this->originalWidth * 
		this->pointSize / this->originalResolution;
	this->currentHeight = this->originalHeight * 
		this->pointSize / this->originalResolution;
	this->currentResolution = this->originalResolution;

	// Get the window handles
	auto const ids = ::boost::assign::list_of
		(IDC_SETTINGS_SIZE)
		(IDC_SETTINGS_SIZELINK)
		(IDC_SETTINGS_WIDTH)
		(IDC_SETTINGS_WIDTH_UNIT)
		(IDC_SETTINGS_WIDTH_TITLE)
		(IDC_SETTINGS_HEIGHT)
		(IDC_SETTINGS_HEIGHT_UNIT)
		(IDC_SETTINGS_HEIGHT_TITLE)
		(IDC_SETTINGS_RESOLUTION)
		(IDC_SETTINGS_RESOLUTION_UNIT)
		(IDC_SETTINGS_RESOLUTION_TITLE)
		(IDC_SETTINGS_PROPORTIONS);

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

	// Hand cursor
	this->handCursor = ::LoadCursorW(NULL, IDC_HAND);

	if (NULL == this->handCursor)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"LoadCursorW: error loading the hand cursor."));
	}

	// Size cursor
	this->sizeCursor.reset(new HCURSOR(
		::LoadCursorW(this->module, MAKEINTRESOURCE(IDC_CURSOR_SIZE))));

	if ((nullptr == this->sizeCursor) || (NULL == *(this->sizeCursor)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"LoadCursorW: error loading the size cursor."));
	}

	// Init fonts
	initFonts(wnd);

	// Init listboxes
	initLists();

	refreshValue(IDC_SETTINGS_WIDTH);
	refreshValue(IDC_SETTINGS_HEIGHT);
	refreshValue(IDC_SETTINGS_RESOLUTION);

	//  Init the proportions
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_SETTINGS_PROPORTIONS),
		"The window handle is not found.");

	::SendMessageW(this->windows.at(IDC_SETTINGS_PROPORTIONS), BM_SETCHECK, 
		this->constrainProportions ? BST_CHECKED : BST_UNCHECKED, 0);

	enableProportions(this->constrainProportions);

	// Set the dialog title
	BOOST_ASSERT_MSG(::std::wstring::npos != TITLE.find(L"%s"), 
		"Invalid template.");

	if (FALSE == ::SetWindowTextW(wnd, ::boost::replace_first_copy(
		TITLE, L"%s", ::com::wholegroup::vector::ps::Version::FORMAT()).c_str()))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetWindowTextW: error setting the dialog title."));
	}

	// Calculate the image size
	calculateSize();

	// Subclass edit boxes 
	subclassValue(wnd, IDC_SETTINGS_WIDTH);
	subclassValue(wnd, IDC_SETTINGS_HEIGHT);
	subclassValue(wnd, IDC_SETTINGS_RESOLUTION);

	// Subclass static controls
	subclassStatic(wnd, IDC_SETTINGS_WIDTH_TITLE);
	subclassStatic(wnd, IDC_SETTINGS_HEIGHT_TITLE);
	subclassStatic(wnd, IDC_SETTINGS_RESOLUTION_TITLE);

	// Save the original size of the dialog
	if (FALSE == ::GetWindowRect(wnd, &originalRect))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowRect: error getting the dialog rectangle."));
	}
}

/************************************************************************/
/* Initialization fonts.                                                */
/************************************************************************/
void SettingsDialog::initFonts(HWND const wnd)
{
	// Get the current dialog font
	HFONT wndFont = reinterpret_cast<HFONT>(::SendMessage(wnd, WM_GETFONT, 0, 0));

	if (NULL == wndFont)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessage: error getting the current font."));
	}
}

/************************************************************************/
/* Initializition listboxes.                                            */
/************************************************************************/
void SettingsDialog::initLists() const
{
	// Get the window handles (width and height)
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_SETTINGS_WIDTH_UNIT),
		"The window handle is not found.");
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_SETTINGS_HEIGHT_UNIT),
		"The window handle is not found.");

	HWND const windowWidthUnits  = this->windows.at(IDC_SETTINGS_WIDTH_UNIT);
	HWND const windowHeightUnits = this->windows.at(IDC_SETTINGS_HEIGHT_UNIT);

	// Adding the values to the lists
	for(auto unit = VALUE_UNITS.cbegin(); VALUE_UNITS.cend() != unit; ++unit)
	{
		addToList(windowWidthUnits, unit->second, unit->first, DEFAULT_SIZE_UNIT);

		if (COLUMNS != unit->first)
		{
			addToList(windowHeightUnits, unit->second, unit->first, DEFAULT_SIZE_UNIT);
		}
	}

	// Get the window handles (resolution)
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_SETTINGS_RESOLUTION_UNIT),
		"The window handle is not found.");

	HWND const windowResolutionUnits = this->windows.at(IDC_SETTINGS_RESOLUTION_UNIT);

	// Adding the values to the resolution list
	for(auto resolution = RESOLUTION_UNITS.cbegin(); 
		RESOLUTION_UNITS.cend() != resolution; ++resolution)
	{
		addToList(windowResolutionUnits, resolution->second, 
			resolution->first, DEFAULT_RESOLUTION_UNIT);
	}
}

/************************************************************************/
/* Add the string to the list.                                          */
/************************************************************************/
void SettingsDialog::addToList(HWND const list, ::std::wstring const value, 
	Unit const unit, Unit const defaultUnit) const
{
	// Add the new string
	int const index = static_cast<int>(::SendMessageW(list, 
		CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(value.c_str())));

	if (CB_ERR == index)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error adding the new string."));
	}

	// Set the unit id
	if (CB_ERR == ::SendMessageW(list, CB_SETITEMDATA, index, 
		static_cast<LPARAM>(unit)))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error setting the unit id."));
	}

	// Set the selected value (resolution)
	if (defaultUnit == unit)
	{
		if (CB_ERR == ::SendMessageW(list, CB_SETCURSEL, index, 0))
		{
			BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
				L"SendMessageW: error setting the selected value."));
		}
	}
}

/************************************************************************/
/* Subclassing the specified edit control of units.                     */
/************************************************************************/
void SettingsDialog::subclassValue(HWND const wnd, int const valueId)
{
	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(valueId),
		"The window handle is not found.");

	HWND const valueWnd = this->windows.at(valueId);

	// Get the original message procedure
	WNDPROC const originalProc = reinterpret_cast<WNDPROC const>(
		::GetWindowLongPtrW(valueWnd, GWLP_WNDPROC));

	if (nullptr == originalProc)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowLongPtrW: error getting the original message procedure."));
	}

	// And save
	this->editUnitsOriginalProcs[valueWnd] = originalProc;

	// Subclassing
	::SetWindowLongPtrW(valueWnd, GWLP_WNDPROC, 
		reinterpret_cast<LONG_PTR>(&SettingsDialog::valueProc));
	::SetWindowLongPtrW(valueWnd, GWLP_USERDATA, 
		reinterpret_cast<LONG_PTR>(this));
}

/************************************************************************/
/* Message processing of edit controls of units.                        */
/************************************************************************/
LRESULT CALLBACK SettingsDialog::valueProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Get the dialog pointer
	SettingsDialog * const dialog = reinterpret_cast<SettingsDialog * const>(
		GetWindowLongPtrW(wnd, GWLP_USERDATA));

	BOOST_ASSERT_MSG(nullptr != dialog, 
		"The pointer to the dialog is null.");

	// Process the message
	switch(msg)
	{
		// Character filter
		case WM_CHAR:
			if (VALUE_FILTER.end() == ::std::find(
				VALUE_FILTER.begin(), VALUE_FILTER.end(), wParam))
			{
				::MessageBeep(MB_OK);

				return 0;
			}

			break;

		// Shift the value
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_UP:
				case VK_DOWN:
					try
					{
						// Get the control id
						int const valueId = ::GetDlgCtrlID(wnd);

						if (0 == valueId)
						{
							BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
								L"GetDlgCtrlID: error getting the control id."));
						}

						// Increase/Decrease
						if (VK_UP == wParam)
						{
							dialog->increaseValue(valueId,
								(0 != (::GetKeyState(VK_SHIFT) & 0x8000)) ? true : false);
						}
						else if (VK_DOWN == wParam)
						{
							dialog->decreaseValue(valueId,
								(0 != (::GetKeyState(VK_SHIFT) & 0x8000)) ? true : false);
						}

						return 0;
					}
					catch (::std::exception & ex)
					{
						UNREFERENCED_PARAMETER(ex);
					}

					break;

				default:
					break;
			}

			break;

		// Validate after killing focus
		case WM_KILLFOCUS:
			try
			{
				// Get the control id
				int const valueId = ::GetDlgCtrlID(wnd);

				if (0 == valueId)
				{
					BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
						L"GetDlgCtrlID: error getting the control id."));
				}

				// Get the parent window
				HWND const dialogWnd = ::GetParent(wnd);

				if (NULL == dialogWnd)
				{
					BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
						L"GetParent: error getting the parent window."));
				}

				// Validate
				HWND const focusWnd = reinterpret_cast<HWND>(wParam);
				int  const focusId  = ::GetDlgCtrlID(focusWnd);
				
				if ((NULL != focusWnd) && ::IsChild(dialogWnd, focusWnd) &&
					(IDOK != focusId) && (IDCANCEL != focusId) &&
					!dialog->validateValue(valueId))
				{
					return 0;
				}
			}
			catch (::std::exception & ex)
			{
				UNREFERENCED_PARAMETER(ex);
			}

			break;

		default:
			break;
	}

	// Call the original message procedure
	BOOST_ASSERT_MSG(dialog->editUnitsOriginalProcs.end() != dialog->editUnitsOriginalProcs.find(wnd), 
		"The original function is is null.");

	return CallWindowProc(
		dialog->editUnitsOriginalProcs.at(wnd), wnd, msg, wParam, lParam);
}

/************************************************************************/
/* Subclassing the specified static control of units.                   */
/************************************************************************/
void SettingsDialog::subclassStatic(HWND const wnd, int const staticId)
{
	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(staticId),
		"The window handle is not found.");

	HWND const staticWnd = this->windows.at(staticId);

	// Get the original message procedure
	WNDPROC const originalProc = reinterpret_cast<WNDPROC const>(
		::GetWindowLongPtrW(staticWnd, GWLP_WNDPROC));

	if (nullptr == originalProc)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowLongPtrW: error getting the original message procedure."));
	}

	// And save
	this->staticUnitsOriginalProcs[staticWnd] = originalProc;
	this->staticUnitsMouseX[staticWnd] = 0;

	// Subclassing
	::SetWindowLongPtrW(staticWnd, GWLP_WNDPROC, 
		reinterpret_cast<LONG_PTR>(&SettingsDialog::staticProc));
	::SetWindowLongPtrW(staticWnd, GWLP_USERDATA, 
		reinterpret_cast<LONG_PTR>(this));
}

/************************************************************************/
/* Message processing of static controls of units.                      */
/************************************************************************/
LRESULT CALLBACK SettingsDialog::staticProc(HWND wnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	// Get the dialog pointer
	SettingsDialog * const dialog = reinterpret_cast<SettingsDialog * const>(
		GetWindowLongPtrW(wnd, GWLP_USERDATA));

	BOOST_ASSERT_MSG(nullptr != dialog, 
		"The pointer to the dialog is null.");

	// Process the message
	switch(uiMsg)
	{
		// Set the hand cursor
		case WM_SETCURSOR:
			::SetCursor(*(dialog->sizeCursor));
			return TRUE;

		// Capture the mouse cursor
		case WM_LBUTTONDOWN:
			// Capture
			::SetCapture(wnd);

			// Save the position
			dialog->staticUnitsMouseX[wnd] = LOWORD(lParam);

			return 0;

		// Relese the mouse cursor
		case WM_LBUTTONUP:
			::ReleaseCapture();
			return 0;

		// Change values of units
		case WM_MOUSEMOVE:
			if (wnd == ::GetCapture())
			{
				try
				{
					// Get the control id
					int const staticId = ::GetDlgCtrlID(wnd);

					if (0 == staticId)
					{
						BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
							L"GetDlgCtrlID: error getting the control id."));
					}

					// Get the value id
					BOOST_ASSERT_MSG(VALUE_STATIC_LINKS.right.end() != VALUE_STATIC_LINKS.right.find(staticId), 
						"The link with the title is not found.");

					int const valueId = VALUE_STATIC_LINKS.right.at(staticId);

					// Increase/Descrease the value
					int const oldX = dialog->staticUnitsMouseX.at(wnd);
					int const newX = LOWORD(lParam); 

					if (newX > oldX)
					{
						dialog->increaseValue(valueId,
							(0 != (wParam & MK_SHIFT)) ? true : false);
					}
					else if (newX < oldX)
					{
						dialog->decreaseValue(valueId,
							(0 != (wParam & MK_SHIFT)) ? true : false);
					}

					// Save the new cursor position
					dialog->staticUnitsMouseX[wnd] = newX;

					return 0;
				}
				catch (::std::exception & ex)
				{
					UNREFERENCED_PARAMETER(ex);
				}
			}

			break;

		default:
			break;
	}

	BOOST_ASSERT_MSG(dialog->staticUnitsOriginalProcs.end() != dialog->staticUnitsOriginalProcs.find(wnd), 
		"The original function is is null.");

	return CallWindowProc(
		dialog->staticUnitsOriginalProcs.at(wnd), wnd, uiMsg, wParam, lParam);
}

/************************************************************************/
/* Subclassing the specified link control of units.                     */
/************************************************************************/
void SettingsDialog::subclassLink(int const linkId)
{
	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(linkId),
		"The window handle is not found.");

	HWND const linkWnd = this->windows.at(linkId);

	// Get the original message procedure
	WNDPROC const originalProc = reinterpret_cast<WNDPROC const>(
		::GetWindowLongPtrW(linkWnd, GWLP_WNDPROC));

	if (nullptr == originalProc)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowLongPtrW: error getting the message procedure."));
	}

	// And save
	this->linkOriginalProcs[linkWnd] = originalProc;

	// Subclassing
	::SetWindowLongPtrW(linkWnd, GWLP_WNDPROC, 
		reinterpret_cast<LONG_PTR>(&SettingsDialog::linkProc));
	::SetWindowLongPtrW(linkWnd, GWLP_USERDATA, 
		reinterpret_cast<LONG_PTR>(this));
}

/************************************************************************/
/* Message processing of button-link controls.                          */
/************************************************************************/
LRESULT CALLBACK SettingsDialog::linkProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Get the dialog pointer
	SettingsDialog * const dialog = reinterpret_cast<SettingsDialog * const>(
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
			try
			{
				// Get the link id
				int const linkId = ::GetDlgCtrlID(wnd);

				if (0 == linkId)
				{
					BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
						L"GetDlgCtrlID: error getting the control id."));
				}

				// Run tracking
				BOOST_ASSERT_MSG(dialog->linkHovers.end() != dialog->linkHovers.find(linkId), 
					"The unknown link id.");

				if (!dialog->linkHovers.at(linkId))
				{
					TRACKMOUSEEVENT tme = { sizeof(tme) };

					tme.dwFlags   = TME_LEAVE;
					tme.hwndTrack = wnd;

					if (TRUE == ::TrackMouseEvent(&tme))
					{
						dialog->linkHovers[linkId] = true;

						::InvalidateRect(wnd, NULL, TRUE);
						::UpdateWindow(wnd);
					}
				}
			}
			catch (::std::exception & ex)
			{
				UNREFERENCED_PARAMETER(ex);
			}

			break;

			// Notice of leaving mouse
		case WM_MOUSELEAVE:
			try
			{
				// Get the link id
				int const linkId = ::GetDlgCtrlID(wnd);

				if (0 == linkId)
				{
					BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
						L"GetDlgCtrlID: error getting the control id."));
				}

				// Stop tracking
				BOOST_ASSERT_MSG(dialog->linkHovers.end() != dialog->linkHovers.find(linkId), 
					"The unknown link id.");

				dialog->linkHovers.at(linkId) = false;

				::InvalidateRect(wnd, NULL, TRUE);
				::UpdateWindow(wnd);
			}
			catch (::std::exception & ex)
			{
				UNREFERENCED_PARAMETER(ex);
			}

			break;

		default:
			break;
	}

	// Call the original message procedure
	BOOST_ASSERT_MSG(dialog->linkOriginalProcs.end() != dialog->linkOriginalProcs.find(wnd),
		"The original function is null.");

	return CallWindowProc(
		dialog->linkOriginalProcs.at(wnd), wnd, msg, wParam, lParam);
}

/************************************************************************/
/* Paint a combobox item.                                               */
/************************************************************************/
void SettingsDialog::paintList(int const listId, PDRAWITEMSTRUCT const item) const
{
	// Get the value id
	BOOST_ASSERT_MSG(VALUE_UNIT_LINKS.right.end() != VALUE_UNIT_LINKS.right.find(listId),
		"The link with an editbox is not found.");

	int const valueId = VALUE_UNIT_LINKS.right.at(listId);

	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(valueId),
		"The window handle is not found.");

	HWND const valueWnd = this->windows.at(valueId);

	// Get info of the value window
	WINDOWINFO valueWndInfo = { sizeof(WINDOWINFO) };

	if (FALSE == ::GetWindowInfo(valueWnd, &valueWndInfo))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowInfo: error getting the window info."));
	}

	// Get the margins of the value window
	DWORD margins = static_cast<DWORD>(
		::SendMessageW(valueWnd, EM_GETMARGINS, 0, 0));

	// Get the list info
	COMBOBOXINFO listInfo = { sizeof(COMBOBOXINFO) };

	if (FALSE == ::GetComboBoxInfo(item->hwndItem, &listInfo))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetComboBoxInfo: error getting the combobox info."));
	}

	// Get info about the listbox window of the list
	WINDOWINFO listboxWndInfo = { sizeof(WINDOWINFO) };

	if (FALSE == ::GetWindowInfo(listInfo.hwndList, &listboxWndInfo))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowInfo: error getting the window info."));
	}

	// Calculate the offsets
	int const x = (valueWndInfo.rcClient.left - valueWndInfo.rcWindow.left) -
		(ODS_COMBOBOXEDIT != (ODS_COMBOBOXEDIT & item->itemState) ? 
		(listboxWndInfo.rcClient.left - listboxWndInfo.rcWindow.left) : 0)
		+ HIWORD(margins) + 1;
	int const y = item->rcItem.top;

	// Get the item text
	int const itemLength = static_cast<int>(
		::SendMessageW(item->hwndItem, CB_GETLBTEXTLEN, item->itemID, 0));

	if (CB_ERR == itemLength)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error getting the length of the listbox item."));
	}

	::std::vector<wchar_t> itemBuffer(itemLength + 1, 0);

	if (CB_ERR == ::SendMessageW(item->hwndItem, CB_GETLBTEXT,
		item->itemID, (LPARAM)&itemBuffer[0]))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error getting the text of the listbox item."));
	}

	// The colors depend on whether the item is selected.
	COLORREF const clrForeground = ::SetTextColor(item->hDC, 
		::GetSysColor((ODS_SELECTED == (ODS_SELECTED & item->itemState)) ? 
			COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

	if (CLR_INVALID == clrForeground)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetTextColor: error setting the foreground color."));
	}

	COLORREF const clrBackground = ::SetBkColor(item->hDC, 
		::GetSysColor((ODS_SELECTED == (ODS_SELECTED & item->itemState)) ? 
			COLOR_HIGHLIGHT : COLOR_WINDOW));

	if (CLR_INVALID == clrBackground)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetBkColor: error setting the background color."));
	}

	// Display the text for the list item.
	if (FALSE == ::ExtTextOutW(item->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, &item->rcItem,
		&itemBuffer[0], static_cast<UINT>(itemBuffer.size() - 1), NULL))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"ExtTextOutW: error drawing the item text."));
	}

	// Restore the previous colors
	if (CLR_INVALID == ::SetTextColor(item->hDC, clrForeground))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetTextColor: error setting the foreground color."));
	}
	
	if (CLR_INVALID == ::SetBkColor(item->hDC, clrBackground))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetBkColor: error setting the background color."));
	}

	// Draw a focus rectangle if needed.
	if (ODS_FOCUS == (ODS_FOCUS & item->itemState))
	{
		if (FALSE == ::DrawFocusRect(item->hDC, &item->rcItem))
		{
			BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
				L"DrawFocusRect: error drawing a focus rectangle."));
		}
	}
}

/************************************************************************/
/* Paint a link-button.                                                 */
/************************************************************************/
void SettingsDialog::paintLink(int const linkId, PDRAWITEMSTRUCT const item) const
{
	// Get the link text
	BOOST_ASSERT_MSG(this->linkTexts.end() != this->linkTexts.find(linkId), 
		"The link text is not found.");

	::std::wstring const linkText = this->linkTexts.at(linkId);

	// Get the link styles
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(linkId),
		"The window handle is not found.");

	LONG_PTR const gwlStyle = ::GetWindowLongPtrW(this->windows.at(linkId), GWL_STYLE);

	if (0 == gwlStyle)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"GetWindowLongPtrW: error getting the link styles."));
	}

	// Set the font
	BOOST_ASSERT_MSG(this->linkFonts.end() != this->linkFonts.find(linkId),
		"The font is not found.");

	HGDIOBJ const savedFont = ::SelectObject(
		item->hDC, *(this->linkFonts.at(linkId)));

	if (NULL == savedFont)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SelectObject: error setting the link font."));
	}

	// Get size of the link text
	SIZE size;

	::GetTextExtentPoint32(item->hDC, linkText.c_str(), 
		static_cast<int>(linkText.length()), &size);

	// Calculate the text position
	RECT rect(item->rcItem);

	if (BS_LEFT != (gwlStyle & BS_LEFT))
	{
		rect.left += ((rect.right - rect.left) - size.cx) / 2;
	}

	rect.top += ((rect.bottom - rect.top) - size.cy) / 2;

	// Set the foreground color
	BOOST_ASSERT_MSG(this->linkHovers.end() != this->linkHovers.find(linkId), 
		"The link hover flag is not found.");
	BOOST_ASSERT_MSG(LINK_COLORS.end() != LINK_COLORS.find(linkId), 
		"The text color is not found.");

	COLORREF const clrForeground = ::SetTextColor(item->hDC, this->linkHovers.at(linkId) ? 
		LINK_COLORS.at(linkId).second : LINK_COLORS.at(linkId).first);

	if (CLR_INVALID == clrForeground)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetTextColor: error setting the foreground color."));
	}

	// Set the background color
	COLORREF const clrBackground = ::SetBkColor(
		item->hDC, ::GetSysColor(COLOR_3DFACE));

	if (CLR_INVALID == clrBackground)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetBkColor: error setting the background color."));
	}

	// Draw the text
	if (FALSE == ::ExtTextOutW(item->hDC, rect.left, rect.top, 
		ETO_CLIPPED | ETO_OPAQUE, &item->rcItem, linkText.c_str(), 
		static_cast<int>(linkText.length()), NULL))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"ExtTextOut: error drawing the text."));
	}

	// Restore the previous colors
	if (CLR_INVALID == ::SetTextColor(item->hDC, clrForeground))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetTextColor: error setting the foreground color."));
	}

	if (CLR_INVALID == ::SetBkColor(item->hDC, clrBackground))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetBkColor: error setting the background color."));
	}

	// Restore the previous font
	if (NULL == ::SelectObject(item->hDC, savedFont))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SelectObject: error setting the font."));
	}

	// Draw a focus rectangle if need
	if (ODS_FOCUS == (item->itemState & ODS_FOCUS))
	{
		::DrawFocusRect(item->hDC, &item->rcItem);
	}
}

/************************************************************************/
/* Calculate the image size.                                            */
/************************************************************************/
void SettingsDialog::calculateSize()
{
	BOOST_ASSERT_MSG(0 < SIZES.size(), "Array is empty.");

	// Get the short view of the size
	double shortSize = floor(this->currentWidth * this->currentResolution / this->pointSize + 0.5l) *
		floor(this->currentHeight * this->currentResolution / this->pointSize + 0.5l) * 3.0l; 

	unsigned int power = 0;

	while ((1024 <= shortSize) && (power < (SIZES.size() - 1)))
	{
		shortSize /= 1024;

		power++;
	}

	// Format the size 
	::std::wostringstream formatStream;

	formatStream << ::std::fixed << ::std::setprecision(1) << shortSize;

	::std::wstring stringSize(formatStream.str());

	::boost::trim_right_if(stringSize, ::boost::is_any_of(L"0"));
	::boost::trim_right_if(stringSize, ::boost::is_any_of(L"."));

	// Set the new text
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_SETTINGS_SIZE),
		"The window handle is not found.");

	BOOST_ASSERT_MSG(::std::wstring::npos != IMAGE_SIZE.find(L"%s"), 
		"Invalid template.");

	if (FALSE == ::SetWindowTextW(this->windows.at(IDC_SETTINGS_SIZE), 
		::boost::replace_first_copy(IMAGE_SIZE, L"%s", 
		stringSize + SIZES[power]).c_str()))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetWindowTextW: error setting the text."));
	}
}

/************************************************************************/
/* Get the value.                                                       */
/************************************************************************/
double SettingsDialog::getValue(int const valueId) const
{
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(valueId),
		"The window handle is not found.");

	return ::boost::lexical_cast<double>(
		UiWin::getWindowText(this->windows.at(valueId)));
}

/************************************************************************/
/* Get the original value.                                              */
/************************************************************************/
double SettingsDialog::getOriginalValue(int const valueId) const
{
	switch (valueId)
	{
		case IDC_SETTINGS_WIDTH:
			return this->originalWidth * 
				this->pointSize / this->originalResolution;

		case IDC_SETTINGS_HEIGHT:
			return this->originalHeight * 
				this->pointSize / this->originalResolution;

		case IDC_SETTINGS_RESOLUTION:
			return this->originalResolution;

		default:
			BOOST_ASSERT_MSG(false, "The unknown value id.");
			break;
	}

	return 0.l;
}

/************************************************************************/
/* Get the minimum value.                                               */
/************************************************************************/
double SettingsDialog::getMinimum(int const valueId) const
{
	switch (valueId)
	{
		case IDC_SETTINGS_WIDTH:
		case IDC_SETTINGS_HEIGHT:
			return this->pointSize / this->currentResolution;

		case IDC_SETTINGS_RESOLUTION:
			return 1.0;

		default:
			BOOST_ASSERT_MSG(false, "The unknown value id.");
			break;
	}

	return 0.l;
}

/************************************************************************/
/* Get the maximum value.                                               */
/************************************************************************/
double SettingsDialog::getMaximum(int const valueId) const
{
	// Get the proportions
	double const maxProportions = static_cast<double>(MAX_WIDTH) / static_cast<double>(MAX_HEIGHT);

	double const proportions = static_cast<double>(this->originalWidth) / 
		static_cast<double>(this->originalHeight);

	// Maximum values of the sides in pixels
	double const maximumWidth = this->constrainProportions && (maxProportions > proportions) ? 
		(MAX_WIDTH) / maxProportions * proportions : (MAX_WIDTH);

	double const maximumHeight = this->constrainProportions && (maxProportions < proportions) ? 
		(MAX_HEIGHT) * maxProportions / proportions : (MAX_HEIGHT);

	// Maximum
	switch (valueId)
	{
		case IDC_SETTINGS_WIDTH:
			return maximumWidth * this->pointSize / this->currentResolution;

		case IDC_SETTINGS_HEIGHT:
			return maximumHeight * this->pointSize / this->currentResolution;

		case IDC_SETTINGS_RESOLUTION:
			return min(maximumWidth / this->currentWidth, 
				maximumHeight / currentHeight) * this->pointSize;

		default:
			BOOST_ASSERT_MSG(false, "The unknown value id.");
			break;
	}

	return 0.l;
}

/************************************************************************/
/* Set the value.                                                       */
/************************************************************************/
void SettingsDialog::setValue(int const valueId, double const value)
{
	// Check minimum and maximum
	double newValue = value;

	double const minimum = getMinimum(valueId);
	double const maximum = getMaximum(valueId);

	if (minimum > newValue)
	{
		newValue = minimum;
	}

	if (maximum < newValue)
	{
		newValue = maximum;
	}

	// Set the new value
	switch (valueId)
	{
		case IDC_SETTINGS_WIDTH:
			this->currentWidth = newValue;

			if (this->constrainProportions)
			{
				this->currentHeight = this->currentWidth / 
					this->originalWidth * this->originalHeight;

				refreshValue(IDC_SETTINGS_HEIGHT);
			}

			break;

		case IDC_SETTINGS_HEIGHT:
			this->currentHeight = newValue;

			if (this->constrainProportions)
			{
				this->currentWidth = this->currentHeight * 
					this->originalWidth / this->originalHeight;

				refreshValue(IDC_SETTINGS_WIDTH);
			}

			break;

		case IDC_SETTINGS_RESOLUTION:
			this->currentResolution = newValue;

			refreshValue(IDC_SETTINGS_WIDTH);
			refreshValue(IDC_SETTINGS_HEIGHT);

			break;

		default:
			BOOST_ASSERT_MSG(false, "The unknown value id.");
			break;
	}

	// Recalculate the image size
	calculateSize();
}

/************************************************************************/
/* Refresh the value.                                                   */
/************************************************************************/
void SettingsDialog::refreshValue(int const valueId) const
{
	// Get the unit type
	BOOST_ASSERT_MSG(VALUE_UNIT_LINKS.left.end() != VALUE_UNIT_LINKS.left.find(valueId),
		"The link with an editbox is not found.");

	Unit const unit = getUnit(VALUE_UNIT_LINKS.left.at(valueId));

	// Get the value
	double value;

	switch (valueId)
	{
		case IDC_SETTINGS_WIDTH:
			value = toUnit(valueId, this->currentWidth, unit);
			break;

		case IDC_SETTINGS_HEIGHT:
			value = toUnit(valueId, this->currentHeight, unit);
			break;

		case IDC_SETTINGS_RESOLUTION:
			value = toUnit(valueId, this->currentResolution, unit);
			break;

		default:
			BOOST_ASSERT_MSG(false, "The unknown value id.");
			break;
	}

	// Update
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(valueId),
		"The window handle is not found.");

	HWND const wndValue = this->windows.at(valueId);

	if (0 ==::SendMessageW(wndValue, EM_SETREADONLY, TRUE, 0))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error setting the readonly mode."));
	}

	if (FALSE == ::SetWindowTextW(wndValue, formatValue(value, 
		toUnit(valueId, getMinimum(valueId), unit), 
		toUnit(valueId, getMaximum(valueId), unit), 
		unit).c_str()))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SetWindowTextW: error setting the window text."));
	}

	if (0 == ::SendMessageW(wndValue, EM_SETREADONLY, FALSE, 0))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error setting the readonly mode."));
	}
}

/************************************************************************/
/* Select all text of the value.                                        */
/************************************************************************/
void SettingsDialog::selectValue(int const valueId) const
{
	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(valueId),
		"The window handle is not found.");

	HWND const valueWnd = this->windows.at(valueId);

	// Focus on the window
	if (NULL == ::SetFocus(valueWnd))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"Error setting focus."));
	}

	// Select the text
	if (FALSE == ::PostMessageW(valueWnd, EM_SETSEL, 0, -1))
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"Error selecting value."));
	}
}

/************************************************************************/
/* Select an item value.                                                */
/************************************************************************/
bool SettingsDialog::changeValue(int const valueId)
{
	// Get the unit
	BOOST_ASSERT_MSG(VALUE_UNIT_LINKS.left.end() != VALUE_UNIT_LINKS.left.find(valueId),
		"The link with an editbox is not found.");

	Unit const unit = getUnit(VALUE_UNIT_LINKS.left.at(valueId));

	// Get the input value
	double newValue; 

	try
	{
		newValue = fromUnit(valueId, getValue(valueId), unit);
	}
	catch (::boost::bad_lexical_cast & ex)
	{
		UNREFERENCED_PARAMETER(ex);

		return false;
	}

	// Check the minimum and the maximum
	if ((getMinimum(valueId) > newValue) || 
		(getMaximum(valueId) < newValue))
	{
		return false;
	}

	// Use the new value
	setValue(valueId, newValue);

	return true;
}

/************************************************************************/
/* Increase the value.                                                  */
/************************************************************************/
bool SettingsDialog::increaseValue(int const valueId, bool const orderMagnitude)
{
	return stepValue(valueId, (orderMagnitude ? 1 : 0.1l));
}

/************************************************************************/
/* Decrease the value.                                                  */
/************************************************************************/
bool SettingsDialog::decreaseValue(int const valueId, bool const orderMagnitude)
{
	return stepValue(valueId, -(orderMagnitude ? 1 : 0.1l));
}

/************************************************************************/
/* Step the value.                                                      */
/************************************************************************/
bool SettingsDialog::stepValue(int const valueId, double const step)
{
	// Get the unit
	BOOST_ASSERT_MSG(VALUE_UNIT_LINKS.left.end() != VALUE_UNIT_LINKS.left.find(valueId),
		"The link with an editbox is not found.");

	Unit const unit = getUnit(VALUE_UNIT_LINKS.left.at(valueId));

	// Get the precision
	BOOST_ASSERT_MSG(UNIT_PRECISIONS.end() != UNIT_PRECISIONS.find(unit), 
		"The unit type is not found.");

	int const precision = UNIT_PRECISIONS.at(unit);

	// Set the new value
	double value;

	try
	{
		value = getValue(valueId);
	}
	catch (::boost::bad_lexical_cast & ex)
	{
		UNREFERENCED_PARAMETER(ex);

		return false;
	}

	setValue(valueId, fromUnit(valueId, value + 
		step * (0 == precision ? 10 : 1), unit));

	// Refresh and select
	refreshValue(valueId);
	selectValue(valueId);

	return true;
}

/************************************************************************/
/* Validate the value.                                                  */
/************************************************************************/
bool SettingsDialog::validateValue(int const valueId)
{
	// Get the window handle
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(valueId),
		"The window handle is not found.");

	HWND const valueWnd = this->windows.at(valueId);

	// Get the unit type
	BOOST_ASSERT_MSG(VALUE_UNIT_LINKS.left.end() != VALUE_UNIT_LINKS.left.find(valueId),
		"The link with an combobox is not found.");

	Unit const unit = getUnit(VALUE_UNIT_LINKS.left.at(valueId));

	// Get the minimum and the maximum
	double const minimum = toUnit(valueId, getMinimum(valueId), unit);
	double const maximum = toUnit(valueId, getMaximum(valueId), unit);

	// Parse the value
	double itemValue;

	try
	{
		itemValue = getValue(valueId);
	}
	catch (::boost::bad_lexical_cast & ex)
	{
		UNREFERENCED_PARAMETER(ex);

		// Show an error message
		::std::wstring msg = L"Invalid numeric entry. A number between ";

		msg += formatValue(minimum, minimum, maximum, unit, false);
		msg += L"\nand ";
		msg += formatValue(maximum, minimum, maximum, unit, false);
		msg += L" is required. Previous value\ninserted.";

		::MessageBoxW(valueWnd, msg.c_str(), 
			UiWin::getWindowText(valueWnd).c_str(), MB_OK | MB_ICONERROR);

		// Insert and select the previous value.
		refreshValue(valueId);
		selectValue(valueId);

		return false;
	}

	// Check minimum and maximum
	if ((minimum > itemValue) || (maximum < itemValue))
	{
		::std::wstring const formattedMinimum = formatValue(
			minimum, minimum, maximum, unit, false);
		::std::wstring const formattedMaximum = formatValue(
			maximum, minimum, maximum, unit, false);

		// Show the error message
		::std::wstring msg = L"A number between ";

		msg += formattedMinimum;
		msg += L" and ";
		msg += formattedMaximum;
		msg += L" is required.\nClosest value inserted.";

		::MessageBoxExW(valueWnd, msg.c_str(), 
			UiWin::getWindowText(valueWnd).c_str(), MB_OK | MB_ICONERROR, 0);

		// Insert, refresh and select
		setValue(valueId, fromUnit(valueId, ::boost::lexical_cast<double>(
			maximum < itemValue ? formattedMaximum : formattedMinimum), unit));

		refreshValue(valueId);
		selectValue(valueId);

		return false;
	}

	return true;
}

/************************************************************************/
/* Format the value.                                                    */
/************************************************************************/
::std::wstring SettingsDialog::formatValue(double const value, 
	double const minimum, double const maximum,
	Unit const unit, bool const trim) const
{
	// Get the precision
	BOOST_ASSERT_MSG(UNIT_PRECISIONS.end() != UNIT_PRECISIONS.find(unit),
		"The unit type is not found.");

	int const precision = UNIT_PRECISIONS.at(unit);

	// Borders
	double const tens = pow(10.0l, precision);

	double const minimumBorder = ceil(minimum * tens);
	double const maximumBorder = floor(maximum * tens);

	double validValue = floor(value * tens + 0.5l);

	if (minimumBorder > validValue)
	{
		validValue = minimumBorder;
	}

	if (maximumBorder < validValue)
	{
		validValue = maximumBorder;
	}

	validValue /= tens;

	// To string
	::std::wostringstream formatStream;

	formatStream << ::std::fixed << ::std::setprecision(precision) << validValue;

	::std::wstring result(formatStream.str());

	// Trim
	if (trim && (0 < precision))
	{
		::boost::trim_right_if(result, ::boost::is_any_of(L"0"));
		::boost::trim_right_if(result, ::boost::is_any_of(L"."));
	}

	return result;
}

/************************************************************************/
/* Get the selected unit.                                               */
/************************************************************************/
SettingsDialog::Unit SettingsDialog::getUnit(int const unitId) const
{
	// Get the dialog item
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(unitId),
		"The window handle is not found.");

	HWND const unitWnd = this->windows.at(unitId);

	// Get the selected id
	LRESULT const selectedId = ::SendMessageW(unitWnd, CB_GETCURSEL, 0, 0);

	if (CB_ERR == selectedId)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error getting the selected item."));
	}

	// Get the type of the selected item
	LRESULT const unit = ::SendMessageW(unitWnd, 
		CB_GETITEMDATA, selectedId, 0);

	if (CB_ERR == unit)
	{
		BOOST_THROW_EXCEPTION(exception::WinApiException() << exception::ErrorMessage(
			L"SendMessageW: error getting the item data."));
	}

	return static_cast<Unit>(unit);
}

/************************************************************************/
/* Change the unit.                                                     */
/************************************************************************/
void SettingsDialog::changeUnit(HWND const wnd, int const unitId)
{
	// Get the value id
	BOOST_ASSERT_MSG(VALUE_UNIT_LINKS.right.end() != VALUE_UNIT_LINKS.right.find(unitId), 
		"The link with an editbox is not found.");

	int const valueId = VALUE_UNIT_LINKS.right.at(unitId);

	// Refresh and select the value
	refreshValue(valueId);
	selectValue(valueId);
}

/************************************************************************/
/* Convert the value from the specified unit.                           */
/************************************************************************/
double SettingsDialog::fromUnit(int const valueId, double const value,
	Unit const unit) const
{
	switch (unit)
	{
		case PERCENT:
			return value / 100.l * getOriginalValue(valueId);

		case PIXELS:
			return value / this->currentResolution * this->pointSize;

		case INCHES:
			return value * this->pointSize;

		case CM:
			return value * this->pointSize / 2.54l;

		case MM:
			return value * this->pointSize / 2.54l / 10.l;

		case POINTS:
			return value;

		case PICAS:
			return value * 12.l;

		case COLUMNS:
			return value * (this->columnWidth + this->gutterWidth) - 
				this->gutterWidth;

		case PIXELS_INCH:
			return value;
			break;

		case PIXELS_CM:
			return value * 2.54l;
			break;

		default:
			BOOST_ASSERT_MSG(false, "The unknown unit.");
			break;
	}

	return value;
}

/************************************************************************/
/* Convert to an item value.                                            */
/************************************************************************/
double SettingsDialog::toUnit(int const valueId, double const value, 
	Unit const unit) const
{
	switch (unit)
	{
		case PERCENT:
			return value / getOriginalValue(valueId) * 100.l;

		case PIXELS:
			return value * this->currentResolution / this->pointSize;

		case INCHES:
			return value / this->pointSize;

		case CM:
			return value / this->pointSize * 2.54l;

		case MM:
			return value / this->pointSize * 2.54l * 10.l;

		case POINTS:
			return value;

		case PICAS:
			return value / 12.l;

		case COLUMNS:
			return (value + this->gutterWidth) / 
				(this->columnWidth + this->gutterWidth);

		case PIXELS_INCH:
			return value;

		case PIXELS_CM:
			return value / 2.54l;

		default:
			BOOST_ASSERT_MSG(false, "The unknown unit.");
			break;
	}

	return value;
}

/************************************************************************/
/* Set the constrain proportions flag.                                  */
/************************************************************************/
void SettingsDialog::enableProportions(bool const enable)
{
	// Save the flag
	this->constrainProportions = enable;

	// Show/hide the icon
	BOOST_ASSERT_MSG(this->windows.end() != this->windows.find(IDC_SETTINGS_SIZELINK),
		"The window handle is not found.");

	::ShowWindow(this->windows.at(IDC_SETTINGS_SIZELINK), 
		enable ? SW_SHOW : SW_HIDE);

	// Recalculate
	if (enable)
	{
		double const currentProportions = this->currentWidth / this->currentHeight;
		double const originalProportions = static_cast<double>(this->originalWidth) / 
			static_cast<double>(this->originalHeight);

		if (currentProportions < originalProportions)
		{
			setValue(IDC_SETTINGS_WIDTH, this->currentWidth);
		}
		else
		{
			setValue(IDC_SETTINGS_HEIGHT, this->currentHeight);
		}
	}
}

/************************************************************************/
/* Show the dialog.                                                     */
/************************************************************************/
bool SettingsDialog::show()
{
	auto const code = ::DialogBoxParamW(
		this->module, MAKEINTRESOURCE(IDD_SETTINGS), ::GetActiveWindow(), 
		reinterpret_cast<DLGPROC>(&SettingsDialog::dialogProc), 
		reinterpret_cast<LPARAM>(this));

	return (IDOK == code) ? true : false;
}

/************************************************************************/
/* Set values of the original image size.                               */
/************************************************************************/
void SettingsDialog::setOriginalSize(int const width, int const height)
{
	this->originalWidth  = width;
	this->originalHeight = height;
}

/************************************************************************/
/* Set a value of the original image resolution.                        */
/************************************************************************/
void SettingsDialog::setOriginalResolution(double const resolution)
{
	this->originalResolution = resolution;
}

/************************************************************************/
/* Set a value of the width of the column size.                         */
/************************************************************************/
void SettingsDialog::setColumnWidth(double const columnWidth)
{
	this->columnWidth = columnWidth;
}

/************************************************************************/
/* Set a value of the gutter of the column size.                        */
/************************************************************************/
void SettingsDialog::setGutterWidth(double const gutterWidth)
{
	this->gutterWidth = gutterWidth;
}

/************************************************************************/
/* Set a value of the point/picas size.                                 */
/************************************************************************/
void SettingsDialog::setPointSize(double const pointSize)
{
	this->pointSize = pointSize;
}

/************************************************************************/
/* Get the current width (in pixels).                                   */
/************************************************************************/
int SettingsDialog::getCurrentWidth() const
{
	// Get the width
	int width = 0;
	
	try
	{
		width = ::boost::numeric_cast<int>(
			toUnit(IDC_SETTINGS_WIDTH, this->currentWidth, PIXELS));
	}
	catch (::boost::bad_numeric_cast & ex)
	{
		UNREFERENCED_PARAMETER(ex);
	}

	// Check the minimum and maximum
	if (1 > width)
	{
		width = 1;
	}

	int32_t const maxWidth = MAX_WIDTH;

	if (maxWidth < width)
	{
		width = maxWidth;
	}

	return width;
}

/************************************************************************/
/* Get the current height (in pixels).                                  */
/************************************************************************/
int SettingsDialog::getCurrentHeight() const
{
	// Get the width
	int height = 0;

	try
	{
		height = ::boost::numeric_cast<int>(
			toUnit(IDC_SETTINGS_HEIGHT, this->currentHeight, PIXELS));
	}
	catch (::boost::bad_numeric_cast & ex)
	{
		UNREFERENCED_PARAMETER(ex);
	}

	// Check the minimum and maximum
	if (1 > height)
	{
		height = 1;
	}

	int32_t const maxHeight = MAX_HEIGHT;

	if (maxHeight < height)
	{
		height = maxHeight;
	}

	return height;
}

/************************************************************************/
/* Get current resolution.                                              */
/************************************************************************/
double SettingsDialog::getCurrentResolution() const
{
	return this->currentResolution;
}
