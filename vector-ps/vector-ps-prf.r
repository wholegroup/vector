#define helperInName        "Vector Import Helper Plugin"
#define plugInName          helperInName
#define plugInDescription   plugInName
#define plugInCopyrightYear "2012"

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

resource 'PiPL' (ResourceID, helperInName " PiPL", purgeable)
{
	 {
		  Kind { Actions },
		  Name { helperInName  },
		  Category { "**Hidden**" },
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