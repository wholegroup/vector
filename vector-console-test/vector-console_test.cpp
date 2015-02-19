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

using ::testing::_;
using ::testing::Return;
using ::testing::ByRef;
using ::testing::Invoke;
using ::testing::WithArgs;
using ::testing::Throw;

using namespace ::com::wholegroup::vector;

//
class ProcessMock
	: public console::Process
{
	public:

		ProcessMock()
		{
			ON_CALL(*this, validateOptions(_))
				.WillByDefault(Invoke(this, &ProcessMock::wrapValidateOptions));

			ON_CALL(*this, createDirectoryTree(_))
				.WillByDefault(Return());
		};

		MOCK_CONST_METHOD0(help, void(void));

		MOCK_CONST_METHOD1(validateOptions, 
			void(::boost::program_options::variables_map const &));

		void wrapValidateOptions(::boost::program_options::variables_map const & vm) { 
			return Process::validateOptions(vm); }

		MOCK_CONST_METHOD5(renderFile, console::BytesVector const (jni::Jvm&, 
			::boost::filesystem::path const &, unsigned int&, unsigned int&, bool));

		MOCK_CONST_METHOD4(writePng, void(console::BytesVector const &, 
			unsigned int, unsigned int, ::boost::filesystem::path const &));

		MOCK_METHOD8(renderList, void(jni::Jvm&, console::PathsPairsVector const &, 
			unsigned int, unsigned int, bool, unsigned int, bool, bool));

		MOCK_CONST_METHOD1(createDirectoryTree, void(::boost::filesystem::path const &));
};

//
class ProcessTest
	: public ::testing::Test
{
	protected:

		::boost::filesystem::path executablePath;

	protected:

		virtual void SetUp()
		{
			this->executablePath = ::boost::filesystem::path(
				::testing::internal::g_argvs[0].c_str()).parent_path();
		}
};

class ProcessTestDeath : public ProcessTest {};

//
void validateTemplate(::std::string const & str)
{
	// Split the string
	::std::vector< ::std::string> options;
	::std::string strWithSpace(" " + str);

	::boost::split(options, strWithSpace, ::boost::is_any_of(" "),
		::boost::token_compress_on);

	::std::vector<char const *> argv;

	for(auto it = options.begin(); it != options.end(); ++it)
	{
		argv.push_back(it->c_str());
	}

	// Prepare program options
	::boost::program_options::variables_map vm;

	::boost::program_options::store(
		::boost::program_options::parse_command_line(static_cast<int>(argv.size()), &argv[0], 
		console::Process::defaultDescription), vm);
	::boost::program_options::notify(vm);

	// Validate
	console::Process pr;

	pr.validateOptions(vm);
}

//
TEST_F(ProcessTest, validateOptions)
{
	EXPECT_THROW(validateTemplate(""), exception::IOException);
	EXPECT_THROW(validateTemplate("--file vector.svg --width 0"), exception::IOException);
	EXPECT_THROW(validateTemplate("--file vector.svg --width " + 
		::boost::lexical_cast< ::std::string>(console::MAX_WIDTH + 1)), exception::IOException);
	EXPECT_THROW(validateTemplate("--file vector.svg --height 0"), exception::IOException);
	EXPECT_THROW(validateTemplate("--file vector.svg --height " + 
		::boost::lexical_cast< ::std::string>(console::MAX_HEIGHT + 1)), exception::IOException);
	EXPECT_THROW(validateTemplate("--file vector.svg --repeat 0"), exception::IOException);
	EXPECT_THROW(validateTemplate("--file vector.svg --repeat " + 
		::boost::lexical_cast< ::std::string>(console::MAX_REPEAT + 1)), exception::IOException);

	validateTemplate("--file vector.svg");
	validateTemplate("--file vector.svg --width 1 --height 1 --repeat 1");
	validateTemplate("--file vector.svg --width " + 
		::boost::lexical_cast< ::std::string>(console::MAX_WIDTH) + 
		" --height " + 
		::boost::lexical_cast< ::std::string>(console::MAX_HEIGHT) + 
		" --repeat " + 
		::boost::lexical_cast< ::std::string>(console::MAX_REPEAT));
}

//
TEST_F(ProcessTest, start_Help)
{
	ProcessMock pr;

	EXPECT_CALL(pr, help());

	const wchar_t* argv[] =  {L""};

	pr.console::Process::start(sizeof(argv) / sizeof(argv[0]), argv);
}

//
TEST_F(ProcessTest, start_InvalidOptions)
{
	console::Process pr;

	const wchar_t* argv[] =  {L"", L"--invalid"};

	EXPECT_THROW(pr.start(sizeof(argv) / sizeof(argv[0]), argv), 
		::boost::program_options::unknown_option);
}

//
TEST_F(ProcessTest, start_NotValidate)
{
	ProcessMock pr;

	EXPECT_CALL(pr, validateOptions(_))
		.Times(1);

	const wchar_t* argv[] =  {L"", L"--width", L"0"};

	EXPECT_THROW(pr.console::Process::start(sizeof(argv) / sizeof(argv[0]), argv), ::exception::IOException);
}

//
TEST_F(ProcessTest, start_File)
{
	ProcessMock pr;

	EXPECT_CALL(pr, validateOptions(_));

	console::PathsPairsVector const pairs = ::boost::assign::list_of
		(console::PathsPair(::boost::filesystem::current_path() / L"vector.svg", 
		::boost::filesystem::current_path() / L"vector.png"));

	EXPECT_CALL(pr, renderList(_, 
		pairs,
		console::DEFAULT_WIDTH, 
		console::DEFAULT_HEIGHT, 
		console::DEFAULT_TRIM,
		1, 
		console::DEFAULT_WRITE, 
		console::DEFAULT_REWRITE));

	EXPECT_CALL(pr, createDirectoryTree(_))
		.Times(static_cast<int>(pairs.size()));

	const wchar_t* argv[] =  {L"", L"--file", L"vector.svg"};

	pr.console::Process::start(sizeof(argv) / sizeof(argv[0]), argv);
}

//
TEST_F(ProcessTest, start_Zip)
{
	ProcessMock pr;

	EXPECT_CALL(pr, validateOptions(_));

	console::PathsPairsVector const pairs = ::boost::assign::list_of
		(console::PathsPair(::boost::filesystem::system_complete(
		this->executablePath / L".." / L".." / L"resources" / L"archive.zip").wstring() + L"!/WMF/Butterfly.WmF", 
		::boost::filesystem::current_path() / L"WMF" / L"Butterfly.png"))
		(console::PathsPair(::boost::filesystem::system_complete(
		this->executablePath / L".." / L".." / L"resources" / L"archive.zip").wstring() + L"!/SVG/tiger.SVG", 
		::boost::filesystem::current_path() / L"SVG" / L"tiger.png"))
		(console::PathsPair(::boost::filesystem::system_complete(
		this->executablePath / L".." / L".." / L"resources" / L"archive.zip").wstring() + L"!/SVG/tiger.Svgz", 
		::boost::filesystem::current_path() / L"SVG" / L"tiger.png"));

	EXPECT_CALL(pr, renderList(_, 
		pairs,
		console::DEFAULT_WIDTH, 
		console::DEFAULT_HEIGHT, 
		console::DEFAULT_TRIM,
		1, 
		console::DEFAULT_WRITE, 
		console::DEFAULT_REWRITE));

	EXPECT_CALL(pr, createDirectoryTree(_))
		.Times(static_cast<int>(pairs.size()));

	::std::wstring const archive = ::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring();

	const wchar_t* argv[] =  {L"", L"--zip", archive.c_str()};

	pr.console::Process::start(sizeof(argv) / sizeof(argv[0]), argv);
}

//
TEST_F(ProcessTest, start_Unicode)
{
	ProcessMock pr;

	EXPECT_CALL(pr, validateOptions(_));

	console::PathsPairsVector const pairs = ::boost::assign::list_of
		(console::PathsPair(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"unicode.zip").wstring() + ::boost::locale::conv::utf_to_utf<wchar_t>("!/Skåning.wmf"), 
		::boost::filesystem::current_path() / ::boost::locale::conv::utf_to_utf<wchar_t>("Skåning.png")))
		(console::PathsPair(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"unicode.zip").wstring() + ::boost::locale::conv::utf_to_utf<wchar_t>("!/你好.wmf"), 
		::boost::filesystem::current_path() / ::boost::locale::conv::utf_to_utf<wchar_t>("你好.png")));

	::std::wstring a = ::boost::filesystem::path(::boost::locale::conv::utf_to_utf<wchar_t>("你好.png")).wstring();
	::std::wstring b = ::boost::locale::conv::utf_to_utf<wchar_t>("你好.png");

	EXPECT_CALL(pr, renderList(_, 
		pairs,
		console::DEFAULT_WIDTH, 
		console::DEFAULT_HEIGHT, 
		console::DEFAULT_TRIM,
		1, 
		console::DEFAULT_WRITE, 
		console::DEFAULT_REWRITE));

	EXPECT_CALL(pr, createDirectoryTree(_))
		.Times(static_cast<int>(pairs.size()));

	::std::wstring const unicode = ::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"unicode.zip").wstring();

	wchar_t const * argv[] =  {L"", L"--zip", unicode.c_str()};

	pr.console::Process::start(sizeof(argv) / sizeof(argv[0]), argv);
}

//
TEST_F(ProcessTest, listFromZip_NotFound)
{
	console::Process pr;

	EXPECT_THROW(pr.listFromZip(::boost::filesystem::system_complete(L"resources/not.found.zip")), 
		exception::IOException);
}

//
TEST_F(ProcessTest, listFromZip_NotArchive)
{
	console::Process pr;

	EXPECT_THROW(pr.listFromZip(::boost::filesystem::system_complete(
		this->executablePath / L".." / L".." / L"resources" / L"lion.svg")), 
		exception::IOException);
}

//
TEST_F(ProcessTest, listFromZip_Unicode)
{
	console::Process pr;

	auto fileList = pr.listFromZip(::boost::filesystem::system_complete(
		this->executablePath / L".." / L".." / L"resources" / L"unicode.zip"));

	EXPECT_EQ(2, fileList.size());
	EXPECT_EQ(::boost::assign::list_of
		(::std::wstring(::boost::locale::conv::utf_to_utf<wchar_t>("Skåning.wmf")))
		(::std::wstring(::boost::locale::conv::utf_to_utf<wchar_t>("你好.wmf"))), fileList);
}

//
TEST_F(ProcessTest, listFromZip)
{
	console::Process pr;
	
	auto fileList = pr.listFromZip(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"archive.zip"));

	EXPECT_EQ(3, fileList.size());
	EXPECT_EQ(::boost::assign::list_of(L"WMF/Butterfly.WmF")(L"SVG/tiger.SVG")
		(L"SVG/tiger.Svgz"), fileList);
}

//
TEST_F(ProcessTestDeath, renderFile_InvalidParameters)
{
	console::Process pr;

	unsigned int width  = 512;
	unsigned int height = 512;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());

	EXPECT_DEATH(pr.renderFile(jvm, L"", width, height, true), "create");

	jvm.createJvm(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

	width = 0; height = 1;
	EXPECT_DEATH(pr.renderFile(jvm, L"", width, height, true), "zero");

	width = 1; height = 0;
	EXPECT_DEATH(pr.renderFile(jvm, L"", width, height, true), "zero");

	width = console::MAX_WIDTH + 1; height = 1;
	EXPECT_DEATH(pr.renderFile(jvm, L"", width, height, true), "max");

	width = 1; height = console::MAX_HEIGHT + 1;
	EXPECT_DEATH(pr.renderFile(jvm, L"", width, height, true), "max");
}

//
TEST_F(ProcessTest, renderFile_UnknowType)
{
	console::Process pr;

	unsigned int width  = 512;
	unsigned int height = 512;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

	EXPECT_THROW(pr.renderFile(jvm, ::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring() + L"!/text.txt", 
		width, height, true), exception::IOException);
}

//
TEST_F(ProcessTest, renderFile_Unsupported)
{
	console::Process pr(console::Process::defaultDescription, 
		::boost::assign::map_list_of(L".txt", L"txt"));

	unsigned int width  = 512;
	unsigned int height = 512;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

	EXPECT_THROW(pr.renderFile(jvm, ::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring() + L"!/text.txt", 
		width, height, true), exception::IOException);
}

//
TEST_F(ProcessTest, renderFile_Trim)
{
	console::Process pr;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

	// Trim off
	{
		unsigned int width  = 512;
		unsigned int height = 512;

		const auto& image = pr.renderFile(jvm, ::boost::filesystem::system_complete(
			this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring() + L"!/WMF/Butterfly.WmF", 
			width, height, false);

		// ==
		EXPECT_EQ(width, height); 
		EXPECT_EQ(image.size(), width * height * 4);
	}

	// Trim on
	{
		unsigned int width  = 512;
		unsigned int height = 512;

		const auto& image = pr.renderFile(jvm, ::boost::filesystem::system_complete(
			this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring() + L"!/WMF/Butterfly.WmF", 
			width, height, true);

		// !=
		EXPECT_NE(width, height);
		EXPECT_EQ(image.size(), width * height * 4);
	}
}

//
TEST_F(ProcessTest, renderFile_Wmf)
{
	console::Process pr;

	unsigned int width  = 512;
	unsigned int height = 512;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

	const auto& image = pr.renderFile(jvm, ::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring() + L"!/WMF/Butterfly.WmF", 
		width, height, true);

	EXPECT_NE(width, height);
	EXPECT_EQ(image.size(), width * height * 4);
}

//
TEST_F(ProcessTest, renderFile_Svg)
{
	console::Process pr;

	unsigned int width  = 512;
	unsigned int height = 512;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

	const auto& image = pr.renderFile(jvm, ::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring() + L"!/SVG/tiger.SVG", 
		width, height, true);

	EXPECT_NE(width, height);
	EXPECT_EQ(image.size(), width * height * 4);
}

//
TEST_F(ProcessTest, renderFile_Svgz)
{
	console::Process pr;

	unsigned int width  = 512;
	unsigned int height = 512;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"vector-core" / jni::VECTOR_CORE_JAR).wstring());

	const auto& image = pr.renderFile(jvm, ::boost::filesystem::system_complete(
		this->executablePath / ".." / ".." / L"resources" / L"archive.zip").wstring() + L"!/SVG/tiger.Svgz", 
		width, height, false);

	EXPECT_EQ(width, height);
	EXPECT_EQ(image.size(), width * height * 4);
}

//
TEST_F(ProcessTestDeath, writePng_InvalidParameters)
{
	console::Process pr;

	unsigned int width  = 1;
	unsigned int height = 1;

	::std::vector<unsigned char> buffer(width * height * 4, 0xFF);

	EXPECT_DEATH(pr.writePng(::std::vector<unsigned char>(), 0, 0, ::std::wstring(L"out.png")), "zero");
	EXPECT_DEATH(pr.writePng(buffer, 0, 0, ::std::wstring(L"out.png")), "size");
}

//
TEST_F(ProcessTest, writePng_ErrorFile)
{
	console::Process pr;

	unsigned int width  = 1;
	unsigned int height = 1;

	::std::vector<unsigned char> buffer(width * height * 4, 0xFF);

	EXPECT_THROW(pr.writePng(buffer, width, height, ::std::wstring(L"..")), exception::IOException);
}

//
TEST_F(ProcessTest, writePng)
{
	console::Process pr;

	unsigned int width  = 1;
	unsigned int height = 1;
	::std::vector<unsigned char> buffer(width * height * 4, 0xFF);

	pr.writePng(buffer, width, height, ::std::wstring(L"out.png"));

	::boost::filesystem::remove(L"out.png");
}

//
TEST_F(ProcessTestDeath, renderList_InvalidParameters)
{
	console::Process pr;

	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(L"resources/" + jni::VECTOR_CORE_JAR).wstring());

	EXPECT_DEATH(pr.renderList(jvm, console::PathsPairsVector(), 
		console::DEFAULT_WIDTH, console::DEFAULT_HEIGHT, true, 0, true, false), "zero");
	EXPECT_DEATH(pr.renderList(jvm, console::PathsPairsVector(), 
		console::DEFAULT_WIDTH, console::DEFAULT_HEIGHT, true, console::MAX_REPEAT + 1, true, false), "big");
}

//
TEST_F(ProcessTest, renderList_ErrorRenderFile)
{
	ProcessMock pr;

	console::PathsPairsVector const list = ::boost::assign::list_of
		(console::PathsPair(L"A", L"B"))
		(console::PathsPair(L"C", L"D"));

	EXPECT_CALL(pr, renderFile(_, _, _, _, _))
		.Times(static_cast<int>(list.size()))
		.WillOnce(Throw(exception::IOException() << exception::ErrorMessage(L"Test exception")))
		.WillRepeatedly(Return(console::BytesVector()));

	EXPECT_CALL(pr, writePng(_, _, _, _))
		.Times(static_cast<int>(list.size() - 1))
		.WillRepeatedly(Return());

	jni::Jvm jvm;

	pr.console::Process::renderList(jvm, 
		list, console::DEFAULT_WIDTH, console::DEFAULT_HEIGHT, true, 1, true, false);

	EXPECT_EQ(list.size() - 1, pr.getErrorCounter());
	EXPECT_EQ(1, pr.getSuccessCounter());
}

//
TEST_F(ProcessTest, renderList_ErrorWritePng)
{
	ProcessMock pr;

	console::PathsPairsVector const list = ::boost::assign::list_of
		(console::PathsPair(L"A", L"B"))
		(console::PathsPair(L"C", L"D"));

	EXPECT_CALL(pr, renderFile(_, _, _, _, _))
		.Times(static_cast<int>(list.size()))
		.WillRepeatedly(Return(console::BytesVector()));

	EXPECT_CALL(pr, writePng(_, _, _, _))
		.Times(static_cast<int>(list.size()))
		.WillRepeatedly(Throw(exception::IOException() << exception::ErrorMessage(L"Test exception")));

	jni::Jvm jvm;

	pr.console::Process::renderList(jvm, 
		list, console::DEFAULT_WIDTH, console::DEFAULT_HEIGHT, true, 1, true, false);

	EXPECT_EQ(list.size(), pr.getErrorCounter());
	EXPECT_EQ(0, pr.getSuccessCounter());
}

//
TEST_F(ProcessTest, renderList)
{
	ProcessMock pr;

	console::PathsPairsVector const list = ::boost::assign::list_of
		(console::PathsPair(L"A", L"B"))
		(console::PathsPair(L"C", L"D"));

	unsigned int repeat = 3;
	unsigned int width  = console::DEFAULT_WIDTH;
	unsigned int height = console::DEFAULT_HEIGHT;

	for (auto it = list.begin(); it != list.end(); ++it)
	{
		EXPECT_CALL(pr, renderFile(_, it->first, width, height, true))
			.Times(repeat)
			.WillRepeatedly(WithArgs<2, 3>(Invoke( [](unsigned int & w, unsigned int & h) -> console::BytesVector {
				--w;
				--h;

				return console::BytesVector(1, 0xF0);
			})));

		EXPECT_CALL(pr, writePng(console::BytesVector(1, 0xF0), console::DEFAULT_WIDTH - 1, console::DEFAULT_HEIGHT - 1, it->second))
			.Times(repeat)
			.WillRepeatedly(Return());
	}

	jni::Jvm jvm;

	pr.console::Process::renderList(jvm, 
		list, console::DEFAULT_WIDTH, console::DEFAULT_HEIGHT, true, repeat, true, false);

	EXPECT_EQ(0, pr.getErrorCounter());
	EXPECT_EQ(list.size() * repeat, pr.getSuccessCounter());
}
