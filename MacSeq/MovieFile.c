/*
	File:		MovieFile.c

	Contains:	Movie file support for simple text application

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
#include "SimpleText.h"
#include "MovieFile.h"

// --------------------------------------------------------------------------------------------------------------
static void DoFindInMovie(Movie theMovie, long *searchOffset, Boolean goBackwards)
{
	Track	searchTrack = nil;
	OSStatus	anErr;
	
	if (goBackwards)
		(*searchOffset)--;
	else
		(*searchOffset)++;

	anErr = MovieSearchText(theMovie, (Ptr)&gFindString[1], gFindString[0],
				(gWrapAround ? findTextWrapAround : 0) |
				(gCaseSensitive ? findTextCaseSensitive : 0) |
				findTextUseOffset |
				(goBackwards ? findTextReverseSearch : 0),
				&searchTrack, (TimeValue*)nil, searchOffset);
				
	if (anErr)
		SysBeep(1);
} // DoFindInMovie

// --------------------------------------------------------------------------------------------------------------

static OSStatus	MovieAdjustMenus(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow)

	if (GetMovieIndTrackType( ((MovieDataPtr)pData)->theMovie, 1, 'text', movieTrackCharacteristic) != nil)
	{
		EnableCommand(cFind);
		if (gFindString[0] != 0)
			EnableCommand(cFindAgain);
	}

	return noErr;
	
} // MovieAdjustMenus

// --------------------------------------------------------------------------------------------------------------

static OSStatus	MovieCommand(WindowPtr pWindow, WindowDataPtr pData, short commandID, long menuResult)
{
#pragma unused (pWindow, menuResult)

	OSStatus	anErr = noErr;
	
	switch (commandID)
	{
		case cFind:
			if (ConductFindOrReplaceDialog(kFindWindowID) == cancel)
				break;
		
		case cFindAgain:
			DoFindInMovie(((MovieDataPtr)pData)->theMovie, 
						&((MovieDataPtr)pData)->searchOffset,
						((gEvent.modifiers & shiftKey) != 0));
			break;
	}
	
	return(anErr);
	
} // MovieCommand

// --------------------------------------------------------------------------------------------------------------

static OSStatus	MovieCloseWindow(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow)

	DisposeMovieController( ((MovieDataPtr)pData)->thePlayer);
	DisposeMovie( ((MovieDataPtr)pData)->theMovie);
	CloseMovieFile(pData->resRefNum);
	pData->resRefNum = -1;
	
	return(noErr);
	
} // MovieCloseWindow

// --------------------------------------------------------------------------------------------------------------
static OSStatus MovieAdjustCursor(WindowPtr pWindow, WindowDataPtr pData, Point *localMouse, RgnHandle globalRgn)
{
#pragma unused (pWindow, pData, globalRgn, localMouse)
	
	return(eActionAlreadyHandled);
	
} // MovieAdjustCursor

// --------------------------------------------------------------------------------------------------------------

static OSStatus	MovieGetBalloon(WindowPtr pWindow, WindowDataPtr pData, 
		Point *localMouse, short * returnedBalloonIndex, Rect *returnedRectangle)
{
#pragma unused (pWindow, pData, localMouse, returnedRectangle)

	*returnedBalloonIndex = iDidTheBalloon;
	
	return(noErr);
	
} // MovieGetBalloon

// --------------------------------------------------------------------------------------------------------------

static Boolean	MovieFilterEvent(WindowPtr pWindow, WindowDataPtr pData, EventRecord *pEvent)
{
#pragma unused (pWindow)
	
	return(MCIsPlayerEvent( ((MovieDataPtr)pData)->thePlayer, pEvent));
} // MovieFilterEvent

// --------------------------------------------------------------------------------------------------------------

static long MovieCalculateIdleTime(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow, pData)

	if (!IsMovieDone( ((MovieDataPtr)pData)->theMovie))
		return(0);
	else
		return(0x7FFFFFFF);
		
} // MovieCalculateIdleTime

// --------------------------------------------------------------------------------------------------------------

static OSStatus	MovieMakeWindow(WindowPtr pWindow, WindowDataPtr pData)
{
	OSStatus		anErr;
	short			actualResId;
	Movie			theMovie;
	MovieController		thePlayer;
	Rect			movieBounds;
	long			version;
	FSSpec			fileSpec;
	
	Gestalt(gestaltQuickTime, &version);
	
	pData->pAdjustMenus		= (AdjustMenusProc)		MovieAdjustMenus;
	pData->pCommand			= (CommandProc)			MovieCommand;
	pData->pCloseWindow 		= (CloseWindowProc)		MovieCloseWindow;
	pData->pFilterEvent 		= (FilterEventProc)		MovieFilterEvent;
	pData->pGetBalloon 		= (GetBalloonProc)		MovieGetBalloon;
	pData->pCalculateIdleTime	= (CalculateIdleTimeProc)	MovieCalculateIdleTime;
	pData->pAdjustCursor		= (AdjustCursorProc)		MovieAdjustCursor;
	pData->dragWindowAligned	= (version >= 0x01508000);
	
	actualResId = DoTheRightThing;

	// close down other paths to the file -- because we don't need them
	if (pData->resRefNum != -1)
	{
		CloseResFile(pData->resRefNum);
		pData->resRefNum = -1;
	}
	if (pData->dataRefNum != -1)
	{
		FSClose(pData->dataRefNum);
		pData->dataRefNum = -1;
	}

	anErr = FSGetCatalogInfo( &pData->fileRef, kFSCatInfoNone, NULL, NULL, &fileSpec, NULL );
	anErr = OpenMovieFile(&fileSpec, &pData->resRefNum, 0);
	if (anErr == noErr)
	{
		anErr = NewMovieFromFile(&theMovie, pData->resRefNum, &actualResId, (unsigned char *) 0, newMovieActive, (Boolean *) 0);
		if (anErr == noErr)
			anErr = GetMoviesError();
		
		// leave slop because some movies/VR don't work with low mem, but eat it anyway
		if (FreeMem() < kRAMNeededForNew)
		{
			DisposeMovie(theMovie);
			anErr = memFullErr;
		}
	}
	else
	{
		pData->resRefNum = -1;
	}
	nrequire(anErr, NewMovieFromFile);
	
	// position the movie
	GetMovieBox(theMovie, &movieBounds);
	OffsetRect(&movieBounds, -movieBounds.left, -movieBounds.top);
	SetMovieBox(theMovie, &movieBounds);
	OffsetRect(&movieBounds, pData->contentRect.left, pData->contentRect.top);

	// make it draw in the correct window
	SetMovieGWorld(theMovie, GetWindowPort(pWindow), 0);
	thePlayer = NewMovieController(theMovie, &movieBounds, mcTopLeftMovie);
	anErr = GetMoviesError();
	if ((anErr == noErr) && (!thePlayer)) anErr = memFullErr;
	nrequire(anErr, NewMovieFromFile);
	MCGetControllerBoundsRect(thePlayer, &movieBounds);

	// make sure the window is the proper size
	SizeWindow(pWindow, movieBounds.right - movieBounds.left,
							movieBounds.bottom - movieBounds.top, false);
	pData->contentRect.right = pData->contentRect.left + 
		movieBounds.right - movieBounds.left;
	pData->contentRect.bottom = pData->contentRect.top + 
		movieBounds.bottom - movieBounds.top;
	if (pData->dragWindowAligned)
		AlignWindow((WindowPtr)pWindow, false, nil, nil);
	
	// enable keyboard events
	MCDoAction(thePlayer, mcActionSetKeysEnabled, (void*)1);
	
	// Save a reference to the movie
	((MovieDataPtr)pData)->theMovie = theMovie;
	((MovieDataPtr)pData)->thePlayer = thePlayer;
	((MovieDataPtr)pData)->searchOffset = 0;
	
// FALL THROUGH EXCEPTION HANDLING
NewMovieFromFile:
	return(anErr);
} // MovieMakeWindow

// --------------------------------------------------------------------------------------------------------------

OSStatus	MoviePreflightWindow(PreflightPtr pPreflightData)
{	
	pPreflightData->continueWithOpen 	= true;
	pPreflightData->makeProcPtr 		= MovieMakeWindow;
	pPreflightData->resourceID			= kMovieWindowID;
	pPreflightData->storageSize 		= sizeof(MovieDataRecord);

	return(noErr);
	
} // MoviePreflightWindow

// --------------------------------------------------------------------------------------------------------------

void MovieGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes)
{
	if (gMachineInfo.haveQuickTime)
	{
		pFileTypes[*numTypes] 		= 'MooV';
		pDocumentTypes[*numTypes] 	= kMovieWindow;
		(*numTypes)++;
	}
} // MovieGetFileTypes
