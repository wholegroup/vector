#pragma once

#ifndef VECTOR_CONSOLE_CORE_H
#define VECTOR_CONSOLE_CORE_H

// com.wholegroup.vector.console
namespace com {namespace wholegroup {namespace vector {namespace console
{
	// Default width.
	unsigned int const DEFAULT_WIDTH(1024);

	// Maximum width
	unsigned int const MAX_WIDTH(8192);

	// Default height.
	unsigned int const DEFAULT_HEIGHT(1024);

	// Maximum height
	unsigned int const MAX_HEIGHT(8192);

	// Default trimming
	bool const DEFAULT_TRIM(true);

	// Default write
	bool const DEFAULT_WRITE(true);

	// Default rewrite
	bool const DEFAULT_REWRITE(false);

	// Maximum number of rendering repetition.
	unsigned int const MAX_REPEAT(100000);

	// Default side size.
	unsigned int const DEFAULT_SIDE(1024);

	typedef ::std::vector<unsigned char> BytesVector;

	typedef ::std::pair< ::boost::filesystem::path, ::boost::filesystem::path> PathsPair;

	typedef ::std::vector<PathsPair> PathsPairsVector;

	class Process
	{
		private:

			// Description
			::boost::program_options::options_description const description;

			// Extensions map
			::std::map< ::std::wstring, ::std::wstring> const extensionsMap;

			// Error counter
			unsigned long errorCounter;

			// The mutex is used for locking the error counter.
			static ::boost::mutex errorCounterLock;

			// Success counter
			unsigned long successCounter;

			// The mutex is used for locking the success counter.
			static ::boost::mutex successCounterLock;

		public:

			// Default description
			static ::boost::program_options::options_description const defaultDescription;

			// Default extensions map
			static ::std::map< ::std::wstring, ::std::wstring> const defaultExtensionsMap;

		public:

			// Default constructor.
			Process();

			// Constructor with parameters.
			Process(::boost::program_options::options_description const & description, 
				::std::map< ::std::wstring, ::std::wstring> const & extensionsMap);

			// Destructor.
			virtual ~Process();

			// Start the process.
			virtual void start(int argc, wchar_t const * argv[]);

			// Show the help info.
			virtual void help() const;

			// Validate the specified options.
			virtual void validateOptions(::boost::program_options::variables_map const & vm) const;

			// Get a vector files list from the specified ZIP.
			virtual ::std::vector< ::std::wstring> const listFromZip(
				::boost::filesystem::path const & zipPath) const;

			// Render the specified vector file.
			virtual BytesVector const renderFile(jni::Jvm& jvm, 
				::boost::filesystem::path const & vectorPath,
				unsigned int& width, unsigned int& height, bool trim) const;

			// Write the buffer to a new PNG file.
			virtual void writePng(BytesVector const & buffer, 
				unsigned int width, unsigned int height,
				::boost::filesystem::path const & pngPath) const;

			// PNG error handler.
			static void pngErrorHandler(png_structp pngPtr, png_const_charp message);

			// Render the specified list.
			virtual void renderList(jni::Jvm& jvm, PathsPairsVector const & list, 
				unsigned int width, unsigned int height, bool trim, 
				unsigned int repeatCount, bool write, bool rewrite);

			// Increase the error counter.
			void increaseErrorCounter(unsigned long n);

			// Get the value of the error counter.
			inline unsigned long getErrorCounter() { return this->errorCounter; }

			// Increase the success counter.
			void increaseSuccessCounter(unsigned long n);

			// Get the value of the success counter.
			inline unsigned long getSuccessCounter() { return this->successCounter; }

			// Create a directory tree.
			virtual void createDirectoryTree(::boost::filesystem::path const & path) const;
	};

};};};};

#endif // VECTOR_CONSOLE_H
