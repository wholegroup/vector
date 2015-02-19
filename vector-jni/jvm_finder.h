#pragma once

#ifndef VECTOR_JNI_JVM_FINDER_H
#define VECTOR_JNI_JVM_FINDER_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class JvmFinder
	{
		public:

			//
			static ::std::wstring JRE_KEY;

			//
			static ::std::wstring JVM_DLL;

			//
			static ::std::wstring JAVA_DLL;

			//
			static ::std::wstring JRE_CLIENT;

			//
			static ::std::wstring JRE_SERVER;

			//
			static ::std::wstring JVM_CONFIG;

		protected:

			//
			::std::wstring neededVersion;

			//
			::std::wstring applicationHome;

		public:

			// The default constructor.
			JvmFinder();

			//
			JvmFinder(::std::wstring const & neededVersion);

			// Set the application home.
			virtual JvmFinder & setHome(::std::wstring const home);

			// Find the JVM.
			virtual ::std::wstring findJvm() const;

			// Get the JRE path.
			virtual ::std::wstring getJrePath() const;

			// Get the local JRE path.
			virtual ::std::wstring getLocalJreHome() const;

			// Get the public JRE path.
			virtual ::std::wstring getPublicJreHome() const;

			// Get the JVM path.
			virtual ::std::wstring getJvmPath(::std::wstring const & jrePath, 
				::std::wstring const & architecture,
				::std::wstring const & jvmType) const;

			// Check known JVM for the specified architecture.
			virtual bool checkKnownVMs(::std::wstring const & jrePath, 
				::std::wstring const & architecture) const;

			// Get the architecture of this built module.
			virtual ::std::wstring getArchitecture() const;

#ifdef WIN32
			// Get the specified string from the Windows registry.
			virtual ::std::wstring getStringFromRegistry(HKEY key, 
				::std::wstring const & name) const;

			// Wrapper for ::RegOpenKeyEx. (for unit test)
			virtual HKEY regOpenKeyEx(HKEY hKey, LPCTSTR lpSubKey, 
				DWORD ulOptions, REGSAM samDesired) const;
#endif // WIN32

#ifdef __APPLE__
			// Checking the path is Apple Java
			virtual bool isAppleJava(::std::wstring const & javaHome) const;
			
			// Checking the path is Oracle Java
			virtual bool isOracleJava(::std::wstring const & javaHome) const;
#endif // __APPLE__
	};

#ifdef WIN32
	// Deleter for the RegKey type
	struct RegKeyDeleter
	{
		// Close the key of registry.
		void operator()(HKEY hkey);
	};

	// Type safe wrapper for HKEY
	typedef ::std::unique_ptr<HKEY__, RegKeyDeleter> RegKey;
#endif // WIN32
};};};};

#endif // VECTOR_JNI_JVM_FINDER_H
