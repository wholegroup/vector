// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		HistoryUIMac.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//
//	Description:
//		This file contains the user interface routines
//		for the Export module History, a module that
//		displays and lets the user manipulate
//		pseudo-resources of type 'hist'.
//
//	Use:
//		This module shows how to examine, display, and work
//		with pseudo-resources.  An additional element might
//		be to have it export those remaining pseudo-resources
//		to a text file.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "History.h"

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------

const uint8 kDTrimFirst = 3;
const uint8 kDTrimLast = kDTrimFirst+1;

const uint8 kDUpButton = 5;
const uint8 kDDownButton = kDUpButton+1;

const uint8 kDStatusText = 7;

const uint8 kDHistTotal = 7;
const uint8 kDHistItem1 = kDStatusText+1;
const uint8 kDHistItemEnd = kDHistItem1 + kDHistTotal;

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

void UpdateHistories (GPtr globals,
					  DialogPtr dp,
					  short count,
					  Str255 hS);

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

void DoAbout (AboutRecordPtr /*about*/)
{
	ShowAbout (AboutID);
} // end DoAbout

//-------------------------------------------------------------------------------

Boolean DoUI (GPtr globals)
{
	DebugStr("\pNOT IMPLEMENTED Boolean DoUI (GPtr globals)");
	return FALSE;
}

//-------------------------------------------------------------------------------

// end HistoryUIMac.cpp
