#pragma once

#ifndef VECTOR_JNI_J_UNIQUE_PTR_H
#define VECTOR_JNI_J_UNIQUE_PTR_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	// The type include an information about the VM environment and a java object.
	typedef ::std::pair<Jvm&, jobject> JPtr, *PJPtr;

	// Deleter for the JLocal type.
	struct JPtrDeleter
	{
		void operator()(PJPtr object);
	};

	// Type safe wrapper for local reference.
	template<class JT>
	struct JUniquePtr
		: virtual ::boost::shared_ptr<JPtr>
		//: virtual ::std::unique_ptr<JPtr, JPtrDeleter>
	{
		JUniquePtr(Jvm& jvm, jobject obj) 
			: ::boost::shared_ptr<JPtr>(new JPtr(jvm, obj), JPtrDeleter())
		{
			jvm.addCounterJPtr();
		}

		JT operator*() const
		{
			return reinterpret_cast<JT>(this->get()->second);
		}

		// C++0x
		/*
		explicit operator bool() const
		{
			return (nullptr != this->get()->second ? true : false);
		}*/

		// required for Xcode and stdlibc++ (operator bool() doesn't work)
		// TODO: need tests!
		bool operator! () const
		{
			return !(nullptr != this->get()->second ? true : false);
		}
	};

	// Types
	typedef JUniquePtr<jobject>      JObject;
	typedef JUniquePtr<jclass>       JClass;
	typedef JUniquePtr<jobjectArray> JObjectArray;
	typedef JUniquePtr<jstring>      JString;
	typedef JUniquePtr<jbyteArray>   JByteArray;

};};};};

#endif // VECTOR_JNI_J_UNIQUE_PTR_H
