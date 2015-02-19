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
#include "../vector-jni/jvm_finder.h"
#include "../vector-jni/jvm.h"
#include "../vector-jni/jar.h"
#include "../vector-jni/base_render.h"
#include "../vector-jni/svg_render.h"

using namespace ::com::wholegroup::vector;

class SvgRenderMock
	: public jni::SvgRender
{
	public:

		static ::boost::shared_ptr<jni::Jvm> jvm;

		static ::boost::shared_ptr< ::boost::filesystem::path> executablePath;

	public:

		SvgRenderMock()
			: jni::SvgRender(SvgRenderMock::constructor())
		{
		}

		static jni::Jvm * constructor()
		{
			SvgRenderMock::executablePath = ::boost::make_shared< ::boost::filesystem::path>(
				::boost::filesystem::path(::testing::internal::g_argvs[0].c_str()).parent_path());

			SvgRenderMock::jvm = ::boost::make_shared<jni::Jvm>();
			(*SvgRenderMock::jvm).loadJvm(jni::JvmFinder().findJvm());
			(*SvgRenderMock::jvm).createJvm(::boost::filesystem::system_complete(
				*SvgRenderMock::executablePath / L".." / L".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

			return &(*SvgRenderMock::jvm);
		}

		virtual ~SvgRenderMock()
		{
			destroy();

			SvgRenderMock::jvm.reset();
		}
};

::boost::shared_ptr<jni::Jvm> SvgRenderMock::jvm;

::boost::shared_ptr< ::boost::filesystem::path> SvgRenderMock::executablePath;

//
class SvgRenderTest
	: public	::testing::Test
{

};

// 
TEST_F(SvgRenderTest, render_Svg)
{
	SvgRenderMock renderMock;

	renderMock.SvgRender::init();
	renderMock.SvgRender::load(::boost::filesystem::system_complete(
		*SvgRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg").wstring());
	renderMock.SvgRender::create();
	renderMock.SvgRender::normalize();

	::std::vector<unsigned char> buffer(4, 0);

	renderMock.SvgRender::render(1, 1, 0, 0, 1, &buffer[0]);

	EXPECT_TRUE((0 != buffer[0]) || (0 != buffer[1]) || (0 != buffer[2]) || (0 != buffer[3]));
}

// 
TEST_F(SvgRenderTest, render_SvgGz)
{
	SvgRenderMock renderMock;

	renderMock.SvgRender::init();
	renderMock.SvgRender::load(::boost::filesystem::system_complete(
		*SvgRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg.gz").wstring());
	renderMock.SvgRender::create();
	renderMock.SvgRender::normalize();

	::std::vector<unsigned char> buffer(4, 0);

	renderMock.SvgRender::render(1, 1, 0, 0, 1, &buffer[0]);

	EXPECT_TRUE((0 != buffer[0]) || (0 != buffer[1]) || (0 != buffer[2]) || (0 != buffer[3]));
}

// 
TEST_F(SvgRenderTest, render_SvgZip)
{
	SvgRenderMock renderMock;

	renderMock.SvgRender::init();
	renderMock.SvgRender::load(::boost::filesystem::system_complete(
		*SvgRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg.zip").wstring());
	renderMock.SvgRender::create();
	renderMock.SvgRender::normalize();

	::std::vector<unsigned char> buffer(4, 0);

	renderMock.SvgRender::render(1, 1, 0, 0, 1, &buffer[0]);

	EXPECT_TRUE((0 != buffer[0]) || (0 != buffer[1]) || (0 != buffer[2]) || (0 != buffer[3]));
}
