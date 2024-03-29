/*
	File:		TextFile.c

	Contains:	Text file support for simple text application.

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

#include "TextFile.h"
#include "MacSeq.h"

// --------------------------------------------------------------------------------------------------------------
// INTERNAL DEFINES
// --------------------------------------------------------------------------------------------------------------
#define kOnePageWidth 		600		// preferred width of a window
#define kMargins		4		// margins in window
#define kPrintMargins		8		// margins in printing window

#define kPictureBase		1000		// resource base ID for PICTs in documents
#define kSoundBase		10000		// resource base ID for 'snd 's in documents

// resources for controling printing
#define kFormResource		'form'
#define kFormFeed		0x00000001	// form feed after this line

#define	kContentsListID		10000		// resource ID for STR# contents menu 		

// some memory requirements (6 times bigger buffer in case siGoodQuality did not do 6:1 MACE compression)
#define kPrefBufferSize		6*90*1024
#define kMinBufferSize		6*5*1024

#define kDeleteKey		8
#define kForwardDeleteKey	0x75


extern pascal unsigned char * AsmClikLoop(TEPtr pTE);

#define ABS(n)				(((n) < 0) ? -(n) : (n))

// --------------------------------------------------------------------------------------------------------------
// GLOBALS USED ONLY BY THESE ROUTINES
// --------------------------------------------------------------------------------------------------------------

// These variables are used for speech, notice that on purpose we only
// support a single channel at a time.
static SpeechChannel	gSpeechChannel = nil;		
static VoiceSpec	gCurrentVoice;
static Ptr		gSpeakPtr = nil;
static Boolean		gAddedVoices = false;

static Str31		gPictMarker1, gPictMarker2;

// Other globals local to this file (and TextDrag.c)
Boolean					gTE6Version = false;

// External globals for AppleScript
extern	ComponentInstance gOSAComponent;

// --------------------------------------------------------------------------------------------------------------
// EXTERNAL FUNCTIONS
// --------------------------------------------------------------------------------------------------------------

extern OSStatus	TextDragTracking(WindowPtr pWindow, void *pData, DragReference theDragRef, short message);
extern OSStatus	TextDragReceive(WindowPtr pWindow, void *refCon, DragReference theDragRef);
extern Boolean	DragText(WindowPtr pWindow, void *pData, EventRecord *pEvent, RgnHandle hilightRgn);

// --------------------------------------------------------------------------------------------------------------
// INTERNAL ROUTINES
// --------------------------------------------------------------------------------------------------------------

static void TextAddContentsMenu(WindowDataPtr pData);
static void TextRemoveContentsMenu(WindowDataPtr pData);
static OSStatus TextGetContentsListItem(WindowDataPtr pData, short itemNum, StringPtr menuStr, StringPtr searchStr, short *totalItems);
static OSStatus TextAdjustMenus(WindowPtr pWindow, WindowDataPtr pData);
static OSStatus TextAdjustContentsMenu(WindowDataPtr pData);
static void TextAddVoices();

static StScrpHandle	GetDocumentStyleScrapHandle (TEHandle inTEHandle)
{
	// save the new style -- get the scrap handle from the TE record
	// To do this, we must select the text -- BUT doing so through the
	// TextEdit API results in lots of flashing and annoying behavior.
	// So we just change the offsets by hand
	short		oldStart, oldEnd;
	StScrpHandle	theHandle;
	
	oldStart = (** inTEHandle).selStart;
	oldEnd   = (** inTEHandle).selEnd;
	
	(** inTEHandle).selStart = 0;
	(** inTEHandle).selEnd = 32767;

	theHandle = TEGetStyleScrapHandle( inTEHandle );

	(** inTEHandle).selStart = oldStart;
	(** inTEHandle).selEnd = oldEnd;
	
	return theHandle;
}

static void	UseStyleScrapForDocument (StScrpHandle inStyle, Boolean inUpdate, TEHandle inTEHandle )
{
	TEUseStyleScrap(0, 32767, inStyle, inUpdate, inTEHandle);
}

// --------------------------------------------------------------------------------------------------------------
// UNDO UTILITY FUNCTIONS
// --------------------------------------------------------------------------------------------------------------
OSStatus SaveCurrentUndoState(WindowDataPtr pData, short newCommandID)
{
	OSStatus		anErr = noErr;
	TEHandle	hTE = ((TextDataPtr) pData)->hTE;
	
	// this is only a new command if:
	//	the command ID is different
	// OR
	//	the selection is a range, and not equal to the old one
	if 	(
			( ((TextDataPtr) pData)->prevCommandID != newCommandID )
		||
			(
				( (**hTE).selStart != (**hTE).selEnd )
			||
				( ABS((**hTE).selStart - ((TextDataPtr) pData)->prevSelStart) > 1 )
			)
		)
	{
		Handle		tempHandle;
		Size		tempSize;
		
		// if we don't have save handle, make it!
		if (!((TextDataPtr) pData)->prevText)
		{
			((TextDataPtr) pData)->prevText = NewHandle(0);
			anErr = MemError();
			nrequire(anErr, MakeTextSave);
		}
		// if we have style recond allocated already - remove it
		if  ( ((TextDataPtr) pData)->prevStyle ) 
		{
			DisposeHandle((Handle)((TextDataPtr) pData)->prevStyle);
			anErr = MemError();
			nrequire(anErr, MakeStyleSave);
		}
			
		// grow the save handles and block move into them
		tempHandle = (**hTE).hText;
		tempSize = GetHandleSize(tempHandle);
		SetHandleSize( ((TextDataPtr) pData)->prevText, tempSize);
		anErr = MemError();
		nrequire(anErr, GrowTextSave);
		BlockMoveData(*tempHandle, * ((TextDataPtr) pData)->prevText, tempSize );
		
		((TextDataPtr) pData)->prevStyle =  GetDocumentStyleScrapHandle(hTE);

		// save length
		((TextDataPtr) pData)->prevLength = (**hTE).teLength;
		((TextDataPtr) pData)->beforeSelStart = (**hTE).selStart;
		((TextDataPtr) pData)->beforeSelEnd = (**hTE).selEnd;
	}
	
	// save start and end of the selection
	((TextDataPtr) pData)->prevSelStart = (**hTE).selStart;

	// if we didn't have any problems, then we can undo this operation
	((TextDataPtr) pData)->prevCommandID = newCommandID;
	return(noErr);
	
// EXCEPTION HANDLING
GrowTextSave:
MakeStyleSave:
MakeTextSave:
	// can't undo because of an error 
	// (at some point might warn the user, but that's probably more annoying)
	((TextDataPtr) pData)->prevCommandID = cNull;
	
	return(anErr);
	
} // SaveCurrentUndoState

// --------------------------------------------------------------------------------------------------------------
static void PerformUndo(WindowDataPtr pData)
{
	if (((TextDataPtr) pData)->prevCommandID != cNull)
	{
		TEHandle	hTE = ((TextDataPtr) pData)->hTE;
		long		tempLong;

		// undo text
		tempLong = (long) (**hTE).hText;
		(**hTE).hText = ((TextDataPtr) pData)->prevText;
		((TextDataPtr) pData)->prevText = (Handle)tempLong;

		// undo length
		tempLong = (long) (**hTE).teLength;
		(**hTE).teLength = ((TextDataPtr) pData)->prevLength;
		((TextDataPtr) pData)->prevLength = tempLong;

		// undo style
		tempLong = (long) GetDocumentStyleScrapHandle(hTE);
		UseStyleScrapForDocument( ((TextDataPtr) pData)->prevStyle, false, hTE);
		DisposeHandle((Handle)((TextDataPtr) pData)->prevStyle);
		((TextDataPtr) pData)->prevStyle = (StScrpHandle)tempLong;
		
		// undo selection
		{
			short	start, end;
			
			start = ((TextDataPtr) pData)->beforeSelStart;
			end = ((TextDataPtr) pData)->beforeSelEnd;
			TESetSelect(start, end, hTE);
		}
	}
		
} // PerformUndo


// --------------------------------------------------------------------------------------------------------------
// TEXT EDIT UTILITY FUNCTIONS
// --------------------------------------------------------------------------------------------------------------
static long CalculateTextEditHeight(TEHandle hTE)
{
	long	result;
	short	length;
	
	result = TEGetHeight(32767, 0, hTE);
	length = (**hTE).teLength;
	
	// Text Edit doesn't return the height of the last character, if that
	// character is a <cr>.  So if we see that, we go grab the height of
	// that last character and add it into the total height.
	if ( (length) && ( (*(**hTE).hText)[length-1] == '\r') )
	{
		TextStyle	theStyle;
		short		theHeight;
		short		theAscent;

		TEGetStyle(length, &theStyle, &theHeight, &theAscent, hTE);
		result += theHeight;
	}

	return result;

} // CalculateTextEditHeight

// --------------------------------------------------------------------------------------------------------------
void AdjustTE(WindowDataPtr pData, Boolean doScroll)
{
	TEPtr		te;
	short		value;
	short		prevValue;
	
	te = *((TextDataPtr) pData)->hTE;
	prevValue = GetControlValue(pData->vScroll);
	value = te->viewRect.top - te->destRect.top;
	SetControlValue(pData->vScroll, value);

	te = *((TextDataPtr) pData)->hTE;
	if (doScroll)
	{
		short	hScroll = te->viewRect.left - te->destRect.left;
		short	vScroll = value - prevValue;
		if ((hScroll != 0) || (vScroll != 0))
			TEScroll(hScroll, vScroll, ((TextDataPtr) pData)->hTE);
	}
	
			
} // AdjustTE

// --------------------------------------------------------------------------------------------------------------

static void RecalcTE(WindowDataPtr pData, Boolean doInval)
{
	Rect	viewRect, destRect;
	short	oldOffset;
	
	destRect = (**((TextDataPtr) pData)->hTE).destRect;
	viewRect = (**((TextDataPtr) pData)->hTE).viewRect;
	oldOffset = destRect.top - viewRect.top;
	
	viewRect = pData->contentRect;

	InsetRect(&viewRect, kMargins, kMargins);
	destRect = viewRect;
	
	OffsetRect(&destRect, 0, oldOffset);
	
	(**((TextDataPtr) pData)->hTE).viewRect = viewRect;
	(**((TextDataPtr) pData)->hTE).destRect = destRect;
	
	TECalText(((TextDataPtr) pData)->hTE);

	if (doInval)
	{
		WindowPtr window = FrontWindow();
		if ( window )
		{
			Rect		rect;

			GetPortBounds( GetWindowPort( window ), &rect );
			InvalWindowRect( window, &rect );
		}
	}
} // RecalcTE

// --------------------------------------------------------------------------------------------------------------

pascal TEClickLoopUPP GetOldClickLoop(void)
{
// AEC, changed to obtain application data from the WRefCon
     TextDataPtr    textData;

     textData = (TextDataPtr) GetWRefCon(GetWindowFromPort(GetQDGlobalsThePort()));
     return textData->docClick;     
} // GetOldClikLoop

pascal void TextClickLoop(void)
{	
	WindowPtr	window;
	RgnHandle	region;
	Rect		bounds;
	TextDataPtr	textData;

	window = FrontWindow();
	region = NewRgn();
	GetClip(region);					/* save clip */
	ClipRect(GetWindowPortBounds(window, &bounds));
	
// AEC, changed to obtain application data from the WRefCon
    textData = (TextDataPtr) GetWRefCon(window);
     
	textData->insideClickLoop = true;
		AdjustScrollBars(window, false, false, nil);
	textData->insideClickLoop = false;

	SetClip(region);					/* restore clip */
	DisposeRgn(region);

} // TextClickLoop

static pascal Boolean CClikLoop(TEPtr pTE)
{
	InvokeTEClickLoopUPP(pTE, GetOldClickLoop());
	
	TextClickLoop();
	
	return(true);
	
} // CClikLoop

// --------------------------------------------------------------------------------------------------------------
pascal void MyDrawHook ( unsigned short offset, unsigned short textLen,
	 Ptr textPtr, TEPtr tePtr, TEHandle teHdl )
{
#pragma unused	( teHdl, tePtr)

	register short			drawLen = 0;
	register Ptr			drawPtr;
	
	drawPtr = textPtr + (long)offset;
	
	while ( textLen--)
	{
		if ( *drawPtr == (char)0xCA &&
			 ( *(drawPtr - 1) == 0x0D || *tePtr->hText == textPtr) )
		{
			DrawText( textPtr, offset, drawLen);
			DrawChar( '\040');
			offset += drawLen + 1;
			drawLen = 0;
		}
		else
		{
			++drawLen;
		}
		++drawPtr;
	}
	
	DrawText( textPtr, offset, drawLen);
	
} // MyDrawHook


// --------------------------------------------------------------------------------------------------------------

static void DisposeOfSpeech(Boolean throwAway)
{
	if (gSpeechChannel)
	{
		(void) StopSpeech( gSpeechChannel );
		if (throwAway)
		{
			(void) DisposeSpeechChannel( gSpeechChannel );
			gSpeechChannel = nil;
		}
	}
	if (gSpeakPtr)
	{
		DisposePtr(gSpeakPtr);
		gSpeakPtr = nil;
	}
		
} // DisposeOfSpeech

// --------------------------------------------------------------------------------------------------------------

static Boolean FindNextPicture(Handle textHandle, short *offset, short *delta)
{
	long result;
	
	// marker at begining of document means a picture there
	if ( (*offset == 0) && ( (*(unsigned char*)(*textHandle + (*offset))) == 0xCA) )
	{
		*delta = 1;
		return true;
	}

	if (gPictMarker1[0] != 0)
	{
		result = Munger(textHandle, *offset, &gPictMarker1[1], gPictMarker1[0], nil, 0);
		if (result >= 0)
		{
			*offset = result;
			*delta = gPictMarker1[0];
			return true;
		}
	}

	if (gPictMarker2[0] != 0)
	{
		result = Munger(textHandle, *offset, &gPictMarker2[1], gPictMarker2[0], nil, 0);
		if (result >= 0)
		{
			*offset = result;
			*delta = gPictMarker2[0];
			return true;
		}
	}
		
	*delta = 1;
	return false;
	
} // FindNextPicture

// --------------------------------------------------------------------------------------------------------------
static Boolean LineHasPageBreak(short lineNum, TEHandle hTE)
{
	Boolean		result = false;
	short		offset, delta, lineStartOffset;
	short		textLength;
	Handle		textHandle;
	short		pictIndex = 0;
	
	textHandle = (** hTE).hText;
	lineStartOffset = (**hTE).lineStarts[lineNum];
	textLength = (**hTE).lineStarts[lineNum+1];
	
	offset = 0;
	while (offset < textLength)
	{
		if (FindNextPicture(textHandle, &offset, &delta))
		{
			Handle	formHandle;
			
			formHandle = Get1Resource(kFormResource, kFormResource + pictIndex);
			if (formHandle)
			{
				long	options = **(long**)formHandle;
				
				ReleaseResource(formHandle);
				if ( (options & kFormFeed) && (offset >= lineStartOffset) && (offset < textLength) )
				{
					result = true;
					break;
				}
			}
			++pictIndex;
		}
		
		offset += delta;
	}
		
	return(result);
	
} // LineHasPageBreak

// --------------------------------------------------------------------------------------------------------------
#define USE_PICT_SPOOL_CACHE 1

enum {kSpoolCacheBlockSize = 1024};		// 1K is arbitrary, perhaps could be tuned

static Handle	gSpoolPicture;
static long		gSpoolOffset;
static Handle	gSpoolCacheBlock;
static long		gSpoolCacheOffset;
static long		gSpoolCacheSize;

// --------------------------------------------------------------------------------------------------------------
/*
	ReadPartialResource is very painful over slow links such as ARA or ISDN. This code implements a simple
	cache that vastly improves the performance of displaying embedded pictures over a slow network link.
	The cache also improves scrolling performance in documents with many embedded pictures, even on local
	volumes.
*/
static short ReadPartialData(Ptr dataPtr, short byteCount)
{
#if USE_PICT_SPOOL_CACHE

	if (gSpoolCacheBlock == NULL)
	{
		gSpoolCacheBlock = NewHandle(kSpoolCacheBlockSize);
		if (gSpoolCacheBlock != NULL)
		{
			long	cacheBytes	= GetResourceSizeOnDisk(gSpoolPicture) - gSpoolOffset;
			
			if (cacheBytes > kSpoolCacheBlockSize)
				cacheBytes = kSpoolCacheBlockSize;
			
			HLock(gSpoolCacheBlock);
			ReadPartialResource(gSpoolPicture, gSpoolOffset, *gSpoolCacheBlock, cacheBytes);
			HUnlock(gSpoolCacheBlock);
			
			gSpoolCacheOffset = gSpoolOffset;
			gSpoolCacheSize = cacheBytes;
		}
	}
	
	// if the requested data is entirely present in the cache block, get it from there�
	if (gSpoolCacheBlock != NULL &&
		gSpoolOffset >= gSpoolCacheOffset && gSpoolOffset + byteCount <= gSpoolCacheOffset + gSpoolCacheSize)
	{
		BlockMoveData(*gSpoolCacheBlock + (gSpoolOffset - gSpoolCacheOffset), dataPtr, byteCount);
		return byteCount;
	}
	// �else read it directly from disk, and force the cache block to be filled with new data
	else
	{
		ReadPartialResource(gSpoolPicture, gSpoolOffset, dataPtr, byteCount);
		if (gSpoolCacheBlock != NULL)
		{
			DisposeHandle(gSpoolCacheBlock);
			gSpoolCacheBlock = NULL;
		}
		return byteCount;
	}

#else

	ReadPartialResource(gSpoolPicture, gSpoolOffset, dataPtr, byteCount);
	return byteCount;
	
#endif
	
} // ReadPartialData

// --------------------------------------------------------------------------------------------------------------
static pascal void GetPartialPICTData(void* dataPtr,short byteCount)
{
	while (byteCount > 0)
	{
		short	readBytes	= ReadPartialData(dataPtr, byteCount);
		
		byteCount -= readBytes;
		dataPtr += readBytes;
		
		gSpoolOffset += readBytes;
	}
	
} // GetPartialPICTData

// --------------------------------------------------------------------------------------------------------------
static void SpoolDrawPicture(Handle spoolPicture, PicHandle pictureHeader, Rect *pRect)
{
    static QDGetPicUPP gGetPartialPICTData = NULL;
    CQDProcs	spoolProcs, *oldProcsPtr;
    CGrafPtr	thePort = GetQDGlobalsThePort();

    if (!gGetPartialPICTData)
    {
         gGetPartialPICTData = NewQDGetPicUPP(GetPartialPICTData);
    }

    gSpoolPicture = spoolPicture;
    gSpoolOffset = sizeof(Picture);

#if 0
    // 4-12-00 lhotak: this would ignore printing bottlenecks.
    // Indeed, no images will ever print with this code.

//	if (!GetPortGrafProcs(thePort, &spoolProcs))
        SetStdCProcs(&spoolProcs);
#else
    // Port's procs will be NULL if never set (window ports).
    // We need to check for this case and make use of StdProcs.
    // Otherwise the procs (printing ports) need to be saved
    // and restored after drawing.
    oldProcsPtr = GetPortGrafProcs(thePort);
    if (oldProcsPtr != NULL)
        spoolProcs = *oldProcsPtr;
    else
        SetStdCProcs(&spoolProcs);
#endif

    // Use custom getPicProc for our picture drawing:
    spoolProcs.getPicProc = gGetPartialPICTData;
    SetPortGrafProcs(thePort, &spoolProcs);

    // Draw the picture and restore old bottlenecks:
    DrawPicture(pictureHeader, pRect);
    SetPortGrafProcs(thePort, oldProcsPtr);

    if (gSpoolCacheBlock != NULL)
    {
        DisposeHandle(gSpoolCacheBlock);
        gSpoolCacheBlock = NULL;
    }
} // SpoolDrawPicture

/*	static QDGetPicUPP gGetPartialPICTData = NULL;
#if USE_QD_GRAFPROCS
	CQDProcs	spoolProcs;
	CQDProcs	oldProcs;
	CGrafPtr	thePort = GetQDGlobalsThePort();
#endif

	if (!gGetPartialPICTData) {
		gGetPartialPICTData = NewQDGetPicUPP(GetPartialPICTData);
	}

	gSpoolPicture = spoolPicture;
	gSpoolOffset = sizeof(Picture);
	
#if USE_QD_GRAFPROCS
	if (!GetPortGrafProcs(thePort, &spoolProcs))
		SetStdCProcs(&spoolProcs);
		
	spoolProcs.getPicProc = gGetPartialPICTData;
	GetPortGrafProcs(thePort, &oldProcs);
	SetPortGrafProcs(thePort, &spoolProcs);
#endif
	DrawPicture(pictureHeader, pRect);
#if USE_QD_GRAFPROCS
	SetPortGrafProcs(thePort, &oldProcs);
#endif
	
	if (gSpoolCacheBlock != NULL)
		{
		DisposeHandle(gSpoolCacheBlock);
		gSpoolCacheBlock = NULL;
		}
	
} // SpoolDrawPicture*/

// --------------------------------------------------------------------------------------------------------------
static void DrawPictures( WindowDataPtr pData, TEHandle hTE)
{
	Handle	textHandle;
	long	textLength;
	short	oldResFile;
	short	pictIndex;
	short	numPicts;
	
	if (pData->resRefNum == -1)
		return;
		
	oldResFile = CurResFile();
	UseResFile(pData->resRefNum);
	
	numPicts = Count1Resources('PICT');
	pictIndex = 0;
	
	if (numPicts != 0)
	{
		short				offset, delta;
		RgnHandle			oldClip;
		Rect				theRect;
		Rect				viewRect;
		
		viewRect = theRect = (** hTE).viewRect;
		// intersect our viewing area with the actual clip to avoid
		// drawing over the scroll bars
		{
			RgnHandle	newClip = NewRgn();
			
			oldClip = NewRgn();
			GetClip(oldClip);
			RectRgn(newClip, &theRect);
			SectRgn(oldClip, newClip, newClip);
			SetClip(newClip);
                        DisposeRgn(newClip);
		}
		textHandle = (** hTE).hText;
		textLength = (** hTE).teLength;
		
		offset = 0;
		while (offset < textLength)
		{
			if (FindNextPicture(textHandle, &offset, &delta))
			{
				Handle		pictHandle;
				Point		picturePoint = TEGetPoint(offset, hTE);
				
				SetResLoad(false);
				pictHandle = Get1Resource('PICT', kPictureBase + pictIndex);
				SetResLoad(true);
				if (pictHandle)
				{
					PicHandle	pictHeader = (PicHandle)NewHandle(sizeof(Picture) + sizeof(long)*8);
					
					if (pictHeader)
					{
						HLock((Handle) pictHeader);
						ReadPartialResource(pictHandle, 0, (Ptr)*pictHeader, GetHandleSize((Handle)pictHeader));
						HUnlock((Handle) pictHeader);
						
						// calculate where to draw the picture, this location is
						// computed by:
						//  1) the frame of the original picture, normalized to 0,0
						//	2) the location of the non-breaking space character
						//	3) centering the picture on the content frame horizontally
						//	4) subtracting off the line-height of the character
						
						GetPICTRectangleAt72dpi(pictHeader, &theRect);
						OffsetRect(&theRect, -theRect.left, -theRect.top);
						OffsetRect(&theRect, 
											theRect.left +
											((viewRect.right - viewRect.left) >> 1) -
											((theRect.right - theRect.left) >> 1),
										picturePoint.v-theRect.top - pData->vScrollAmount);
	
						// only draw the picture if it will be visible (vastly improves scrolling
						// performance in documents with many embedded pictures)
						
						//	if (RectInRgn(&theRect, GetQDthePort()->clipRgn))
							SpoolDrawPicture(pictHandle, pictHeader, &theRect);
						DisposeHandle((Handle) pictHeader);
					}
					ReleaseResource((Handle) pictHandle);
				}
				++pictIndex;

				offset += delta;
			}
			else
				break;
		}
				
		SetClip(oldClip);
		DisposeRgn(oldClip);
	}
		
	UseResFile(oldResFile);
	
} // DrawPictures

// --------------------------------------------------------------------------------------------------------------
static void UpdateFileInfo( FSRefPtr pRef, Boolean isStationery )
{
	FInfo*		theInfo;
	FSCatalogInfo	theCatInfo;
	
	FSGetCatalogInfo (pRef, kFSCatInfoFinderInfo, &theCatInfo, NULL, NULL, NULL);
	theInfo = (FInfo*)&theCatInfo.finderInfo;
	theInfo->fdCreator = 'ttxt';
	theInfo->fdType = 'TEXT';

	// set the stationary bit, if we must
	if ( isStationery )
		theInfo->fdFlags |= kIsStationery;
	else
		theInfo->fdFlags &= ~kIsStationery;
	FSSetCatalogInfo (pRef, kFSCatInfoFinderInfo, &theCatInfo);

} // UpdateFileInfo

// --------------------------------------------------------------------------------------------------------------
static OSStatus	TextSave(WindowDataPtr pData, Boolean resetStationary)
{
	OSStatus	anErr;
	long		amountToWrite;
	int		i;
	Ptr		data;
	
	anErr = noErr;
    
	// write out the text
	SetFPos(pData->dataRefNum, fsFromStart, 0);
	amountToWrite = (** ((TextDataPtr) pData)->hTE).teLength;
	data = * (** ((TextDataPtr) pData)->hTE).hText;

	if ( ((TextDataPtr) pData)->saveWithLF )
	{
		for (i=0; i < amountToWrite; i++)
		{
			if (data[i] == '\r')
				data[i] = '\n';
		}
	}

	anErr = FSWrite(pData->dataRefNum, &amountToWrite, data);
	amountToWrite = (** ((TextDataPtr) pData)->hTE).teLength;
	data = * (** ((TextDataPtr) pData)->hTE).hText;

	for (i=0; i < amountToWrite; i++)
	{
		if (data[i] == '\n')
			data[i] = '\r';
	}
	
	nrequire(anErr, FailedWrite);

	SetEOF(pData->dataRefNum, amountToWrite);
	
	if (pData->resRefNum == -1)
	{
		// we use FSpCreateResFile to create res file for now
		FSSpec	fileSpec;
		
		anErr = FSGetCatalogInfo( &pData->fileRef, kFSCatInfoNone, NULL, NULL, &fileSpec, NULL );
		nrequire(anErr, FailedWrite);
		FSpCreateResFile(&fileSpec, 'ttxt', pData->originalFileType, 0);
		pData->resRefNum = FSOpenResFile(&pData->fileRef, fsRdWrPerm );			
	}
	else
	{
		// a save always makes it into file of type 'TEXT', for Save As� we 
		// afterwards set it again if the user is saving as stationary.
		if (resetStationary)
			UpdateFileInfo(&pData->fileRef, false);
	}

	if (pData->resRefNum != -1)
	{
		short	oldResFile = CurResFile();
		Handle	resourceHandle;
		
		UseResFile(pData->resRefNum);
		
		// remove any old sounds
		resourceHandle = Get1Resource('snd ', kSoundBase);
		if (resourceHandle)
		{
			RemoveResource(resourceHandle);
			DisposeHandle(resourceHandle);
		}
			
		// save the new sound
		resourceHandle = ((TextDataPtr) pData)->soundHandle;
		if (resourceHandle)
		{
			anErr = HandToHand(&resourceHandle);
			if (anErr == noErr)
			{
				AddResource(resourceHandle, 'snd ', kSoundBase, "\p");
				anErr = ResError();
			}
			nrequire(anErr, AddSoundResourceFailed);
		}
			
		// remove any old styles
		resourceHandle = Get1Resource('styl', 128);
		if (resourceHandle)
		{
			RemoveResource(resourceHandle);
			DisposeHandle(resourceHandle);
		}
			
		resourceHandle = (Handle) GetDocumentStyleScrapHandle( ((TextDataPtr) pData)->hTE );

		if (resourceHandle)
		{
			AddResource(resourceHandle, 'styl', 128, "\p");
			anErr = ResError();
			nrequire(anErr, AddStyleResourceFailed);
		}
			
		AddSoundResourceFailed:
		AddStyleResourceFailed:	
		
			UpdateResFile(pData->resRefNum);
			UseResFile(oldResFile);
	}

		
// FALL THROUGH EXCEPTION HANDLING
FailedWrite:

	// if everything went okay, then clear the changed bit
	if (anErr == noErr)
	{
		(void) FSFlushFork(pData->dataRefNum);
		if (pData->resRefNum != -1) (void) FSFlushFork(pData->resRefNum);

		// we need not update the proxy here, since the file type never changes in this routine
		SetDocumentContentChanged( pData, false );
	}
		
	return anErr;
	
} // TextSave

// --------------------------------------------------------------------------------------------------------------
// pre and post update procs for inline input 

static pascal void TSMPreUpdateProc(TEHandle textH, long refCon)
{
#pragma unused (refCon)

	ScriptCode 	keyboardScript;
	short		mode;
	TextStyle	theStyle;

	keyboardScript = GetScriptManagerVariable(smKeyScript);
	mode = doFont;
	if 	(!
			(
			(TEContinuousStyle(&mode, &theStyle, textH) )&& 
			(FontToScript(theStyle.tsFont) == keyboardScript) 
			)
		)
	{
		theStyle.tsFont = GetScriptVariable(keyboardScript, smScriptAppFond);
		TESetStyle(doFont, &theStyle, false, textH);
	}
		
} // TSMPreUpdateProc

static pascal void TSMPostUpdateProc(
				TEHandle textH,
				long fixLen,
				long inputAreaStart,
				long inputAreaEnd,
				long pinStart,
				long pinEnd,
				long refCon)
{
#pragma unused (textH, fixLen, inputAreaStart, inputAreaEnd, pinStart, pinEnd)

	AdjustScrollBars((WindowPtr)refCon, false, false, nil);
	AdjustTE((WindowDataPtr)refCon, true);
	
	SetDocumentContentChanged( (WindowDataPtr)refCon, true );
	
} // TSMPostUpdateProc


// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextSaveAs( WindowPtr pWindow, WindowDataPtr pData )
{	
	// ask where and how to save this document

	CFStringRef	defaultName;
	Cursor		arrow;
	OSStatus	anErr;

	// setup for the call
	CopyWindowTitleAsCFString(pWindow, &defaultName);

	SetCursor(GetQDGlobalsArrow(&arrow));
	
	// in case we have attached dialog already - dispose it first
	if (pData->navDialog)
	{
		TerminateDialog( pData->navDialog );
		pData->navDialog = NULL;
	}
	
	anErr = SaveFileDialog( pWindow, defaultName, pData->originalFileType, 'ttxt', pData, &pData->navDialog );
	CFRelease(defaultName);

	return anErr;
} // TextSaveAs


// --------------------------------------------------------------------------------------------------------------
static void ApplyFace(short requestedFace, WindowPtr pWindow, WindowDataPtr pData, short commandID)
{
	TextStyle	style;
	short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;
	
	SaveCurrentUndoState(pData, commandID);
	
	style.tsFace = requestedFace;
	TESetStyle(doFace + ((requestedFace != normal) ? doToggle : 0), &style, true, ((TextDataPtr) pData)->hTE);
	TECalText(((TextDataPtr) pData)->hTE);
	
	oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
	AdjustTE(pData, false);
	AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);

	SetDocumentContentChanged( (WindowDataPtr) pData, true );	
} // ApplyFace

// --------------------------------------------------------------------------------------------------------------
static OSStatus ApplySize(short requestedSize, WindowPtr pWindow, WindowDataPtr pData, short commandID)
{
	OSStatus		anErr = noErr;
	TextStyle	style;
	short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;
	
	SaveCurrentUndoState(pData, commandID);

	style.tsSize = requestedSize;
	TESetStyle(doSize, &style, true, ((TextDataPtr) pData)->hTE);
	TECalText(((TextDataPtr) pData)->hTE);
	if (CalculateTextEditHeight(((TextDataPtr) pData)->hTE) > 32767)
	{		
		style.tsSize = 0;
		TESetStyle(doSize, &style, true, ((TextDataPtr) pData)->hTE);
		anErr = eDocumentTooLarge;
	}

	oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
	AdjustTE(pData, false);
	AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);

	SetDocumentContentChanged( (WindowDataPtr) pData, true );
	
	return anErr;

} // ApplySize

// --------------------------------------------------------------------------------------------------------------
// OOP INTERFACE ROUTINES
// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextUpdateWindow(WindowPtr pWindow, WindowDataPtr pData)
{
	Rect	updateArea = pData->contentRect;
	Rect	bounds;

	// be sure to also erase the area where the horizontal scroll bar
	// is missing.
	updateArea.bottom = GetWindowPortBounds(pWindow, &bounds)->bottom;
	EraseRect(&updateArea);
	
	TEUpdate(&pData->contentRect, ((TextDataPtr) pData)->hTE);

	DrawPictures(pData, ((TextDataPtr) pData)->hTE);
	
	DrawControls(pWindow);
	DrawGrowIcon(pWindow);
	
	return noErr;	
} // TextUpdateWindow

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextCloseWindow(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow)

	// shut down text services
	if (pData->docTSMDoc)
	{
		FixTSMDocument(pData->docTSMDoc);
		DeactivateTSMDocument(pData->docTSMDoc);
		DeleteTSMDocument(pData->docTSMDoc);
	}
	DisposeOfSpeech(true);

	DisposeHandle(((TextDataPtr) pData)->soundHandle);
	TEDispose(((TextDataPtr) pData)->hTE);

	DisposeHandle(((TextDataPtr) pData)->prevText);
	DisposeHandle((Handle)((TextDataPtr) pData)->prevStyle);
	
	TextRemoveContentsMenu(pData);

	return noErr;

} // TextCloseWindow

// --------------------------------------------------------------------------------------------------------------

static OSStatus TextSaveTo( WindowPtr pWindow, WindowDataPtr pData, FSRef *fileRef, Boolean isStationery )
{
	OSStatus	anErr = noErr;
        FSSpec		fileSpec;
        CFStringRef	pCFFileName = NULL;
	
	if (fileRef != nil)
        {
            HFSUniStr255	theFileName;
            anErr = FSGetCatalogInfo( fileRef, kFSCatInfoNone, NULL, &theFileName, &fileSpec, NULL );
            nrequire(anErr, FSGetCatalogInfo);
            
            pCFFileName = CFStringCreateWithCharacters( NULL, theFileName.unicode, theFileName.length );
        }
		
	//
	// Same file?  If so, just save.
	//
	if 	( FSCompareFSRefs (&(pData->fileRef), fileRef) == noErr )
	{
                // we are SavingAs into to the same file. So just do Save...
		anErr = TextSave(pData, true);
		
		if ( anErr == noErr )
		{
			UpdateFileInfo( fileRef, isStationery );
		}
	}
	else
	{
		short				oldRes, oldData;
		FSRef				oldFileRef;
		
		// save the old references -- if there is an error, we restore them
		oldRes				= pData->resRefNum;
		oldData				= pData->dataRefNum;
		BlockMoveData( &pData->fileRef, &oldFileRef, sizeof( FSRef ));

		// create the data file and resource fork
		FSpCreateResFile( &fileSpec, 'ttxt', 'TEXT', 0 );
		require( anErr == noErr || anErr == dupFNErr, FailedCreate);

		UpdateFileInfo( fileRef, isStationery );

		// open both of forks
		anErr = FSOpenFork( fileRef, 0, NULL, fsRdWrPerm, &pData->dataRefNum );
		if ( anErr == noErr )
		{
			pData->resRefNum = FSOpenResFile( fileRef, fsRdWrPerm );
			anErr = ResError();
		}
		nrequire(anErr, FailedOpen);

                BlockMoveData( fileRef, &pData->fileRef, sizeof( FSRef ));
		// call the standard save function to do the save
		anErr = TextSave(pData, false);

	FailedOpen:

		// finally, close the old files if everything went okay
		if ( anErr == noErr )
		{
			if (oldRes != -1)
				CloseResFile(oldRes);
			if (oldData != -1)
				FSClose(oldData);
			pData->isWritable = true;
			SetWindowTitleWithCFString(pWindow, pCFFileName);
			
			// update the window icon
			if( gMachineInfo.haveProxyIcons )
				SetWindowProxyFSSpec( pWindow, &fileSpec );
		}
		else
		{
			pData->resRefNum = oldRes;
			pData->dataRefNum = oldData;
			BlockMoveData( &oldFileRef, &pData->fileRef, sizeof( FSRef ));
		}
		
		// create alias record in WindowData
		if (pData->fileAlias) DisposeHandle( (Handle)pData->fileAlias );
		FSNewAlias( NULL, &pData->fileRef, &pData->fileAlias );
	}
	
	FailedCreate:

	// file must be closed in order to 'translate in place', so close both forks:
	if ( pData->resRefNum != -1 )
	{
		CloseResFile( pData->resRefNum );
		pData->resRefNum = -1;
	}
	if ( pData->dataRefNum != -1 )
	{
		FSClose( pData->dataRefNum );
		pData->dataRefNum = -1;
	}

		// �� problem:
		// Save As� model fails in this case because we still have the old window open,
		// Should SimpleText close the current text window and open a new one with the translated data?

		// ANSWER: keep the reply record and call NavCompleteSave every time you save the
		// file from now on. This will translate every time.  (CSL)
FSGetCatalogInfo:
        if (pCFFileName) CFRelease (pCFFileName);
	return anErr;
	
} // TextSaveTo

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextActivateEvent(WindowPtr pWindow, WindowDataPtr pData, Boolean activating)
{
#pragma unused (pWindow)

	// only modifyable docs can be active
	if (pData->windowKind == kTextWindow)
	{
		if (activating)
		{
			TEActivate(((TextDataPtr) pData)->hTE);
			if (pData->docTSMDoc != nil)
				ActivateTSMDocument(pData->docTSMDoc);
		}
		else
		{
			TEDeactivate(((TextDataPtr) pData)->hTE);
			if (pData->docTSMDoc != nil)
				DeactivateTSMDocument(pData->docTSMDoc);
		}
	}
		
	// add contents menu, if appropriate		
	if (activating)
	{
		TextAddContentsMenu(pData);
	}
	else
	{
		TextRemoveContentsMenu(pData);
	}
	
	return noErr;
	
} // TextActivateEvent

// --------------------------------------------------------------------------------------------------------------

static Boolean	TextFilterEvent(WindowPtr pWindow, WindowDataPtr pData, EventRecord *pEvent)
{	
	switch (pEvent->what)
	{
		case nullEvent:
			if (pData->windowKind == kTextWindow)
			{
				if ( pWindow == FrontWindow() )
					TEIdle(((TextDataPtr) pData)->hTE);
			}
			
			// if we stop speaking, ditch the channel
			if (gSpeechChannel) 
			{
				SpeechStatusInfo	status;				// Status of our speech channel.
				
				if ( 
					(GetSpeechInfo( gSpeechChannel, soStatus, (void*) &status ) == noErr)
					&&  (!status.outputBusy )
					)
					DisposeOfSpeech(true);
			}
			break;
	}
		
	return false;
	
} // TextFilterEvent

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextScrollContent(WindowPtr pWindow, WindowDataPtr pData, short deltaH, short deltaV)
{
	CGrafPtr	port = GetWindowPort(pWindow);
	RgnHandle	srcRgn, dstRgn, visRgn, clipRgn;
	Rect		viewRect;
	
	// scroll the text area
	TEScroll(deltaH, deltaV, ((TextDataPtr) pData)->hTE);

	// calculate the region that is uncovered by the scroll
	srcRgn = NewRgn();
        dstRgn = NewRgn();
        visRgn = NewRgn();
        clipRgn = NewRgn();
        GetPortVisibleRegion(port, visRgn);
        GetPortClipRegion(port, clipRgn);
	viewRect = (**((TextDataPtr) pData)->hTE).viewRect;
	RectRgn( srcRgn, &viewRect );
	SectRgn( srcRgn, visRgn,  srcRgn );
	SectRgn( srcRgn, clipRgn, srcRgn );
	CopyRgn( srcRgn, dstRgn );
	OffsetRgn( dstRgn, deltaH, deltaV );
	SectRgn( srcRgn, dstRgn, dstRgn );
	DiffRgn( srcRgn, dstRgn, srcRgn );

	// clip to this new area
	GetClip(dstRgn);
	SetClip(srcRgn);
	DrawPictures(pData, ((TextDataPtr) pData)->hTE);
	SetClip(dstRgn);
	
	// all done with these calculation regions
	DisposeRgn( srcRgn );
        DisposeRgn( dstRgn );
        DisposeRgn( visRgn );
        DisposeRgn( clipRgn );
	
	return eActionAlreadyHandled;
	
} // TextScrollContent

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextKeyEvent(WindowPtr pWindow, WindowDataPtr pData, EventRecord *pEvent, Boolean isMotionKey)
{	
	OSStatus	anErr = noErr;
	
	if (!(pEvent->modifiers & cmdKey)) 
	{
		char	theKey = pEvent->message & charCodeMask;
		char	theKeyCode = (pEvent->message >> 8) & charCodeMask;
		
		if ( 
			((** ((TextDataPtr) pData)->hTE).teLength+1 > kMaxLength) &&
				(	
				(theKey != kDeleteKey) &&
				(theKeyCode != kForwardDeleteKey) &&
				(theKeyCode != kUpArrow) &&
				(theKeyCode != kDownArrow) &&
				(theKeyCode != kRightArrow) &&
				(theKeyCode != kLeftArrow)
				) 
			)
			anErr = eDocumentTooLarge;
		else
		{
			long		oldHeight = CalculateTextEditHeight(((TextDataPtr) pData)->hTE);
			long		end = (**(((TextDataPtr) pData)->hTE)).selEnd;
			long		start = (**(((TextDataPtr) pData)->hTE)).selStart;

			ObscureCursor();
			SaveCurrentUndoState(pData, cTypingCommand);
			if (theKeyCode != kForwardDeleteKey)
			{
				if (pEvent->modifiers & shiftKey)
				{
					switch (theKeyCode)
					{
						case kUpArrow:
						case kLeftArrow:
							TEKey(theKey, ((TextDataPtr) pData)->hTE);
							TESetSelect((**(((TextDataPtr) pData)->hTE)).selStart, end, ((TextDataPtr) pData)->hTE);
							break;
							
						case kDownArrow:
						case kRightArrow:
							TESetSelect(end, end, ((TextDataPtr) pData)->hTE);
							TEKey(theKey, ((TextDataPtr) pData)->hTE);
							TESetSelect(start, (**(((TextDataPtr) pData)->hTE)).selEnd, ((TextDataPtr) pData)->hTE);
							break;
						
						default:
							TEKey(theKey, ((TextDataPtr) pData)->hTE);
					}
				}
				else
					TEKey(theKey, ((TextDataPtr) pData)->hTE);
			}
			else
			{
				if (gTE6Version)
				{
					TEKey(theKey, ((TextDataPtr) pData)->hTE);
				}
				else
				{
					if 	( end < (**(((TextDataPtr) pData)->hTE)).teLength )
					{
						if (start == end)
							TEKey(kRightArrowCharCode, ((TextDataPtr) pData)->hTE);
						TEKey(kBackspaceCharCode, ((TextDataPtr) pData)->hTE);
					}
				}
			}
			oldHeight -= CalculateTextEditHeight(((TextDataPtr) pData)->hTE);
			
			// AEC, changed pWindow to pData for 'TextDataPtr' casts
			((TextDataPtr) pData)->insideClickLoop = true;
				AdjustTE(pData, false);
				AdjustScrollBars(pWindow, (oldHeight > 0), (oldHeight > 0), nil);
			((TextDataPtr) pData)->insideClickLoop = false;

			if (!isMotionKey)
			{
				SetDocumentContentChanged( pData, true );
			}
			
		}
	}

	return anErr;

} // TextKeyEvent

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextContentClick(WindowPtr pWindow, WindowDataPtr pData, EventRecord *pEvent)
{
	OSStatus			anErr = noErr;
	Point			clickPoint = pEvent->where;
	ControlHandle	theControl;
	RgnHandle		hilightRgn;

	GlobalToLocal(&clickPoint);
	if (FindControl(clickPoint, pWindow, &theControl) == 0)
	{
		if (gMachineInfo.haveDragMgr)
		{
			hilightRgn = NewRgn();
			TEGetHiliteRgn(hilightRgn, ((TextDataPtr) pData)->hTE);

			if (PtInRgn(clickPoint, hilightRgn))
			{
				SaveCurrentUndoState(pData, cTypingCommand);
				if (!DragText(pWindow, pData, pEvent, hilightRgn))
					anErr = eActionAlreadyHandled;
			}
			else		
			{
				anErr = eActionAlreadyHandled;
			}
	
			DisposeRgn(hilightRgn);
		}
		else
		{
			anErr = eActionAlreadyHandled;
		}
	}

	if ( (anErr == eActionAlreadyHandled) && (PtInRect(clickPoint, &pData->contentRect)) )
	{
		TEClick(clickPoint, (pEvent->modifiers & shiftKey) != 0, ((TextDataPtr) pData)->hTE);
	}
		
	return anErr;
	
} // TextContentClick

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextAdjustSize(WindowPtr pWindow, WindowDataPtr pData, 
			Boolean *didReSize) // input: was window resized, output: t->we resized something
{
#pragma unused (pWindow)
	
	if (*didReSize)
	{
		RecalcTE(pData, true);
		AdjustTE(pData, true);
	}
	else
	{
		AdjustTE(pData, false);
	}
		
	return noErr;
	
} // TextAdjustSize

/* static */ OSStatus	TextCommand(WindowPtr pWindow, WindowDataPtr pData, short commandID, long menuResult)
{

	OSStatus	anErr = noErr;
	AEDesc 	theSourceText = {typeNull, NULL};			// for AppleScript
	AEDesc 	theResultText = {typeNull, NULL};			// for AppleScript
	OSAID 	resultID;						// for AppleScript
	Ptr 	resultTextPtr;						// for AppleScript
	Size 	resultDataSize;						// for AppleScript

	SetPortWindowPort (pWindow);

	if ( (pData->docTSMDoc) && (menuResult != 0) )
		FixTSMDocument(pData->docTSMDoc);
		
	switch (commandID)
	{			
		case cUndo:
		{
			short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;

			PerformUndo(pData);
			oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
			AdjustTE(pData, false);
			AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);
			RecalcTE(pData, true);
			
			SetDocumentContentChanged( pData, true );
				
		}
		break;
			
		case cCut:
			SaveCurrentUndoState(pData, cCut);
			{
			short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;

			TECut(((TextDataPtr) pData)->hTE);	// no need for TEToScrap with styled TE record
			oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
			AdjustTE(pData, false);
			AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);
		
			SetDocumentContentChanged( (WindowDataPtr) pData, true );
				
			}
		break;
			
		case cCopy:
			TECopy(((TextDataPtr) pData)->hTE);	// no need for TEToScrap with styled TE record
			AdjustTE(pData, false);
		break;
			
		case cClear:
			SaveCurrentUndoState(pData, cClear);
			{
			short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;

			TEDelete(((TextDataPtr) pData)->hTE);
			oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
			AdjustTE(pData, false);
			AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);
			
			SetDocumentContentChanged( pData, true );
				
			}
		break;
			
		case cPaste:
			SaveCurrentUndoState(pData, cPaste);
			{
			short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;

			anErr = TEFromScrap();			
			if (anErr == noErr)
			{				
				// if the current length, plus the paste data, minus the data in the selection
				// would make the document too large, say so
				if 	( 
						((** ((TextDataPtr) pData)->hTE).teLength +
						TEGetScrapLength() -
						((** ((TextDataPtr) pData)->hTE).selEnd-(** ((TextDataPtr) pData)->hTE).selStart)
						)
					> kMaxLength)
				{
					anErr = eDocumentTooLarge;
				}
				else
				{
					Handle	aHandle = (Handle) TEGetText(((TextDataPtr) pData)->hTE);
					Size	oldSize = GetHandleSize(aHandle);
					Size	newSize = oldSize + TEGetScrapLength();
					OSStatus	saveErr;
					
					SetHandleSize(aHandle, newSize);
					saveErr = MemError();
					SetHandleSize(aHandle, oldSize);
					if (saveErr != noErr)
						anErr = eDocumentTooLarge;
					else
						TEStylePaste(((TextDataPtr) pData)->hTE);
				}
			}
			oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
			AdjustTE(pData, false);
			AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);
			
			SetDocumentContentChanged( pData, true );
			}
		break;
#if !SIMPLER_TEXT			
		case cReplace:
			SaveCurrentUndoState(pData, cReplace);
			{
			short result = ConductFindOrReplaceDialog(kReplaceWindowID);
			
			if (result == cancel)
				break;
				
			if (result == iReplaceAll)
			{
				long 		newStart, newEnd;
				short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;
				
				TESetSelect(0, 0, ((TextDataPtr) pData)->hTE);
				while (PerformSearch((**((TextDataPtr) pData)->hTE).hText,
									(**((TextDataPtr) pData)->hTE).selStart,
									gFindString, gCaseSensitive, false, false,
									&newStart, &newEnd))
				{
					TESetSelect(newStart, newEnd, ((TextDataPtr) pData)->hTE);
					TEDelete(((TextDataPtr) pData)->hTE);
					TEInsert(&gReplaceString[1], gReplaceString[0], ((TextDataPtr) pData)->hTE);
					TESetSelect(newStart+gReplaceString[0], newStart+gReplaceString[0], ((TextDataPtr) pData)->hTE);				

					SetDocumentContentChanged( (WindowDataPtr) pData, true );	
				}
				oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
				
				AdjustTE(pData, false);
				AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);
	
				anErr = eActionAlreadyHandled;
				break;
			}
			}
		/* no break here - we go straight to cReplaceAgain */
		
		// fall through from replace
		case cReplaceAgain:
			SaveCurrentUndoState(pData, cReplaceAgain);
			{
			long 	newStart, newEnd;
			Boolean	isBackwards = ((gEvent.modifiers & shiftKey) != 0);
			
			if (PerformSearch((**((TextDataPtr) pData)->hTE).hText,
								isBackwards ? (**((TextDataPtr) pData)->hTE).selEnd : (**((TextDataPtr) pData)->hTE).selStart,
								gFindString, gCaseSensitive, isBackwards, gWrapAround,
								&newStart, &newEnd))
			{
				short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;
				
				TESetSelect(newStart, newEnd, ((TextDataPtr) pData)->hTE);
				TEDelete(((TextDataPtr) pData)->hTE);
				TEInsert(&gReplaceString[1], gReplaceString[0], ((TextDataPtr) pData)->hTE);
				TESetSelect(newStart+gReplaceString[0], newStart+gReplaceString[0], ((TextDataPtr) pData)->hTE);				
				oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
				
				AdjustTE(pData, false);
				AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);

				SetDocumentContentChanged( pData, true );
			}
			else
				SysBeep(1);

			anErr = eActionAlreadyHandled;
			}
		break;
#endif /* !SIMPLER_TEXT */
            
		// for AppleScript
		case cExecute:
			{
			//fprintf(stderr, "Attempting to execute and applescript selection.\n");
			SaveCurrentUndoState(pData, cExecute);
			AECreateDesc(typeChar, (*(**((TextDataPtr) pData)->hTE).hText) + (**((TextDataPtr) pData)->hTE).selStart, 
						 (**((TextDataPtr) pData)->hTE).selEnd - (**((TextDataPtr) pData)->hTE).selStart,
						 &theSourceText);
			anErr = OSACompileExecute( gOSAComponent, &theSourceText, kOSANullScript, 
						 kAECanInteract, &resultID);
			if (anErr == noErr)
			{
				anErr = OSADisplay( gOSAComponent, resultID, typeChar, kOSAModeNull, &theResultText);
				if (anErr == noErr) 
				{
					TESetSelect((**((TextDataPtr) pData)->hTE).selEnd, (**((TextDataPtr) pData)->hTE).selEnd, 
							  ((TextDataPtr) pData)->hTE);
					resultTextPtr = NewPtr(resultDataSize = AEGetDescDataSize(&theResultText));
					AEGetDescData(&theResultText, resultTextPtr, resultDataSize);
					TEInsert(" --> ", 5, ((TextDataPtr) pData)->hTE);
					TEInsert(resultTextPtr, resultDataSize, ((TextDataPtr) pData)->hTE);
					AdjustTE(pData, false);
					DisposePtr(resultTextPtr);
					AEDisposeDesc(&theResultText);
				}
				//else fprintf(stderr, "OSADisplay returned error %i.\n", anErr);
			}
			//else fprintf(stderr, "OSACompileExecute returned error %i.\n", anErr);

			anErr = OSADispose( gOSAComponent, resultID);
			AEDisposeDesc(&theSourceText);
			SetDocumentContentChanged( pData, true);
			}
		break;

#if !SIMPLER_TEXT
		case cFind:
		{
			if (ConductFindOrReplaceDialog(kFindWindowID) == cancel) break;
		}
		/* no break here - we go straight to cFindAgain */
		
		case cFindAgain:
		{
			long 	newStart, newEnd;
			Boolean	isBackwards = ((gEvent.modifiers & shiftKey) != 0);
			
			if (PerformSearch((**((TextDataPtr) pData)->hTE).hText,
								isBackwards ? (**((TextDataPtr) pData)->hTE).selStart : (**((TextDataPtr) pData)->hTE).selEnd,
								gFindString, gCaseSensitive, isBackwards, gWrapAround,
								&newStart, &newEnd))
			{
				TESetSelect(newStart, newEnd, ((TextDataPtr) pData)->hTE);
				AdjustTE(pData, false);
				AdjustScrollBars(pWindow, false, false, nil);
			}
			else
				SysBeep(1);

			anErr = eActionAlreadyHandled;
		}
		break;
		
		case cFindSelection:
		{
			ScrapRef findScrap;
			
			gFindString[0] = (**((TextDataPtr) pData)->hTE).selEnd - (**((TextDataPtr) pData)->hTE).selStart;
			BlockMoveData( (*(**((TextDataPtr) pData)->hTE).hText) + (**((TextDataPtr) pData)->hTE).selStart, &gFindString[1], gFindString[0]);
			
			// Set the global Find Scrap string
			if( GetScrapByName( kScrapFindScrap, kScrapClearNamedScrap, &findScrap ) == noErr )
				PutScrapFlavor( findScrap, kScrapFlavorTypeText, kScrapFlavorMaskNone, gFindString[0], gFindString+1 );
		}
		break;
#endif
            
		case cSelectAll:
			TESetSelect(0, (**((TextDataPtr) pData)->hTE).teLength, 
						((TextDataPtr) pData)->hTE);
			AdjustTE(pData, false);
			AdjustScrollBars(pWindow, false, false, nil);
			DoAdjustCursor(pWindow, nil);
			anErr = eActionAlreadyHandled;
		break;
		
		// save turns into save as if this is a new document or if the original wasn't
		// available for writing
		case cSave:
			if 	(
				(!pData->isWritable) || 
				( (pData->dataRefNum == -1) && (pData->resRefNum == -1) )
				)
				anErr = TextSaveAs(pWindow, pData);
			else
				anErr = TextSave(pData, true);
		break;
			
		case cSaveAs:
			anErr = TextSaveAs(pWindow, pData);
		break;
			
		// SUPPORTED FONTS
		case cSelectFont:
			SaveCurrentUndoState(pData, cSelectFont);
			{
			Str255		itemName;
			TextStyle	theStyle;
			short		oldNumLines = (**(((TextDataPtr) pData)->hTE)).nLines;
			
#if USE_SIMPLETEXT
			GetMenuItemText( GetMenuHandle( menuResult>>16 ), menuResult & 0xFFFF, itemName );
#else
                        strcpy(itemName,"\pMonaco");
#endif
			GetFNum( itemName, &theStyle.tsFont );
			TESetStyle( doFont, &theStyle, true, ((TextDataPtr) pData)->hTE );
			TECalText(((TextDataPtr) pData)->hTE);
			oldNumLines -= (**(((TextDataPtr) pData)->hTE)).nLines;
			AdjustTE(pData, false);
			AdjustScrollBars(pWindow, (oldNumLines > 0), (oldNumLines > 0), nil);
			
			SetDocumentContentChanged( pData, true );
			}
		break;
			
			
		// SUPPORTED STYLES
		case cPlain:
			ApplyFace(normal, pWindow, pData, cPlain);
		break;
			
		case cBold:
			ApplyFace(bold, pWindow, pData, cBold);
		break;

		case cItalic:
			ApplyFace(italic, pWindow, pData, cItalic);
		break;
			
		case cUnderline:
			ApplyFace(underline, pWindow, pData, cUnderline);
		break;
			
		case cOutline:
			ApplyFace(outline, pWindow, pData, cOutline);
		break;
			
		case cShadow:
			ApplyFace(shadow, pWindow, pData, cShadow);
		break;

		case cCondensed:
			ApplyFace(condense, pWindow, pData, cCondensed);
		break;
			
		case cExtended:
			ApplyFace(extend, pWindow, pData, cExtended);
		break;
			
	
		// SUPPORTED SIZES
		case cSize9:
			anErr = ApplySize(9, pWindow, pData, cSize9);
		break;
			
		case cSize10:
			anErr = ApplySize(10, pWindow, pData, cSize10);
		break;
			
		case cSize12:
			anErr = ApplySize(12, pWindow, pData, cSize12);
		break;
			
		case cSize14:
			anErr = ApplySize(14, pWindow, pData, cSize14);
		break;
			
		case cSize18:
			anErr = ApplySize(18, pWindow, pData, cSize18);
		break;
			
		case cSize24:
			anErr = ApplySize(24, pWindow, pData, cSize24);
		break;
			
		case cSize36:
			anErr = ApplySize(36, pWindow, pData, cSize36);
		break;
			
		// SUPPORTED SOUND COMMANDS
		case cRecord:
		{
			Handle	tempHandle;

			// allocate our prefered buffer if we can, but if we can't
			// make sure that at least a minimum amount of RAM is around
			// before recording.
			tempHandle = NewHandle(kPrefBufferSize);
			anErr = MemError();
			if (anErr != noErr)
			{
				tempHandle = NewHandle(kMinBufferSize);
				anErr = MemError();
				DisposeHandle(tempHandle);
				tempHandle = nil;
			}
			
			// if the preflight goes okay, do the record
			if (anErr == noErr)
			{
				Point	where = {50, 100};
				
				anErr = SndRecord(nil, where, siGoodQuality, (SndListHandle*) &tempHandle);
				if (anErr == noErr)
				{
					DisposeHandle(((TextDataPtr) pData)->soundHandle);
					((TextDataPtr) pData)->soundHandle = tempHandle;
					
					SetDocumentContentChanged( (WindowDataPtr) pData, true );
				}
				else
					DisposeHandle(tempHandle);
					
				if (anErr == userCanceledErr)
					anErr = noErr;
			}
		}
		break;
			
		case cPlay:
			if (((TextDataPtr) pData)->soundHandle)
				(void) SndPlay(nil, (SndListHandle) ((TextDataPtr) pData)->soundHandle, false);
		break;

		case cErase:
			DisposeHandle(((TextDataPtr) pData)->soundHandle);
			((TextDataPtr) pData)->soundHandle = nil;
			
			SetDocumentContentChanged( (WindowDataPtr) pData, true );
		break;
			
		case cSpeak:
			DisposeOfSpeech(false);
			if (gSpeechChannel == nil)
				anErr = NewSpeechChannel( &gCurrentVoice, &gSpeechChannel );
				
			if ( anErr == noErr )
			{
				short	textLength, textStart;
									
				// determine which text to speak
				if ( (**((TextDataPtr) pData)->hTE).selEnd > (**((TextDataPtr) pData)->hTE).selStart )	// If there is a selection.
				{
					textLength = (**((TextDataPtr) pData)->hTE).selEnd - (**((TextDataPtr) pData)->hTE).selStart;
					textStart = (**((TextDataPtr) pData)->hTE).selStart;
				}
				else											// No text selected.
				{
					textLength = (**((TextDataPtr) pData)->hTE).teLength;
					textStart = 0;
				}
					
					
				gSpeakPtr = NewPtr(textLength);
				anErr = MemError();
				if (anErr == noErr)
				{
					BlockMoveData( *((**((TextDataPtr) pData)->hTE).hText) + textStart, gSpeakPtr, (Size) textLength );
                                        if (textLength)
                                            anErr = SpeakText( gSpeechChannel, gSpeakPtr, textLength );
                                        else
                                            anErr = noErr;
				}
			}
		break;

		case cStopSpeaking:
			DisposeOfSpeech(true);
		break;
			
		case cSelectVoiceSubMenu:
		{
			VoiceSpec	newSpec;
			short		i, menuIndex;
			Str255		itemText;
			short		theVoiceCount;
			MenuHandle 	menu = GetMenuHandle(mVoices);
			
			// in order to change voices, we need to ditch the speaking
			DisposeOfSpeech(true);

			// get the name of the selected voice				
			menuIndex = menuResult & 0xFFFF;
			GetMenuItemText(menu, menuIndex, itemText);
			
			if (CountVoices( &theVoiceCount ) == noErr)
			{
				VoiceDescription	description;		// Info about a voice.
	
				for (i = 1; i <= theVoiceCount; ++i)
				{
					if ( (GetIndVoice( i, &newSpec ) == noErr)  &&
							(GetVoiceDescription( &newSpec, &description, sizeof(description) ) == noErr ) )
					{
						if (PLstrcmp( itemText, description.name ) == 0)
							break;
					}
				}
			}
				
			gCurrentVoice = newSpec;
			for (i = CountMenuItems(menu); i >= 1; --i)
				MacCheckMenuItem(menu, i, (menuIndex == i));
		}
		break;
		
		case cSelectContents:
		{
			Str255	searchStr;
			short	menuIndex;
			long 	newStart, newEnd;
			
			menuIndex = menuResult & 0xFFFF;

			// get the search string for this menu item
			anErr = TextGetContentsListItem(pData, menuIndex, nil, searchStr, nil);
			
			if (anErr == noErr)
			{
				if (PerformSearch(
					(**((TextDataPtr) pData)->hTE).hText,
					0,			// start at beginning of text
					searchStr,
					false,		// not case sensitive
					false,		// forwards
					false,		// wrap
					&newStart, 
					&newEnd))
				{
					
					// <7>
					
					short 	amount;
					Point	newSelectionPt;
					
					// get QuickDraw offset of found text,  
					// scroll that amount plus a line height,
					// and add a fifth of the window for aesthetics (and  
					// for slop to avoid fraction-of-line problems)

					newSelectionPt = TEGetPoint(newEnd, ((TextDataPtr) pData)->hTE);
					
					amount = - newSelectionPt.v + pData->vScrollAmount;
					amount += (pData->contentRect.bottom - pData->contentRect.top) / 5;
					
					SetControlAndClipAmount(pData->vScroll, &amount);
					if (amount != 0)
					{
						DoScrollContent(pWindow, pData, 0, amount);
					}

					// move selection to beginning of found text 
					// (are the Adjust calls necessary?)
					
					TESetSelect(newStart, newStart, ((TextDataPtr) pData)->hTE);	
					AdjustTE(pData, false);
					AdjustScrollBars(pWindow, false, false, nil);

				}
				else
				{
					// search failed
					SysBeep(10);
				}
			}
		}
		break;
	} // end switch()
		
	return anErr;
	
} // TextCommand

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextPreMenuAccess(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow, pData)

	TextAddVoices();

	return noErr;
}

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextAdjustMenus(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow)

	// enable the commands that we support for editable text document
	if ( pData->windowKind == kTextWindow)
	{		
		if (((TextDataPtr) pData)->prevCommandID != cNull)
			EnableCommand(cUndo);
			
		if ( (**((TextDataPtr) pData)->hTE).selEnd > (**((TextDataPtr) pData)->hTE).selStart )	// If there is a selection.
		{
			EnableCommand(cCut);
			EnableCommand(cCopy);
			EnableCommand(cClear);

			EnableCommand(cExecute);	// for AppleScript
			
			EnableCommand(cFindSelection);
		}
		
		TEFromScrap();
		if (TEGetScrapLength() > 0)
			EnableCommand(cPaste);
			
		EnableCommand(cSaveAs);
		EnableCommand(cSelectAll);
		
		EnableCommand(cFind);
		EnableCommand(cReplace);
		if (gFindString[0] != 0)
		{
			EnableCommand(cFindAgain);
			EnableCommand(cReplaceAgain);
		}
			
		// enable all fonts, select the font current, if that's what's best
		EnableCommand(cSelectFont);
		{
		short		mode = doFont;
		Str255		fontName, itemName;
		Str255		styleName;
		TextStyle	theStyle;
		Boolean		isCont;
		
		isCont = TEContinuousStyle(&mode, &theStyle, ((TextDataPtr) pData)->hTE);
		if (isCont)
		{
			GetFontName(theStyle.tsFont, fontName);
		}


		{
			MenuHandle	menu = GetMenuHandle( mFont );
			short		count = CountMenuItems(menu);
			short		index;
			
			for (index = 1; index <= count; ++index)
			{
				short	mark;
				
				GetItemMark(menu, index, &mark);
				if (isCont)
				{
					GetMenuItemText( menu, index, itemName );
					
					// don't change the checkmark if it's a heirarchichal menu, because
					// the mark actually holds the ID of sub-menu
					if ((mark == noMark) || (mark == checkMark))
					{
						MacCheckMenuItem(menu, index, EqualString(itemName, fontName, true, true) );
					}
					else
					{
						// if it is a sub menu, we check there too
						MenuHandle	subMenu = GetMenuHandle(mark);
						short		subCount = CountMenuItems(subMenu);
						short		subIndex;
						
						if (EqualString(itemName, fontName, true, true))
						{
							SetItemStyle(menu, index, underline);
							for (subIndex = 1; subIndex <= subCount; ++subIndex)
							{
								GetMenuItemText(subMenu, subIndex, itemName);
								MacCheckMenuItem(subMenu, subIndex, EqualString(itemName, styleName, true, true) );
							}
						}
						else
						{
							SetItemStyle(menu, index, normal);
							for (subIndex = 1; subIndex <= subCount; ++subIndex)
								MacCheckMenuItem(subMenu, subIndex, false );
						}
					}
				}
				else
				{
					if ((mark == noMark) || (mark == checkMark))
						MacCheckMenuItem(menu, index, false);
					else
						SetItemStyle(menu, index, normal);
				}
			}
		}
		}

		// enable the sizes, and outline what's currently valid
		{
		short		mode;
		TextStyle	theStyle;
		Boolean		isCont;
		short		whichToCheck;
		
		// find out the continuous run of sizes
		whichToCheck = 0;
		mode = doSize;
		if (TEContinuousStyle(&mode, &theStyle, ((TextDataPtr) pData)->hTE))
			{			
			whichToCheck = theStyle.tsSize;
			
			// default font size -> proper size
			if (whichToCheck == 0)
				whichToCheck = GetDefFontSize();
			}
			
		// find out the font runs
		mode = doFont;
		isCont = TEContinuousStyle(&mode, &theStyle, ((TextDataPtr) pData)->hTE);
		
		EnableCommandCheckStyle(cSize9,  whichToCheck == 9, (isCont & RealFont(theStyle.tsFont, 9)) ? outline : normal);
		EnableCommandCheckStyle(cSize10, whichToCheck == 10, (isCont & RealFont(theStyle.tsFont, 10)) ? outline : normal);
		EnableCommandCheckStyle(cSize12, whichToCheck == 12, (isCont & RealFont(theStyle.tsFont, 12)) ? outline : normal);
		EnableCommandCheckStyle(cSize14, whichToCheck == 14, (isCont & RealFont(theStyle.tsFont, 14)) ? outline : normal);
		EnableCommandCheckStyle(cSize18, whichToCheck == 18, (isCont & RealFont(theStyle.tsFont, 18)) ? outline : normal);
		EnableCommandCheckStyle(cSize24, whichToCheck == 24, (isCont & RealFont(theStyle.tsFont, 24)) ? outline : normal);
		EnableCommandCheckStyle(cSize36, whichToCheck == 36, (isCont & RealFont(theStyle.tsFont, 36)) ? outline : normal);			
		}
		
		{
		short		mode = doFace;
		TextStyle	theStyle;
		Style		legalStyles;
		
		if (!TEContinuousStyle(&mode, &theStyle, ((TextDataPtr) pData)->hTE))
		{
			theStyle.tsFace = normal;
			EnableCommandCheck(cPlain, false);
		}
		else
			EnableCommandCheck(cPlain, theStyle.tsFace == normal);
			
		// <39> use the script manager to determine legal styles for this
		// run of text.  If the legal styles are zero (trap unimplemented),
		// then we assume all styles.
		legalStyles = GetScriptVariable(GetScriptManagerVariable(smKeyScript), smScriptValidStyles);
		if (legalStyles == 0)
			legalStyles = (Style)0xFFFF;
			
		if (legalStyles & bold)
			EnableCommandCheck(cBold, 			theStyle.tsFace & bold);
		if (legalStyles & italic)
			EnableCommandCheck(cItalic, 		theStyle.tsFace & italic);
		if (legalStyles & underline)
			EnableCommandCheck(cUnderline, 		theStyle.tsFace & underline);
		if (legalStyles & outline)
			EnableCommandCheck(cOutline, 		theStyle.tsFace & outline);
		if (legalStyles & shadow)
			EnableCommandCheck(cShadow, 		theStyle.tsFace & shadow);
		if (legalStyles & condense)
			EnableCommandCheck(cCondensed, 		theStyle.tsFace & condense);
		if (legalStyles & extend)
			EnableCommandCheck(cExtended, 		theStyle.tsFace & extend);
		}
		
	}

	// enable commands related to speaking the content if we have support for that
	if (gMachineInfo.haveTTS)
	{
		// note that none of this code should depend on the actual contents of the voices menu,
		// since the voices thread might not have finished setting up the menu yet

		// if we are speaking, we can stop
		if (gSpeechChannel) 
			EnableCommand(cStopSpeaking);

		// even while speaking, you can re-speak or select a new voice
		EnableCommand(cSpeak);
		EnableCommand(cSelectVoice);
		EnableCommand(cSelectVoiceSubMenu);
		
		if ( (**((TextDataPtr) pData)->hTE).selEnd > (**((TextDataPtr) pData)->hTE).selStart )	// If there is a selection.
			ChangeCommandName(cSpeak, kTextStrings, iSpeakSelection);
		else
			ChangeCommandName(cSpeak, kTextStrings, iSpeakAll);
			
	}
		
	// enable the correct controls to go with sound input/output
	if (((TextDataPtr) pData)->soundHandle)
		EnableCommand(cPlay);
	if (pData->windowKind == kTextWindow)
	{
		if (((TextDataPtr) pData)->soundHandle)
			EnableCommand(cErase);
		else
		{
			if (gMachineInfo.haveRecording)
				EnableCommand(cRecord);
		}
	}
	
	// enable the contents menu, if any 		
	(void) TextAdjustContentsMenu(pData);
	
	// enable commands that we support at all times
	if (GetControlMaximum(pData->vScroll) != 0)
	{
		EnableCommand(cNextPage);
		EnableCommand(cPreviousPage);
	}
	
	return noErr;
	
} // TextAdjustMenus

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextGetDocumentRect(WindowPtr pWindow, WindowDataPtr pData, 
			LongRect * documentRectangle, Boolean forGrow)
{
#pragma unused (pWindow)
	
	Rect	theRect = pData->contentRect;
	Rect	maxRect;
	
	GetRegionBounds( GetGrayRgn(), &maxRect );
	
	if ( (!forGrow) && (!(((TextDataPtr) pData)->insideClickLoop) ) )
		RecalcTE(pData, false);
	
	theRect.bottom = CalculateTextEditHeight(((TextDataPtr) pData)->hTE);
	theRect.bottom += kMargins*2;
	theRect.right = maxRect.right;
		
	if (theRect.bottom < pData->contentRect.bottom)
		theRect.bottom = pData->contentRect.bottom;
		
	if (forGrow)
		theRect.bottom = maxRect.bottom-kScrollBarSize;
		
	RectToLongRect(&theRect, documentRectangle);
	
	return noErr;
	
} // TextGetDocumentRect

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextGetBalloon(WindowPtr pWindow, WindowDataPtr pData, 
		Point *localMouse, short * returnedBalloonIndex, Rect *returnedRectangle)
{
#pragma unused (pWindow, pData, localMouse, returnedRectangle)

	*returnedBalloonIndex = iHelpTextContent;
	
	return noErr;
	
} // TextGetBalloon

// --------------------------------------------------------------------------------------------------------------

static long TextCalculateIdleTime(WindowPtr pWindow, WindowDataPtr pData)
{
#pragma unused (pWindow, pData)

	if ( (gMachineInfo.amInBackground) || (! (**(((TextDataPtr) pData)->hTE)).active) )
		return(0x7FFFFFF);
	else
		return(GetCaretTime());
		
} // TextCalculateIdleTime

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextAdjustCursor(WindowPtr pWindow, WindowDataPtr pData, 
						Point * localMouse, RgnHandle globalRgn)
{
#pragma unused (pWindow)

	OSStatus			anErr = noErr;
	CursHandle		theCross;
	RgnHandle		hilightRgn;
	Cursor			arrow;

	if (gMachineInfo.haveDragMgr)
	{
		RgnHandle globalHilight;
		
		hilightRgn = NewRgn();
		TEGetHiliteRgn(hilightRgn, ((TextDataPtr) pData)->hTE);
		
		globalHilight = NewRgn();
		CopyRgn(hilightRgn, globalHilight);
		LocalToGlobalRgn(globalHilight);
		
		if (PtInRgn(*localMouse, hilightRgn))
		{
			// we're already in the hilight rgn, so we don't need mouse-moved events as long
			// as we stay there
			CopyRgn(globalHilight, globalRgn);
			
			SetCursor(GetQDGlobalsArrow(&arrow));
			DisposeRgn(hilightRgn);
			return eActionAlreadyHandled;	
		}
		else if (!EmptyRgn(hilightRgn))
		{
			// make sure we get mouse-moved events if the mouse moves into the hilight rgn,
			// so that we can change the cursor
			DiffRgn(globalRgn, globalHilight, globalRgn);
		}
	
		DisposeRgn(globalHilight);
		DisposeRgn(hilightRgn);
	}
		
	theCross = MacGetCursor(iBeamCursor);
	if (theCross)
	{
		char	oldState;
		
		oldState = HGetState((Handle) theCross);
		HLock((Handle) theCross);
		SetCursor(*theCross);
		HSetState((Handle) theCross, oldState);
		anErr = eActionAlreadyHandled;
	}
		
	return anErr;
	
} // TextAdjustCursor

// --------------------------------------------------------------------------------------------------------------

short gNilCaretProc[] = { 
						0x584F, 		// ADDQ.W	#$4, A7
						0x4E75};		// RTS

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextPrintPage(WindowPtr pWindow, WindowDataPtr pData,
					Rect * pageRect, long *pageNum)
{
#pragma unused (pWindow)

	OSStatus		anErr = noErr;
	short		footerHeight;
	TEHandle	hTE;
	Rect		areaForThisPage;
	short		ourPage = 1;
	Boolean		documentHasFormControl = Count1Resources(kFormResource) != 0;
	
	// calculate area for the footer (page number)
	{
	FontInfo	theInfo;
	
	TextFont(0);
	TextSize(0);
	TextFace(normal);
	GetFontInfo(&theInfo);
	footerHeight = (theInfo.ascent + theInfo.descent + theInfo.leading) << 1;
	}
	
	// duplicate the text edit record, disable the selection before swapping the new port in
	hTE = ((TextDataPtr) pData)->hTE;
	TEDeactivate(hTE);
	
	anErr = HandToHand((Handle*) &hTE);
	nrequire(anErr, DuplicateTE);

	// turn off outline hilighting -- because the window is disabled while
	// printing is going on, but we don't want that disabled hilight to draw
	TEFeatureFlag(teFOutlineHilite, teBitClear, ((TextDataPtr) pData)->hTE);
		
	// now HERE'S a real hack!  Under certain conditions, Text Edit will draw the
	// cursor, even if you said the edit record is inactive!  This happens when
	// the internal state sez that the cursor hasn't been drawn yet.  Lucky
	// for us, the caret is drawn through a hook, which we replace with a NOP.
	(**hTE).caretHook = (CaretHookUPP) gNilCaretProc;
	
	// point the rectangles to be the page rect minus the footer
	areaForThisPage = *pageRect;
	areaForThisPage.bottom -= footerHeight;
	
	InsetRect(&areaForThisPage, kPrintMargins, kPrintMargins);
	(**hTE).viewRect = (**hTE).destRect = areaForThisPage;

	// recalculate the line breaks
	TECalText(hTE);

	// point it at the printing port.
	(**hTE).inPort = GetQDGlobalsThePort();

	// now loop over all pages doing page breaking until we find our current
	// page, which we print, and then return.
	{
	Rect	oldPageHeight = (**hTE).viewRect;
	short	currentLine = 0;
	long	prevPageHeight = 0;

	while (ourPage <= *pageNum)
	{
		long	currentPageHeight = 0;
					
		// calculate the height including the current page, breaks
		// when one of three things happen:
		// 1) adding another line to this page would go beyond the length of the page
		// 2) a picture needs to be broken to the next page (NOT YET IMPLEMENTED)
		// 3) we run out of lines for the document 
		// 4) if the line has a page break (defined as a non breaking space w/o a PICT)

		// POTENTIAL BUG CASES:
		// If a single line > the page height.  Can that happen?  If so, we need to 
		// add something to handle it.
		do
		{
			long currentLineHeight;
			
			// zero based count -- but one based calls to TEGetHeight 
			currentLineHeight = TEGetHeight(currentLine+1, currentLine+1, hTE);
				
			// if adding this line would just be too much, break out of here
			if ((currentLineHeight + currentPageHeight) > (areaForThisPage.bottom - areaForThisPage.top))
				break;
				
			++ currentLine;
			currentPageHeight += currentLineHeight;
			
			// if this line had a page break on it, break out of pagination
			if (documentHasFormControl && LineHasPageBreak(currentLine-1, hTE))
				break;
				
		} while (currentLine < (**hTE).nLines);
		
		// if this the page we are trying to print
		if (ourPage == *pageNum)
		{
			Str255		pageString;
			RgnHandle	oldRgn = NewRgn();
			
			// move onto the next page via offset by the previous pages -- but
			// clip to the current page height because we wouldn't want to see
			// half of a line from the next page at the bottom of a page.
			OffsetRect(&oldPageHeight, 0, -(prevPageHeight));
			oldPageHeight.bottom = oldPageHeight.top + currentPageHeight;
			(**hTE).destRect = oldPageHeight;
			
			// clip to this area as well
			areaForThisPage.bottom = areaForThisPage.top + currentPageHeight;
			GetClip(oldRgn);
			ClipRect(&areaForThisPage);
			
			// draw the edit record, plus our cool pictures	
			TEUpdate(&areaForThisPage, hTE); 
			DrawPictures(pData, hTE);
			
			// restore the clip
			SetClip(oldRgn);
			DisposeRgn(oldRgn);
			
			// Draw the page string at the bottom of the page, centered
			pageString[0] = 2;
			pageString[1] = '-';
			NumToString(*pageNum, &pageString[2]);
			pageString[0] += pageString[2];
			pageString[2] = ' ';
			pageString[++pageString[0]] = ' ';
			pageString[++pageString[0]] = '-';
			
			MoveTo(
				pageRect->left + 
					((pageRect->right - pageRect->left) >> 1) - 
					(StringWidth(pageString)>>1),
				pageRect->bottom - kPrintMargins);
				
			DrawString(pageString);
			
			// if we have completed all pages
			if (currentLine >= (**hTE).nLines)
				{
				// tell it to stop printing
				*pageNum = -1;
				}
				
			// get out of here!
			break;
		}
	
		// move onto the next page via count
		++ourPage;
		
		// and the list of pages before now includes this page we just finished
		prevPageHeight += currentPageHeight;
		
	}
	
	}
	
	
	// restore text for visible page if done
	if (*pageNum == -1)
	{
		TEFeatureFlag(teFOutlineHilite, teBitSet, ((TextDataPtr) pData)->hTE);
		TECalText(((TextDataPtr) pData)->hTE);
		if (pData->originalFileType != 'ttro')
			TEActivate(((TextDataPtr) pData)->hTE);
	}
		
// FALL THROUGH EXCEPTION HANDLING

	// Dispose this way to avoid disposing of any owned objects
	DisposeHandle((Handle) hTE);
DuplicateTE:

	return anErr;
	
} // TextPrintPage

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextInitData(WindowDataPtr pData)
{
	static TEClickLoopUPP gMyClickLoop = NULL;
	OSStatus				anErr = noErr;
        Str255					itemName;
        SInt16					iFontNum;			

	if (!gMyClickLoop)
	{
		gMyClickLoop = NewTEClickLoopUPP(CClikLoop);
	}

	pData->bumpUntitledCount	= true;
	
	pData->pScrollContent 		= (ScrollContentProc)	TextScrollContent;
	pData->pAdjustSize	 		= (AdjustSizeProc)		TextAdjustSize;
	pData->pGetDocumentRect 	= (GetDocumentRectProc)	TextGetDocumentRect;
	pData->pPreMenuAccess		= (PreMenuAccessProc)	TextPreMenuAccess;
	pData->pAdjustMenus	 		= (AdjustMenusProc)		TextAdjustMenus;
	pData->pCommand	 			= (CommandProc)			TextCommand;

	pData->pCloseWindow 		= (CloseWindowProc)		TextCloseWindow;
	pData->pFilterEvent 		= (FilterEventProc)		TextFilterEvent;
	pData->pActivateEvent 		= (ActivateEventProc)	TextActivateEvent;
	pData->pUpdateWindow 		= (UpdateWindowProc)	TextUpdateWindow;
	pData->pPrintPage	 		= (PrintPageProc)		TextPrintPage;
	pData->pSaveTo				= (SaveToProc)			TextSaveTo;

	// we only support keydowns and editing for modifable docs
	if (pData->originalFileType != 'ttro')
	{
		pData->pKeyEvent	 		= (KeyEventProc)			TextKeyEvent;
		pData->pContentClick		= (ContentClickProc)		TextContentClick;
		pData->pAdjustCursor		= (AdjustCursorProc)		TextAdjustCursor;
		pData->pGetBalloon			= (GetBalloonProc)			TextGetBalloon;
		pData->pCalculateIdleTime	= (CalculateIdleTimeProc)	TextCalculateIdleTime;
		
		// We can always reference our Drag handlers, because they will not be called if we
		// don't have the Drag Manager available. We needn't check here (it would be redundant).
		pData->pDragTracking		= (DragTrackingProc)	TextDragTracking;
		pData->pDragReceive			= (DragReceiveProc)		TextDragReceive;

		pData->documentAcceptsText	= true;
	}
		
	// leave room for the grow area at bottom
	pData->hasGrow				= true;
#if 0
	//	This is already taken care of in MakeNewWindow (SimpleText.c)
	//	Look for "thePreflight.wantHScroll"
	pData->contentRect.bottom	-= kScrollBarSize;
#endif
	if ((pData->contentRect.right - pData->contentRect.left) > kOnePageWidth)
		pData->contentRect.right = pData->contentRect.left + kOnePageWidth;
		
        // While SimpleText supports multiple text styles, we only want single style for MacSeq
#if USE_SIMPLETEXT
	((TextDataPtr) pData)->hTE	= TEStyleNew(&pData->contentRect, &pData->contentRect);
#else
        // Default mono-spaced font = Monaco (#4) must be active in graphics port at TENew() call
        strcpy(itemName,"\pMonaco");
        GetFNum(itemName, &iFontNum);
        TextFont(iFontNum);
	((TextDataPtr) pData)->hTE	= TENew(&pData->contentRect, &pData->contentRect);
#endif
	anErr = MemError();
	nrequire(anErr, TENewFailed);
		
	pData->hScrollAmount		= 0;
	pData->vScrollAmount		= TEGetHeight(0, 0, ((TextDataPtr) pData)->hTE);

	TEAutoView(true, ((TextDataPtr) pData)->hTE);

	// Setup our click loop to handle autoscrolling
	((TextDataPtr) pData)->docClick = (**(((TextDataPtr) pData)->hTE)).clickLoop;
	TESetClickLoop(gMyClickLoop, ((TextDataPtr) pData)->hTE);
	
	// by default we are saving with LFs (UNIX style line-ends)
	((TextDataPtr) pData)->saveWithLF = true;			

	// initalize undo information
	((TextDataPtr) pData)->prevCommandID = cNull;

	return noErr;
	
TENewFailed:
 
        DebugStr("\pTextFile: TEStyleNew returned an error." );

	return anErr;
	
} // TextInitData

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextReadDataFork(WindowDataPtr pData)
{	
	OSStatus	anErr = noErr;
	
	// if we have a data fork, read the contents into the record	
	if (pData->dataRefNum != -1)
	{
		long	dataSize;
		
		GetEOF(pData->dataRefNum, &dataSize);
		if (dataSize > kMaxLength)
			anErr = eDocumentTooLarge;
		else
		{
			Handle	tempHandle = NewHandle(dataSize);
			anErr = MemError();
			if (anErr == noErr)
			{
				int i;

				// read the text in
				SetFPos(pData->dataRefNum, fsFromStart, 0);
				anErr = FSRead(pData->dataRefNum, &dataSize, * tempHandle);
				((TextDataPtr) pData)->saveWithLF = false;			
				for (i=0; i < dataSize; i++)
				{
					if ((*tempHandle)[i] == '\n')
					{
						(*tempHandle)[i] = '\r';
						// the file has UNIX style line ends, remember it to save the file with LFs as well
						((TextDataPtr) pData)->saveWithLF = true;
					}
				}

				// then insert it.
				if (anErr == noErr)
				{
					HLock(tempHandle);
					TEStyleInsert(*tempHandle, dataSize, nil, ((TextDataPtr) pData)->hTE);
					anErr = MemError();
				}
				DisposeHandle(tempHandle);
			}
			
		}
		
	}
	nrequire(anErr, ReadData);
	
	return noErr;
	
ReadData:

	return anErr;
	
} // TextReadDataFork

// --------------------------------------------------------------------------------------------------------------

static void		TextReadResourceFork(WindowDataPtr pData)
{	
	// if we have a resource fork, read the contents
	if (pData->resRefNum != -1)
	{
		short	oldResFile = CurResFile();
		Handle	theStyle;
		
		// read the style information
		UseResFile(pData->resRefNum);
		theStyle = Get1Resource('styl', 128);
		if (theStyle)
		{
			HNoPurge(theStyle);
			UseStyleScrapForDocument((StScrpHandle) theStyle, true, ((TextDataPtr) pData)->hTE);
			ReleaseResource(theStyle);
		}

		// if we have sound, load it in and detach it
		{
		Handle	soundHandle = Get1Resource('snd ', kSoundBase);
		if (soundHandle)
		{
			HNoPurge(soundHandle);
			DetachResource(soundHandle);
			((TextDataPtr) pData)->soundHandle = soundHandle;
		}
		}

		UseResFile(oldResFile);
	}
			
} // TextReadResourceFork

// --------------------------------------------------------------------------------------------------------------

static void		TextFinishTESetup(WindowDataPtr pData)
{			
	// hook out drawing of the non-breaking space for read only documents,
	// for modifiable documents, enable outline hiliting (ie, when TE window
	// isn't in front, show the gray outline)
	if (pData->originalFileType == 'ttro')
	{
		static DrawHookUPP gMyDrawHook = NULL;
		UniversalProcPtr	hookRoutine;

		if (!gMyDrawHook)
		{
			gMyDrawHook = NewDrawHookUPP((DrawHookProcPtr)MyDrawHook);
		}

		hookRoutine = (UniversalProcPtr)gMyDrawHook;
		
		TECustomHook(intDrawHook, &hookRoutine, ((TextDataPtr) pData)->hTE);
	}
	else
	{
		TEFeatureFlag(teFOutlineHilite, teBitSet, ((TextDataPtr) pData)->hTE);
	}

	// make a TSM document if this is editable
	if 	(
		(pData->originalFileType != 'ttro') &&
		(gMachineInfo.haveTSMTE)
		)
	{
		OSType	supportedInterfaces[1];

		supportedInterfaces[0] = kTSMTEInterfaceType;
		
		if (NewTSMDocument(1, supportedInterfaces, 
			&pData->docTSMDoc, (long)&pData->docTSMRecHandle) == noErr)
		{
			static TSMTEPreUpdateUPP gTSMPreUpdateProc = NULL;
			static TSMTEPostUpdateUPP gTSMPostUpdateProc = NULL;
			long response;

			if (!gTSMPreUpdateProc)
			{
				gTSMPreUpdateProc = NewTSMTEPreUpdateUPP(TSMPreUpdateProc);
			}
			if (!gTSMPostUpdateProc)
			{
				gTSMPostUpdateProc = NewTSMTEPostUpdateUPP(TSMPostUpdateProc);
			}

			(**(pData->docTSMRecHandle)).textH 				= ((TextDataPtr) pData)->hTE;
			if ((Gestalt(gestaltTSMTEVersion, &response) == noErr) && (response == gestaltTSMTE1))
				(**(pData->docTSMRecHandle)).preUpdateProc 	= gTSMPreUpdateProc;
			(**(pData->docTSMRecHandle)).postUpdateProc 		= gTSMPostUpdateProc;
			(**(pData->docTSMRecHandle)).updateFlag 		= kTSMTEAutoScroll;
			(**(pData->docTSMRecHandle)).refCon 			= (long)pData;
		}
	}

	// now we have added text, so adjust views and such as needed
	TESetSelect(0, 0, ((TextDataPtr) pData)->hTE);
	RecalcTE(pData, true);
	AdjustTE(pData, true);

	// ???? Hack to get around a 7.0 TextEdit bug.  If you are pasting a multiple
	// line clipboard into TE, *and* the TextEdit record is new, *and* the selection
	// is at the begining of the doc (0,0 as above), *and* you haven't moved the
	// cursor around at all, then TE pastes thinking it's at the end of the line,
	// when it really should be at the begining.  Then if you <cr> with the cursor
	// visible, it'll leave a copy behind.  
	
	// I'm not happy with this, but I don't know another way around the problem.
	if (pData->originalFileType != 'ttro') 
	{
		TEKey(0x1F, ((TextDataPtr) pData)->hTE);
		TEKey(0x1E, ((TextDataPtr) pData)->hTE);
		ShowCursor ( ); // on 8, TEKey calls ObscureCursor
	}
	
	// <39> if this is a new document, convert the "system size", "system font", and
	// "application font" into real font IDs and sizes.  This is so that
	// if someone saves this document and opens it with another script
	// system, they don't get all huffy that the font changed on them.
	// It also solves problems with cut and paste to applications too stupid
	// to know that "zero" means system size.
	if (pData->dataRefNum == -1)
	{
		TEHandle	hTE = ((TextDataPtr) pData)->hTE;
		short		mode = doAll;
		TextStyle	theStyle;
                Str255		itemName;
	
		TEContinuousStyle(&mode, &theStyle, hTE);
		if (theStyle.tsSize == 0)
			theStyle.tsSize = GetDefFontSize();
		if (theStyle.tsFont == systemFont)
			theStyle.tsFont = GetSysFont();
		if (theStyle.tsFont == applFont)
			theStyle.tsFont = GetAppFont();
		
                // With single-style TextEdit record, the default font has already been
                // selected at time of creation, but we do this anyway just because...
                strcpy(itemName,"\pMonaco");
                GetFNum( itemName, &theStyle.tsFont );

		mode = doAll;
		TESetStyle(mode, &theStyle, false, hTE);
	}

/*	// To work around some of the font sizing
	// problems let's try to puppet string the
	// right result. 
	//DebugStr("\pSetting size to 18 point." );

	TESetSelect(0,32000,((TextDataPtr) pData)->hTE);
	{
		TEHandle        hTE = ((TextDataPtr) pData)->hTE;
                short           mode = doAll;
		short		saveSize;
                TextStyle       theStyle;

                TEContinuousStyle(&mode, &theStyle, hTE);
                if (theStyle.tsSize == 0)
                        theStyle.tsSize = GetDefFontSize();
                if (theStyle.tsFont == systemFont)
                        theStyle.tsFont = GetSysFont();
                if (theStyle.tsFont == applFont)
                        theStyle.tsFont = GetAppFont();

		saveSize = theStyle.tsSize;
		theStyle.tsSize = 18;
                TESetStyle(doSize, &theStyle, true, hTE);
		TECalText(((TextDataPtr) pData)->hTE);
       		AdjustTE(pData, false);

		//theStyle.tsSize = saveSize;
                //TESetStyle(doSize, &theStyle, true, hTE);
		//TECalText(((TextDataPtr) pData)->hTE);
       		//AdjustTE(pData, false);
	}
	TESetSelect(0,0,((TextDataPtr) pData)->hTE);

	//DebugStr( "\pDone setting size.\n" ); */

} // TextFinishTESetup

// --------------------------------------------------------------------------------------------------------------

static void		TextCloseStationery(WindowDataPtr pData)
{
	// if stationery, use untitled and close down the files
	if (pData->originalFileType == 'sEXT')
	{
		pData->originalFileType = 'TEXT';
		pData->openAsNew = true;
		if (pData->resRefNum != -1)
			CloseResFile(pData->resRefNum);
		if (pData->dataRefNum != -1)
			FSClose(pData->dataRefNum);
		pData->resRefNum = pData->dataRefNum = -1;
	}
		
} // TextCloseStationery

// --------------------------------------------------------------------------------------------------------------
static int		FindVoicePosition(MenuHandle voicesMenu, short menuCount, VoiceDescription* pdesc)
{
	short	item;
	
	for ( item = 1; item <= menuCount; ++item )
	{
		Str255	itemText;
		
		GetMenuItemText( voicesMenu, item, itemText );
		/*1st > 2nd*/
		if ( PLstrcmp( itemText, pdesc->name ) == 1 )
			break;						// Found where name goes in list.
	}
		
	return item;
	
} // FindVoicePosition

// --------------------------------------------------------------------------------------------------------------

static void		TextAdd1Voice(short i, MenuHandle voicesMenu)
{
	VoiceSpec			spec;				// A voice to add to the menu.
	VoiceDescription	description;		// Info about a voice.
	short				item;

	if ( (GetIndVoice( i, &spec ) == noErr)  &&
		 (GetVoiceDescription( &spec, &description, sizeof(description) ) == noErr ) )
	{
		short	menuCount = CountMenuItems( voicesMenu );
		
		// first one we are adding == get rid of item already there
		if ( (i == 1)  && (menuCount > 0) )
		{
			DeleteMenuItem( voicesMenu, 1 );
			--menuCount;
		}
			
		item = FindVoicePosition(voicesMenu, menuCount, &description);

		InsertMenuItem( voicesMenu, "\p ", item - 1 );
		SetMenuItemText( voicesMenu, item, description.name );
		
		MacCheckMenuItem(voicesMenu, item, 
			((gCurrentVoice.creator == spec.creator) && (gCurrentVoice.id == spec.id)) );
	}
		
} // TextAdd1Voice

// --------------------------------------------------------------------------------------------------------------

void	TextAddVoices()
{
#pragma unused(threadParam)

	short	theVoiceCount;
	
	if (gAddedVoices)
		return;
	
	if (gMachineInfo.haveTTS && CountVoices( &theVoiceCount ) == noErr)
	{
		OSStatus		anErr;
		VoiceDescription	description;		// Info about a voice.
		MenuHandle		voicesMenu = GetMenuHandle(mVoices);
		
		anErr = GetVoiceDescription( nil, &description, sizeof(description) );
		if (anErr == noErr)
		{
			int		i;
			
			gCurrentVoice = description.voice;
			
			for (i = 1; i <= theVoiceCount; ++i)
			{
				TextAdd1Voice(i, voicesMenu);
			}
		}
	}
	
	gAddedVoices = true;

//	return 0;	
} // TextAddVoices

// --------------------------------------------------------------------------------------------------------------

static OSStatus	TextMakeWindow(WindowPtr pWindow, WindowDataPtr pData)
{
//#pragma unused(pWindow)

	OSStatus	anErr;
	
	anErr = TextInitData(pData);
	nrequire(anErr, InitData);
	
	anErr = TextReadDataFork(pData);
	nrequire(anErr, ReadData);
	
	TextReadResourceFork(pData);
	TextFinishTESetup(pData);
	TextCloseStationery(pData);
	
	if(gMachineInfo.haveProxyIcons)
	{
		// Set the view size of the scrollbar to the textedit view size (for proportional scrolling)
		SetControlViewSize( pData->vScroll, pData->contentRect.bottom - pData->contentRect.top );

		// make the document happy
		SetWindowProxyCreatorAndType( pWindow, 'ttxt', 'TEXT', kOnSystemDisk );
	}

	return noErr;
	
// EXCEPTION HANDLING
ReadData:
	TEDispose(((TextDataPtr) pData)->hTE);
InitData:
	return anErr;
	
} // TextMakeWindow

// --------------------------------------------------------------------------------------------------------------

OSStatus	TextPreflightWindow(PreflightPtr pPreflightData)
{	
	pPreflightData->continueWithOpen 	= true;
	pPreflightData->wantVScroll			= true;
	pPreflightData->doZoom				= true;
	pPreflightData->makeProcPtr 		= TextMakeWindow;
	if (pPreflightData->fileType != 'ttro')
		pPreflightData->openKind			= fsRdWrPerm;
	
	pPreflightData->storageSize 		= sizeof(TextDataRecord);

	// get strings that mark the picture
	GetIndString(gPictMarker1, kTextStrings, iPictureMarker1);
	GetIndString(gPictMarker2, kTextStrings, iPictureMarker2);

	// do we need to account for bugs in older TEs?
	{
	long	version;
	
	if ( (Gestalt(gestaltTextEditVersion, &version) == noErr) && (version > gestaltTE5) )
		gTE6Version = true;
	}
	
	return noErr;
	
} // TextPreflightWindow

// --------------------------------------------------------------------------------------------------------------

void TextGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes)
{
        // What is OS X TextEdit type?

	pFileTypes[*numTypes] 		= 'TEXT';
	pDocumentTypes[*numTypes] 	= kTextWindow;
	(*numTypes)++;
		
	pFileTypes[*numTypes] 		= 'ttro';
	pDocumentTypes[*numTypes] 	= kTextWindow;
	(*numTypes)++;
		
	pFileTypes[*numTypes] 		= 'sEXT';
	pDocumentTypes[*numTypes] 	= kTextWindow;
	(*numTypes)++;
		
} // TextGetFileTypes

// --------------------------------------------------------------------------------------------------------------



// TextAddContentsMenu checks if there is a contents list and, if there
// is, creates a new menu handle for the contents list and fills it with 
// the appropriate visible items

void TextAddContentsMenu(WindowDataPtr pData)
{
	MenuHandle	contentsMenu;
	Str255		menuStr;
	short		totalItems;
	short		index;
	OSStatus		err;
	
	contentsMenu = GetMenuHandle(mContents);
	require(contentsMenu == nil, ContentsMenuAlreadyInstalled);
	
	// Is there a contents list?  If so, get the menu name 
	// and the number of items in the list
	
	if (TextGetContentsListItem(pData, 0, menuStr, nil, &totalItems) == noErr)
	{
		
		// create the menu and fill it with all the items
		// listed in the string list resource
		
		contentsMenu = NewMenu(mContents, menuStr);
		require(contentsMenu != nil, CantCreateContentsMenu);
		
		for (index = 1; index < totalItems; index++)
		{
			err = TextGetContentsListItem(pData, index, menuStr, nil, nil);
			require(err == noErr, CantGetItem);
			
			AppendMenu(contentsMenu, menuStr);
		}

		// add the menu to the menu bar, and redraw the menu bar
		InsertMenu(contentsMenu, 0);
		InvalMenuBar();
	}
	else
	{
		// no contents, do nothing
	}
	
	return;
	
// error handling
CantGetItem:
CantCreateContentsMenu:
	
	if (contentsMenu) 	DisposeMenu(contentsMenu);

ContentsMenuAlreadyInstalled:

	return;
		
} // TextAddContentsMenu




// TextRemoveContentsMenu removes the contents menu, if any,
// and redraws the menu bar

static void TextRemoveContentsMenu(WindowDataPtr pData)
{
#pragma unused (pData)

	MenuHandle	contentsMenu;
	
	contentsMenu = GetMenuHandle(mContents);
	if (contentsMenu)
	{
		DeleteMenu(mContents);
		DisposeMenu(contentsMenu);
		InvalMenuBar();
	}

} // TextRemoveContentsMenu



// TextGetContentsListItem is a general utility routine for examining the
// contents menu list, returning the menu and search strings, and returning
// the total number of entries in the contents list.
//
// Pass 0 as itemNum to retrieve the strings for the contents menu title.
//
// Pass nil for menuStr, searchStr, or totalItems if you don't want that
// info returned.
//
// Returns eDocumentHasNoContentsEntries if there is no contents string list
// resource for the specified window

static OSStatus TextGetContentsListItem(WindowDataPtr pData, short itemNum, 
							  StringPtr menuStr, StringPtr searchStr, 
							  short *totalItems)
{

	OSStatus	err;
	short	oldResFile;
	short	menuItemNum;
	short	searchItemNum;
	Handle	contentsStrListHandle;
	
	// if no original resource file, don't bother
	if (pData->resRefNum == -1)	
	{
		return eDocumentHasNoContentsEntries;
	}
		
	err = noErr;
	
	oldResFile = CurResFile();
	UseResFile(pData->resRefNum);
	
	// two entries per item
	//
	// first (itemNum zero) is content menu title
	// (second -- itemNum one, search string for menu title -- is unused)
	
	menuItemNum = itemNum * 2 + 1;
	searchItemNum = menuItemNum + 1;
	
	contentsStrListHandle = Get1Resource('STR#', kContentsListID);
	if (contentsStrListHandle)
	{
		if (totalItems)	*totalItems = (*(short *)*contentsStrListHandle) / 2;

		if (menuStr)	GetIndString(menuStr, kContentsListID, menuItemNum);
		if (searchStr)	
		{
			GetIndString(searchStr, kContentsListID, searchItemNum);
		
			if (searchStr[0] == 0)
			{
				// search string was empty, so use the
				// menu string as the search string
				GetIndString(searchStr, kContentsListID, menuItemNum);
			}
		}
	}
	else
	{
		err = eDocumentHasNoContentsEntries;
		if (totalItems)	*totalItems = 0;
	}
	
	UseResFile(oldResFile);
	
	return err;
} // TextGetContentsListItem


// TextAdjustContentsMenu enables the items in the contents menu
//
// This routine is essentially a placeholder in case the contents
// menu really were to be dynamically enabled.

static OSStatus TextAdjustContentsMenu(WindowDataPtr pData)
{
#pragma unused (pData)
	
	EnableCommand(cSelectContents);
	return(noErr);
	
} // TextAdjustContentsMenu

// --------------------------------------------------------------------------------------------------------------

OSStatus MacSeqSetDefaultText(WindowPtr pWindow, WindowDataPtr pData)
{
        Str255		itemName;
        Str255		defaultText;
        TextStyle	theStyle;
        
        // For single-style TextEdit records, the default font has already been
        // selected when they are created, so this step is really unnecessary.
        // We do it anyway out of susperstition...
        
        // Default mono-spaced font = Monaco
        strcpy(itemName,"\pMonaco");
        GetFNum( itemName, &theStyle.tsFont );
        TESetStyle( doFont, &theStyle, true, ((TextDataPtr) pData)->hTE );
        TECalText(((TextDataPtr) pData)->hTE);

	theStyle.tsSize = 12;
	TESetStyle(doSize, &theStyle, true, ((TextDataPtr) pData)->hTE);
	TECalText(((TextDataPtr) pData)->hTE);

        // Insert Pascal string into TextEdit buffer
        strcpy(defaultText,"\p* ThingM Sequencer Script for Mac OS X\r\r");
        MacSeqInsertText(pWindow, pData, defaultText);
        
        return noErr;
}

// --------------------------------------------------------------------------------------------------------------

OSStatus MacSeqInsertText(WindowPtr pWindow, WindowDataPtr pData, Str255 pStr)
{
	OSStatus	anErr = noErr;
	TEHandle	hTE = ((TextDataPtr) pData)->hTE;

        // Insert Pascal string into TextEdit buffer
        TEInsert(&pStr[1], pStr[0], hTE);
        
        // Auto scroll TextEdit record
        TESelView(hTE);
        // Update window state 
        AdjustScrollBars(pWindow, true, true, nil);
        // Update doc state 
        SetDocumentContentChanged( (WindowDataPtr) pData, true );	

        return noErr;
}

// --------------------------------------------------------------------------------------------------------------

OSStatus MacSeqExtractText(WindowDataPtr pData, char **pText, short *pSize, short *pNumLines, short **pLineStarts)
{
	OSStatus	anErr = noErr;
	TEHandle	hTE = ((TextDataPtr) pData)->hTE;
        TERec		*pTE;
        short		size, lines;
        CharsHandle	hchar;
        char		*pchar;
        char		*pcopy;
        short		*pindexes;
        
        if (hTE == nil || *hTE == nil)
            return anErr - 1;
            
        // The TextEdit record is always updated into one contiguous
        // buffer with line numbers and line start indexes already
        // parsed into Pascal-friendly string arrays. Since the TextEdit
        // record can be dynamically updated for subsequent edits,
        // we just grab a copy of the current text buffer which we
        // will re-parse at line-breaks ourselves. We grab a copy of
        // the line start indexes too to match. 
        pTE = (TERec*)*hTE;
        size = pTE->teLength;
        lines = pTE->nLines;
        hchar = TEGetText(hTE);
        pchar = (char*)*hchar;

#if 0
        {
        short		n, count, delta;
        Str255		buf;

        for (n = count = 0; n < lines; n++) {
            delta = pTE->lineStarts[n+1] - pTE->lineStarts[n];
            memcpy(&buf, &pchar[pTE->lineStarts[n]], delta);
            fprintf(stdout, "%s\n", buf);
            count += delta;
            if (count > size)
                break;
            }
        }    
#endif

        // The allocated buffers which are passed back to the caller
        // will need to be subsequently freed by the caller.    
        pchar = (char*)*hchar;
        pcopy = (char*)malloc(size);
        memcpy(pcopy, pchar, size);
        pindexes = (short*)malloc(lines * sizeof(short));
        memcpy(pindexes, &pTE->lineStarts[0], lines * sizeof(short));
        
        *pText = pcopy;
        *pSize = size;
        *pNumLines = lines;
        *pLineStarts = pindexes;

        return noErr;
}

