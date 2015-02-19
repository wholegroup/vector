#pragma once

#ifndef VECTOR_JNI_CDR_RENDER_H
#define VECTOR_JNI_CDR_RENDER_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class CdrRender
		: public BaseRender
	{
		protected:

			// Default constructor (used in children classes).
			CdrRender() {};

		public:

			//
			CdrRender(Jvm* jvm);

			//
			virtual void load(const ::std::wstring& pathFile);
	};
};};};};

#endif // VECTOR_JNI_CDR_RENDER_H