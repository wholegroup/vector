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
//		ColorMungerUIMac.cpp
//
//	Copyright 1996-1997, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the user interface source
//		for the Filter module ColorMunger, a module
//		showing the use of the Color Services suite.
//
//	Use:
//		This module takes a source color of any color space
//		and converts it to a target color in any color
//		space.  It shows how to convert colors as well as
//		pop the color picker.  It appears in
//		Filters>>Utilities>>ColorMunger.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "PIDefines.h"
#include "ColorMunger.h"

//-------------------------------------------------------------------------------
//	Definitions -- Dialog items
//-------------------------------------------------------------------------------

const uint16 kMin =	0;
const uint16 kMax =	32767;

const uint8 kPrecision = 6;

const uint8 dSourceRGB = 3;
const uint8 dSourceHSB = dSourceRGB+1;
const uint8 dSourceCMYK = dSourceHSB+1;
const uint8 dSourceLab = dSourceCMYK+1;
const uint8 dSourceGray = dSourceLab+1;
const uint8 dSourceHSL = dSourceGray+1;
const uint8 dSourceXYZ = dSourceHSL+1;
const uint8 dSourcePicker = dSourceXYZ+1;

const uint8 dSource1 = dSourceRGB;
const uint8 dSourceEnd = dSourceXYZ;

const uint8 dESource1 = 11;
const uint8 dESource2 = dESource1+1;
const uint8 dESource3 = dESource2+1;
const uint8 dESource4 = dESource3+1;

const uint8 dEditSource1 = dESource1;
const uint8 dEditSourceEnd = dESource4;

const uint8 dSourceGroupItem = 15;

const uint8 dTargetRGB = 17;
const uint8 dTargetHSB = dTargetRGB+1;
const uint8 dTargetCMYK = dTargetHSB+1;
const uint8 dTargetLab = dTargetCMYK+1;
const uint8 dTargetGray = dTargetLab+1;
const uint8 dTargetHSL = dTargetGray+1;
const uint8 dTargetXYZ = dTargetHSL+1;
const uint8 dTargetPicker = dTargetXYZ+1;

const uint8 dTarget1 = dTargetRGB;
const uint8 dTargetEnd = dTargetXYZ;

const uint8 dETarget1 = 25;
const uint8 dETarget2 = dETarget1+1;
const uint8 dETarget3 = dETarget2+1;
const uint8 dETarget4 = dETarget3+1;

const uint8 dEditTarget1 = dETarget1;
const uint8 dEditTargetEnd = dETarget4;

const uint8 dTargetGroupItem = 29;

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

//-------------------------------------------------------------------------------

Boolean DoUI (GPtr globals)
{
	return FALSE;
}
// end ColorMungerUIMac.cpp
