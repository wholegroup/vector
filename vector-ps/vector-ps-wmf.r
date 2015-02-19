#define formatName          "WMF"
#define plugInName          formatName " Import Plugin"
#define plugInDescription   plugInName
#define plugInCopyrightYear "2012"
#define helperInName        formatName " Helper Plugin"

#include "PIDefines.h"

#ifdef __PIMac__
	#include "PIGeneral.r"
	#include "PIUtilities.r"
#elif defined(__PIWin__)
	#define Rez
	#include "PIGeneral.h"
	#include "PIUtilities.r"
#endif

#include "PITerminology.h"
#include "PIActions.h"

resource 'PiPL' (ResourceID, plugInName " PiPL", purgeable)
{
	 {
		  Kind { ImageFormat },
		  Name { plugInName },
#ifdef __PIWin__
		  // Выключено, т.к. в старых версиях шопа (CS, CS2 и возможно вплоть до CS4) из-за 
        // наличия второго плагина About диалог не показывался (какая-то внутренняя ошибка)
        // Поэтому, второй плагин идет с таким же именем, и он отвечает за вызов About диалога.
		  Category { "**Hidden**" },
#endif // __PIWin__
		  Version { (latestFormatVersion << 16) | latestFormatSubVersion },

		  #ifdef __PIMac__
				#if (defined(__i386__))
					 CodeMacIntel32 { "formatEntry" },
				#endif
				#if (defined(__x86_64__))
					 CodeMacIntel64 { "formatEntry" },
				#endif
		  #else
				#if defined(_WIN64)
					 CodeWin64X86 { "formatEntry" },
				#else
					 CodeWin32X86 { "formatEntry" },
				#endif
		  #endif

		  SupportedModes
		  {
				doesSupportBitmap, 
				doesSupportGrayScale,
				doesSupportIndexedColor, 
				doesSupportRGBColor,
				doesSupportCMYKColor, 
				doesSupportHSLColor,
				doesSupportHSBColor, 
				doesSupportMultichannel,
				doesSupportDuotone, 
				doesSupportLABColor
		  },
		
		  // If you want your format module always enabled.	
		  EnableInfo { "true" },

		  // New for Photoshop 8, document sizes that are really big 
		  // 32 bit row and columns, 2,000,000 current limit but we can handle more
		  PlugInMaxSize { 131072, 131072 },

		  // For older Photoshops that only support 30000 pixel documents, 
		  // 16 bit row and columns
		  FormatMaxSize { { 32768, 32768 } },

		  FormatMaxChannels { {   1, 24, 24, 24, 24, 24, 
							   24, 24, 24, 24, 24, 24 } },
	
		  FmtFileType { 'WMF ', '8BIM' },
		  ReadTypes { { 'WMF ', '8BIM' } },
		  ReadExtensions { { 'WMF ', 'WMZ ' } },

		  FormatFlags
		  {
				fmtDoesNotSaveImageResources, 
				fmtCanRead, 
				fmtCannotWrite, 
				fmtWritesAll, 
				fmtCannotWriteTransparency,
				fmtCannotCreateThumbnail
		  },

		  FormatICCFlags
		  {
				iccCannotEmbedGray,
				iccCanEmbedIndexed,
				iccCannotEmbedRGB,
				iccCanEmbedCMYK
		  }
	 }
};

#ifdef __PIWin__
resource 'PiPL' (ResourceID + 1, helperInName " PiPL", purgeable)
{
	 {
		  Kind { Actions },
		  Name { plugInName },
		  Version { (latestActionsPlugInVersion << 16) | latestActionsPlugInSubVersion },

		  #ifdef __PIMac__
				#if (defined(__i386__))
					 CodeMacIntel32 { "helperEntry" },
				#endif
				#if (defined(__x86_64__))
					 CodeMacIntel64 { "helperEntry" },
				#endif
		  #else
				#if defined(_WIN64)
					 CodeWin64X86 { "helperEntry" },
				#else
					 CodeWin32X86 { "helperEntry" },
				#endif
		  #endif

		  // Include "Persistent" if you want your plug-in to stay loaded
		  // until Photoshop quits:
		  Persistent {},
		
		  // Messages {} is only honored if your plug-in is Persistent:
		  Messages
		  {
				startupRequired,
				doesNotPurgeCache,
				shutdownRequired,
				acceptProperty
		  },
	 }
};

#endif // __PIWin__
