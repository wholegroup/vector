#pragma once

#ifndef VECTOR_JNI_WMF_RENDER_H
#define VECTOR_JNI_WMF_RENDER_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class WmfRender
		: public BaseRender
	{
		protected:

			// Default constructor (used in children classes).
			WmfRender() {};

		public:

			//
			WmfRender(Jvm* jvm);
	};
};};};};

#endif // VECTOR_JNI_WMF_RENDER_H