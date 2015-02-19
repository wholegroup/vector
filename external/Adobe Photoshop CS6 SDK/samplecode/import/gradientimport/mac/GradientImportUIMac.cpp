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
//		GradientImportUIMac.cpp
//
//	Copyright 1996-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the dialog and platform-specific functions
//		for the Import module Idle, a very simple example of a module
//		that just idles and sends update events.
//
//	Use:
//		This import module is good as an example of a module
//		that forces the background application to redraw
//		its windows.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "GradientImport.h"

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------

static const uint8 dRowsItem = 3;
static const uint8 dColsItem = 4;
static const uint8 dFirstModeItem = 5;
static const uint8 dLastModeItem = 8;
static const uint8 dBitmapModeItem = dFirstModeItem;
static const uint8 dIndexedModeItem = dLastModeItem-1;
static const uint8 dInvertItem = 9;
static const uint8 dModeGroupItem = 12;
static const uint8 dConvertAlpha = 14;

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

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


Boolean OpenOurDialog (GPtr globals)
{
	DebugStr("\pNOT IMPLEMENTED Boolean OpenOurDialog (GPtr globals)");
	return FALSE;
}

/*****************************************************************************/

Boolean RunOurDialog (GPtr globals)
{
	DebugStr("\pNOT IMPLEMENTED Boolean RunOurDialog (GPtr globals)");
	return FALSE;
}

/*****************************************************************************/

void CloseOurDialog (GPtr globals)
{
	DebugStr("\pNOT IMPLEMENTED void CloseOurDialog (GPtr globals)");
}

/*****************************************************************************/

// end GradientImportUIMac.cpp