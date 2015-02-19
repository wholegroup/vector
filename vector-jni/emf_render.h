#pragma once

#ifndef VECTOR_JNI_EMF_RENDER_H
#define VECTOR_JNI_EMF_RENDER_H

// com.wholegroup.vector.jni
namespace com {namespace wholegroup {namespace vector {namespace jni
{
	class EmfRender
		: public BaseRender
	{
		private:

			//
			ULONG_PTR gdipToken;

			//
			::std::vector<unsigned char> fileData;

			//
			::std::shared_ptr<::Gdiplus::Metafile> emfplus;

			//
			::std::vector<unsigned char> quad;

		protected:

			// Default constructor (used in children classes).
			EmfRender() {};

		public:

			// Constructor
			EmfRender(Jvm* jvm);

			// Destructor.
			virtual ~EmfRender();

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
	};
};};};};

#endif // VECTOR_JNI_EMF_RENDER_H