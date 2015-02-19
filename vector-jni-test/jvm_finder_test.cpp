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
#include "../vector-jni/jvm_finder.h"

using ::testing::_;
using ::testing::DoDefault;
using ::testing::Return;
using ::testing::ReturnNull;
using ::testing::Throw;
using ::testing::Invoke;

using namespace ::com::wholegroup::vector;

//
class JvmFinderMock
	: public jni::JvmFinder
{
	public:

		JvmFinderMock()
		{
#ifdef WIN32
			ON_CALL(*this, regOpenKeyEx(_, _, _, _))
				.WillByDefault(Invoke(&real_, &jni::JvmFinder::regOpenKeyEx));

			ON_CALL(*this, getStringFromRegistry(_, _))
				.WillByDefault(Invoke(&real_, &jni::JvmFinder::getStringFromRegistry));
#endif // WIN32
		}

#ifdef WIN32
		MOCK_CONST_METHOD4(regOpenKeyEx, HKEY(HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired));

		MOCK_CONST_METHOD2(getStringFromRegistry, ::std::wstring(HKEY key, ::std::wstring const & name));
#endif // WIN32

		MOCK_CONST_METHOD0(getApplicationHome, ::std::wstring());

		MOCK_CONST_METHOD0(getLocalJreHome, ::std::wstring());

		MOCK_CONST_METHOD0(getPublicJreHome, ::std::wstring());

		MOCK_CONST_METHOD0(getJrePath, ::std::wstring());

		MOCK_CONST_METHOD2(checkKnownVMs, bool(::std::wstring const & jrePath, ::std::wstring const & architecture));

		MOCK_CONST_METHOD3(getJvmPath, ::std::wstring(::std::wstring const & jrePath, 
			::std::wstring const & architecture,
			::std::wstring const & jvmType));

	private:

		JvmFinder real_;
};

//
class JvmFinderTest
	: public ::testing::Test
{
};

//
TEST_F(JvmFinderTest, getArchitecture)
{
	jni::JvmFinder jvmFinder;

#ifndef __APPLE__
	EXPECT_TRUE(!jvmFinder.getArchitecture().empty());
#endif
}

TEST_F(JvmFinderTest, setHome_NotSet)
{
	jni::JvmFinder jvmFinder;

	EXPECT_EQ(::std::wstring::npos, 
		jvmFinder.getLocalJreHome().find(L"/sweet/home"));
}

TEST_F(JvmFinderTest, setHome_Set)
{
	jni::JvmFinder jvmFinder;

	EXPECT_NE(::std::wstring::npos, jvmFinder.setHome(L"/sweet/home").
		getLocalJreHome().find(L"/sweet/home"));
}

#ifdef WIN32

//
TEST_F(JvmFinderTest, getStringFromRegistry_NonExistentKey)
{
	jni::JvmFinder jvmFinder;

	EXPECT_THROW(jvmFinder.getStringFromRegistry(HKEY_CURRENT_USER, L"nonexistent"), exception::IOException);
}

//
TEST_F(JvmFinderTest, getStringFromRegistry_NonString)
{
	jni::JvmFinder jvmFinder;

	const std::wstring nameKey(L"vector-jni-test-nonstring");
	const DWORD        valueTest = 12345;

	RegSetValueEx(HKEY_CURRENT_USER, nameKey.c_str(), 0, REG_DWORD, (PBYTE)&valueTest, sizeof(valueTest));
	
	EXPECT_THROW(jvmFinder.getStringFromRegistry(HKEY_CURRENT_USER, nameKey.c_str()), exception::InvalidTypeException);
	
	RegDeleteValue(HKEY_CURRENT_USER, nameKey.c_str());
}

//
TEST_F(JvmFinderTest, getStringFromRegistry_Empty)
{
	jni::JvmFinder jvmFinder;

	const std::wstring nameKey(L"vector-jni-test-empty");

	RegSetValueEx(HKEY_CURRENT_USER, nameKey.c_str(), 0, REG_SZ, 0, 0);

	const std::wstring& regString = jvmFinder.getStringFromRegistry(HKEY_CURRENT_USER, nameKey.c_str()); 

	EXPECT_EQ(std::wstring(L""), regString);

	RegDeleteValue(HKEY_CURRENT_USER, nameKey.c_str());
}

//
TEST_F(JvmFinderTest, getStringFromRegistry)
{
	jni::JvmFinder jvmFinder;

	const std::wstring nameKey(L"vector-jni-test-empty");
	const std::wstring valueTest(L"ok");

	RegSetValueEx(HKEY_CURRENT_USER, nameKey.c_str(), 0, REG_SZ, (PBYTE)(valueTest.c_str()), 
		static_cast<DWORD>(valueTest.length() * sizeof(std::wstring::traits_type::char_type)));

	const std::wstring& regString = jvmFinder.getStringFromRegistry(HKEY_CURRENT_USER, nameKey.c_str()); 

	EXPECT_EQ(valueTest, regString);

	RegDeleteValue(HKEY_CURRENT_USER, nameKey.c_str());
}

#endif // WIN32

//
TEST_F(JvmFinderTest, getLocalJreHome)
{
	jni::JvmFinder jvmFinder;

#ifdef __APPLE__
	EXPECT_NE(std::string::npos, 
		jvmFinder.getLocalJreHome().rfind(L"/jre"));
#else
#	ifdef _M_X64
	EXPECT_NE(std::string::npos, 
		jvmFinder.getLocalJreHome().rfind(L"64/jre"));
#	else
	EXPECT_NE(std::string::npos, 
		jvmFinder.getLocalJreHome().rfind(L"32/jre"));
#	endif
#endif // __APPLE__
}

#ifdef WIN32
//
TEST_F(JvmFinderTest, getPublicJREHome_NonExistentKey)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, regOpenKeyEx(_, _, _, _))
		.WillOnce(ReturnNull());

	EXPECT_THROW(jvmFinderMock.JvmFinder::getPublicJreHome(), exception::IOException);
}

//
TEST_F(JvmFinderTest, getPublicJREHome_NoCurrentVersion)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, regOpenKeyEx(_, _, _, _));

	EXPECT_CALL(jvmFinderMock, getStringFromRegistry(_, _))
		.WillOnce(Return(std::wstring(L"")));

	EXPECT_THROW(jvmFinderMock.JvmFinder::getPublicJreHome(), exception::IOException);
}

//
TEST_F(JvmFinderTest, getPublicJREHome_NonExistentKeyCurrentVersion)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, regOpenKeyEx(_, _, _, _))
		.WillOnce(DoDefault())
		.WillOnce(ReturnNull());

	EXPECT_CALL(jvmFinderMock, getStringFromRegistry(_, _));

	EXPECT_THROW(jvmFinderMock.JvmFinder::getPublicJreHome(), exception::IOException);
}

#endif // WIN32

//
TEST_F(JvmFinderTest, getPublicJREHome)
{
	jni::JvmFinder jvmFinder;

	EXPECT_TRUE(!jvmFinder.getPublicJreHome().empty());
}

//
TEST_F(JvmFinderTest, getJvmPath_NoPath)
{
	jni::JvmFinder jvmFinder;

	EXPECT_THROW(jvmFinder.getJvmPath(L"", L"", L""), exception::IOException);
}

//
TEST_F(JvmFinderTest, getJvmPath)
{
	jni::JvmFinder jvmFinder;

#ifdef WIN32
#	ifdef _M_IX86
	EXPECT_TRUE(!jvmFinder.getJvmPath(jvmFinder.getPublicJreHome(), 
		L"i386", jni::JvmFinder::JRE_CLIENT).empty());
#	endif

#	ifdef _M_AMD64
	EXPECT_TRUE(!jvmFinder.getJvmPath(jvmFinder.getPublicJreHome(), 
		L"amd64", jni::JvmFinder::JRE_SERVER).empty());
#	endif
#endif // WIN32

#ifdef __APPLE__
#	ifdef __i386__
	EXPECT_TRUE(!jvmFinder.getJvmPath(jvmFinder.getPublicJreHome(),
		L"", jni::JvmFinder::JRE_CLIENT).empty());
#	endif

#	ifdef __x86_64__
	EXPECT_TRUE(!jvmFinder.getJvmPath(jvmFinder.getPublicJreHome(),
		L"64", jni::JvmFinder::JRE_SERVER).empty());
#	endif
#endif

#	ifdef __linux__
	EXPECT_TRUE(!jvmFinder.getJvmPath(jvmFinder.getPublicJreHome(), 
		L"i386", jni::JvmFinder::JRE_CLIENT).empty());
#	endif
}

//
TEST_F(JvmFinderTest, getJrePath_NotFound)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, getLocalJreHome())
		.WillOnce(Return(L""));

	EXPECT_CALL(jvmFinderMock, getPublicJreHome())
		.WillOnce(Throw(exception::IOException()));

	EXPECT_THROW(jvmFinderMock.JvmFinder::getJrePath(), 
		exception::IOException);
}

#ifdef WIN32

//
TEST_F(JvmFinderTest, getJrePath_Local)
{
	jni::JvmFinder jvmFinder;

	// Get current JDK version
	jni::RegKey const versionRegKey(jvmFinder.regOpenKeyEx(HKEY_LOCAL_MACHINE, 
		L"SOFTWARE\\JavaSoft\\Java Development Kit", 0, KEY_READ));

	ASSERT_TRUE(versionRegKey);

	std::wstring const & jdkVersion = jvmFinder.getStringFromRegistry(versionRegKey.get(), L"CurrentVersion");

	// Get current JDK path with "jre" directory 
	jni::RegKey const jdkRegKey(jvmFinder.regOpenKeyEx(HKEY_LOCAL_MACHINE, 
		std::wstring(L"SOFTWARE\\JavaSoft\\Java Development Kit\\").append(jdkVersion).c_str(), 0, KEY_READ));

	ASSERT_TRUE(jdkRegKey);

	std::wstring const & jdkPath = jvmFinder.getStringFromRegistry(
		jdkRegKey.get(), L"JavaHome");

	// Test local JRE
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, getLocalJreHome())
		.WillOnce(Return(jdkPath + L"/jre"));

	EXPECT_CALL(jvmFinderMock, getPublicJreHome())
		.Times(0);

	EXPECT_EQ(std::wstring(jdkPath).append(L"/jre"), 
		jvmFinderMock.JvmFinder::getJrePath());
}

#endif // WIN32

//
TEST_F(JvmFinderTest, getJrePath_Public)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, getLocalJreHome())
		.WillOnce(Return(L""));

	EXPECT_CALL(jvmFinderMock, getPublicJreHome())
		.WillOnce(Return(L"getPublicJreHome"));

	EXPECT_TRUE(!jvmFinderMock.JvmFinder::getJrePath().empty());
}

#ifndef __APPLE__

//
TEST_F(JvmFinderTest, checkKnownVMs_NotFound)
{
	jni::JvmFinder jvmFinder;

	ASSERT_FALSE(jvmFinder.checkKnownVMs(jvmFinder.getJrePath(), L"none"));
}

//
TEST_F(JvmFinderTest, checkKnownVMs)
{
	jni::JvmFinder jvmFinder;

	ASSERT_TRUE(jvmFinder.checkKnownVMs(jvmFinder.getJrePath(), jvmFinder.getArchitecture()));
}

#endif // __APPLE__

//
TEST_F(JvmFinderTest, findJvm_JreNotFound)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, getJrePath())
		.WillOnce(Throw(exception::IOException()));

	EXPECT_EQ(std::wstring(L""), jvmFinderMock.JvmFinder::findJvm());
}

//
#ifndef __APPLE__
TEST_F(JvmFinderTest, findJvm_UnknowVM)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, getJrePath())
		.WillOnce(Return(L""));

	EXPECT_CALL(jvmFinderMock, checkKnownVMs(_, _))
		.WillOnce(Return(false));
	
	EXPECT_EQ(std::wstring(L""), jvmFinderMock.JvmFinder::findJvm());
}
#endif

//
TEST_F(JvmFinderTest, findJvm_Client)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, getJrePath())
		.WillOnce(Return(L""));

	EXPECT_CALL(jvmFinderMock, checkKnownVMs(_, _))
		.WillOnce(Return(true));
	
	EXPECT_CALL(jvmFinderMock, getJvmPath(_, _, _))
		.WillOnce(Return(L"Client"));

	EXPECT_EQ(std::wstring(L"Client"), jvmFinderMock.JvmFinder::findJvm());
}

//
TEST_F(JvmFinderTest, findJvm_Server)
{
	JvmFinderMock jvmFinderMock;

	EXPECT_CALL(jvmFinderMock, getJrePath())
		.WillOnce(Return(L""));

	EXPECT_CALL(jvmFinderMock, checkKnownVMs(_, _))
		.WillOnce(Return(true));
		
	EXPECT_CALL(jvmFinderMock, getJvmPath(_, _, _))
		.WillOnce(Throw(exception::IOException()))
		.WillOnce(Return(L"Server"));

	EXPECT_EQ(std::wstring(L"Server"), jvmFinderMock.JvmFinder::findJvm());
}

//
TEST_F(JvmFinderTest, javaOwner)
{
	jni::JvmFinder jvmFinder;
	
	::std::wcout << L"getPublicJreHome=" << jvmFinder.getPublicJreHome() << ::std::endl;
}