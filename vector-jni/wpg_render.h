#pragma once

#ifndef VECTOR_JNI_WPG_RENDER_H
#define VECTOR_JNI_WPG_RENDER_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class WpgRender
		: public BaseRender
	{
		protected:

			// Default constructor (used in children classes).
			WpgRender() {};

		public:

			//
			WpgRender(Jvm* jvm);

			//
			virtual void load(const ::std::wstring& pathFile);
	};
};};};};

#endif // VECTOR_JNI_WPG_RENDER_H