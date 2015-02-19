#pragma once

#ifndef VECTOR_JNI_BASE_RENDER_H
#define VECTOR_JNI_BASE_RENDER_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class BaseRender
	{
		private:	

			// The reference to JVM.
			Jvm* jvm;

			// The name of the render class.
			::std::wstring classNameRender;

			// The render class
			jclass classRender;

			// The object of the render class
			jobject objectRender;

		protected:

			// Default constructor (used in children classes).
			BaseRender() {};

			// Constructor with parameters.
			BaseRender(Jvm* jvm, const ::std::wstring& classNameRender);

		public:

			// Destructor.
			virtual ~BaseRender();

			// Initialize this render.
			virtual void init();

			// Check that the render is initialized.
			virtual bool isInit() const;

			// Load the file by the specified path.
			virtual void load(const ::std::wstring & pathFile);

			// Load the file by the specified path.
			virtual void loadFromByteArray(int const length, char const * const data);

			// Create a document.
			virtual void create();

			// Normalize the document.
			virtual double const normalize();

			// Render a piece of the document to the specified buffer.
			virtual void render(int width, int height, int left, int top, int side, unsigned char* buffer);

			// Close the document.
			virtual void close();

			// Destroy this render.
			virtual void destroy();
	};
};};};};

#endif // VECTOR_JNI_BASE_RENDER_H