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
#include "../vector-jni/wpg_render.h"

using namespace ::com::wholegroup::vector;

class WpgRenderMock
	: public jni::WpgRender
{
	public:

		//
		static ::boost::shared_ptr<jni::Jvm> jvm;

		//
		static ::boost::shared_ptr< ::boost::filesystem::path> executablePath;

	public:

		WpgRenderMock()
			: jni::WpgRender(WpgRenderMock::constructor())
		{
		}

		static jni::Jvm * constructor()
		{
			WpgRenderMock::executablePath = ::boost::make_shared< ::boost::filesystem::path>(
				::boost::filesystem::path(::testing::internal::g_argvs[0].c_str()).parent_path());

			WpgRenderMock::jvm = ::boost::make_shared<jni::Jvm>();
			(*WpgRenderMock::jvm).loadJvm(jni::JvmFinder().findJvm());
			(*WpgRenderMock::jvm).createJvm(::boost::filesystem::system_complete(
				*WpgRenderMock::executablePath / L".." / L".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

			return &(*WpgRenderMock::jvm);
		}

		virtual ~WpgRenderMock()
		{
			destroy();

			WpgRenderMock::jvm.reset();
		}
};

::boost::shared_ptr<jni::Jvm> WpgRenderMock::jvm;

::boost::shared_ptr< ::boost::filesystem::path> WpgRenderMock::executablePath;

//
class WpgRenderTest
	: public	::testing::Test
{

};

// 
TEST_F(WpgRenderTest, render_Wpg)
{
	WpgRenderMock renderMock;

	renderMock.WpgRender::init();
	renderMock.WpgRender::load(::boost::filesystem::system_complete(
		*WpgRenderMock::executablePath / L".." / L".." / L"resources" / L"airplane.wpg").wstring());
	renderMock.WpgRender::create();
	renderMock.WpgRender::normalize();

	::std::vector<unsigned char> buffer(16, 0);

	renderMock.WpgRender::render(4, 4, 0, 0, 2, &buffer[0]);

	EXPECT_TRUE((0 != buffer[0]) || (0 != buffer[1]) || (0 != buffer[2]) || (0 != buffer[3]));
}
