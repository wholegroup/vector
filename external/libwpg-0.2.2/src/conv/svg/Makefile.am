bin_PROGRAMS = wpg2svg

AM_CXXFLAGS = -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib $(WPD_CFLAGS) $(DEBUG_CXXFLAGS)

wpg2svg_DEPENDENCIES = @WPG2SVG_WIN32_RESOURCE@

if STATIC_TOOLS
wpg2svg_LDADD = ../../lib/@WPG_OBJDIR@/libwpg-@WPG_MAJOR_VERSION@.@WPG_MINOR_VERSION@.a $(WPD_LIBS) @WPG2SVG_WIN32_RESOURCE@
wpg2svg_LDFLAGS = -all-static 
else
wpg2svg_LDADD = ../../lib/libwpg-@WPG_MAJOR_VERSION@.@WPG_MINOR_VERSION@.la $(WPD_LIBS) @WPG2SVG_WIN32_RESOURCE@ 
endif

wpg2svg_SOURCES = \
	wpg2svg.cpp

if OS_WIN32

@WPG2SVG_WIN32_RESOURCE@ : wpg2svg.rc $(wpg2svg_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource wpg2svg.rc @WPG2SVG_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(wpg2svg_SOURCES) \
	wpg2svg.rc.in \
	wpg2svgbatch.pl

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	wpg2svg.rc

bin_SCRIPTS = wpg2svgbatch.pl
