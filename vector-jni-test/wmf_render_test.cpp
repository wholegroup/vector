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
#include "../vector-jni/wmf_render.h"

using namespace ::com::wholegroup::vector;

class WmfRenderMock
	: public jni::WmfRender
{
	public:

		//
		static ::boost::shared_ptr<jni::Jvm> jvm;

		//
		static ::boost::shared_ptr< ::boost::filesystem::path> executablePath;

	public:

		WmfRenderMock()
			: jni::WmfRender(WmfRenderMock::constructor())
		{
		}

		static jni::Jvm * constructor()
		{
			WmfRenderMock::executablePath = ::boost::make_shared< ::boost::filesystem::path>(
				::boost::filesystem::path(::testing::internal::g_argvs[0].c_str()).parent_path());

			WmfRenderMock::jvm = ::boost::make_shared<jni::Jvm>();
			(*WmfRenderMock::jvm).loadJvm(jni::JvmFinder().findJvm());
			(*WmfRenderMock::jvm).createJvm(::boost::filesystem::system_complete(
				*WmfRenderMock::executablePath / L".." / L".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

			return &(*WmfRenderMock::jvm);
		}

		virtual ~WmfRenderMock()
		{
			destroy();

			WmfRenderMock::jvm.reset();
		}
};

::boost::shared_ptr<jni::Jvm> WmfRenderMock::jvm;

::boost::shared_ptr< ::boost::filesystem::path> WmfRenderMock::executablePath;

//
class WmfRenderTest
	: public	::testing::Test
{

};

// 
TEST_F(WmfRenderTest, render_Wmf)
{
	WmfRenderMock renderMock;

	renderMock.WmfRender::init();
	renderMock.WmfRender::load(::boost::filesystem::system_complete(
		*WmfRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.wmf").wstring());
	renderMock.WmfRender::create();
	renderMock.WmfRender::normalize();

	::std::vector<unsigned char> buffer(4, 0);

	renderMock.WmfRender::render(2, 2, 1, 1, 1, &buffer[0]);

	EXPECT_TRUE((0 != buffer[0]) || (0 != buffer[1]) || (0 != buffer[2]) || (0 != buffer[3]));
}
