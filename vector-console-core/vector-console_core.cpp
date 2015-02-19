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
#include "vector-console_core.h"

using namespace ::com::wholegroup::vector;
using namespace ::com::wholegroup::vector::console;

// Default description
::boost::program_options::options_description defaultDescriptionMaker("Allowed options");

auto defaultDescriptionOptions = defaultDescriptionMaker.add_options()
	("help",    "Produce help message.")
	("file",    ::boost::program_options::wvalue< ::std::wstring>(), "Input file.")
	("zip",     ::boost::program_options::wvalue< ::std::wstring>(), "Zip file.")
	("out",     ::boost::program_options::wvalue< ::std::wstring>(), "Output directory.")
	("width",   ::boost::program_options::value<int>()->default_value(DEFAULT_WIDTH), "Width of an output image.")
	("height",  ::boost::program_options::value<int>()->default_value(DEFAULT_HEIGHT), "Height of an output image.")
	("trim",    ::boost::program_options::value<bool>()->default_value(DEFAULT_TRIM)->implicit_value(DEFAULT_TRIM), "Trimming images.")
	("write",   ::boost::program_options::value<bool>()->default_value(DEFAULT_WRITE)->implicit_value(DEFAULT_WRITE), "Write to files.")
	("rewrite", ::boost::program_options::value<bool>()->default_value(DEFAULT_REWRITE)->implicit_value(DEFAULT_REWRITE), "Rewrite files.")
	("repeat",  ::boost::program_options::value<int>()->default_value(1), "Number of iteration.");

::boost::program_options::options_description const Process::defaultDescription(defaultDescriptionMaker);

// Default extensions map
::std::map< ::std::wstring, ::std::wstring> const Process::defaultExtensionsMap = 
	::boost::assign::map_list_of
	(L".wmf",  L"wmf")
	(L".emf",  L"emf")
	(L".svg",  L"svg")
	(L".svgz", L"svg")
	(L".wpg",  L"wpg")
	(L".cdr",  L"cdr");

// The mutex is used for locking the error counter.
::boost::mutex Process::errorCounterLock;

// The mutex is used for locking the success counter.
::boost::mutex Process::successCounterLock;

/************************************************************************/
/* Default constructor.                                                 */
/************************************************************************/
Process::Process(): 
	description(defaultDescription), 
	extensionsMap(defaultExtensionsMap),
	errorCounter(0),
	successCounter(0)
{
}

/************************************************************************/
/* Constructor with parameters.                                         */
/************************************************************************/
Process::Process(
	::boost::program_options::options_description const & description,
	::std::map< ::std::wstring, ::std::wstring> const & extensionsMap)
	: description(description), extensionsMap(extensionsMap),
	errorCounter(0),
	successCounter(0)
{
}

/************************************************************************/
/* Destructor.                                                          */
/************************************************************************/
Process::~Process()
{
}

/************************************************************************/
/* Start the process.                                                   */
/************************************************************************/
void Process::start(int argc, wchar_t const * argv[])
{
	::boost::timer timer;

	// Parse a command line
	::boost::program_options::variables_map vm;

	if (1 < argc)
	{
		::boost::program_options::store(
			::boost::program_options::parse_command_line(argc, argv, this->description), vm);

		::boost::program_options::notify(vm);
	}

	// Print help if parameters are not defined
	if ((0 == vm.size()) || (0 < vm.count("help")))
	{
		help();

		return;
	}
	
	//
	// Validate the specified options
	validateOptions(vm);

	// Get a file list for processing
	timer.restart();

	bool isFile = (0 < vm.count("file"));
	bool isZip  = (0 < vm.count("zip"));

	::boost::filesystem::path input;

	::std::vector< ::std::wstring> fileList;

	if (isFile)
	{
		input = ::boost::filesystem::system_complete(vm["file"].as< ::std::wstring>());

		fileList.push_back(input.wstring());
	}
	else if (isZip)
	{
		input = ::boost::filesystem::system_complete(vm["zip"].as< ::std::wstring>());

		fileList = listFromZip(input);
	}

	::std::wcout << L"[T] Time of get list is " << ::boost::trim_right_copy_if(::boost::trim_right_copy_if(::boost::str(
		::boost::wformat(L"%|.20f|") % timer.elapsed()), boost::is_any_of("0")), boost::is_any_of(".")) << ::std::endl;
	::std::wcout << ::std::endl; 

	// Get the output path
	::boost::filesystem::path output;

	if (vm.count("out"))
	{
		output = ::boost::filesystem::system_complete(vm["out"].as< ::std::wstring>());
	}
	else
	{
		output = ::boost::filesystem::current_path();
	}

	// Create pairs for next processing
	timer.restart();

	PathsPairsVector pairs;

	pairs.reserve(fileList.size());

	for (auto file = fileList.begin(); file != fileList.end(); ++file)
	{
		PathsPair pair;

		if (isFile)
		{
			pair.first  = *file;
			pair.second = ::boost::filesystem::system_complete(
				output / ::boost::filesystem::path(*file).filename()).replace_extension(L".png");
		}
		else if (isZip)
		{
			pair.first  = input.wstring() + L"!/" + *file;
			pair.second = ::boost::filesystem::system_complete(output / *file).replace_extension(L".png");
		}

		//  Create a directory tree
		if (vm["write"].as<bool>())
		{
			createDirectoryTree(pair.second.parent_path());
		}

		// Add to the list
		pairs.push_back(pair);
	}

	::std::wcout << L"[T] Time of preparing pairs is " << ::boost::trim_right_copy_if(::boost::trim_right_copy_if(::boost::str(
		::boost::wformat(L"%|.20f|") % timer.elapsed()), boost::is_any_of("0")), boost::is_any_of(".")) << ::std::endl;
	::std::wcout << ::std::endl; 

	// Render the list
	jni::Jvm jvm;

	jvm.loadJvm(jni::JvmFinder().findJvm());
	jvm.createJvm(::boost::filesystem::system_complete(jni::VECTOR_CORE_JAR).wstring());

	renderList(jvm, pairs, 
		static_cast<unsigned int>(vm["width"].as<int>()),
		static_cast<unsigned int>(vm["height"].as<int>()), 
		vm["trim"].as<bool>(), 
		static_cast<unsigned int>(vm["repeat"].as<int>()), 
		vm["write"].as<bool>(),
		vm["rewrite"].as<bool>());

	jvm.destroyJvm();
	jvm.unloadJvm();

	// Output statistics
	::std::wcout << L"Success = " << getSuccessCounter() << ::std::endl;
	::std::wcout << L"Error   = " << getErrorCounter() << ::std::endl;
}

/************************************************************************/
/* Show the help info.                                                  */
/************************************************************************/
void Process::help() const
{
	::std::cout << ::std::endl << "::Vector::Console" << ::std::endl;
	::std::cout << ::std::endl << this->description << ::std::endl;
}

/************************************************************************/
/* Validate the specified options.                                      */
/************************************************************************/
void Process::validateOptions(::boost::program_options::variables_map const & vm) const
{
	// If input files are not defined
	if ((0 == vm.count("file")) && (0 == vm.count("zip")))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"You haven't set any input files."));
	}

	// Check the specified width
	int const width = vm["width"].as<int>();

	if (0 >= width)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"The specified width is zero."));
	}

	if (MAX_WIDTH < width)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"The specified width is bigger then the maximum width."));
	}

	// Check the specified heigh
	int const height = vm["height"].as<int>();

	if (0 >= height)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"The specified height is zero."));
	}

	if (MAX_HEIGHT < height)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"The specified height is bigger then the maximum height."));
	}

	// Check the specified number of repeat
	int const repeat = vm["repeat"].as<int>();

	if (0 >= repeat)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"The specified repeat number is zero."));
	}

	if (MAX_REPEAT < repeat)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"The specified repeat number is bigger then the maximum."));
	}
}

/************************************************************************/
/* Get a vector files list from the specified ZIP.                      */
/************************************************************************/
::std::vector< ::std::wstring> const Process::listFromZip(::boost::filesystem::path const & zipPath) const
{
	// Open the zip
	::boost::shared_ptr<unzFile__> zip(unzOpen(zipPath.string().c_str()),
		[](unzFile zip){unzClose(zip);});

	if (!zip)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error loading the ZIP file."));
	}

	// Go to the first record
	if (UNZ_OK != unzGoToFirstFile(zip.get()))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An unknown error."));
	}

	// File-by-file
	::std::vector< ::std::wstring> fileList;

	unz_file_info       fileInfo;
	::std::vector<char> fileName(512, 0);

	do
	{
		// Get info
		fileName[0] = 0;

		if (UNZ_OK != unzGetCurrentFileInfo(zip.get(), &fileInfo, &fileName[0], 
			static_cast<uLong>(fileName.size()), nullptr, 0, nullptr, 0))
		{
			BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
				L"An unknown error."));
		}

		::std::wstring zipEntry;

		try
		{
			zipEntry = ::std::wstring(::boost::locale::conv::utf_to_utf<wchar_t>(&fileName[0]));
		}
		catch (::std::range_error const& ex)
		{
			::std::cout << ex.what() << std::endl;
			::std::cout << ::std::string(fileName.begin(), ::std::find(fileName.begin(), fileName.end(), 0)) << ::std::endl;

			continue;
		}

		// Check if it contains a directory
		if (('/' == zipEntry[zipEntry.length() - 1]) || ('\\' == zipEntry[zipEntry.length() - 1]))
		{
			continue;
		}

		// Filter by an extension
		auto zipPath = ::boost::filesystem::path(zipEntry);

		if (this->extensionsMap.find(::boost::to_lower_copy(zipPath.extension().wstring())) == this->extensionsMap.end())
		{
			continue;
		}

		// Add to file list
		fileList.push_back(zipEntry);

	} while (UNZ_OK == unzGoToNextFile(zip.get()));

	return fileList;
}

/************************************************************************/
/* Render the specified vector file.                                    */
/************************************************************************/
BytesVector const Process::renderFile(jni::Jvm& jvm, 
	::boost::filesystem::path const & vectorPath,
	unsigned int& width, unsigned int& height, bool trim) const
{
	BOOST_ASSERT_MSG(jvm.isCreated(), "JVM must be created.");
	BOOST_ASSERT_MSG(width > 0, "The width is zero.");
	BOOST_ASSERT_MSG(height > 0, "The height is zero.");
	BOOST_ASSERT_MSG(width <= MAX_WIDTH, "The width is bigger than the maximum width.");
	BOOST_ASSERT_MSG(height <= MAX_HEIGHT, "The height is bigger than the maximum height.");

	// Find the extension
	const ::std::map< ::std::wstring, ::std::wstring>::const_iterator extensionIt = 
		this->extensionsMap.find(::boost::to_lower_copy(vectorPath.extension().wstring()));

	if (extensionsMap.end() == extensionIt)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An unknown type of file."));
	}

	// Create a render
	::boost::shared_ptr<jni::BaseRender> render;

	if (L"wmf" == extensionIt->second)
	{
		render.reset(new jni::WmfRender(&jvm));
	}
	else if (L"emf" == extensionIt->second)
	{
		render.reset(new jni::EmfRender(&jvm));
	}
	else if (L"svg" == extensionIt->second)
	{
		render.reset(new jni::SvgRender(&jvm));
	}
	else if (L"wpg" == extensionIt->second)
	{
		render.reset(new jni::WpgRender(&jvm));
	}
	else if (L"cdr" == extensionIt->second)
	{
		render.reset(new jni::CdrRender(&jvm));
	}
	else
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"It's unsupported type."));
	}

	// Init the render
	render->init();

	// Load the specified vector file
	render->load(vectorPath.wstring());

	// Create
	render->create();

	// Normalize
	double const ratio = render->normalize();

	if (trim)
	{
		if (ratio > 1)
		{
			height = ::boost::math::iround(width / ratio);
		}
		else
		{
			width = ::boost::math::iround(height * ratio);
		}
	}

	// Render the vector image
	::std::vector<unsigned char> image(width * height * 4, 0);

	unsigned int side(DEFAULT_SIDE);
	::std::vector<unsigned char> buffer(side * side * 4, 0);

	unsigned int left = 0;
	unsigned int top  = 0;

	while (top < height)
	{
		// Render a block
		render->render(width, height, left, top, side, &buffer[0]);

		// Copy the buffer to the image
		int nRows = side;

		if ((top + side) > height)
		{
			nRows = height - top;
		}

		int nCols = side;

		if ((left + side) > width)
		{
			nCols = width - left;
		}

		for (int y = 0; y < nRows; y++)
		{
			memcpy(&image[((top + y) * width + left) << 2], &buffer[(y * side) << 2], nCols << 2);
		}

		// Calculate next step
		left += side;

		if (left >= width)
		{
			top += side;

			left = 0;
		}
	}

	// Close the render
	render->close();

	return image;
}


/************************************************************************/
/* Write the buffer to a new PNG file.                                  */
/************************************************************************/
void Process::writePng(BytesVector const & buffer, 
	unsigned int width, unsigned int height,
	::boost::filesystem::path const & pngPath) const
{
	BOOST_ASSERT_MSG(0 < buffer.size(), "The buffer size is zero.");
	BOOST_ASSERT_MSG(buffer.size() == width * height * 4, "An error size of buffer.");

	// Variables
	png_structp pngPtr   = nullptr;
	png_infop   infoPtr  = nullptr;
	FILE*       fPng     = nullptr;
	png_byte**  rows     = nullptr;
	bool        complete = false;

	BOOST_SCOPE_EXIT((&rows)(&pngPtr)(&infoPtr)(&fPng)(&complete)(&pngPath)) {
		if ((nullptr != pngPtr) && (nullptr != rows))
		{
			png_free(pngPtr, rows);
		}

		if (nullptr != pngPtr)
		{
			png_destroy_write_struct(&pngPtr, &infoPtr);
		}

		if (fPng)
		{
			fclose(fPng);
		}

		try
		{
			if (!complete && ::boost::filesystem::is_regular_file(pngPath))
			{
				::boost::filesystem::remove(pngPath);
			}
		}
		catch(::boost::filesystem::filesystem_error& ex)
		{
			UNREFERENCED_PARAMETER(ex);
		}
	} BOOST_SCOPE_EXIT_END

	// Open an output file
#ifdef WIN32
	_wfopen_s(&fPng, pngPath.wstring().c_str(), L"wb");
#else
	fPng = fopen(::boost::locale::conv::utf_to_utf<char>(
		pngPath.wstring().c_str()).c_str(), "wb");
#endif

	if (nullptr == fPng)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An error opening file."));
	}

	// Allocates memory
	::std::wstring errorMessage(L"An unknown error");

	pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &errorMessage, 
		Process::pngErrorHandler, Process::pngErrorHandler);

	if (nullptr == pngPtr)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"Out of memory."));
	}

	infoPtr = png_create_info_struct(pngPtr);

	if (nullptr == infoPtr)
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			L"An unknown error."));
	}

	// Set up error handling.
	if (0 != setjmp(png_jmpbuf(pngPtr)))
	{
		BOOST_THROW_EXCEPTION(exception::IOException() << exception::ErrorMessage(
			errorMessage));
	}

	// Set image attributes.
	png_set_IHDR(
		pngPtr, 
		infoPtr, 
		width, 
		height, 
		8, 
		PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	// Set a compression level
	png_set_compression_level(pngPtr, Z_BEST_SPEED);

	// Create rows
	rows = (png_byte**)png_malloc(pngPtr, height * sizeof(png_byte*));

	for (unsigned int y = 0; y < height; y++)
	{
		rows[y] = const_cast<png_byte*>(&buffer[(y * width) << 2]);
	}

	// Write the image 
	png_init_io(pngPtr, fPng);
	png_set_rows(pngPtr, infoPtr, rows);
	png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);

	// Set the complete flag
	complete = true;
}

/************************************************************************/
/* PNG error handler.                                                   */
/************************************************************************/
void Process::pngErrorHandler(png_structp pngPtr, png_const_charp message)
{
	for( ; ; )
	{
		// Check the parameters
		if (nullptr == pngPtr)
		{
			break;
		}

		if (nullptr == message)
		{
			break;
		}

		// Get the pointer
		::std::wstring* errorPtr = (::std::wstring*)png_get_error_ptr(pngPtr);

		if (nullptr == errorPtr)
		{
			break;
		}

		// Set a message
		::std::string strMessage(message);

		*errorPtr = ::std::wstring(strMessage.begin(), strMessage.end());

		break;
	}

	// Return
	png_longjmp(pngPtr, -1);
}

/************************************************************************/
/* Render the specified list.                                           */
/************************************************************************/
void Process::renderList(jni::Jvm& jvm, PathsPairsVector const & list,
	unsigned int width, unsigned int height, bool trim, 
	unsigned int repeatCount, bool write, bool rewrite)
{
	BOOST_ASSERT_MSG(0 < repeatCount, "The repeat count is zero.");
	BOOST_ASSERT_MSG(MAX_REPEAT >= repeatCount, "The repeat count is bigger then the maximum.");

	::boost::timer timer;

	unsigned long successCounter = 0;

	unsigned int currentWidth;
	unsigned int currentHeight;

	for (auto it = list.begin(); it != list.end(); ++it)
	{
		for (unsigned int n = 0; n < repeatCount; ++n)
		{
			::std::wcout << L"[" << n + 1 << L"] Processing " << it->first.wstring() << ::std::endl;

			currentWidth  = width;
			currentHeight = height;

			try
			{
				// Check the output file
				if (write && !rewrite && (1 == repeatCount))
				{
					if (::boost::filesystem::exists(it->second))
					{
						::std::wcout << L"[W] Output file '" << it->second.wstring() << L"' is already exist." << ::std::endl;
						::std::wcout << ::std::endl;

						continue;
					}
				}

				// Render
				timer.restart();

				auto image = renderFile(jvm, it->first, currentWidth, currentHeight, trim);

				::std::wcout << L"[T] Rendering time is " << ::boost::trim_right_copy_if(::boost::trim_right_copy_if(::boost::str(
					::boost::wformat(L"%|.20f|") % timer.elapsed()), boost::is_any_of("0")), boost::is_any_of(".")) << ::std::endl;

				// Write
				if (write)
				{
					::std::wcout << L"[S] Save to " << it->second.wstring() << ::std::endl;

					writePng(image, currentWidth, currentHeight, it->second);
				}

				successCounter++;
			}
			catch (exception::Exception const & ex)
			{
				::std::wcout << L"[E] " << ex.wwhat() << ::std::endl;
			}

			::std::wcout << ::std::endl;
		}
	}

	increaseErrorCounter((static_cast<unsigned long>(list.size()) * repeatCount) - successCounter);
	increaseSuccessCounter(successCounter);
}

/************************************************************************/
/* Increase the error counter.                                          */
/************************************************************************/
void Process::increaseErrorCounter(unsigned long n)
{
	boost::mutex::scoped_lock lock(Process::errorCounterLock);

	this->errorCounter += n;
}

/************************************************************************/
/* Increase the success counter.                                        */
/************************************************************************/
void Process::increaseSuccessCounter(unsigned long n)
{
	boost::mutex::scoped_lock lock(Process::successCounterLock);

	this->successCounter += n;
}

/************************************************************************/
/* Create a directory tree.                                             */
/************************************************************************/
void Process::createDirectoryTree(::boost::filesystem::path const & path) const
{
	::boost::filesystem::create_directories(path);
}
