#pragma once

#ifndef VECTOR_PS_SETTINGS_DIALOG_H
#define VECTOR_PS_SETTINGS_DIALOG_H

// com.wholegroup.vector.ps
namespace com {namespace wholegroup {namespace vector {namespace ps
{
	// Settings Dialog
	class SettingsDialog
	{
		public:

			// Units
			enum Unit {PERCENT, PIXELS, INCHES, CM, MM, POINTS, PICAS, COLUMNS, 
				PIXELS_INCH, PIXELS_CM};

			// Maximum possible width
			static int32_t const MAX_WIDTH;

			// Maximum possible height
			static int32_t const MAX_HEIGHT;

			// Default column width
			static double const DEFAULT_COLUMN_WIDTH;

			// Default gutter width
			static double const DEFAULT_GUTTER_WIDTH;

			// Default point size
			static double const DEFAULT_POINT_SIZE;

			// Default value of the contrain proportions flag 
			static bool const DEFAULT_CONSTRAIN_PROPORTIONS;

			// Default size unit
			static Unit const DEFAULT_SIZE_UNIT;

			// Default resolution unit
			static Unit const DEFAULT_RESOLUTION_UNIT;

			// List of value units
			static ::std::map<Unit, ::std::wstring> const VALUE_UNITS;

			// List of resolution units
			static ::std::map<Unit, ::std::wstring> const RESOLUTION_UNITS;

			// Unit precisions
			static ::std::map<Unit, short> const UNIT_PRECISIONS;

			// Links of value and static
			static ::boost::bimap<int, int> const VALUE_STATIC_LINKS;

			// Links of value and unit
			static ::boost::bimap<int, int> const VALUE_UNIT_LINKS;

			// Value filter
			static ::std::vector<wchar_t> const VALUE_FILTER;

			// Dialog title
			static ::std::wstring const TITLE;

			// Image size text
			static ::std::wstring const IMAGE_SIZE;

			// Sizes
			static ::std::vector<::std::wstring> const SIZES;

			// Link colors
			static ::std::map<int, ::std::pair<COLORREF, COLORREF>> const LINK_COLORS;

		private:

			// Module handle
			HINSTANCE module;

			// Window handles
			::std::map<int, HWND> windows;

			// OK button
			HWND okButton;

			// Cancel button
			HWND cancelButton;

			// Hand cursor
			HCURSOR handCursor;

			// Size cursor
			CursorObject sizeCursor;

			// Map of original message procs of edit units
			::std::map<HWND, WNDPROC> editUnitsOriginalProcs;

			// Map of original message procs of static units
			::std::map<HWND, WNDPROC> staticUnitsOriginalProcs;

			// Map of a mouse X-coordinates of static units 
			::std::map<HWND, int> staticUnitsMouseX;

			// Map of original message procs of button-links
			::std::map<HWND, WNDPROC> linkOriginalProcs;

			// Original size of width in pixels
			int originalWidth;

			// Original size of height in pixels
			int originalHeight;

			// Original resolution in pixels/inch
			double originalResolution;

			// Current width in points
			double currentWidth;

			// Current height in points
			double currentHeight;

			// Current resolution in pixels/inch
			double currentResolution;

			// Width of Column Size in points
			double columnWidth;

			// Gutter of Column Size in points
			double gutterWidth;

			// Point/Pica Size
			double pointSize;

			// Constrain Proportions
			bool constrainProportions;

			// Original dialog rectangle
			RECT originalRect;

			// Link fonts
			::std::map<int, FontObject> linkFonts;

			// Link texts
			::std::map<int, ::std::wstring> linkTexts;

			// Link hovers
			::std::map<int, bool> linkHovers;

		protected:

			// Message processing of the dialog.
			static LRESULT CALLBACK dialogProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

			// Common initialization.
			void init(HWND const wnd);

			// Initialization fonts.
			void initFonts(HWND const wnd);

			// Initializition listboxes.
			void initLists() const;

			// Add the string to the list.
			void addToList(HWND const list, ::std::wstring const value, 
				Unit const unit, Unit const defaultUnit) const;

			// Subclassing the specified edit control of units.
			void subclassValue(HWND const wnd, int const valueId);

			// Message processing of edit controls of units.
			static LRESULT CALLBACK valueProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

			// Subclassing the specified static control of units.
			void subclassStatic(HWND const wnd, int const staticId);

			// Message processing of static controls of units.
			static LRESULT CALLBACK staticProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

			// Subclassing the specified static control of units.
			void subclassLink(int const linkId);

			// Message processing of button-link controls.
			static LRESULT CALLBACK linkProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

			// Paint a combobox item.
			void paintList(int const listId, PDRAWITEMSTRUCT const item) const;

			// Paint a link-button.
			void paintLink(int const linkId, PDRAWITEMSTRUCT const item) const;

			// Calculate the image size.
			void calculateSize();

			// Get the value.
			double getValue(int const valueId) const;

			// Get the original value.
			double getOriginalValue(int const valueId) const;

			// Get the minimum value.
			double getMinimum(int const valueId) const;

			// Get the maximum value.
			double getMaximum(int const valueId) const;

			// Set the value.
			void setValue(int const valueId, double const value);

			// Refresh the value.
			void refreshValue(int const valueId) const;

			// Select all text of the value.
			void selectValue(int const valueId) const;

			// Change the value.
			bool changeValue(int const valueId);

			// Increase the value.
			bool increaseValue(int const valueId, bool const orderMagnitude);

			// Decrease the value.
			bool decreaseValue(int const valueId, bool const orderMagnitude);

			// Step the value.
			bool stepValue(int const valueId, double const step);

			// Validate the value.
			bool validateValue(int const valueId);

			// Format the value.
			::std::wstring formatValue(double const value, 
				double const minimum, double const maximum,
				Unit const unit, bool const trim = true) const;

			// Get the selected unit.
			Unit getUnit(int const unitId) const;

			// Change the unit.
			void changeUnit(HWND const wnd, int const unitId);

			// Convert the value from the specified unit.
			double fromUnit(int const valueId, double const value, 
				Unit const unit) const;

			// Convert the value to the specified unit.
			double toUnit(int const valueId, double const value, 
				Unit const unit) const;

			// Set the constrain proportions flag.
			void enableProportions(bool const enable);

		public:

			// Constructor.
			SettingsDialog(HINSTANCE module);

			// Destructor.
			virtual ~SettingsDialog();

			// Show the dialog.
			virtual bool show();

			// Set values of the original image size.
			virtual void setOriginalSize(int const width, int const height);

			// Set a value of the original image resolution.
			virtual void setOriginalResolution(double const resolution);

			// Set a value of the width of the column size.
			virtual void setColumnWidth(double const columnWidth);

			// Set a value of the gutter of the column size.
			virtual void setGutterWidth(double const gutterWidth);

			// Set a value of the point/picas size.
			virtual void setPointSize(double const pointSize);

			// Get the current width (in pixels).
			virtual int getCurrentWidth() const;

			// Get the current height (in pixels).
			virtual int getCurrentHeight() const;

			// Get the current resolution (pixels/inches).
			virtual double getCurrentResolution() const;
	};
}}}}

#endif // VECTOR_PS_ABOUT_DIALOG_H