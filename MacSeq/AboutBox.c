/*
	File:		AboutBox.c

	Contains:	About Box support for SimpleText

	Version:	Mac OS X

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Copyright � 1993-2001 Apple Computer, Inc., All Rights Reserved
*/

#include "MacIncludes.h"

#include "AboutBox.h"


// --------------------------------------------------------------------------------------------------------------
// INTERNAL ROUTINES
// --------------------------------------------------------------------------------------------------------------
static void DrawCenteredStringAt(Str255 theString, short yLocation)
{
	Rect	portRect;
	CGrafPtr thePort = GetQDGlobalsThePort();

	GetPortBounds(thePort, &portRect);
	
	MoveTo(portRect.left + ((portRect.right-portRect.left) >> 1) -
							(StringWidth(theString) >> 1), yLocation);
	DrawString(theString);
	
} // DrawCenteredStringAt

// --------------------------------------------------------------------------------------------------------------
// OOP INTERFACE ROUTINES
// --------------------------------------------------------------------------------------------------------------

static OSStatus	AboutUpdateWindow(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pData)

	Str255		theString;
	Rect		bounds;

	// type style for application name
	TextFont(systemFont);
	TextSize(12);
	
	// name of application
	GetIndString(theString, kAboutStrings, 1);
	DrawCenteredStringAt(theString, 32);
	
	// type style for all others
	TextFont(applFont);
	TextSize(9);
	
	// author names
	GetIndString(theString, kAboutStrings, 2);
	DrawCenteredStringAt(theString, 50);
	GetIndString(theString, kAboutStrings, 3);
	DrawCenteredStringAt(theString, 65);
	GetIndString(theString, kAboutStrings, 4);
	DrawCenteredStringAt(theString, 80);
	
	// copyright, on the left
	GetIndString(theString, kAboutStrings, 5);
	MoveTo(10, 105);
	DrawString(theString);
	
	// version, on the right
	GetIndString(theString, kAboutStrings, 6);
        MoveTo((GetWindowPortBounds(pWindow, &bounds)->right - 10) - StringWidth(theString), 105);
	DrawString(theString);
	
	return noErr;
	
} // AboutUpdateWindow

// --------------------------------------------------------------------------------------------------------------

static OSStatus	AboutGetBalloon(WindowPtr pWindow, WindowDataPtr pData, 
		Point *localMouse, short * returnedBalloonIndex, Rect *returnedRectangle)
{
#pragma unused (pWindow, pData, localMouse, returnedRectangle)

	*returnedBalloonIndex = iNoBalloon;
	
	return noErr;
	
} // AboutGetBalloon

// --------------------------------------------------------------------------------------------------------------

static OSStatus	AboutKeyEvent(WindowPtr pWindow, WindowDataPtr pData, EventRecord *pEvent, Boolean isMotionKey)
{
	#pragma unused(pWindow, pData, pEvent, isMotionKey)

	return noErr;

} // AboutKeyEvent

// --------------------------------------------------------------------------------------------------------------

static OSStatus	AboutMakeWindow(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow)

	pData->pUpdateWindow = (UpdateWindowProc)	AboutUpdateWindow;
	pData->pGetBalloon	 = (GetBalloonProc)		AboutGetBalloon;
	pData->pKeyEvent	 = (KeyEventProc)		AboutKeyEvent;

	return noErr;
	
} // AboutMakeWindow

// --------------------------------------------------------------------------------------------------------------

OSStatus	AboutPreflightWindow(PreflightPtr pPreflightData)
{
	pPreflightData->resourceID 		= kAboutWindowID;
	pPreflightData->continueWithOpen 	= true;
	pPreflightData->makeProcPtr 		= (MakeWindowProc) AboutMakeWindow;
	
	return noErr;
	
} // AboutPreflightWindow

// --------------------------------------------------------------------------------------------------------------

void AboutGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes)
{
#pragma unused (pFileTypes, pDocumentTypes, numTypes)

} // AboutGetFileTypes
