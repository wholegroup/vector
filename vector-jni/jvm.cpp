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
#include "jvm.h"
#include "j_unique_ptr.h"

using namespace ::com::wholegroup::vector;
using namespace ::com::wholegroup::vector::jni;

// Ansi => Unicode
#define WIDEN(quote)  WIDEN2(quote)
#define WIDEN2(quote) L##quote

#ifdef __linux__
#	define RTLD_UNKNOWN RTLD_NEXT // because RTLD_DEFAULT == nullptr
#elif __APPLE__
#	define RTLD_UNKNOWN RTLD_DEFAULT
#endif

// Types of JNI functions
#define JNI_CreateJavaVM           "JNI_CreateJavaVM"
#define JNI_CreateJavaVM_Impl      "JNI_CreateJavaVM_Impl"
#define JNI_GetCreatedJavaVMs      "JNI_GetCreatedJavaVMs"
#define JNI_GetCreatedJavaVMs_Impl "JNI_GetCreatedJavaVMs_Impl"

typedef jint (JNICALL CreateJavaVM_t)(JavaVM **pvm, JNIEnv **env, void *args);
typedef jint (JNICALL GetCreatedJavaVMs_t)(JavaVM **vmBuf, jsize bufLen, jsize *nVMs);

// Initialization of static members
::boost::thread_specific_ptr<unsigned long> Jvm::ptrNumberPerThread;
::boost::mutex Jvm::mutexLock;

/************************************************************************/
/* Constructor.                                                         */
/************************************************************************/
Jvm::Jvm() :
	jvmHandle(nullptr), 
	jvm(nullptr), 
	jniEnv(nullptr), 
	classLoader(nullptr), 
	methodLoadClass(nullptr),
	counterJPtr(0)
{
}

/************************************************************************/
/* Destructor.                                                          */
/************************************************************************/
Jvm::~Jvm()
{
	try
	{
		destructor();
	}
	catch(...)
	{
	}
}

/************************************************************************/
/* Destructor is created for check in tests.                            */
/************************************************************************/
void Jvm::destructor()
{
	// Destroy the VM if it is created (double check)
	try
	{
		if (this->isCreated())
		{
			this->destroyJvm();
		}
	}
	catch (exception::Exception const & ex)
	{
		UNREFERENCED_PARAMETER(ex);
	}

	// Unload the JVM library if it is loaded (double check)
	try
	{
		if (this->isLoaded())
		{
			this->unloadJvm();
		}
	}
	catch (exception::Exception const & ex)
	{
		UNREFERENCED_PARAMETER(ex);
	}
}

/************************************************************************/
/* Load the JVM library by the specified path.                          */
/************************************************************************/
void Jvm::loadJvm(const ::std::wstring& pathToJvm)
{
	BOOST_ASSERT_MSG(!this->isLoaded(), "The JVM library has been loaded already.");
	
#ifdef WIN32
	// Check if the JVM library is already loaded 
	if (0 != GetModuleHandleEx(0, JvmFinder::JVM_DLL.c_str(), &this->jvmHandle))
	{
		// Check that this is JVM library
		CreateJavaVM_t* pfnCreateJavaVM = (CreateJavaVM_t*)GetProcAddress(this->jvmHandle, JNI_CreateJavaVM);

		if (nullptr != pfnCreateJavaVM)
		{
			return;
		}

		FreeLibrary(this->jvmHandle);

		this->jvmHandle = nullptr;
	}

	// Try to load by specified path
	// fixed bug #10 (http://www.duckware.com/tech/java6msvcr71.html)
	::SetDllDirectoryW(::boost::filesystem::path(pathToJvm).parent_path().parent_path().wstring().c_str());

	this->jvmHandle = LoadLibrary(pathToJvm.c_str());

	if (nullptr == this->jvmHandle)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			::std::wstring(L"An error loading the JVM library ").append(pathToJvm).append(L".")));
	}
#endif // WIN32

#if defined(__APPLE__) || defined(__linux__)
	// Check that JVM library is loaded
	this->jvmHandle = RTLD_UNKNOWN;

	CreateJavaVM_t * const pfnCreateJavaVM = (CreateJavaVM_t*)dlsym(
		this->jvmHandle, JNI_CreateJavaVM);

	if (nullptr != pfnCreateJavaVM)
	{
		return;
	}

	CreateJavaVM_t * const pfnCreateJavaVM_Impl = (CreateJavaVM_t*)dlsym(
		this->jvmHandle, JNI_CreateJavaVM_Impl);

	if (nullptr != pfnCreateJavaVM_Impl)
	{
		return;
	}

	// Load
	this->jvmHandle = dlopen(::boost::locale::conv::utf_to_utf<char>(
		pathToJvm).c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
	
	if (nullptr == this->jvmHandle)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			::std::wstring(L"An error loading the JVM library ").append(pathToJvm).append(L".")));
	}
#endif // __APPLE__ || __linux__
}

/************************************************************************/
/* Check that the JVM library was loaded.                               */
/************************************************************************/
bool Jvm::isLoaded() const
{
#ifdef __linux__
//	return nullptr != dlsym(this->jvmHandle, JNI_CreateJavaVM);
#endif

	return (nullptr != this->jvmHandle);
}

/************************************************************************/
/* Unload the JVM library.                                              */
/************************************************************************/
void Jvm::unloadJvm()
{
	BOOST_ASSERT_MSG(this->isLoaded(), "The JVM library hasn't been loaded.");

	// IMPORTANT java library shouldn't be unloaded (reinit fails)
	/*
#ifdef WIN32
	if (!FreeLibrary(this->jvmHandle))
#else
	if ((RTLD_UNKNOWN != this->jvmHandle) && dlclose(this->jvmHandle))
#endif // WIN32
	{
		this->jvmHandle = nullptr;

		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error freeing the JVM library."));
	}
	*/

	this->jvmHandle = nullptr;
}

/************************************************************************/
/* Create a VM.                                                         */
/************************************************************************/
void Jvm::createJvm(const ::std::wstring& jarPath)
{
	BOOST_ASSERT_MSG(this->isLoaded(), "The JVM library hasn't been loaded.");
	BOOST_ASSERT_MSG(!this->isCreated(), "The VM has been created already.");

	// Save the jar path 
	this->jarPath = jarPath;

	// Get the address of JNI_GetCreatedJavaVMs function
	GetCreatedJavaVMs_t* pfnGetCreatedJavaVMs = nullptr;

#ifdef WIN32	
	pfnGetCreatedJavaVMs = (GetCreatedJavaVMs_t*)GetProcAddress(
		this->jvmHandle, JNI_GetCreatedJavaVMs);
#endif // WIN32

#if defined(__APPLE__) || (__linux__)
	pfnGetCreatedJavaVMs = (GetCreatedJavaVMs_t*)dlsym(
		this->jvmHandle, JNI_GetCreatedJavaVMs);

	if (nullptr == pfnGetCreatedJavaVMs)
	{
		pfnGetCreatedJavaVMs = (GetCreatedJavaVMs_t*)dlsym(
			this->jvmHandle, JNI_GetCreatedJavaVMs_Impl);
	}
#endif // __APPLE__ || __linux__

	if (nullptr == pfnGetCreatedJavaVMs)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			WIDEN(JNI_GetCreatedJavaVMs) L" isn't found."));
	}

	// Get the address of JNI_CreateJavaVM function
	CreateJavaVM_t* pfnCreateJavaVM = nullptr;

#ifdef WIN32
	pfnCreateJavaVM = (CreateJavaVM_t*)GetProcAddress(
		this->jvmHandle, JNI_CreateJavaVM);
#endif // WIN32

#if defined(__APPLE__) || defined(__linux__)
	pfnCreateJavaVM = (CreateJavaVM_t*)dlsym(
		this->jvmHandle, JNI_CreateJavaVM);

	if (nullptr == pfnCreateJavaVM)
	{
		pfnCreateJavaVM = (CreateJavaVM_t*)dlsym(
			this->jvmHandle, JNI_CreateJavaVM_Impl);
	}
#endif // __APPLE__ || __linux__

	if (nullptr == pfnCreateJavaVM)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			WIDEN(JNI_CreateJavaVM) L" isn't found."));
	}
	
	// LOCK
	boost::mutex::scoped_lock lock(Jvm::mutexLock);

	// Init the vm thread counter
	if (nullptr == Jvm::ptrNumberPerThread.get())
	{
		Jvm::ptrNumberPerThread.reset(new unsigned long(0));
	}

	// Try get the number of run VM
	jsize nVMs;

	if (0 != pfnGetCreatedJavaVMs(&(this->jvm), 1, &nVMs))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error calling " WIDEN(JNI_GetCreatedJavaVMs) L"."));
	}

	if ((0 < nVMs) && (nullptr != this->jvm))
	{
		jint res = this->jvm->AttachCurrentThread((void**)&this->jniEnv, nullptr);

		if ((0 != res) || (nullptr == this->jniEnv))
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An error attaching to VM."));
		}
	}

	// The VM has been attached
	if ((nullptr != this->jvm) && (nullptr != this->jniEnv))
	{
		// The vm thread counter +1
		++(*Jvm::ptrNumberPerThread);

		return;
	}

	// Set VM options
	JavaVMOption options[6];

#if defined(_M_X64) || defined(__x86_64__)
	options[0].optionString = (char*)"-Xms128m";
	options[1].optionString = (char*)"-Xmx512m";
	options[2].optionString = (char*)"-XX:PermSize=64m";
	options[3].optionString = (char*)"-XX:MaxPermSize=256m";
#else
#	ifdef WIN32
	LPVOID lpvBase = nullptr;

	while(true, true)
	{
		// 512Mb
		lpvBase = VirtualAlloc(NULL, 512 * 1048576, MEM_RESERVE, PAGE_READWRITE);

		if (nullptr !=lpvBase)
		{
			VirtualFree(lpvBase, 0, MEM_RELEASE);

			options[0].optionString = "-Xms128m";
			options[1].optionString = "-Xmx256m";
			options[2].optionString = "-XX:PermSize=32m";
			options[3].optionString = "-XX:MaxPermSize=64m";

			break;
		}

		// 384
		lpvBase = VirtualAlloc(NULL, 384 * 1048576, MEM_RESERVE, PAGE_READWRITE);

		if (nullptr !=lpvBase)
		{
			VirtualFree(lpvBase, 0, MEM_RELEASE);

			options[0].optionString = "-Xms96m";
			options[1].optionString = "-Xmx192m";
			options[2].optionString = "-XX:PermSize=16m";
			options[3].optionString = "-XX:MaxPermSize=32m";

			break;
		}

		// 256
		lpvBase = VirtualAlloc(NULL, 256 * 1048576, MEM_RESERVE, PAGE_READWRITE);

		if (nullptr !=lpvBase)
		{
			VirtualFree(lpvBase, 0, MEM_RELEASE);

			options[0].optionString = "-Xms64m";
			options[1].optionString = "-Xmx128m";
			options[2].optionString = "-XX:PermSize=16m";
			options[3].optionString = "-XX:MaxPermSize=32m";

			break;
		}

		// 256
		lpvBase = VirtualAlloc(NULL, 128 * 1048576, MEM_RESERVE, PAGE_READWRITE);

		if (nullptr !=lpvBase)
		{
			VirtualFree(lpvBase, 0, MEM_RELEASE);

			options[0].optionString = "-Xms32m";
			options[1].optionString = "-Xmx64m";
			options[2].optionString = "-XX:PermSize=16m";
			options[3].optionString = "-XX:MaxPermSize=32m";

			break;
		}

		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Not enough memory."));
	}

#	else // OS X
	options[0].optionString = (char*)"-Xms128m";
	options[1].optionString = (char*)"-Xmx256m";
	options[2].optionString = (char*)"-XX:PermSize=32m";
	options[3].optionString = (char*)"-XX:MaxPermSize=64m";
#	endif // WIN32
#endif

	options[4].optionString = (char*)"-Djava.awt.headless=true"; // needed !!! for Mac OS X
	options[5].optionString = (char*)"-verbose:jni";

	// JDK/JRE 6 VM initialization arguments
	JavaVMInitArgs jvmArgs; 

	jvmArgs.version            = JNI_VERSION_1_6;
	jvmArgs.nOptions           = 5; // without verbose
	jvmArgs.options            = options;
	jvmArgs.ignoreUnrecognized = false;

	// Create a new VM
	jint res = pfnCreateJavaVM(&(this->jvm), &(this->jniEnv), &jvmArgs);

	if ((0 != res) || (nullptr == this->jvm) || (nullptr == this->jniEnv))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error creating VM."));
	}

	// The vm thread counter +1
	++(*Jvm::ptrNumberPerThread);
}

/************************************************************************/
/* Check that the Jvm is created.                                       */
/************************************************************************/
bool Jvm::isCreated() const
{
	return (nullptr != this->jvm) && (nullptr != this->jniEnv);
}

/************************************************************************/
/* Get the pointer of JNI environment.                                  */
/************************************************************************/
JNIEnv* const Jvm::getJniEnv() const
{
	return this->jniEnv;
}

/************************************************************************/
/* Destroy the created JVM.                                             */
/************************************************************************/
void Jvm::destroyJvm()
{
	BOOST_ASSERT_MSG(this->isCreated(), 
		"The JVM hasn't been created.");
	BOOST_ASSERT_MSG(0 == this->counterJPtr, 
		"Not all links have been destroyed.");

	// Delete the class loader
	if (nullptr != this->classLoader)
	{
		deleteGlobalRef(this->classLoader);

		this->classLoader     = nullptr;
		this->methodLoadClass = nullptr;
	}

	// LOCK
	boost::mutex::scoped_lock lock(Jvm::mutexLock);

	// Substruct a thread
	BOOST_ASSERT_MSG(0 < (*Jvm::ptrNumberPerThread), 
		"The number VM per thread is zero.");

	--(*Jvm::ptrNumberPerThread);

	// Detach the created JVM. (javabug: DestroyJavaVM isn't working)
	if (0 == (*Jvm::ptrNumberPerThread))
	{
		if (0 != this->jvm->DetachCurrentThread())
		{
			this->counterJPtr = 0;
			this->jvm         = nullptr;
			this->jniEnv      = nullptr;

			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An error detaching the JVM."));
		}
	}

	this->counterJPtr = 0;
	this->jniEnv      = nullptr;
	this->jvm         = nullptr;
}

/************************************************************************/
/* Check for a java exception and throw it.                             */
/************************************************************************/
void Jvm::checkException() const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");

	// Check that an exception is occured
	jthrowable throwableException = this->jniEnv->ExceptionOccurred();

	if (!throwableException)
	{
		return;
	}

	// Get the exception message
	::std::wstring exceptionMessage(L"");

	jclass  classException(nullptr);
	jclass  classClass(nullptr);
	jstring jstringName(nullptr);
	jstring jstringMessage(nullptr);

	for( ; ; )
	{
		classException = this->jniEnv->GetObjectClass(throwableException);

		if (nullptr == classException)
		{
			break;
		}

		classClass = this->jniEnv->FindClass("java/lang/Class");

		if (nullptr == classClass)
		{
			break;
		}

		// Get the name of the class exception
		jmethodID getName(this->jniEnv->GetMethodID(classClass, "getName", "()Ljava/lang/String;"));

		if (nullptr == getName)
		{
			break;
		}

		jstringName = static_cast<jstring>(this->jniEnv->CallObjectMethod(classException, getName));

		if (nullptr == jstringName)
		{
			break;
		}

		// Convert the name to ::std::wstring
		const char * rawName = this->jniEnv->GetStringUTFChars(jstringName, nullptr);

		if (nullptr == rawName)
		{
			break;
		}

		::std::wstring name(::boost::locale::conv::utf_to_utf<wchar_t>(rawName));

		this->jniEnv->ReleaseStringUTFChars(jstringName, rawName);

		// The exception class
		exceptionMessage.append(name);

		// Get the message is thrown of the exception class 
		jmethodID getMessage(this->jniEnv->GetMethodID(classException, "getMessage", "()Ljava/lang/String;"));

		if (nullptr == getMessage)
		{
			break;
		}

		jstring jstringMessage = static_cast<jstring>(this->jniEnv->CallObjectMethod(throwableException, getMessage));

		if (nullptr == jstringMessage)
		{
			break;
		}

		// Convert the message to ::std::wstring
		const char * rawMessage = this->jniEnv->GetStringUTFChars(jstringMessage, nullptr);

		if (nullptr == rawMessage)
		{
			break;
		}

		::std::wstring message(::boost::locale::conv::utf_to_utf<wchar_t>(rawMessage));

		this->jniEnv->ReleaseStringUTFChars(jstringMessage, rawMessage);

		// The exception message
		exceptionMessage.append(L": ").append(message);
		
		break;
	}

	// Delete local references
	if (nullptr != classException)
	{
		this->jniEnv->DeleteLocalRef(classException);
	}

	if (nullptr != classClass)
	{
		this->jniEnv->DeleteLocalRef(classClass);
	}

	if (nullptr != jstringName)
	{
		this->jniEnv->DeleteLocalRef(jstringName);
	}

	if (nullptr != jstringMessage)
	{
		this->jniEnv->DeleteLocalRef(jstringMessage);
	}

	this->jniEnv->DeleteLocalRef(throwableException);

	// Clear the exception
	this->jniEnv->ExceptionClear();

	// Throw exception with the exception message
	if (0 == exceptionMessage.length())
	{
		exceptionMessage.append(L"An unknown exception.");
	}

	BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
		exceptionMessage));
}

/************************************************************************/
/* Convert a ::std::wstring to a java/lang/String object.               */
/************************************************************************/
jstring const Jvm::wstringToJString(const ::std::wstring& inputWstring) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");

	jstring outputJstring = this->jniEnv->NewStringUTF(
		::boost::locale::conv::utf_to_utf<char>(inputWstring).c_str());

	checkException();

	return outputJstring;
}

/************************************************************************/
/* Convert a java/lang/String object to a ::std::wstring.               */
/************************************************************************/
::std::wstring const Jvm::jstringToWstring(const jstring& inputJstring) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");

	// Get the size of the java string
	jsize sizeString = this->jniEnv->GetStringUTFLength(inputJstring);
	UNREFERENCED_PARAMETER(sizeString);
	
	checkException();

	// Get the raw data of the java string
	const char * raw = this->jniEnv->GetStringUTFChars(inputJstring, nullptr);

	checkException();

	// Create a wstring
	::std::wstring outputWstring(::boost::locale::conv::utf_to_utf<wchar_t>(raw));

	// Release the raw data of the java string
	this->jniEnv->ReleaseStringUTFChars(inputJstring, raw);

	checkException();

	return outputWstring;
}

/************************************************************************/
/* Get the class path from the specified jar.                           */
/************************************************************************/
::std::wstring const Jvm::getClassPath(const ::std::wstring& jarPath)
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");

	// java.util.zip.ZipFile
	JClass classZipFile(*this, stdFindClass("java/util/zip/ZipFile"));

	// java.util.zip.ZipFile$constructor
	jmethodID methodZipFileConstructor(getMethodID(*classZipFile, "<init>", "(Ljava/lang/String;)V"));

	// ZipFile jarFile = new ZipFile(jarPath);
	JObject jarFile(*this, 
		newObject(*classZipFile, methodZipFileConstructor, *JString(*this, wstringToJString(jarPath))));

	// java.util.zip.ZipFile$getEntry
	jmethodID methodZipEntryGetEntry(getMethodID(*classZipFile, "getEntry", "(Ljava/lang/String;)Ljava/util/zip/ZipEntry;"));

	// ZipEntry manifestEntry = jarFile.getEntry("META-INF/MANIFEST.MF")
	JObject manifestEntry(*this, 
		callObjectMethod(*jarFile, methodZipEntryGetEntry, *JString(*this, wstringToJString(L"META-INF/MANIFEST.MF"))));

	if (!manifestEntry)
	{
		return ::std::wstring(L"");
	}

	// java.util.zip.ZipFile$getInputStream
	jmethodID methodZipFileGetInputStream(getMethodID(*classZipFile, "getInputStream", 
		"(Ljava/util/zip/ZipEntry;)Ljava/io/InputStream;"));

	// InputStream manifestStream = jarFile.getInputStream(manifestEntry)
	JObject manifestStream(*this, 
		callObjectMethod(*jarFile, methodZipFileGetInputStream, *manifestEntry));

	// java.util.jar.Manifest
	JClass classManifest(*this, stdFindClass("java/util/jar/Manifest"));

	// java.util.jar.Manifest$constructor
	jmethodID methodManifestConstructor(getMethodID(*classManifest, "<init>", "(Ljava/io/InputStream;)V"));

	// Manifest manifest = new Manifest(manifestStream);
	JObject manifest(*this, newObject(*classManifest, methodManifestConstructor, *manifestStream));

	// java.io.InputStream
	JClass classInputStream(*this, stdFindClass("java/io/InputStream"));

	// java.io.InputStream$close
	jmethodID methodInputStreamClose(getMethodID(*classInputStream, "close", "()V"));

	// manifestStream.close();
	callVoidMethod(*manifestStream, methodInputStreamClose);

	// java.util.zip.ZipFile$close
	jmethodID methodZipFileClose(getMethodID(*classZipFile, "close", "()V"));

	// jarFile.close()
	callVoidMethod(*jarFile, methodZipFileClose);

	// java.util.jar.Manifest$getMainAttributes
	jmethodID methodManifestGetMainAttributes(getMethodID(*classManifest, 
		"getMainAttributes", "()Ljava/util/jar/Attributes;"));

	// java.util.jar.Attributes
	JClass classAttributes(*this, stdFindClass("java/util/jar/Attributes"));

	// Attributes manifestAttributes = manifest.getMainAttributes();
	JObject manifestAttributes(*this, callObjectMethod(*manifest, methodManifestGetMainAttributes));

	// java.util.jar.Attributes.Name
	JClass classAttributesName(*this, stdFindClass("java/util/jar/Attributes$Name"));

	// java.util.jar.Attributes.Name$CLASS_PATH
	jfieldID fieldCLASS_PATH(getStaticFieldID(*classAttributesName, 
		"CLASS_PATH", "Ljava/util/jar/Attributes$Name;"));

	// Attributes.Name.CLASS_PATH
	jni::JObject CLASS_PATH(*this, getStaticObjectField(*classAttributesName, fieldCLASS_PATH));

	// java.util.jar.Attributes$getValue
	jmethodID methodAttributesGetValue(getMethodID(*classAttributes, 
		"getValue", "(Ljava/util/jar/Attributes$Name;)Ljava/lang/String;"));

	// String manifestClassPath = manifestAttributes.getValue("Class-Path");
	JObject manifestClassPath(*this, callObjectMethod(*manifestAttributes, methodAttributesGetValue, *CLASS_PATH));

	if (!manifestClassPath)
	{
		return ::std::wstring(L"");
	}

	::std::wstring classPath(jstringToWstring(static_cast<jstring>(*manifestClassPath)));

	::boost::trim(classPath);

	return classPath;
}

/************************************************************************/
/* Create a class loader for load jar.                                  */
/************************************************************************/
void Jvm::createClassLoader()
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(!this->isClassLoaderCreated(), "A class loader has been created already.");

	// Generate a list of jars
	::std::vector< ::std::wstring> jars;

	if (!this->jarPath.empty())
	{
		::std::wstring strClassPath(getClassPath(this->jarPath));

		if (!strClassPath.empty())
		{
			::boost::filesystem::path pathJarDirectory(::boost::filesystem::path(this->jarPath).parent_path());

			::boost::split(jars, strClassPath, ::boost::is_any_of(L" "), ::boost::token_compress_on);

			for(auto iter = jars.begin(); jars.end() != iter; ++iter)
			{
				*iter = ::boost::filesystem::system_complete(
					pathJarDirectory / ::boost::filesystem::path(*iter)).wstring();
			}
		}
	}

	jars.push_back(this->jarPath);
	
	// java.net.URL
	JClass classURL(*this, stdFindClass("java/net/URL"));

	// java.net.URL$constructor
	jmethodID methodURLConstructor(getMethodID(*classURL, "<init>", "(Ljava/lang/String;)V"));

	// New URL[]
	JObjectArray urls(*this, newObjectArray(static_cast<jsize>(jars.size()), *classURL));

	for(auto iter = jars.begin(); jars.end() != iter; ++iter)
	{
		JString jstrUrl(*this, wstringToJString(::std::wstring(L"jar:file:").append(*iter).append(L"!/")));
		JObject url(*this, newObject(*classURL, methodURLConstructor, *jstrUrl));

		setObjectArrayElement(*urls, static_cast<jsize>(::std::distance(jars.begin(), iter)), *url);
	}

	// java.net.URLClassLoader
	JClass classURLClassLoader(*this, stdFindClass("java/net/URLClassLoader"));

	// java.net.URLClassLoader$newInstance
	jmethodID methodNewInstance(getStaticMethodID(*classURLClassLoader, 
		"newInstance", "([Ljava/net/URL;Ljava/lang/ClassLoader;)Ljava/net/URLClassLoader;"));

	// URLClassLoader classLoader = URLClassLoader.newInstance(urls)
	JObject classLoader(*this, callStaticObjectMethod(*classURLClassLoader, methodNewInstance, *urls, nullptr));

	// Set classLoader as global reference
	this->classLoader = static_cast<jclass>(newGlobalRef(*classLoader));

	// java.net.URLClassLoader$loadClass
	this->methodLoadClass = getMethodID(*classURLClassLoader, 
		"loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
}

/************************************************************************/
/* Check that a class loader was created.                               */
/************************************************************************/
bool const Jvm::isClassLoaderCreated() const
{
	return (nullptr != this->classLoader);
}

/************************************************************************/
/* Get the class loader.                                                */
/************************************************************************/
jclass const Jvm::getClassLoader() const
{
	return this->classLoader;
}

/************************************************************************/
/* Get java version.                                                    */
/************************************************************************/
::std::wstring const Jvm::getJavaVersion()
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");

	// java.lang.System
	JClass classSystem(*this, this->stdFindClass("java/lang/System"));

	// java.lang.System$getProperty
	jmethodID methodGetProperty(
		this->getStaticMethodID(*classSystem, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;"));

	//	System.getProperty("java.version")
	JString javaVersion(*this, 
		this->callStaticObjectMethod(*classSystem, methodGetProperty, 
		*(JString(*this, wstringToJString(L"java.runtime.version")))));

	return jstringToWstring(*javaVersion);
}

/************************************************************************/
/* Find the class by specified name using the bootstrap class loader.   */
/************************************************************************/
jclass const Jvm::stdFindClass(const ::std::string& className) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");

	jclass foundClass = this->jniEnv->FindClass(className.c_str());

	checkException();

	return foundClass;
}

/************************************************************************/
/* Find the class by specified name using the created class loader.     */
/************************************************************************/
jclass const Jvm::findClass(const ::std::wstring& className)
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");

	// Create a class loader
	if (!isClassLoaderCreated())
	{
		createClassLoader();
	}

	// Find the class by name with the class loader
	JString jstrClassName(*this, wstringToJString(className));

	return static_cast<jclass>(callObjectMethod(this->classLoader, this->methodLoadClass, *jstrClassName));
}

/************************************************************************/
/* Create a new java object.                                            */
/************************************************************************/
jobject const Jvm::newObject(jclass clazz, jmethodID methodID, ...) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(clazz, "The specified class is null.");
	BOOST_ASSERT_MSG(methodID, "The specified method ID is null.");

	jobject objectResult;

	va_list args;

	va_start(args, methodID);

	objectResult = this->jniEnv->NewObjectV(clazz, methodID, args);

	va_end(args);

	checkException();

	return objectResult;
}

/************************************************************************/
/* Create a new object array.                                           */
/************************************************************************/
jobjectArray const Jvm::newObjectArray(jsize len, jclass clazz) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(0 < len, "The length should be greater than zero.");
	BOOST_ASSERT_MSG(clazz, "The specified class is null.");

	jobjectArray objectArray = this->jniEnv->NewObjectArray(len, clazz, nullptr);

	checkException();

	return objectArray;
}

/************************************************************************/
/* Create a new byte array.                                             */
/************************************************************************/
jbyteArray const Jvm::newByteArray(jsize size) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(0 < size, "The size must be greater then zero.");

	jbyteArray newArray = this->jniEnv->NewByteArray(size);

	checkException();

	return newArray;
}

/************************************************************************/
/* Create a global reference from the local object.                     */
/************************************************************************/
jobject const Jvm::newGlobalRef(jobject lobj) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(lobj, "The specified object is null.");

	jobject globalObject = this->jniEnv->NewGlobalRef(lobj);

	checkException();

	return globalObject;
}

/************************************************************************/
/* Delete the global reference.                                         */
/************************************************************************/
void Jvm::deleteGlobalRef(jobject gref) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(gref, "The specified reference is null.");

	this->jniEnv->DeleteGlobalRef(gref);

	checkException();
}

/************************************************************************/
/* Delete the local reference.                                          */
/************************************************************************/
void Jvm::deleteLocalRef(jobject lref) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(lref, "The specified reference is null.");

	this->jniEnv->DeleteLocalRef(lref);

	checkException();
}

/************************************************************************/
/* Get the static field ID from the specified class.                    */
/************************************************************************/
jfieldID const Jvm::getStaticFieldID(jclass clazz, const ::std::string& name, const ::std::string& sig) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(clazz, "The specified class is null.");

	jfieldID fieldId = this->jniEnv->GetStaticFieldID(clazz, name.c_str(), sig.c_str());

	checkException();

	return fieldId;
}

/************************************************************************/
/* Get the static object from the specified class.                      */
/************************************************************************/
jobject const Jvm::getStaticObjectField(const jclass clazz, const jfieldID fieldID) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(clazz, "The specified class is null.");
	BOOST_ASSERT_MSG(fieldID, "The specified field is null.");

	jobject field = this->jniEnv->GetStaticObjectField(clazz, fieldID);

	checkException();

	return field;
}


/************************************************************************/
/* Get the static method ID from the specified class.                   */
/************************************************************************/
jmethodID const Jvm::getStaticMethodID(jclass clazz, const ::std::string& name, const ::std::string& sig) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(clazz, "The specified class is null.");

	jmethodID methodId = this->jniEnv->GetStaticMethodID(clazz, name.c_str(), sig.c_str());

	checkException();

	return methodId;
}

/************************************************************************/
/* Get the method ID from the specified class.                          */
/************************************************************************/
jmethodID const Jvm::getMethodID(jclass clazz, const ::std::string& name, const ::std::string& sig) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(clazz, "The specified class is null.");

	jmethodID method = this->jniEnv->GetMethodID(clazz, name.c_str(), sig.c_str());

	checkException();

	return method;
}

/************************************************************************/
/* Get the length of the array.                                         */
/************************************************************************/
jsize const Jvm::getArrayLength(jarray array) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(array, "The specified array is null.");

	jsize sizeArray = this->jniEnv->GetArrayLength(array);

	checkException();

	return sizeArray;
}

/************************************************************************/
/* Get the element from the object array.                               */
/************************************************************************/
jobject const Jvm::getObjectArrayElement(jobjectArray objectArray, jsize index) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(objectArray, "The specified array is null.");
	BOOST_ASSERT_MSG(0 <= index, "The index should be greater than or equal to zero.");
	BOOST_ASSERT_MSG(getArrayLength(objectArray) > index, "The index out of the array bounds.");

	jobject object = this->jniEnv->GetObjectArrayElement(objectArray, index);

	checkException();

	return object;
}

/************************************************************************/
/* Copy the region of the byte array into the buffer.                   */
/************************************************************************/
void Jvm::getByteArrayRegion(jbyteArray array, jsize start, jsize len, jbyte *buf) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(array, "The specified array is null.");
	BOOST_ASSERT_MSG(0 <= start, "The start index should be greater than or equal to zero.");
	BOOST_ASSERT_MSG(getArrayLength(array) > start, "The start index out of the array bounds.");
	BOOST_ASSERT_MSG(0 < len, "The lenght should be greater than zero.");
	BOOST_ASSERT_MSG(getArrayLength(array) >= (start + len), "The length out of the array bounds.");
	BOOST_ASSERT_MSG(buf, "The specified buffer is null.");

	this->jniEnv->GetByteArrayRegion(array, start, len, buf);

	checkException();
}

/************************************************************************/
/* Set the element into the object array.                               */
/************************************************************************/
void Jvm::setObjectArrayElement(jobjectArray objectArray, jsize index, jobject val) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(objectArray, "The specified array is null.");
	BOOST_ASSERT_MSG(0 <= index, "The index should be greater than or equal to zero.");
	BOOST_ASSERT_MSG(getArrayLength(objectArray) > index, "The index out of the array bounds.");
	BOOST_ASSERT_MSG(val, "The specified value is null.");

	this->jniEnv->SetObjectArrayElement(objectArray, index, val);

	checkException();
}

/************************************************************************/
/* Set the elements of the byte array.                                  */
/************************************************************************/
void Jvm::setByteArrayRegion(jbyteArray byteArray, jsize start, jsize len, jbyte const * const buf) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(byteArray, "The specified array is null.");
	BOOST_ASSERT_MSG(0 <= start, "The start should be greater than or equal to zero.");
	BOOST_ASSERT_MSG(getArrayLength(byteArray) >= (start + len), "The length is too big.");

	this->jniEnv->SetByteArrayRegion(byteArray, start, len, buf);

	checkException();
}

/************************************************************************/
/* Call the specified static method and get a result object.            */
/************************************************************************/
jobject const Jvm::callStaticObjectMethod(jclass clazz, jmethodID methodID, ...) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(clazz, "The specified class is null.");
	BOOST_ASSERT_MSG(methodID, "The specified method ID is null.");

	jobject objectResult;

	va_list args;

	va_start(args, methodID);

	objectResult = this->jniEnv->CallStaticObjectMethodV(clazz, methodID, args);

	va_end(args);

	checkException();

	return objectResult;
}

/************************************************************************/
/* Call the specified method.                                           */
/************************************************************************/
void Jvm::callVoidMethod(jobject obj, jmethodID methodID, ...) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(obj, "The specified object is null.");
	BOOST_ASSERT_MSG(methodID, "The specified method ID is null.");

	va_list args;

	va_start(args, methodID);

	this->jniEnv->CallVoidMethodV(obj, methodID, args);

	va_end(args);

	checkException();
}

/************************************************************************/
/* Call the specified method and get a result double number.            */
/************************************************************************/
jdouble const Jvm::callDoubleMethod(jobject obj, jmethodID methodID, ...) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(obj, "The specified object is null.");
	BOOST_ASSERT_MSG(methodID, "The specified method ID is null.");

	jdouble doubleResult;

	va_list args;

	va_start(args, methodID);

	doubleResult = this->jniEnv->CallDoubleMethodV(obj, methodID, args);

	va_end(args);

	checkException();

	return doubleResult;
}

/************************************************************************/
/* Call the specified method and get a result object.                   */
/************************************************************************/
jobject const Jvm::callObjectMethod(jobject obj, jmethodID methodID, ...) const
{
	BOOST_ASSERT_MSG(this->isCreated(), "The JVM hasn't been created.");
	BOOST_ASSERT_MSG(obj, "The specified object is null.");
	BOOST_ASSERT_MSG(methodID, "The specified method ID is null.");

	jobject objectResult;

	va_list args;

	va_start(args, methodID);

	objectResult = this->jniEnv->CallObjectMethodV(obj, methodID, args);

	va_end(args);

	checkException();

	return objectResult;
}

/************************************************************************/
/* Get the number of java object.                                       */
/************************************************************************/
unsigned long const Jvm::getCounterJPtr() const
{
	return this->counterJPtr;
}

/************************************************************************/
/* Add one to the number of java object.                                */
/************************************************************************/
unsigned long const Jvm::addCounterJPtr()
{
	return ++this->counterJPtr;
}

/************************************************************************/
/* Subtract one from the number of java object.                         */
/************************************************************************/
unsigned long const Jvm::subCounterJPtr()
{
	BOOST_ASSERT_MSG(0 < this->counterJPtr, "The counter is zero.");

	return --this->counterJPtr;
}
