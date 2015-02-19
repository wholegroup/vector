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
#include "jvm_finder.h"

using namespace ::com::wholegroup::vector;
using namespace ::com::wholegroup::vector::jni;

//
::std::wstring JvmFinder::JRE_KEY(
	L"Software\\JavaSoft\\Java Runtime Environment");

//
#ifdef WIN32
::std::wstring JvmFinder::JVM_DLL(L"jvm.dll");
#elif __APPLE__
::std::wstring JvmFinder::JVM_DLL(L"libclient");
#else
::std::wstring JvmFinder::JVM_DLL(L"libjvm.so");
#endif

//
::std::wstring JvmFinder::JAVA_DLL(L"java.dll");

//
::std::wstring JvmFinder::JRE_CLIENT(L"client");

//
::std::wstring JvmFinder::JRE_SERVER(L"server");

//
::std::wstring JvmFinder::JVM_CONFIG(L"jvm.cfg");

/************************************************************************/
/* The default constructor.                                             */
/************************************************************************/
JvmFinder::JvmFinder()
	: neededVersion(L"CurrentVersion")
{
}

/************************************************************************/
/* The constructor with setting needed JVM version.                     */
/************************************************************************/
JvmFinder::JvmFinder(::std::wstring const & neededVersion)
	: neededVersion(neededVersion)
{
}

/************************************************************************/
/* Set the application home.                                            */
/************************************************************************/
JvmFinder & JvmFinder::setHome(::std::wstring const home)
{
	this->applicationHome = home;

	return *this;
}

/************************************************************************/
/* Find the JVM.                                                        */
/************************************************************************/
::std::wstring JvmFinder::findJvm() const
{
	try
	{
		// Get a path to JRE
		::std::wstring const jrePath = getJrePath();

		// Check supporting the architecture
		::std::wstring architecture = getArchitecture();

		if (false == checkKnownVMs(jrePath, architecture))
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"Current architecture isn't supported."));
		}

		// Client JVM
		try
		{
			return getJvmPath(jrePath, architecture, JRE_CLIENT);
		}
		catch(exception::Exception const & ex)
		{
			UNREFERENCED_PARAMETER(ex);
		}

		// Server JVM
		try
		{
			return getJvmPath(jrePath, architecture, JRE_SERVER);
		}
		catch(exception::Exception const & ex)
		{
			UNREFERENCED_PARAMETER(ex);
		}

		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"the JVM isn't found."));
	}
	catch (exception::Exception const & ex)
	{
		UNREFERENCED_PARAMETER(ex);
	}

	return ::std::wstring(L"");
}

/************************************************************************/
/* Get the JRE path.                                                    */
/************************************************************************/
::std::wstring JvmFinder::getJrePath() const
{
	// Check the local JRE path
	::std::wstring const jreLocalPath(getLocalJreHome());

#ifdef WIN32
	if (boost::filesystem::exists(::std::wstring(jreLocalPath)
		.append(L"/bin/").append(JAVA_DLL)))
	{
		return jreLocalPath;
	}
#endif // WIN32

#ifdef __APPLE__
	// local JRE isn't supported yet
	/*
	if (boost::filesystem::exists(::std::wstring(jreLocalPath)))
	{
		return jreLocalPath;
	}
	*/
#endif // __APPLE__

	return getPublicJreHome();
}

/************************************************************************/
/* Check known JVM for the specified architecture.                      */
/************************************************************************/
bool JvmFinder::checkKnownVMs(::std::wstring const & jrePath, 
	::std::wstring const & architecture) const
{
#ifdef WIN32
	// Assembly a path
	::std::wstring config(jrePath);

	config.append(L"/");
	config.append(L"lib");
	config.append(L"/");
	config.append(architecture);
	config.append(L"/");
	config.append(JVM_CONFIG);

	// Check the path
	if (boost::filesystem::exists(config))
	{
		return true;
	}

	return false;
#endif // WIN32

	return true;
}

/************************************************************************/
/* Get the architecture of this built module.                           */
/************************************************************************/
::std::wstring JvmFinder::getArchitecture() const
{
#ifdef WIN32
#	ifdef _M_AMD64
	return ::std::wstring(L"amd64");
#	endif

#	ifdef _M_IA64
	return ::std::wstring(L"ia64");
#	endif

#	ifdef _M_IX86
	return ::std::wstring(L"i386");
#	endif
#endif // WIN32

#ifdef __APPLE__
#	ifdef __x86_64__
	return ::std::wstring(L"64");
#	endif
#endif  // __APPLE__

	return ::std::wstring(L"");
}

/************************************************************************/
/* Get the JVM path.                                                    */
/************************************************************************/
::std::wstring JvmFinder::getJvmPath(::std::wstring const & jrePath, 
	::std::wstring const & architecture,
	::std::wstring const & jvmType) const
{
	// Assemble a path.
	::std::wstring jvmPath(L"");

	jvmPath.append(jrePath);
	jvmPath.append(L"/");

#ifdef WIN32
	jvmPath.append(L"bin");
	jvmPath.append(L"/");
	jvmPath.append(jvmType);
	jvmPath.append(L"/");
	jvmPath.append(JVM_DLL);
#endif // WIN32

#ifdef __APPLE__
	if (isAppleJava(jrePath))
	{
		jvmPath.append(L"..");
		jvmPath.append(L"/");
		jvmPath.append(L"Libraries");
		jvmPath.append(L"/");
		jvmPath.append(L"lib");
		jvmPath.append(jvmType);
		
		if (JRE_CLIENT == jvmType)
		{
			jvmPath.append(architecture);
		}
		
		jvmPath.append(L".dylib");
	}
	
	else if (isOracleJava(jrePath))
	{
		jvmPath.append(L"lib");
		jvmPath.append(L"/");
		jvmPath.append(L"/jli/");
		jvmPath.append(L"/");
		jvmPath.append(L"libjli.dylib");
	}
	
	else
	{
		jvmPath.append(L"path_not_found");
	}
#endif // __APPLE__

#ifdef __linux__
	jvmPath.append(L"lib");
	jvmPath.append(L"/");
	jvmPath.append(architecture);
	jvmPath.append(L"/");
	jvmPath.append(jvmType);
	jvmPath.append(L"/");
	jvmPath.append(JVM_DLL);
#endif // __linux__

	// Get status of the file.
	if (!::boost::filesystem::exists(jvmPath))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Error getting status of the JVM file (" + jvmPath + L")."));
	}

	return jvmPath;
}

/************************************************************************/
/* Get the local JRE path.                                              */
/************************************************************************/
::std::wstring JvmFinder::getLocalJreHome() const
{
	::std::wstring jreLocalPath(this->applicationHome);

#ifdef WIN32
#	ifdef _M_X64
	jreLocalPath.append(L"/64");
#	endif

#	ifdef _M_IX86
	jreLocalPath.append(L"/32");
#	endif
#endif // WIN32

	jreLocalPath.append(L"/jre");

	return jreLocalPath;
}

/************************************************************************/
/* Get the public JRE home.                                             */
/************************************************************************/
::std::wstring JvmFinder::getPublicJreHome() const
{
#ifdef WIN32
	// Open the JRE key.
	RegKey const jreRegKey(regOpenKeyEx(HKEY_LOCAL_MACHINE, JRE_KEY.c_str(), 0, KEY_READ));

	if (!jreRegKey)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Error opening the JRE key."));
	}

	// Get current version of JRE.
	::std::wstring const version = getStringFromRegistry(jreRegKey.get(), neededVersion.c_str());

	if (version.empty())
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Error getting the current version of JRE."));
	}

	// Open current version key of JRE.
	RegKey const versionRegKey(regOpenKeyEx(jreRegKey.get(), version.c_str(), 0, KEY_READ));

	if (!versionRegKey)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Error opening the current version key."));
	}

	return getStringFromRegistry(versionRegKey.get(), L"JavaHome");
#endif // WIN32

#ifdef __APPLE__
	// check JAVA_HOME
	char const * const JAVA_HOME = ::std::getenv("JAVA_HOME");
	
	if (nullptr != JAVA_HOME)
	{
		::std::string javaHome(JAVA_HOME);
	
		if ((0 < javaHome.length()) && ::boost::filesystem::exists(javaHome))
		{
			::std::wstring const javaHomeW(::boost::filesystem::canonical(javaHome).wstring());
		
			if (isAppleJava(javaHomeW))
			{
				return javaHomeW;
			}
		
#	ifdef __x86_64__
			if (isOracleJava(javaHomeW))
			{
				return javaHomeW;
			}

			::std::wstring const jre = javaHomeW + L"/jre";
			
			if (::boost::filesystem::exists(jre) && isOracleJava(jre))
			{
				return jre;
			}
#	endif // __x86_64__
		}
	}
	
	// check CurrentJDK
	::std::wstring const currentJDK = L"/System/Library/Frameworks/JavaVM.framework/"
		L"Versions/CurrentJDK/Home";

	if (::boost::filesystem::exists(currentJDK))
	{
		if (isAppleJava(currentJDK))
		{
			return currentJDK;
		}

#	ifdef __x86_64__
		if (isOracleJava(currentJDK))
		{
			return currentJDK;
		}
#	endif // __x86_64__
	}

	// check the internet plugin
	::std::wstring const internetPlugin = L"/Library/Internet Plug-Ins/JavaAppletPlugin.plugin"
		L"/Contents/Home";
		
	if (::boost::filesystem::exists(internetPlugin))
	{
		if (isAppleJava(internetPlugin))
		{
			return internetPlugin;
		}

#	ifdef __x86_64__
		if (isOracleJava(internetPlugin))
		{
			return internetPlugin;
		}
#	endif // __x86_64__
	}

	// jdks
	::boost::filesystem::path jdksPath = L"/Library/Java/JavaVirtualMachines";
	
	if (::boost::filesystem::exists(jdksPath) && ::boost::filesystem::is_directory(jdksPath))
	{
		::std::vector<::boost::filesystem::path> jdks;
		
		//
		::boost::filesystem::directory_iterator const end;
		
		for (::boost::filesystem::directory_iterator it(jdksPath); end != it; it++)
		{
			jdks.push_back(it->path());
		}
		
		//
		for (auto jdkIt = jdks.rbegin(); jdks.rend() != jdkIt; jdkIt++)
		{
			::std::wstring const maybe6 = jdkIt->wstring() + L"/Contents/Home";
			
			if (!::boost::filesystem::exists(maybe6) || !::boost::filesystem::is_directory(maybe6))
			{
				continue;
			}
			
			if (isAppleJava(maybe6))
			{
				return maybe6;
			}

#	ifdef __x86_64__
			::std::wstring const maybe7 = maybe6 + L"/jre";
			
			if (!::boost::filesystem::exists(maybe7) || !::boost::filesystem::is_directory(maybe7))
			{
				continue;
			}

			if (isOracleJava(maybe7))
			{
				return maybe7;
			}
#	endif // __x86_64__
		}
	}
	
	BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
		L"Java is not found."));

#endif // __APPLE__

#ifdef __linux__
	::boost::filesystem::path current(L"/usr/bin/java");

	if (!boost::filesystem::exists(current))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Java is not found."));
	}

	while (::boost::filesystem::is_symlink(current))
	{
		current = ::boost::filesystem::read_symlink(current);

		if (current.empty())
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An error resolving java path."));
		}
	}

	return current.parent_path().parent_path().wstring();
#endif // __linux__

	BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
		L"An unsupported OS."));
}

#ifdef __APPLE__
/************************************************************************/
/* Checking the path is Apple Java.                                     */
/************************************************************************/
bool JvmFinder::isAppleJava(::std::wstring const & javaHome) const
{
	return ::boost::filesystem::exists(
		::boost::filesystem::path(javaHome).parent_path() / L"Libraries" / L"libclient.dylib");
}
#endif // __APPLE__

#ifdef __APPLE__
/************************************************************************/
/* Checking the path is Oracle Java.                                    */
/************************************************************************/
bool JvmFinder::isOracleJava(::std::wstring const & javaHome) const
{
	return ::boost::filesystem::exists(
		::boost::filesystem::path(javaHome) / L"lib" / L"libjava.dylib");
}
#endif // __APPLE__

#ifdef WIN32
/************************************************************************/
/* Get the specified string from the Windows registry.                  */
/************************************************************************/
::std::wstring JvmFinder::getStringFromRegistry(HKEY key, ::std::wstring const & name) const
{
	DWORD typeValue;
	DWORD sizeValue;

	// Get size of value
	if (ERROR_SUCCESS != RegQueryValueEx(key, name.c_str(), 0, &typeValue, 0, &sizeValue))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Error quering value from the windows registry."));
	}

	if (REG_SZ != typeValue)
	{
		BOOST_THROW_EXCEPTION(exception::InvalidTypeException() << exception::ErrorMessage(
			L"Type of the value is not a string."));
	}

	if (sizeValue <= 0)
	{
		return ::std::wstring(L"");
	}

	// Get the value (+1 for guaranteed null character)
	::std::vector<wchar_t> buffer(sizeValue / sizeof(::std::wstring::traits_type::char_type) + 1, L'\0');

	if (ERROR_SUCCESS != RegQueryValueEx(key, name.c_str(), 0, &typeValue, (LPBYTE)&buffer[0], &sizeValue))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Error quering value from the windows registry."));
	}

	// Double check
	if (REG_SZ != typeValue)
	{
		BOOST_THROW_EXCEPTION(exception::InvalidTypeException() << exception::ErrorMessage(
			L"Type of value isn't string."));
	}

	return ::std::wstring(buffer.begin(), ::std::find(buffer.begin(), buffer.end(), L'\0'));
}
#endif // WIN32

#ifdef WIN32
/************************************************************************/
/* Wrapper for ::RegOpenKeyEx.                                          */
/************************************************************************/
HKEY JvmFinder::regOpenKeyEx(HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired) const
{
	HKEY key = nullptr;

	if (ERROR_SUCCESS != ::RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, &key))
	{
		return nullptr;
	}

	return key;
}
#endif // WIN32

#ifdef WIN32
/************************************************************************/
/* Close the key of registry.                                           */
/************************************************************************/
void RegKeyDeleter::operator()(HKEY hkey)
{
	::RegCloseKey(hkey);
}
#endif // WIN32
