#pragma once

#ifndef VECTOR_JNI_SVG_RENDER_H
#define VECTOR_JNI_SVG_RENDER_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class SvgRender
		: public BaseRender
	{
		protected:

			// Default constructor (used in children classes).
			SvgRender() {};

		public:

			//
			SvgRender(Jvm* jvm);
	};
};};};};

#endif // VECTOR_JNI_SVG_RENDER_H