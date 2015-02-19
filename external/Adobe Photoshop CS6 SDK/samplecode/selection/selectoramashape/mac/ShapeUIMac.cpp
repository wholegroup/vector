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
//		ShapeUIMac.cpp
//
//	Copyright 1996-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the source and functions for the
//		UI for the Import module Shape, which returns a path, layer,
//		or selection with an interesting shape.
//
//	Use:
//		This module specifically works the path return
//		functionality of Photoshop.  The Paths are stored
//		by derezing a work path saved in Photoshop then
//		storing them in a "Path" resource, which is a
//		binary data resource of a "Path" as defined in the
//		"Path layout" section of the Photosop File Format.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "Shape.h"

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------

static const uint8 kDFirstItem = 4;
static const uint8 kDLastItem = kDFirstItem+6;
static const uint8 kDCreateRadio1 = 12;
static const uint8 kDCreateRadioLast = kDCreateRadio1+2;
static const short uiID1 = 16500;

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

//void DoAbout (AboutRecordPtr /*about*/)
//{
//	ShowAbout (AboutID);
//} // end DoAbout

//-------------------------------------------------------------------------------
//
//	DoUI
//
//	Displays the Parameters dialog
//
//	Inputs:
//		GPtr globals		Pointer to global structure.
//
//	Outputs:
//		Displays parameters dialog.
//
//		returns TRUE		If displaying dialog succeeded, user picked an
//							option, and clicked OK.
//		returns FALSE		If displaying dialog failed or user clicked CANCEL.
//
//-------------------------------------------------------------------------------

Boolean DoUIShape (GPtr globals)
{
	return false;
}

//-------------------------------------------------------------------------------

// end ShapeUIMac.cpp
