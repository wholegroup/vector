#pragma once

#ifndef VECTOR_PS_VERSION_H
#define VECTOR_PS_VERSION_H

// com.wholegroup.vector.ps
namespace com {namespace wholegroup {namespace vector {namespace ps
{
	class Version
	{
		public:

			// Manufacturer
			static ::std::wstring const & MANUFACTURER();

			// The plugin version
			static ::std::wstring const & VERSION();

			// The product name
	#ifdef __APPLE__
			static ::std::wstring const & PRODUCT();
	#endif

			// The plugin name
			static ::std::wstring const & NAME();

			// The format name
			static ::std::wstring const & FORMAT();
		};

};};};};

#endif // VECTOR_PS_VERSION_H