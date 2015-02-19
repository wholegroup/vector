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
//		OutboundUIMac.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//
//	Description:
//		This file contains the user interface source
//		for the Export module Outbound, a module that
//		creates a file and stores raw pixel data in it.
//
//	Use:
//		This module shows how to export raw data to a file.
//		It uses a simple "FileUtilities" library that comes
//		with the SDK.  You use it via File>>Export>>Outbound.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "Outbound.h"
#include <Navigation.h>

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
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

/*****************************************************************************/

static FSSpec reply;

/*****************************************************************************/

void DoAbout (AboutRecordPtr /*about*/)
{
	ShowAbout (AboutID);
} // end DoAbout

/*****************************************************************************/

Boolean DoUI (GPtr globals)
{
	return FALSE;
}

/*****************************************************************************/

Boolean CreateExportFile (GPtr globals)
{
	return PIOpenFile(reply, 
					  &gFRefNum,
					  &gResult);
}

/*****************************************************************************/

Boolean CloseExportFile (GPtr globals)
{
	return PICloseFileAndMakeAlias(reply,
					               gFRefNum,
					               gSameNames,
					               &gStuff->dirty,
					               &gAliasHandle,
					               &gResult);	
}
//-------------------------------------------------------------------------------
// end OutboundUIMac.cpp