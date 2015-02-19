if PLATFORM_WIN32
no_undefined = -no-undefined
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
no_undefined =
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

lib_LTLIBRARIES = libwpg-@WPG_MAJOR_VERSION@.@WPG_MINOR_VERSION@.la
libwpg_@WPG_MAJOR_VERSION@_@WPG_MINOR_VERSION@_includedir = $(includedir)/libwpg-@WPG_MAJOR_VERSION@.@WPG_MINOR_VERSION@/libwpg
libwpg_@WPG_MAJOR_VERSION@_@WPG_MINOR_VERSION@_include_HEADERS = \
	libwpg.h \
	WPGraphics.h \
	WPGPaintInterface.h

AM_CXXFLAGS = $(WPD_CFLAGS) $(DEBUG_CXXFLAGS)

libwpg_@WPG_MAJOR_VERSION@_@WPG_MINOR_VERSION@_la_LIBADD  = $(WPD_LIBS) @LIBWPG_WIN32_RESOURCE@
libwpg_@WPG_MAJOR_VERSION@_@WPG_MINOR_VERSION@_la_DEPENDENCIES = @LIBWPG_WIN32_RESOURCE@
libwpg_@WPG_MAJOR_VERSION@_@WPG_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic $(no_undefined)
libwpg_@WPG_MAJOR_VERSION@_@WPG_MINOR_VERSION@_la_SOURCES = \
	WPGraphics.cpp \
	WPGDashArray.cpp \
	WPGColor.cpp \
	WPGHeader.cpp \
	WPGSVGGenerator.cpp \
	WPGXParser.cpp \
	WPG1Parser.cpp \
	WPG2Parser.cpp \
	WPGBitmap.cpp \
	WPGInternalStream.cpp \
	libwpg.h \
	libwpg_utils.h \
	WPGColor.h \
	WPGDashArray.h \
	WPGHeader.h \
	WPGSVGGenerator.h \
	WPGXParser.h \
	WPG1Parser.h \
	WPG2Parser.h \
	WPGBitmap.h \
	WPGInternalStream.h\
	WPGPaintInterface.h

if OS_WIN32

@LIBWPG_WIN32_RESOURCE@ : libwpg.rc $(libwpg_@WPG_MAJOR_VERSION@_@WPG_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libwpg.rc @LIBWPG_WIN32_RESOURCE@

endif


EXTRA_DIST = \
	libwpg.h.in \
	libwpg.rc.in \
	makefile.mk

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libwpg.rc
