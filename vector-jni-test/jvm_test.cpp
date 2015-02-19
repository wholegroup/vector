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
#include "../vector-jni/jvm.h"
#include "../vector-jni/j_unique_ptr.h"
#include "../vector-jni/jar.h"

using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::DoDefault;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;

using namespace ::com::wholegroup::vector;

//
class JvmMock
	: public jni::Jvm
{
	public:

		JvmMock()
		{
			ON_CALL(*this, isLoaded())
				.WillByDefault(Invoke(this, &JvmMock::wrapIsLoaded));

			ON_CALL(*this, isCreated())
				.WillByDefault(Invoke(this, &JvmMock::wrapIsCreated));

			ON_CALL(*this, createClassLoader())
				.WillByDefault(Invoke(this, &JvmMock::wrapCreateClassLoader));

			ON_CALL(*this, destroyJvm())
				.WillByDefault(Invoke(this, &JvmMock::wrapdestroyJvm));

			ON_CALL(*this, unloadJvm())
				.WillByDefault(Invoke(this, &JvmMock::wrapUnloadJvm));
		}

		MOCK_CONST_METHOD0(isLoaded, bool());

		bool wrapIsLoaded() { return Jvm::isLoaded(); }

		MOCK_CONST_METHOD0(isCreated, bool());

		bool wrapIsCreated() { return Jvm::isCreated(); }

		MOCK_METHOD0(createClassLoader, void());

		void wrapCreateClassLoader() { Jvm::createClassLoader(); }

		MOCK_METHOD0(destroyJvm, void());

		void wrapdestroyJvm() { Jvm::destroyJvm(); }

		MOCK_METHOD0(unloadJvm, void());

		void wrapUnloadJvm() { Jvm::unloadJvm(); }
};

//
class JvmTest
	: public	::testing::Test
{
	protected:

		::boost::filesystem::path executablePath;

	protected:

		virtual void SetUp()
		{
			this->executablePath = ::boost::filesystem::path(
				::testing::internal::g_argvs[0].c_str()).parent_path();
		}

		// Create the test jar.
		virtual void createJar(::std::wstring const & jar, 
			::std::vector< ::std::wstring> const & classPath) const;
};

class JvmTestDeath: public JvmTest {};

//
TEST_F(JvmTest, jar)
{
	EXPECT_NE(0, com::wholegroup::vector::jni::VECTOR_CORE_JAR.length());
}

//
TEST_F(JvmTest, destructor)
{
	JvmMock jvmMock;

	// Mark uninteresting mock function calls
	EXPECT_CALL(jvmMock, isLoaded())
		.Times(AnyNumber());

	EXPECT_CALL(jvmMock, isCreated())
		.Times(AnyNumber());

	// Create a VM
	jvmMock.Jvm::loadJvm(jni::JvmFinder().findJvm());
	jvmMock.Jvm::createJvm(L"");

	// Check the destructor
	EXPECT_CALL(jvmMock, destroyJvm())
		.Times(1);

	EXPECT_CALL(jvmMock, unloadJvm())
		.Times(1);

	jvmMock.Jvm::destructor();
}

//
TEST_F(JvmTestDeath, loadJvm_Already)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());

	EXPECT_DEATH(jvm.loadJvm(jni::JvmFinder().findJvm()), "");
}

//
TEST_F(JvmTest, loadJvm_MultiJava)
{
	jni::Jvm jvmFirst;

	jvmFirst.loadJvm(jni::JvmFinder().findJvm());
	jvmFirst.createJvm(L"");

	jni::Jvm jvmSecond;

	jvmSecond.loadJvm(L"Will be used the previously loaded version of a JVM library!");
	jvmSecond.createJvm(L"");
}

#ifdef WIN32 
//
TEST_F(JvmTest, loadJvm_Parent)
{
	// Create a previously loaded library
	::std::wstring firstVersion;

	{
		jni::Jvm jvmFirst;

		jvmFirst.loadJvm(jni::JvmFinder(L"Java7FamilyVersion").findJvm());
		jvmFirst.createJvm(L"");

		firstVersion = jvmFirst.getJavaVersion();
	}

	// Try to load another version of a JVM library
	jni::Jvm jvmSecond;

	jvmSecond.loadJvm(jni::JvmFinder(L"Java6FamilyVersion").findJvm());

	// Will be created VM of parent version
	jvmSecond.createJvm(L"");

	EXPECT_EQ(firstVersion, jvmSecond.getJavaVersion());
}
#endif // WIN32

//
TEST_F(JvmTest, loadJvm)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
}

//
TEST_F(JvmTest, isLoaded_Before)
{
	jni::Jvm jvm;

	EXPECT_FALSE(jvm.isLoaded());
}

//
TEST_F(JvmTest, isLoaded_After)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.unloadJvm();

	EXPECT_FALSE(jvm.isLoaded());
}

//
TEST_F(JvmTest, isLoaded_True)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());

	EXPECT_TRUE(jvm.isLoaded());
}

//
TEST_F(JvmTestDeath, unloadJvm_NotLoaded)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.unloadJvm(), "");
}

//
/*
TEST_F(JvmTestDeath, unloadJvm_ErrorFree)
{
	JvmMock jvmMock;

	EXPECT_CALL(jvmMock, isLoaded())
		.WillOnce(Return(true));

	EXPECT_THROW(jvmMock.Jvm::unloadJvm(), exception::IOException);
}
*/

//
TEST_F(JvmTest, unloadJvm)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.unloadJvm();
}

//
TEST_F(JvmTestDeath, createJvm_NotLoaded)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.createJvm(L""), "");
}

//
TEST_F(JvmTestDeath, createJvm_AlreadyCreated)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.createJvm(L""), "");
}

//
TEST_F(JvmTest, createJvm_SerialAtOneThread)
{
	{
		jni::Jvm jvm;

		// Loan and Create
		jvm.loadJvm(jni::JvmFinder().findJvm());
		jvm.createJvm(L"");

		// Check that it works
		{
			jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

			EXPECT_TRUE((bool)classString);
		}
	
		jvm.destroyJvm();
	}

	{
		jni::Jvm jvm;
		
		// Create one more time
		jvm.loadJvm(jni::JvmFinder().findJvm());
		jvm.createJvm(L"");

		// Check that it works
		{
			jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

			EXPECT_TRUE((bool)classString);
		}
	}
}

//
TEST_F(JvmTest, createJvm_ParallelAtOneThread)
{
	// Create the first
	jni::Jvm jvmFirst;

	jvmFirst.loadJvm(jni::JvmFinder().findJvm());
	jvmFirst.createJvm(L"");

	{
		jni::JClass classString(jvmFirst, jvmFirst.stdFindClass("java/lang/String"));

		EXPECT_TRUE((bool)classString);
	}

	// Create and destroy the second
	jni::Jvm jvmSecond;

	jvmSecond.loadJvm(jni::JvmFinder().findJvm());
	jvmSecond.createJvm(L"");

	// Check that the second works
	{
		jni::JClass classString(jvmSecond, jvmSecond.stdFindClass("java/lang/String"));

		EXPECT_TRUE((bool)classString);
	}

	jvmSecond.destroyJvm();

	// Check that the first works
	{
		jni::JClass classString(jvmFirst, jvmFirst.stdFindClass("java/lang/String"));

		EXPECT_TRUE((bool)classString);
	}
}

//
void anotherThread()
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	// Check that it works
	{
		jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

		EXPECT_TRUE((bool)classString);
	}

	jvm.destroyJvm();
}

TEST_F(JvmTest, createJvm_Multithreading)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	// Check that it works before
	{
		jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

		EXPECT_TRUE((bool)classString);
	}

	// Run the second thread and wait until stop
	::boost::thread thread(anotherThread);

	thread.join();

	// Check that it works after
	{
		jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

		EXPECT_TRUE((bool)classString);
	}
}

//
TEST_F(JvmTest, createJvm)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");
}

//
TEST_F(JvmTest, isCreated_Before)
{
	jni::Jvm jvm;

	EXPECT_FALSE(jvm.isCreated());
}

//
TEST_F(JvmTest, isCreated_After)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");
	jvm.destroyJvm();

	EXPECT_FALSE(jvm.isCreated());
}

//
TEST_F(JvmTest, isCreated_True)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_TRUE(jvm.isCreated());
}

//
TEST_F(JvmTest, getJniEnv)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());

	EXPECT_EQ(nullptr, jvm.getJniEnv());

	jvm.createJvm(L"");

	EXPECT_NE(nullptr, jvm.getJniEnv());

	jvm.destroyJvm();

	EXPECT_EQ(nullptr, jvm.getJniEnv());
}

//
TEST_F(JvmTestDeath, destroyJvm_NotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.destroyJvm(), "");
}

//
TEST_F(JvmTestDeath, destroyJvm_NonZeroCounterJPtr)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");
	jvm.addCounterJPtr();

	EXPECT_DEATH(jvm.destroyJvm(), "");

	jvm.subCounterJPtr();
}

//
TEST_F(JvmTestDeath, destroyJvm_NumberPerThreadIsZero)
{
	JvmMock jvmMock;

	EXPECT_CALL(jvmMock, isLoaded())
		.Times(AnyNumber());

	EXPECT_CALL(jvmMock, isCreated())
		.Times(AnyNumber());

	jvmMock.Jvm::loadJvm(jni::JvmFinder().findJvm());
	jvmMock.Jvm::createJvm(L"");
	jvmMock.Jvm::destroyJvm();

	EXPECT_CALL(jvmMock, isCreated())
		.WillOnce(Return(true));

	EXPECT_DEATH(jvmMock.Jvm::destroyJvm(), "");

	jvmMock.isCreated();
}

//
TEST_F(JvmTest, destroyJvm)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");
	jvm.destroyJvm();
}

//
TEST_F(JvmTestDeath, checkException_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.checkException(), "created");
}

//
TEST_F(JvmTest, checkException_No)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jvm.checkException();
}

//
TEST_F(JvmTest, checkException_Yes)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jvm.getJniEnv()->FindClass("class/not/found");

	EXPECT_THROW(jvm.checkException(), exception::IOException);
}

//
TEST_F(JvmTest, checkException_OnlyName)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jvm.getJniEnv()->FindClass(nullptr);

	// Check the message
	::std::string exceptionMessage;

	try
	{
		jvm.checkException();
	}
	catch (exception::IOException& ex)
	{
		exceptionMessage = ex.what();
	}

	EXPECT_EQ(::std::string("java.lang.NoClassDefFoundError"), exceptionMessage);
}


//
TEST_F(JvmTestDeath, wstringToJString_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.jstringToWstring(nullptr), "created");
}

//
TEST_F(JvmTest, wstringToJString)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JString testString(jvm, jvm.wstringToJString(L"test"));

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jmethodID methodToUpperCase(jvm.getMethodID(*classString, "toUpperCase", "()Ljava/lang/String;"));

	jni::JString testStringUpper(jvm, jvm.callObjectMethod(*testString, methodToUpperCase));

	EXPECT_EQ(::std::wstring(L"TEST"), jvm.jstringToWstring(*testStringUpper));
}

//
TEST_F(JvmTestDeath, jstringToWstring_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.jstringToWstring(nullptr), "created");
}

//
TEST_F(JvmTest, jstringToWstring)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));
	
	jmethodID methodStringConstructor(jvm.getMethodID(*classString, "<init>", "()V"));

	jni::JObject testString(jvm, jvm.newObject(*classString, methodStringConstructor));

	jmethodID methodStringGetClass(jvm.getMethodID(*classString, "getClass", "()Ljava/lang/Class;"));

	jni::JObject clazz(jvm, jvm.callObjectMethod(*testString, methodStringGetClass));

	jni::JClass classClass(jvm, jvm.stdFindClass("java/lang/Class"));

	jmethodID methodClassGetCanonicalName(jvm.getMethodID(*classClass, "getCanonicalName", "()Ljava/lang/String;"));

	jni::JString clazzName(jvm, jvm.callObjectMethod(*clazz, methodClassGetCanonicalName));

	EXPECT_EQ(::std::wstring(L"java.lang.String"), jvm.jstringToWstring(*clazzName));
}

//
TEST_F(JvmTestDeath, getClassPath_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getClassPath(L""), "created");
}

//
TEST_F(JvmTest, getClassPath_JarNotFound)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_THROW(jvm.getClassPath(L""), exception::IOException);
}

//
TEST_F(JvmTest, getClassPath_NotDefined)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	::std::wstring const jarFile = ::boost::filesystem::system_complete(
		::boost::filesystem::path(executablePath) / L"test.jar").wstring();
	::std::vector< ::std::wstring> const toClassPath;
	
	createJar(jarFile, toClassPath);

	::std::wstring const classPath = jvm.getClassPath(
		::boost::filesystem::system_complete(jarFile).wstring());

	EXPECT_EQ(::std::wstring(L""), classPath);

	::boost::filesystem::remove(jarFile);
}

//
TEST_F(JvmTest, getClassPath)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	::std::wstring const jarFile = ::boost::filesystem::system_complete(
		::boost::filesystem::path(executablePath) / L"test.jar").wstring();
	
	::std::vector< ::std::wstring> toClassPath = ::boost::assign::list_of
		(L"lib1/file1.jar")(L"lib2/file2.jar");

	createJar(jarFile, toClassPath);

	::std::wstring classPath = jvm.getClassPath(
		::boost::filesystem::system_complete(jarFile).wstring());

	EXPECT_EQ(boost::algorithm::join(toClassPath, L" "), classPath);

	::boost::filesystem::remove(jarFile);
}

//
TEST_F(JvmTestDeath, createClassLoader_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.createClassLoader(), "created");
}

//
TEST_F(JvmTestDeath, createClassLoader_AlreadyCreated)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jvm.createClassLoader();

	EXPECT_DEATH(jvm.createClassLoader(), "");
}

//
TEST_F(JvmTest, createClassLoader_WithoutJar)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jvm.createClassLoader();
}

//
TEST_F(JvmTest, createClassLoader_WithJar)
{
	// Create the jar
	::std::wstring const jarFile = ::boost::filesystem::system_complete(
		::boost::filesystem::path(executablePath) / L"test.jar").wstring();
	::std::vector< ::std::wstring> checkedJars = ::boost::assign::list_of
		(L"lib1/a.jar")(L"lib2/b.jar")(L"lib3/c.jar");

	createJar(jarFile, checkedJars);

	// Check the created jar
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(jarFile).wstring());

	jvm.createClassLoader();

	// Check the created class loader
	jobject classLoader = jvm.getClassLoader();

	// java.net.URLClassLoader
	jni::JClass classURLClassLoader(jvm, jvm.stdFindClass("java/net/URLClassLoader"));

	// java.net.URLClassLoader$getURLs
	jmethodID methodURLClassLoaderGetURLs(
		jvm.getMethodID(*classURLClassLoader, "getURLs", "()[Ljava/net/URL;"));

	// java.net.URL
	jni::JClass classURL(jvm, jvm.stdFindClass("java/net/URL"));

	// java.net.URL$getPath
	jmethodID methodURLGetPath(jvm.getMethodID(*classURL, "getPath", "()Ljava/lang/String;"));

	// URL[] urls = classLoader.getURLs()
	jni::JObjectArray urls(jvm, jvm.callObjectMethod(classLoader, methodURLClassLoaderGetURLs));

	// Check size
	checkedJars.push_back(
		::boost::filesystem::path(jarFile).filename().wstring());

	long sizeArray = static_cast<long>(jvm.getArrayLength(*urls));

	EXPECT_EQ(sizeArray, checkedJars.size());

	// Compare the jars
	for (long i = 0; i < sizeArray; ++i)
	{
		jni::JObject url(jvm, jvm.getObjectArrayElement(*urls, static_cast<jsize>(i)));
		jni::JString urlPath(jvm, jvm.callObjectMethod(*url, methodURLGetPath));

		EXPECT_EQ(
			jvm.jstringToWstring(*urlPath), 
			::std::wstring(L"file:").append(::boost::filesystem::system_complete(
				::boost::filesystem::path(executablePath) / checkedJars[i]).wstring()).append(L"!/")
		);
	}
}

//
TEST_F(JvmTest, createClassLoader_DoubleCall)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());

	// The first call
	jvm.createJvm(L"");
	jvm.createClassLoader();
	jvm.destroyJvm();

	// The second call
	jvm.createJvm(L"");
	jvm.createClassLoader();
	jvm.destroyJvm();
}

//
TEST_F(JvmTest, isClassLoaderCreated_Before)
{
	jni::Jvm jvm;

	EXPECT_FALSE(jvm.isClassLoaderCreated());

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_FALSE(jvm.isClassLoaderCreated());
}

//
TEST_F(JvmTest, isClassLoaderCreated_After)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");
	jvm.createClassLoader();
	jvm.destroyJvm();

	EXPECT_FALSE(jvm.isClassLoaderCreated());
}

//
TEST_F(JvmTest, isClassLoaderCreated_True)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");
	jvm.createClassLoader();

	EXPECT_TRUE(jvm.isClassLoaderCreated());
}

//
TEST_F(JvmTestDeath, getJavaVersion_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getJavaVersion(), "created");
}

//
TEST_F(JvmTest, getJavaVersion)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");
	
	EXPECT_TRUE(0 < jvm.getJavaVersion().length());
	
	::std::wcout << jvm.getJavaVersion() << ::std::endl;
}

//
TEST_F(JvmTestDeath, stdFindClass_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.stdFindClass("java/lang/String"), "created");
}

//
TEST_F(JvmTest, stdFindClass_NotFound)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_THROW(jvm.stdFindClass("not/found/Class"), exception::IOException);
}

//
TEST_F(JvmTest, stdFindClass)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_TRUE((bool)jni::JClass(jvm, jvm.stdFindClass("java/lang/String")));
}

//
TEST_F(JvmTestDeath, findClass_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.findClass(L"java.lang.String"), "created");
}

//
TEST_F(JvmTest, findClass_NotFound)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_THROW(jvm.findClass(L"not.found.Class"), exception::IOException);
}

//
TEST_F(JvmTest, findClass)
{
	JvmMock jvmMock;

	// Mark uninteresting mock function calls
	EXPECT_CALL(jvmMock, isLoaded())
		.Times(AnyNumber());

	EXPECT_CALL(jvmMock, isCreated())
		.Times(AnyNumber());

	jvmMock.Jvm::loadJvm(jni::JvmFinder().findJvm());
	jvmMock.Jvm::createJvm(L"");

	EXPECT_CALL(jvmMock, createClassLoader())
		.WillOnce(DoDefault());

	EXPECT_TRUE((bool)jni::JClass(jvmMock, jvmMock.Jvm::findClass(L"java.lang.String")));
}

//
TEST_F(JvmTestDeath, newObject_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.newObject(nullptr, nullptr), "created");
}

//
TEST_F(JvmTestDeath, newObject_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));
	jmethodID   constructor(jvm.getMethodID(*classString, "<init>", "()V"));

	EXPECT_DEATH(jvm.newObject(nullptr, constructor), "null");
	EXPECT_DEATH(jvm.newObject(*classString, nullptr), "null");
}

//
TEST_F(JvmTest, newObject_Error)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));
	jmethodID   constructor(jvm.getMethodID(*classString, "<init>", "(Ljava/lang/String;)V"));

	EXPECT_THROW(jvm.newObject(*classString, constructor, nullptr), exception::IOException);
}

//
TEST_F(JvmTest, newObject)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));
	jmethodID   constructor(jvm.getMethodID(*classString, "<init>", "(Ljava/lang/String;)V"));

	jni::JString okString(jvm, 
		jvm.newObject(*classString, constructor, *jni::JString(jvm, jvm.wstringToJString(L"OK"))));

	EXPECT_TRUE((bool)okString);
	EXPECT_EQ(::std::wstring(L"OK"), jvm.jstringToWstring(*okString));
}

//
TEST_F(JvmTestDeath, newObjectArray_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.newObjectArray(0, nullptr), "created");
}

//
TEST_F(JvmTestDeath, newObjectArray_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	EXPECT_DEATH(jvm.newObjectArray(0, *classString), "zero");
	EXPECT_DEATH(jvm.newObjectArray(1, nullptr), "null");
}

//
TEST_F(JvmTest, newObjectArray)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JObjectArray testArray(jvm, 
		jvm.newObjectArray(1, *jni::JClass(jvm, jvm.stdFindClass("java/lang/String"))));

	EXPECT_TRUE((bool)testArray);
}

//
TEST_F(JvmTestDeath, newByteArray_ZeroLength)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.newByteArray(0), "zero");
	EXPECT_DEATH(jvm.newByteArray(-1), "greater");
}

//
TEST_F(JvmTestDeath, newByteArray_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.newByteArray(256), "created");
}

//
TEST_F(JvmTest, newByteArray)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JByteArray testArray(jvm, jvm.newByteArray(256));

	EXPECT_TRUE((bool)testArray);
	EXPECT_EQ(256, jvm.getArrayLength(*testArray));
}

//
TEST_F(JvmTestDeath, newGlobalRef_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.newGlobalRef(nullptr), "created");
}

//
TEST_F(JvmTestDeath, newGlobalRef_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.newGlobalRef(nullptr), "null");
}

//
TEST_F(JvmTest, newGlobalRef)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jobject localObject  = jvm.stdFindClass("java/lang/String");
	jobject globalObject = jvm.newGlobalRef(localObject);

	EXPECT_NE(localObject, globalObject);

	jvm.deleteGlobalRef(globalObject);
	jvm.deleteLocalRef(localObject);
}

//
TEST_F(JvmTestDeath, deleteGlobalRef_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.deleteGlobalRef(nullptr), "created");
}

//
TEST_F(JvmTestDeath, deleteGlobalRef_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.deleteGlobalRef(nullptr), "null");
}

//
TEST_F(JvmTest, deleteGlobalRef)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jobject globalObject = jvm.newGlobalRef(*jni::JClass(jvm, jvm.stdFindClass("java/lang/String")));

	jvm.deleteGlobalRef(globalObject);
}

//
TEST_F(JvmTestDeath, deleteLocalRef_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.deleteLocalRef(nullptr), "created");
}

//
TEST_F(JvmTestDeath, deleteLocalRef_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.deleteLocalRef(nullptr), "null");
}

//
TEST_F(JvmTest, deleteLocalRef)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jvm.deleteLocalRef(jvm.stdFindClass("java/lang/String"));
}

//
TEST_F(JvmTestDeath, getStaticFieldID_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getStaticFieldID(nullptr, "", ""), "created");
}

//
TEST_F(JvmTestDeath, getStaticFieldID_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.getStaticFieldID(nullptr, "", ""), "null");
}

//
TEST_F(JvmTest, getStaticFieldID_Error)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_THROW(jvm.getStaticFieldID(*jni::JClass(jvm, 
		jvm.stdFindClass("java/lang/String")), "", ""), exception::IOException);
}

//
TEST_F(JvmTest, getStaticFieldID)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jfieldID field = jvm.getStaticFieldID(
		*jni::JClass(jvm, jvm.stdFindClass("java/lang/Integer")), "MIN_VALUE", "I");
	UNREFERENCED_PARAMETER(field);
}

//
TEST_F(JvmTestDeath, getStaticObjectField_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getStaticObjectField(nullptr, nullptr), "created");
}

//
TEST_F(JvmTestDeath, getStaticObjectField_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classInteger(jvm, jvm.stdFindClass("java/lang/Integer"));
	jfieldID    fieldMIN_VALUE(jvm.getStaticFieldID(*classInteger, "MIN_VALUE", "I"));

	EXPECT_DEATH(jvm.getStaticObjectField(nullptr, fieldMIN_VALUE), "null");
	EXPECT_DEATH(jvm.getStaticObjectField(*classInteger, nullptr), "null");
}

//
TEST_F(JvmTest, getStaticObjectField)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classAttributesName(jvm, jvm.stdFindClass("java/util/jar/Attributes$Name"));

	jfieldID fieldMANIFEST_VERSION(
		jvm.getStaticFieldID(*classAttributesName, "MANIFEST_VERSION", "Ljava/util/jar/Attributes$Name;"));

	jni::JObject objectMANIFEST_VERSION(jvm, 
		jvm.getStaticObjectField(*classAttributesName, fieldMANIFEST_VERSION));

	jmethodID toString(jvm.getMethodID(*classAttributesName, "toString", "()Ljava/lang/String;"));

	jni::JString jstrMANIFEST_VERSION(jvm, jvm.callObjectMethod(*objectMANIFEST_VERSION, toString));

	EXPECT_EQ(::std::wstring(L"Manifest-Version"), jvm.jstringToWstring(*jstrMANIFEST_VERSION));
}

//
TEST_F(JvmTestDeath, getStaticMethodID_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getStaticMethodID(nullptr, "", ""), "created");
}

//
TEST_F(JvmTestDeath, getStaticMethodID_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.getStaticMethodID(nullptr, "", ""), "null");
}

//
TEST_F(JvmTest, getStaticMethodID_Error)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	EXPECT_THROW(jvm.getStaticMethodID(*classString, "<init>", "(I)Z"), exception::IOException);
}

//
TEST_F(JvmTest, getStaticMethodID)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jmethodID methodValueOf(jvm.getStaticMethodID(*classString, "valueOf", "(Ljava/lang/Object;)Ljava/lang/String;"));

	EXPECT_TRUE(nullptr != methodValueOf);
}

//
TEST_F(JvmTestDeath, getMethodID_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getMethodID(nullptr, "", ""), "created");
}

//
TEST_F(JvmTestDeath, getMethodID_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.getMethodID(nullptr, "", ""), "null");
}

//
TEST_F(JvmTest, getMethodID_Error)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	EXPECT_THROW(jvm.getMethodID(*classString, "<init>", "(I)Z"), exception::IOException);
}

//
TEST_F(JvmTest, getMethodID)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jmethodID methodConstructor = jvm.getMethodID(*classString, "<init>", "()V");

	EXPECT_TRUE(nullptr != methodConstructor);
}

//
TEST_F(JvmTestDeath, getArrayLength_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getArrayLength(nullptr), "created");
}

//
TEST_F(JvmTestDeath, getArrayLength_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	EXPECT_DEATH(jvm.getArrayLength(nullptr), "null");
}

//
TEST_F(JvmTest, getArrayLength)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	for(int i = 1; i < 10; ++i)
	{
		jni::JObjectArray array(jvm, jvm.newObjectArray(i, *classString));
		
		jsize sizeArray = jvm.getArrayLength(*array);

		EXPECT_EQ(i, sizeArray);
	}
}

//
TEST_F(JvmTestDeath, getObjectArrayElement_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getObjectArrayElement(nullptr, 0), "created");
}

//
TEST_F(JvmTestDeath, getObjectArrayElement_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));
	jni::JObjectArray array(jvm, jvm.newObjectArray(1, *classString));

	EXPECT_DEATH(jvm.getObjectArrayElement(nullptr, 0), "null");
	EXPECT_DEATH(jvm.getObjectArrayElement(*array, -1), "zero");
	EXPECT_DEATH(jvm.getObjectArrayElement(*array, jvm.getArrayLength(*array)), "bounds");
}

//
TEST_F(JvmTest, getObjectArrayElement)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));
	jni::JObjectArray array(jvm, jvm.newObjectArray(1, *classString));

	jvm.setObjectArrayElement(*array, 0, *jni::JString(jvm, jvm.wstringToJString(L"Test")));

	jni::JString jstrTest(jvm, jvm.getObjectArrayElement(*array, 0));

	EXPECT_EQ(::std::wstring(L"Test"), jvm.jstringToWstring(*jstrTest));
}

//
TEST_F(JvmTestDeath, getByteArrayRegion_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.getByteArrayRegion(nullptr, 0, 0, nullptr), "created");
}

//
TEST_F(JvmTestDeath, getByteArrayRegion_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	::std::wstring word(L"Bytes");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jmethodID methodStringGetBytes(jvm.getMethodID(*classString, "getBytes", "()[B"));

	jni::JString jstrBytes(jvm, jvm.wstringToJString(word));

	jni::JByteArray bytes(jvm, jvm.callObjectMethod(*jstrBytes, methodStringGetBytes));

	::std::vector<char> checkedBytes(word.size(), 0);

	jsize len  = static_cast<jsize>(word.size());
	jbyte *buf = reinterpret_cast<jbyte*>(&checkedBytes[0]);

	EXPECT_DEATH(jvm.getByteArrayRegion(nullptr, 0, len, buf), "null");
	EXPECT_DEATH(jvm.getByteArrayRegion(*bytes, -1, len, buf), "zero");
	EXPECT_DEATH(jvm.getByteArrayRegion(*bytes, len, 1, buf), "bounds");
	EXPECT_DEATH(jvm.getByteArrayRegion(*bytes, 0, 0, buf), "zero");
	EXPECT_DEATH(jvm.getByteArrayRegion(*bytes, 1, len, buf), "bounds");
	EXPECT_DEATH(jvm.getByteArrayRegion(*bytes, 0, len, nullptr), "null");
}

//
TEST_F(JvmTest, getByteArrayRegion)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	::std::wstring word(L"Bytes");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jmethodID methodStringGetBytes(jvm.getMethodID(*classString, "getBytes", "()[B"));

	jni::JString jstrBytes(jvm, jvm.wstringToJString(word));

	jni::JByteArray bytes(jvm, jvm.callObjectMethod(*jstrBytes, methodStringGetBytes));

	::std::vector<char> checkedBytes(word.size(), 0);

	jvm.getByteArrayRegion(*bytes, 0, static_cast<jsize>(word.size()), reinterpret_cast<jbyte*>(&checkedBytes[0]));

	EXPECT_EQ(::std::wstring(checkedBytes.begin(), checkedBytes.end()), word);
}

//
TEST_F(JvmTestDeath, setObjectArrayElement_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.setObjectArrayElement(nullptr, 0, nullptr), "created");
}

//
TEST_F(JvmTestDeath, setObjectArrayElement_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jni::JObjectArray array(jvm, jvm.newObjectArray(1, *classString));

	jni::JString jstrTest(jvm, jvm.wstringToJString(L"test"));

	EXPECT_DEATH(jvm.setObjectArrayElement(nullptr, 0, *jstrTest), "null");
	EXPECT_DEATH(jvm.setObjectArrayElement(*array, -1, *jstrTest), "zero");
	EXPECT_DEATH(jvm.setObjectArrayElement(*array, jvm.getArrayLength(*array), *jstrTest), "bounds");
	EXPECT_DEATH(jvm.setObjectArrayElement(*array,0, nullptr), "null");
}

//
TEST_F(JvmTest, setObjectArrayElement)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	::std::wstring word(L"Test");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jni::JObjectArray array(jvm, jvm.newObjectArray(1, *classString));

	jni::JString jstrTest(jvm, jvm.wstringToJString(word));

	jvm.setObjectArrayElement(*array,0, *jstrTest);

	jni::JString jstrChecked(jvm, jvm.getObjectArrayElement(*array, 0));

	EXPECT_EQ(word, jvm.jstringToWstring(*jstrChecked));
}

//
TEST_F(JvmTestDeath, setByteArrayRegion_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.setByteArrayRegion(nullptr, 0, 256, nullptr), "created");
}

//
TEST_F(JvmTestDeath, setByteArrayRegion_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	//
	::std::vector<char> carray(256, 0);

	jni::JByteArray jarray(jvm, jvm.newByteArray(128));

	//
	EXPECT_DEATH(jvm.setByteArrayRegion(nullptr, 0, 128, (jbyte*)&carray[0]), "null");
	EXPECT_DEATH(jvm.setByteArrayRegion(*jarray, -1, static_cast<jsize>(carray.size()), (jbyte*)&carray[0]), "zero");
	EXPECT_DEATH(jvm.setByteArrayRegion(*jarray, 0, static_cast<jsize>(carray.size()), (jbyte*)&carray[0]), "big");
}

//
TEST_F(JvmTestDeath, setByteArrayRegion)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	//
	::std::vector<char> carray(128, 0);

	jni::JByteArray jarray(jvm, jvm.newByteArray(128));

	//
	jvm.setByteArrayRegion(*jarray, 0, static_cast<jsize>(carray.size()), (jbyte*)&carray[0]);
}

//
TEST_F(JvmTestDeath, callStaticObjectMethod_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.callStaticObjectMethod(nullptr, nullptr), "created");
}

//
TEST_F(JvmTestDeath, callStaticObjectMethod_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jmethodID methodValueOf(jvm.getStaticMethodID(*classString, 
		"valueOf", "(Ljava/lang/Object;)Ljava/lang/String;"));

	EXPECT_DEATH(jvm.callStaticObjectMethod(nullptr, methodValueOf), "null");
	EXPECT_DEATH(jvm.callStaticObjectMethod(*classString, nullptr), "null");
}

//
TEST_F(JvmTest, callStaticObjectMethod)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	int number = 54321;

	jni::JClass classInteger(jvm, jvm.stdFindClass("java/lang/Integer"));

	jmethodID methodIntegerConstructor(jvm.getMethodID(*classInteger, "<init>", "(I)V"));

	jni::JObject objNumber(jvm, jvm.newObject(*classInteger, methodIntegerConstructor, number));

	jni::JClass classString(jvm, jvm.stdFindClass("java/lang/String"));

	jmethodID methodValueOf(jvm.getStaticMethodID(*classString, "valueOf", "(Ljava/lang/Object;)Ljava/lang/String;"));

	jni::JString jstrChecked(jvm, jvm.callStaticObjectMethod(*classString, methodValueOf, *objNumber));

	EXPECT_EQ(number, ::boost::lexical_cast<int>(jvm.jstringToWstring(*jstrChecked)));
}

//
TEST_F(JvmTestDeath, callVoidMethod_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.callVoidMethod(nullptr, nullptr), "created");
}

//
TEST_F(JvmTestDeath, callVoidMethod_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classList(jvm, jvm.stdFindClass("java/util/ArrayList"));

	jmethodID methodClear(jvm.getMethodID(*classList, "clear", "()V"));

	EXPECT_DEATH(jvm.callStaticObjectMethod(nullptr, methodClear), "null");
	EXPECT_DEATH(jvm.callStaticObjectMethod(*classList, nullptr), "null");
}

//
TEST_F(JvmTest, callVoidMethod)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classList(jvm, jvm.stdFindClass("java/util/ArrayList"));

	jmethodID methodConstructor(jvm.getMethodID(*classList, "<init>", "()V"));
	jmethodID methodAdd(jvm.getMethodID(*classList, "add", "(ILjava/lang/Object;)V"));
	jmethodID methodToArray(jvm.getMethodID(*classList, "toArray", "()[Ljava/lang/Object;"));

	jni::JObject testList(jvm, jvm.newObject(*classList, methodConstructor));

	// check the empty array
	jni::JObjectArray emptyArray(jvm, jvm.callObjectMethod(*testList, methodToArray));

	EXPECT_EQ(0, jvm.getArrayLength(*emptyArray));

	// add
	jni::JString jstrFirst(jvm, jvm.wstringToJString(L"First"));

	for(int i = 0; i < 10; ++i)
	{
		jvm.callVoidMethod(*testList, methodAdd, 0, *jstrFirst);
	}

	// check the nonempty array
	jni::JObjectArray nonemptyArray(jvm, jvm.callObjectMethod(*testList, methodToArray));

	EXPECT_EQ(10, jvm.getArrayLength(*nonemptyArray));
}

//
TEST_F(JvmTestDeath, callDoubleMethod_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.callDoubleMethod(nullptr, nullptr), "created");
}

//
TEST_F(JvmTestDeath, callDoubleMethod_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classDouble(jvm, jvm.stdFindClass("java/lang/Double"));

	jmethodID methodDoubleValue(jvm.getMethodID(*classDouble, "doubleValue", "()D"));

	EXPECT_DEATH(jvm.callDoubleMethod(nullptr, methodDoubleValue), "null");
	EXPECT_DEATH(jvm.callDoubleMethod(*classDouble, nullptr), "null");
}

//
TEST_F(JvmTest, callDoubleMethod)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	::std::wstring number(L"3.1415");

	jni::JClass classDouble(jvm, jvm.stdFindClass("java/lang/Double"));

	jmethodID methodConstructor(jvm.getMethodID(*classDouble, "<init>", "(Ljava/lang/String;)V"));
	jmethodID methodDoubleValue(jvm.getMethodID(*classDouble, "doubleValue", "()D"));

	jni::JObject testDouble(jvm, 
		jvm.newObject(*classDouble, methodConstructor, *jni::JString(jvm, jvm.wstringToJString(number))));

	double checkedDouble = jvm.callDoubleMethod(*testDouble, methodDoubleValue);

	EXPECT_DOUBLE_EQ(::boost::lexical_cast<double>(number), checkedDouble);
}

//
TEST_F(JvmTestDeath, callObjectMethod_JvmIsNotCreated)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.callObjectMethod(nullptr, nullptr), "created");
}

//
TEST_F(JvmTestDeath, callObjectMethod_InvalidParameters)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	jni::JClass classDouble(jvm, jvm.stdFindClass("java/lang/Double"));

	jmethodID methodToString(jvm.getMethodID(*classDouble, "toString", "()Ljava/lang/String;"));

	EXPECT_DEATH(jvm.callObjectMethod(nullptr, methodToString), "null");
	EXPECT_DEATH(jvm.callObjectMethod(*classDouble, nullptr), "null");
}

//
TEST_F(JvmTest, callObjectMethod)
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(L"");

	::std::wstring number(L"3.1415");

	jni::JClass classDouble(jvm, jvm.stdFindClass("java/lang/Double"));

	jmethodID methodConstructor(jvm.getMethodID(*classDouble, "<init>", "(Ljava/lang/String;)V"));
	jmethodID methodToString(jvm.getMethodID(*classDouble, "toString", "()Ljava/lang/String;"));

	jni::JObject testDouble(jvm, 
		jvm.newObject(*classDouble, methodConstructor, *jni::JString(jvm, jvm.wstringToJString(number))));

	jni::JString jstrDouble(jvm, jvm.callObjectMethod(*testDouble, methodToString));

	EXPECT_EQ(number, jvm.jstringToWstring(*jstrDouble));
}

//
TEST_F(JvmTest, getCounterJPtr)
{
	jni::Jvm jvm;

	EXPECT_EQ(0, jvm.getCounterJPtr());

	jvm.addCounterJPtr();

	EXPECT_EQ(1, jvm.getCounterJPtr());
}

//
TEST_F(JvmTest, addCounterJPtr)
{
	jni::Jvm jvm;

	EXPECT_EQ(0, jvm.getCounterJPtr());

	jvm.addCounterJPtr();

	EXPECT_EQ(1, jvm.getCounterJPtr());
}

//
TEST_F(JvmTestDeath, subCounterJPtr_Invalid)
{
	jni::Jvm jvm;

	EXPECT_DEATH(jvm.subCounterJPtr(), "zero");
}

//
TEST_F(JvmTest, subCounterJPtr)
{
	jni::Jvm jvm;

	jvm.addCounterJPtr();

	EXPECT_EQ(1, jvm.getCounterJPtr());

	jvm.subCounterJPtr();

	EXPECT_EQ(0, jvm.getCounterJPtr());
}

/************************************************************************/
/* Create the test jar.                                                 */
/************************************************************************/
void JvmTest::createJar(::std::wstring const & jar, 
	::std::vector< ::std::wstring> const & classPath) const
{
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());

	EXPECT_TRUE(jvm.isLoaded());

	jvm.createJvm(L"");

	EXPECT_TRUE(jvm.isCreated());

	// java.io.FileOutputStream
	jni::JClass classFileOutputStream(jvm, jvm.stdFindClass("java/io/FileOutputStream"));

	// java.io.FileOutputStream$constructor
	jmethodID methodFileOutputStreamConstructor(
		jvm.getMethodID(*classFileOutputStream, "<init>", "(Ljava/lang/String;)V"));

	// FileOutputStream fout = new FileOutputStream("test.jar")
	jni::JString jstrTestJar(jvm, jvm.wstringToJString(jar));
	jni::JObject fout(jvm, jvm.newObject(*classFileOutputStream, methodFileOutputStreamConstructor, *jstrTestJar));

	// java.util.jar.JarOutputStream
	jni::JClass classJarOutputStream(jvm, jvm.stdFindClass("java/util/jar/JarOutputStream"));

	// java.util.jar.JarOutputStream$constructor
	jmethodID methodJarOutputStreamConstructor(
		jvm.getMethodID(*classJarOutputStream, "<init>", "(Ljava/io/OutputStream;)V"));

	// JarOutputStream jarOut = new JarOutputStream(fout)
	jni::JObject jarOut(jvm, 
		jvm.newObject(*classJarOutputStream, methodJarOutputStreamConstructor, *fout));

	// java.util.zip.ZipEntry
	jni::JClass classZipEntry(jvm, jvm.stdFindClass("java/util/zip/ZipEntry"));

	// java.util.zip.ZipEntry$constructor
	jmethodID methodZipEntryConstructor(jvm.getMethodID(*classZipEntry, "<init>", "(Ljava/lang/String;)V"));

	// ZipEntry entryMeta = new ZipEntry("META-INF/")
	jni::JString jstrMetaInf(jvm, jvm.wstringToJString(L"META-INF/"));
	jni::JObject entryMeta(jvm, jvm.newObject(*classZipEntry, methodZipEntryConstructor, *jstrMetaInf));

	// ZipEntry entryManifest = new ZipEntry("META-INF/MANIFEST.MF")
	jni::JString jstrManifest(jvm, jvm.wstringToJString(L"META-INF/MANIFEST.MF"));
	jni::JObject entryManifest(jvm, jvm.newObject(*classZipEntry, methodZipEntryConstructor, *jstrManifest));

	// java.util.jar.JarOutputStream$putNextEntry
	jmethodID methodJarOutputStreamPutNextEntry(
		jvm.getMethodID(*classJarOutputStream, "putNextEntry", "(Ljava/util/zip/ZipEntry;)V"));

	// jarOut.putNextEntry(entryMeta)
	jvm.callVoidMethod(*jarOut, methodJarOutputStreamPutNextEntry, *entryMeta);

	// jarOut.putNextEntry(entryMeta)
	jvm.callVoidMethod(*jarOut, methodJarOutputStreamPutNextEntry, *entryManifest);

	// java.util.jar.Manifest
	jni::JClass classManifest(jvm, jvm.stdFindClass("java/util/jar/Manifest"));

	// java.util.jar.Manifest$constructor
	jmethodID methodManifestConstructor(jvm.getMethodID(*classManifest, "<init>", "()V"));

	// Manifest manifest = new Manifest()
	jni::JObject manifest(jvm, jvm.newObject(*classManifest, methodManifestConstructor));

	// java.util.jar.Manifest$getMainAttributes
	jmethodID methodManifestGetMainAttributes(
		jvm.getMethodID(*classManifest, "getMainAttributes", "()Ljava/util/jar/Attributes;"));

	// Attributes manifestAttributes = manifest.getMainAttributes()
	jni::JObject manifestAttributes(jvm, jvm.callObjectMethod(*manifest, methodManifestGetMainAttributes));

	// java.util.jar.Attributes
	jni::JClass classAttributes(jvm, jvm.stdFindClass("java/util/jar/Attributes"));

	// java.util.jar.Attributes$put
	jmethodID methodAttributesPut(
		jvm.getMethodID(*classAttributes, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"));

	// java.util.jar.Attributes.Name
	jni::JClass classAttributesName(jvm, jvm.stdFindClass("java/util/jar/Attributes$Name"));

	// java.util.jar.Attributes.Name$MANIFEST_VERSION
	jfieldID fieldMANIFEST_VERSION(jvm.getStaticFieldID(*classAttributesName, "MANIFEST_VERSION", "Ljava/util/jar/Attributes$Name;"));

	// Attributes.Name.MANIFEST_VERSION
	jni::JObject MANIFEST_VERSION(jvm, jvm.getStaticObjectField(*classAttributesName, fieldMANIFEST_VERSION));

	// put(Attributes.Name.MANIFEST_VERSION, "1.0")
	jni::JString jstrVersion(jvm, jvm.wstringToJString(L"1.0"));
	jni::JObject attr1(jvm, jvm.callObjectMethod(*manifestAttributes, methodAttributesPut, *MANIFEST_VERSION, *jstrVersion));

	if (!classPath.empty())
	{
		// java.util.jar.Attributes.Name$CLASS_PATH
		jfieldID fieldCLASS_PATH(jvm.getStaticFieldID(*classAttributesName, "CLASS_PATH", "Ljava/util/jar/Attributes$Name;"));

		// Attributes.Name.CLASS_PATH
		jni::JObject CLASS_PATH(jvm, jvm.getStaticObjectField(*classAttributesName, fieldCLASS_PATH));

		// put(Attributes.Name.CLASS_PATH, "lib1/a.jar lib2/b.jar lib3/c.jar")
		jni::JString jstrClassPath(jvm, jvm.wstringToJString(::boost::join(classPath, L" ")));
		jni::JObject attr2(jvm, jvm.callObjectMethod(*manifestAttributes, methodAttributesPut, *CLASS_PATH, *jstrClassPath));
	}

	// java.util.jar.Manifest$write
	jmethodID methodManifestWrite(
		jvm.getMethodID(*classManifest, "write", "(Ljava/io/OutputStream;)V"));

	// manifest.write(jarOut);
	jvm.callVoidMethod(*manifest, methodManifestWrite, *jarOut);

	// java.util.jar.JarOutputStream$close
	jmethodID methodJarOutputStreamClose(jvm.getMethodID(*classJarOutputStream, "close", "()V"));

	// jarOut.close()
	jvm.callVoidMethod(*jarOut, methodJarOutputStreamClose);

	// java.io.FileOutputStream$close
	jmethodID methodFileOutputStreamClose(
		jvm.getMethodID(*classFileOutputStream, "close", "()V"));

	// fout.close();
	jvm.callVoidMethod(*fout, methodFileOutputStreamClose);
}
