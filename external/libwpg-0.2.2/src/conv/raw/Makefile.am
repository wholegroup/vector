bin_PROGRAMS = wpg2raw

AM_CXXFLAGS = -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(WPD_CFLAGS) $(DEBUG_CXXFLAGS)

wpg2raw_DEPENDENCIES = @WPG2RAW_WIN32_RESOURCE@

if STATIC_TOOLS
wpg2raw_LDADD = ../../lib/@WPG_OBJDIR@/libwpg-@WPG_MAJOR_VERSION@.@WPG_MINOR_VERSION@.a $(WPD_LIBS) @WPG2RAW_WIN32_RESOURCE@
wpg2raw_LDFLAGS = -all-static 
else
wpg2raw_LDADD = ../../lib/libwpg-@WPG_MAJOR_VERSION@.@WPG_MINOR_VERSION@.la $(WPD_LIBS) @WPG2RAW_WIN32_RESOURCE@ 
endif

wpg2raw_SOURCES = \
	wpg2raw.cpp

if OS_WIN32

@WPG2RAW_WIN32_RESOURCE@ : wpg2raw.rc $(wpg2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource wpg2raw.rc @WPG2RAW_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(wpg2raw_SOURCES) \
	wpg2raw.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	wpg2raw.rc	 
