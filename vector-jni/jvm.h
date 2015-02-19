#pragma once

#ifndef VECTOR_JNI_JVM_H
#define VECTOR_JNI_JVM_H

#ifndef WIN32
typedef void *HINSTANCE;
#endif // WIN32

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class Jvm
	{
		private:
			
			// The mutex is used for locking.
			static ::boost::mutex mutexLock;

			// Number of created jvms per a thread.
			static ::boost::thread_specific_ptr<unsigned long> ptrNumberPerThread;

			// Path to a jar file
			::std::wstring jarPath;

			// Handle to the jvm.dll
			HINSTANCE jvmHandle; 

			// Denotes a Java VM 
			JavaVM* jvm; 

			// Pointer to native method interface
			JNIEnv* jniEnv; 

			// The class loader (java.net.URLClassLoader)
			jclass classLoader;

			// The method java.net.URLClassLoader.loadClass
			jmethodID methodLoadClass;

			// The JPtr counter
			unsigned long counterJPtr;

		public:

			// Constructor.
			Jvm();

			// Destructor.
			virtual ~Jvm();

			// Destructor is created for check in tests.
			virtual void destructor();

			// Load the JVM library by the specified path.
			virtual void loadJvm(const std::wstring& pathToJvm);

			// Check that the JVM library was loaded.
			virtual bool isLoaded() const;

			// Unload the JVM library.
			virtual void unloadJvm();

			// Create a VM.
			virtual void createJvm(const std::wstring& jarPath);

			// Check that the Jvm is created.
			virtual bool isCreated() const;

			// Get the pointer of JNI environment.
			virtual JNIEnv* const getJniEnv() const;

			// Destroy the created VM.
			virtual void destroyJvm();

			// Check for a java exception and throw it.
			virtual void checkException() const;

			// Convert a ::std::wstring to a java/lang/String object.
			virtual jstring const wstringToJString(const ::std::wstring& inputWstring) const;

			// Convert a java/lang/String object to a ::std::wstring.
			virtual ::std::wstring const jstringToWstring(const jstring& inputJstring) const;

			// Get the class path from the specified jar.
			virtual ::std::wstring const getClassPath(const ::std::wstring& jarPath);

			// Create a class loader for load jar.
			virtual void createClassLoader();

			// Check that a class loader was created.
			virtual bool const isClassLoaderCreated() const;

			// Get the class loader.
			virtual jclass const getClassLoader() const;

			// Get java version.
			virtual ::std::wstring const getJavaVersion();

			// Find the class by specified name using the bootstrap class loader.
			virtual jclass const stdFindClass(const ::std::string& className) const;

			// Find the class by specified name using the created class loader.
			virtual jclass const findClass(const ::std::wstring& className);

			// Create a new java object.
			virtual jobject const newObject(jclass clazz, jmethodID methodID, ...) const;

			// Create a new object array.
			virtual jobjectArray const newObjectArray(jsize len, jclass clazz) const;

			// Create new byte array.
			virtual jbyteArray const newByteArray(jsize size) const;

			// Create a global reference from the local object.
			virtual jobject const newGlobalRef(jobject lobj) const;

			// Delete the global reference.
			virtual void deleteGlobalRef(jobject gref) const;

			// Delete the local reference.
			virtual void deleteLocalRef(jobject lref) const;

			// Get the static field ID from the specified class.
			virtual jfieldID const getStaticFieldID(jclass clazz, const ::std::string& name, const ::std::string& sig) const;

			// Get the static object from the specified class.
			virtual jobject const getStaticObjectField(const jclass clazz, const jfieldID fieldID) const;

			// Get the static method ID from the specified class.
			virtual jmethodID const getStaticMethodID(jclass clazz, const ::std::string& name, const ::std::string& sig) const;

			// Get the method ID from the specified class.
			virtual jmethodID const getMethodID(jclass clazz, const ::std::string& name, const ::std::string& sig) const;

			// Get the length of the array.
			virtual jsize const getArrayLength(jarray array) const;

			// Get the element from the object array.
			virtual jobject const getObjectArrayElement(jobjectArray objectArray, jsize index) const;

			// Copy the region of the byte array into the buffer.
			virtual void getByteArrayRegion(jbyteArray array, jsize start, jsize len, jbyte *buf) const;

			// Set the element into the object array.
			virtual void setObjectArrayElement(jobjectArray objectArray, jsize index, jobject val) const;

			// Set the elements of the byte array.
			virtual void setByteArrayRegion(jbyteArray byteArray, jsize start, jsize len, jbyte const * const buf) const;

			// Call the specified static method and get a result object.
			virtual jobject const callStaticObjectMethod(jclass clazz, jmethodID methodID, ...) const;

			// The specified object is null.
			virtual void callVoidMethod(jobject obj, jmethodID methodID, ...) const;

			// Call the specified method and get a result double number.
			virtual jdouble const callDoubleMethod(jobject obj, jmethodID methodID, ...) const;

			// Call the specified method and get a result object.
			virtual jobject const callObjectMethod(jobject obj, jmethodID methodID, ...) const;

			// Get the number of java object.
			virtual unsigned long const getCounterJPtr() const;

			// Add one to the number of java object.
			virtual unsigned long const addCounterJPtr();

			// Subtract one from the number of java object.
			virtual unsigned long const subCounterJPtr();
	};

};};};};

#endif // VECTOR_JNI_JVM_H
