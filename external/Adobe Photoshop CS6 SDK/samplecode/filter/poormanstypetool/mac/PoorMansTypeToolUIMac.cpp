// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		PoorMansTypeToolUIMac.cpp
//
//	Description:
//		This file contains the source and routines for the
//		Filter module Poor Man's Type Tool, a module that 
//		uses the Channel Ports Suite for pixel munging.
//
//	Use:
//		This is a basic module to exemplify all the typical
//		functions a filter module will do: Read scripting
//		parameters, show a user interface, display a proxy,
//		write changed pixel information, and then write
//		scripting parameters.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "PoorMansTypeTool.h"

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

// None.

//-------------------------------------------------------------------------------
//
//	DoAbout
//
//	Displays the About box.
//
//	Inputs:
//		AboutRecordPtr about	Is actually a pointer to some platform
//								specific data for Windows only.
//		AboutID					#define with ID of String resource to use for text
//
//	Outputs:
//		Displays About box (Dialog).  Press Escape, Enter, or Return to
//		dismiss, or click the mouse.
//
//-------------------------------------------------------------------------------
// Taken from ConditionalHeaders.h
#if !OPAQUE_TOOLBOX_STRUCTS
struct PopupPrivateData {
  MenuRef             mHandle;
  SInt16              mID;
};
typedef struct PopupPrivateData         PopupPrivateData;
typedef PopupPrivateData *              PopupPrivateDataPtr;
typedef PopupPrivateDataPtr *           PopupPrivateDataHandle;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

void InitializeMenu(GPtr globals, DialogPtr hDlg, int item);
void ClearPopUp(MenuHandle menu);
static MenuHandle GetMenuHandleFromItemHandle(Handle itemHandle);
static void CToPStrCopy(StringPtr dst, const char* const src);
static void PToCStrCopy(char* dst, const StringPtr src);


void DoAbout (AboutRecordPtr /*about*/)
{
	ShowAbout (AboutID);
} // end DoAbout

Boolean DoUI (GPtr globals)
{
	DebugStr("\pNOT IMPLEMENTED Boolean DoUI (GPtr globals)");
	return false;
}

// end PoorMansTypeToolUIMac.cpp