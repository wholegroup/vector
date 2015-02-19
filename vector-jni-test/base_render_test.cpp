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
#include "../vector-jni/jar.h"
#include "../vector-jni/jvm.h"
#include "../vector-jni/base_render.h"

using namespace ::com::wholegroup::vector;

class BaseRenderMock
	: public jni::BaseRender
{
	public:

		static ::boost::shared_ptr<jni::Jvm> jvm;

		static ::boost::shared_ptr< ::boost::filesystem::path> executablePath;

	public:

		BaseRenderMock()
			:jni::BaseRender(BaseRenderMock::constructor(), L"Svg")
		{
		}

		BaseRenderMock(::std::wstring const & name)
			:jni::BaseRender(BaseRenderMock::constructor(), name)
		{
		}

		static jni::Jvm * constructor()
		{
			BaseRenderMock::executablePath = ::boost::make_shared< ::boost::filesystem::path>(
				::boost::filesystem::path(::testing::internal::g_argvs[0].c_str()).parent_path());

			BaseRenderMock::jvm = ::boost::make_shared<jni::Jvm>();

			(*BaseRenderMock::jvm).loadJvm(jni::JvmFinder().findJvm());
			(*BaseRenderMock::jvm).createJvm(::boost::filesystem::system_complete(
				*BaseRenderMock::executablePath / L".." / L".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

			return &(*BaseRenderMock::jvm);
		}

		virtual ~BaseRenderMock()
		{
			destroy();

			BaseRenderMock::jvm.reset();
		}
};

::boost::shared_ptr<jni::Jvm> BaseRenderMock::jvm;

::boost::shared_ptr< ::boost::filesystem::path> BaseRenderMock::executablePath;

//
class BaseRenderTest
	: public	::testing::Test
{
};

class BaseRenderTestDeath : public BaseRenderTest { };

//
TEST_F(BaseRenderTest, init_NotFound)
{
	BaseRenderMock renderMock(L"None");

	EXPECT_THROW(renderMock.BaseRender::init(), exception::IOException);
}

//
TEST_F(BaseRenderTestDeath, init_Already)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	EXPECT_DEATH(renderMock.BaseRender::init(), "already");
}

//
TEST_F(BaseRenderTest, init)
{
	BaseRenderMock renderMock;

	EXPECT_FALSE(renderMock.BaseRender::isInit());

	renderMock.BaseRender::init();

	EXPECT_TRUE(renderMock.BaseRender::isInit());
}

//
TEST_F(BaseRenderTest, isInit_Before)
{
	BaseRenderMock renderMock;

	EXPECT_FALSE(renderMock.BaseRender::isInit());
}

//
TEST_F(BaseRenderTest, isInit_After)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();
	renderMock.BaseRender::destroy();

	EXPECT_FALSE(renderMock.BaseRender::isInit());
}

//
TEST_F(BaseRenderTest, isInit)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	EXPECT_TRUE(renderMock.BaseRender::isInit());
}

//
TEST_F(BaseRenderTestDeath, load_NotInit)
{
	BaseRenderMock renderMock;

	EXPECT_DEATH(renderMock.BaseRender::load(L""), "init");
}

//
TEST_F(BaseRenderTest, load_NotFound)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	EXPECT_THROW(renderMock.BaseRender::load(L"file is not found"), exception::IOException);
}

//
TEST_F(BaseRenderTest, load)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	renderMock.BaseRender::load(::boost::filesystem::system_complete(
		*BaseRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg").wstring());
}

//
TEST_F(BaseRenderTestDeath, create_NotInit)
{
	BaseRenderMock renderMock;

	EXPECT_DEATH(renderMock.BaseRender::create(), "init");
}

//
TEST_F(BaseRenderTest, create_NotLoad)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	EXPECT_THROW(renderMock.BaseRender::create(), exception::IOException);
}

//
TEST_F(BaseRenderTest, create_ErrorFormat)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();
	renderMock.BaseRender::load(::boost::filesystem::system_complete(
		*BaseRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.wmf").wstring());

	EXPECT_THROW(renderMock.BaseRender::create(), exception::IOException);
}

//
TEST_F(BaseRenderTest, create)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();
	renderMock.BaseRender::load(::boost::filesystem::system_complete(
		*BaseRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg").wstring());

	renderMock.BaseRender::create();
}

//
TEST_F(BaseRenderTestDeath, normalize_NotInit)
{
	BaseRenderMock renderMock;

	EXPECT_DEATH(renderMock.BaseRender::normalize(), "init");
}

//
TEST_F(BaseRenderTest, normalize_NotCreated)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	EXPECT_THROW(renderMock.BaseRender::normalize(), exception::IOException);
}

//
TEST_F(BaseRenderTest, normalize)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();
	renderMock.BaseRender::load(::boost::filesystem::system_complete(
		*BaseRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg").wstring());
	renderMock.BaseRender::create();

	EXPECT_TRUE(0 < renderMock.BaseRender::normalize());
}

//
TEST_F(BaseRenderTestDeath, render_NotInit)
{
	BaseRenderMock renderMock;

	EXPECT_DEATH(renderMock.BaseRender::render(0, 0, 0, 0, 0, nullptr), "init");
}

//
TEST_F(BaseRenderTestDeath, render_InvalidParameters)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	unsigned char buffer[1];

	EXPECT_DEATH(renderMock.BaseRender::render(0, 100, 0, 0, 100, buffer), "zero");
	EXPECT_DEATH(renderMock.BaseRender::render(100, 0, 0, 0, 100, buffer), "zero");
	EXPECT_DEATH(renderMock.BaseRender::render(100, 100, -1, 0, 100, buffer), "zero");
	EXPECT_DEATH(renderMock.BaseRender::render(100, 100, 0, -1, 100, buffer), "zero");
	EXPECT_DEATH(renderMock.BaseRender::render(100, 100, 0, 0, 0, buffer), "zero");
	EXPECT_DEATH(renderMock.BaseRender::render(100, 100, 0, 0, 100, nullptr), "null");
}

//
TEST_F(BaseRenderTest, render_NotCreated)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();

	unsigned char buffer[1];

	EXPECT_THROW(renderMock.BaseRender::render(1, 1, 0, 0, 1, buffer), exception::IOException);
}

//
TEST_F(BaseRenderTest, render_NotNormalized)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();
	renderMock.BaseRender::load(::boost::filesystem::system_complete(
		*BaseRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg").wstring());
	renderMock.BaseRender::create();

	unsigned char buffer[1];

	EXPECT_THROW(renderMock.BaseRender::render(1, 1, 0, 0, 1, buffer), exception::IOException);
}

//
TEST_F(BaseRenderTest, render)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();
	renderMock.BaseRender::load(::boost::filesystem::system_complete(
		*BaseRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg").wstring());
	renderMock.BaseRender::create();
	renderMock.BaseRender::normalize();

	::std::vector<unsigned char> buffer(4, 0);

	renderMock.BaseRender::render(1, 1, 0, 0, 1, &buffer[0]);

	EXPECT_TRUE((0 != buffer[0]) || (0 != buffer[1]) || (0 != buffer[2]) || (0 != buffer[3]));
}

//
TEST_F(BaseRenderTestDeath, close_NotInit)
{
	BaseRenderMock renderMock;

	EXPECT_DEATH(renderMock.BaseRender::close(), "init");
}

//
TEST_F(BaseRenderTest, close)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::init();
	renderMock.BaseRender::load(::boost::filesystem::system_complete(
		*BaseRenderMock::executablePath / L".." / L".." / L"resources" / L"lion.svg").wstring());
	renderMock.BaseRender::create();
	renderMock.BaseRender::normalize();

	renderMock.BaseRender::close();

	EXPECT_THROW(renderMock.BaseRender::normalize(), exception::IOException);
}

//
TEST_F(BaseRenderTest, destroy_NotInit)
{
	BaseRenderMock renderMock;

	renderMock.BaseRender::destroy();
}

//
TEST_F(BaseRenderTest, destroy)
{
	BaseRenderMock renderMock;

	EXPECT_FALSE(renderMock.BaseRender::isInit());

	renderMock.BaseRender::init();

	EXPECT_TRUE(renderMock.BaseRender::isInit());

	renderMock.BaseRender::destroy();

	EXPECT_FALSE(renderMock.BaseRender::isInit());
}

