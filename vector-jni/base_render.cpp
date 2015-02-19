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
#include "jvm.h"
#include "j_unique_ptr.h"
#include "base_render.h"

using namespace ::com::wholegroup::vector::jni;

/************************************************************************/
/* Constructor with parameters.                                         */
/************************************************************************/
BaseRender::BaseRender(Jvm* jvm, const ::std::wstring& classNameRender)
	: jvm(jvm), classNameRender(classNameRender), 
	classRender(nullptr), objectRender(nullptr)
{
}

/************************************************************************/
/* Destructor.                                                          */
/************************************************************************/
BaseRender::~BaseRender()
{
	try
	{
		destroy();
	}
	catch (...)
	{
	}
}


/************************************************************************/
/* Initialize this render.                                              */
/************************************************************************/
void BaseRender::init()
{
	BOOST_ASSERT_MSG(nullptr != this->jvm, "The JVM is corrupted.");
	BOOST_ASSERT_MSG(!isInit(), "The render has been already initialized.");

	// Get the render class
	JClass classRender(
		*this->jvm, 
		this->jvm->findClass(
			::std::wstring(L"com.wholegroup.vector.core.render.").append(this->classNameRender).append(L"Render")
		)
	);
	
	this->classRender = static_cast<jclass>(this->jvm->newGlobalRef(*classRender));

	// Create the object of the render class
	jmethodID methodRenderConstructor(this->jvm->getMethodID(*classRender, "<init>", "()V"));

	JObject objectRender(*this->jvm, this->jvm->newObject(this->classRender, methodRenderConstructor));

	this->objectRender = static_cast<jobject>(this->jvm->newGlobalRef(*objectRender));
}

/************************************************************************/
/* Check that the render is initialized.                                */
/************************************************************************/
bool BaseRender::isInit() const
{
	BOOST_ASSERT_MSG(nullptr != this->jvm, "The JVM is corrupted.");

	return (nullptr != this->classRender) && (nullptr != this->objectRender);
}

/************************************************************************/
/* Load the file by the specified path.                                 */
/************************************************************************/
void BaseRender::load(const ::std::wstring & pathFile)
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	jmethodID methodLoad = this->jvm->getMethodID(this->classRender, "loadFromFile", "(Ljava/lang/String;)V");

	this->jvm->callVoidMethod(this->objectRender, methodLoad, this->jvm->wstringToJString(pathFile));
}

/************************************************************************/
/* Load the file by the specified path.                                 */
/************************************************************************/
void BaseRender::loadFromByteArray(int const length, char const * const data)
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");
	BOOST_ASSERT_MSG(0 < length, "The length must be greater then 0.");

	jmethodID methodLoad = this->jvm->getMethodID(this->classRender, "loadFromByteArray", "([B)V");

	//
	JByteArray byteArray(*this->jvm, this->jvm->newByteArray(length));

	this->jvm->setByteArrayRegion(*byteArray, 0, length, (const jbyte * const)data);

	//
	this->jvm->callVoidMethod(this->objectRender, methodLoad, *byteArray);
}

/************************************************************************/
/* Create a document.                                                   */
/************************************************************************/
void BaseRender::create()
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	jmethodID methodCreate = this->jvm->getMethodID(this->classRender, "create", "()V");

	this->jvm->callVoidMethod(this->objectRender, methodCreate);
}

/************************************************************************/
/* Normalize the document.                                              */
/************************************************************************/
double const BaseRender::normalize()
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	jmethodID methodNormalize = this->jvm->getMethodID(this->classRender, "normalize", "()D");

	return this->jvm->callDoubleMethod(this->objectRender, methodNormalize);
}

/************************************************************************/
/* Render a piece of the document to the specified buffer.              */
/************************************************************************/
void BaseRender::render(int width, int height, int left, int top, int side, unsigned char* buffer)
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");
	BOOST_ASSERT_MSG(0 < width, "The width should be greater then zero.");
	BOOST_ASSERT_MSG(0 < height, "The height should be greater then zero.");
	BOOST_ASSERT_MSG(0 <= left, "The left position should be greater then or equal zero.");
	BOOST_ASSERT_MSG(0 <= top, "The top position should be greater then or equal zero.");
	BOOST_ASSERT_MSG(0 < side, "The side should be greater then zero.");
	BOOST_ASSERT_MSG(nullptr != buffer, "The buffer is null.");

	// Call
	jmethodID methodRender = this->jvm->getMethodID(this->classRender, "render", "(IIIII)[B");

	JByteArray renderBuffer(*this->jvm, this->jvm->callObjectMethod(
		this->objectRender, methodRender, width, height, left, top, side));

	// Check the result
	jsize sizeRenderBuffer = this->jvm->getArrayLength(*renderBuffer);

	if ((side * side * 4) != sizeRenderBuffer)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An unknow error."));
	}

	// Copy to the specified buffer
	this->jvm->getByteArrayRegion(*renderBuffer, 0, sizeRenderBuffer, (jbyte*)buffer);
}

/************************************************************************/
/* Close the document.                                                  */
/************************************************************************/
void BaseRender::close()
{
	BOOST_ASSERT_MSG(isInit(), "The render hasn't been initialized.");

	jmethodID methodClose = this->jvm->getMethodID(this->classRender, "close", "()V");

	this->jvm->callVoidMethod(this->objectRender, methodClose);
}

/************************************************************************/
/* Destroy this render.                                                 */
/************************************************************************/
void BaseRender::destroy()
{
	if (isInit())
	{
		if (nullptr != this->objectRender)
		{
			this->jvm->deleteGlobalRef(this->objectRender);

			this->objectRender = nullptr;
		}

		if (nullptr != this->classRender)
		{
			this->jvm->deleteGlobalRef(this->classRender);

			this->classRender = nullptr;
		}
	}
}
