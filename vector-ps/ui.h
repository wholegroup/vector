#pragma once

#ifndef VECTOR_PS_UI_H
#define VECTOR_PS_UI_H

// com.wholegroup.vector.ps
namespace com {namespace wholegroup {namespace vector {namespace ps
{
	class Ui
	{
		protected:

			// Base32 table 
			static ::std::vector<wchar_t> const TABLE32;

		public:

			// Default width.
			static int const DEFAULT_WIDTH;

			// Default height.
			static int const DEFAULT_HEIGHT;

			// Default resolution.
			static double const DEFAULT_RESOLUTION;

		protected:

		public:

			// Show the about dialog.
			static void doAbout();

			// Show the settings dialog.
			static bool doSettings(int & width, int & height, double & resolution);

			// Get the ICC profile.
			static ::std::vector<char> getIccProfile(void);

			// Get the path to the vector core.
			static ::std::wstring getPathToCore();

			// Get the filename from the handle
			static ::std::wstring getFilenameByHandle(intptr_t const dataFork);
	};
}}}}


#endif // VECTOR_PS_UI_H
