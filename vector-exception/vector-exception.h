#pragma once

#ifndef VECTOR_EXCEPTION_H
#define VECTOR_EXCEPTION_H

namespace com {namespace wholegroup {namespace vector {namespace exception
{
	class ExString
	{
		private:

			// The message in the wide-string type
			::std::wstring wmessage;

			// The message in the string type
			::std::string message;

		public:

			// Wide-string constructor.
			ExString(::std::wstring const & ws);

			// Wide-char constructor.
			ExString(wchar_t const * const wc);

			//
			::std::wstring getWMessage() const {return wmessage;}

			//
			char const * getMessage() const {return message.c_str();}
	};

	//
	typedef ::boost::error_info<
		struct BOOST_SYMBOL_VISIBLE tagErrorMessage,
		ExString> ErrorMessage;

	// Root exception
	struct Exception
		: virtual ::std::exception,
		virtual ::boost::exception
	{
		public:

			//
			virtual ::std::wstring const wwhat() const;

			//
			virtual char const * what() const BOOST_NOEXCEPT;
	};

	// Null pointer
	struct UnknownException: virtual Exception {};

	// Null pointer
	struct NullPointerException: virtual Exception {};

	// Invalid type
	struct InvalidTypeException: virtual Exception {};

	// Invalid parameter
	struct InvalidParameterException: virtual Exception {};

	// Invalid value
	struct InvalidValueException: virtual Exception {};

	// IO
	struct IOException: virtual Exception {};

	// WinAPI
	struct WinApiException: virtual Exception {};

};};};};

#endif // VECTOR_EXCEPTION_H
