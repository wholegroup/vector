#pragma once

#ifndef VECTOR_PS_ABOUT_DIALOG_H
#define VECTOR_PS_ABOUT_DIALOG_H

// com.wholegroup.vector.ps
namespace com {namespace wholegroup {namespace vector {namespace ps
{
	// About Dialog
	class AboutDialog
	{
		public:

			//
			static int const HORIZ_OFFSET;

			//
			static int const DIALOGFONT_HEIGHT;

			//
			static int const CAPTIONFONT_HEIGHT;

			//
			static ::std::wstring const SITE_URL;

			//
			static int const VERTICAL_OFFSET;

			//
			static ::std::wstring const VERSION;

			// Background Color
			static int const BACKGROUND_COLOR;

			//
			static int const LINK_COLOR;

			//
			static int const LINK_HOVER_COLOR;

		private:

			// Module handle
			HINSTANCE hModule;

			// Link window
			HWND link;

			// Link text
			::std::wstring linkText;

			// Hover link
			bool hoverLink;

			// Link font
			FontObject linkFont;

			// Hand Cursor
			HCURSOR handCursor;

			// Background Brush
			BrushObject backgroundBrush;

			// Dialog font
			FontObject dialogFont;

			// Caption font
			FontObject captionFont;

			// Background bitmap
			BitmapObject background;

			// Map of original messages procs of links
			::std::map<HWND, WNDPROC> linkOriginalProcs;

			//
			::std::map<int, HWND> windows;

		protected:

			// Initializing.
			void init(HWND const wnd);

			// Initializing fonts.
			void initFonts(HWND const wnd);

			// Size.
			void initSize(HWND const wnd) const;

			// Message processing.
			static LRESULT CALLBACK messageProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

			// Message processing of link controls.
			static LRESULT CALLBACK linkProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

			// Subclassing the specified link control.
			void subclassLink(INT const controlId);

			// Fill the background.
			void fillBackground(HWND const hWnd, HDC const dc) const;

			// Paint the site link.
			void paintLink(PDRAWITEMSTRUCT const item) const;

			// Return the font height from the specified dialog item
			int getFontHeightFromDlgItem(HWND const wnd, int const itemId) const;

		public:

			// Constructor.
			AboutDialog(HINSTANCE hModule);

			// Destructor.
			virtual ~AboutDialog();

			// Show the dialog.
			virtual void show();
	};
}}}}

#endif // VECTOR_PS_ABOUT_DIALOG_H