/*
	File:		TextDrag.c

	Contains:	Text document dragging support for SimpleText

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

// --------------------------------------------------------------------------------------------------------------
// FORWARD EXTERNAL DECLARES
// --------------------------------------------------------------------------------------------------------------

extern Boolean IsOnlyThisFlavor(DragReference theDragRef, FlavorType theType);
extern Boolean IsDropInFinderTrash(AEDesc *dropLocation);
extern OSStatus SaveCurrentUndoState(WindowDataPtr pData, short newCommandID);

// --------------------------------------------------------------------------------------------------------------
// INTERNAL DEFINES
// --------------------------------------------------------------------------------------------------------------

// Dragging private globals

extern Boolean	gCanAccept;
extern Boolean	gTE6Version;

// --------------------------------------------------------------------------------------------------------------
// GLOBALS USED ONLY BY THESE ROUTINES
// --------------------------------------------------------------------------------------------------------------

static Boolean	gCaretShow;
static long		gCaretTime;
static short	gCaretOffset, gLastOffset, gInsertPosition;
static Boolean	gCursorInContent;

static unsigned long gAutoScrollTicks;

// --------------------------------------------------------------------------------------------------------------
// INTERNAL ROUTINES
// --------------------------------------------------------------------------------------------------------------

extern void AdjustTE(WindowDataPtr pData, Boolean doScroll);

// --------------------------------------------------------------------------------------------------------------
//
// GetSelectionSize -
//

static short GetSelectionSize(TEHandle hTE)
{

	return((**(hTE)).selEnd - (**(hTE)).selStart);

} // GetSelectionSize


// --------------------------------------------------------------------------------------------------------------
//
// GetSelectedTextPtr
//

static Ptr GetSelectedTextPtr(TEHandle hTE)
{

	return((*(**(hTE)).hText) + (**(hTE)).selStart);

} // GetSelectedTextPtr


// --------------------------------------------------------------------------------------------------------------
//
//	TEIsFrontOfLine - Given a text offset and a TextEdit handle, returns true if the given
//					  offset is at the beginning of a line start.
//

static short TEIsFrontOfLine(short textOffset, TEHandle hTE)
{

	short theLine = 0;

	if ((**hTE).teLength == 0)
		return true;

	if (textOffset >= (**hTE).teLength)
		return( (*((**hTE).hText))[(**hTE).teLength - 1] == 0x0D );

	while ((**hTE).lineStarts[theLine] < textOffset)
		theLine++;

	return( (**hTE).lineStarts[theLine] == textOffset );

} // TEIsFrontOfLine


// --------------------------------------------------------------------------------------------------------------
//
//	TEGetLine - Given an offset and a TextEdit handle, returns the line number that contains the offset.
//

static short TEGetLine(short textOffset, TEHandle hTE)
{	

	short theLine = 0;

	if (textOffset > (**hTE).teLength)
		return((**hTE).nLines);
	else
		{
		while ((**hTE).lineStarts[theLine] <= textOffset)
			++theLine;
		}
		
	return theLine;

} // TEGetLine


// --------------------------------------------------------------------------------------------------------------
//
//	DrawCaret - Draws a caret in a TextEdit field at the given offset by inverting the image of the
//				caret onto the screen. DrawCaret expects the port to be set to the port that the
//				TextEdit record is in.
//

static void DrawCaret(short textOffset, TEHandle hTE)
{	

	Point theLoc;
	short lineHeight, theLine;

	//
	//	Get the coordinates and the line of the offset to draw the caret.
	//

	theLoc  = TEGetPoint(textOffset, hTE);
	theLine = TEGetLine(textOffset, hTE);
	
	if (!gTE6Version)
		{
		// %%% Most heinously bogus - 21-Dec-93 JM3
		//
		//	For some reason, TextEdit dosen't return the proper coordinates
		//	of the last offset in the field if the last character in the record
		//	is a carriage return. TEGetPoint returns a point that is one line
		//	higher than expected. The following code fixes this problem.
	
		// It has also been fixed for TE6
		
		if ((textOffset == (**hTE).teLength) && (*((**hTE).hText))[(**hTE).teLength - 1] == 0x0D)
			theLoc.v += TEGetHeight(theLine, theLine, hTE);
		}
	
	//
	//	Always invert the caret when drawing.
	//

	PenMode(patXor);

	//
	//	Get the height of the line that the offset points to.
	//

	lineHeight = TEGetHeight(theLine, theLine, hTE);

	//
	//	Draw the appropriate caret image.
	//

	MoveTo(theLoc.h - 1, theLoc.v - 1);
	Line(0, 1 - lineHeight);

	PenNormal();

} // DrawCaret


// --------------------------------------------------------------------------------------------------------------
//
//	HitTest - Given a point in global coordinates, HitTest returns an offset into the text if the
//			  point is inside the given TERecord. If the point is not in the text, HitTest returns
//			  -1.
//

static short HitTest(Point theLoc, TEHandle hTE)
{	

	WindowPtr	theWindow;
	short		textOffset = -1;

	if (FindWindow(theLoc, &theWindow) == inContent)
		{
		SetPortWindowPort (theWindow);
		GlobalToLocal(&theLoc);

		if (PtInRect(theLoc, &((** hTE).viewRect)) && PtInRect(theLoc, &((** hTE).viewRect)))
			{
			textOffset = TEGetOffset(theLoc, hTE);

			if ((TEIsFrontOfLine(textOffset, hTE)) && (textOffset) &&
				((*((** hTE).hText))[textOffset - 1] != 0x0D) &&
				(TEGetPoint(textOffset - 1, hTE).h < theLoc.h))
				{
				--textOffset;
				}
			}
		}

	return textOffset;

} // HitTest


// --------------------------------------------------------------------------------------------------------------
//
// GetCharAtOffset - Given a text offset and a TEHandle, returns the character located at that offset in
//					 the TERecord.
//

static char GetCharAtOffset(short offset, TEHandle hTE)
{

	if (offset < 0)
		return 0x0D;

	return(((char *) *((**hTE).hText))[offset]);

} // GetCharAtOffset


// --------------------------------------------------------------------------------------------------------------
//
// WhiteSpace - Determines if the input character is white space.
//

static Boolean WhiteSpace(char theChar)
{

	return((theChar == ' ') || (theChar == 0x0D));

} // WhiteSpace


// --------------------------------------------------------------------------------------------------------------
//
// WhiteSpaceAtOffset - Given a text offset into a TERecord, determines if the character at that location is
//						whitespace.
//

static Boolean WhiteSpaceAtOffset(short offset, TEHandle hTE)
{

	char theChar;

	if ((offset < 0) || (offset > (**hTE).teLength - 1))
		return true;

	theChar = ((char *) *((**hTE).hText))[offset];
	return(WhiteSpace(theChar));

} // WhiteSpaceAtOffset


// --------------------------------------------------------------------------------------------------------------
//
// InsertTextAtOffset -
//

static short InsertTextAtOffset(short textOffset, char *theBuf, long textSize, StScrpHandle styleHand, TEHandle hTE)
{
	short	charactersAdded = 0;
	
	if (textSize == 0)
		return charactersAdded;

	//	If we're inserting at the end of a word and the selection does not begin with
	//	a space, insert a space before the insertion.

	if (!WhiteSpaceAtOffset(textOffset - 1, hTE) &&
		 WhiteSpaceAtOffset(textOffset, hTE) &&
		!WhiteSpace(theBuf[0]))
		{

		TESetSelect(textOffset, textOffset, hTE);
		TEKey(' ', hTE);
		++textOffset;
		++charactersAdded;
		}

	//	If we're inserting at the beginning of a word and the selection does not end
	//	with a space, insert a space after the insertion.

	if ( WhiteSpaceAtOffset(textOffset - 1, hTE) &&
		!WhiteSpaceAtOffset(textOffset, hTE) &&
		!WhiteSpace(theBuf[textSize - 1]))
		{

		TESetSelect(textOffset, textOffset, hTE);
		TEKey(' ', hTE);
		++charactersAdded;
		}

	// Before we insert this text, make sure we set the selection range to a single character.
	// This assures that we won't overwrite the text in the previous selection.
	
	TESetSelect(textOffset, textOffset, hTE);
	TEStyleInsert(theBuf, textSize, styleHand, hTE);

	return charactersAdded;
	
} // InsertTextAtOffset

// --------------------------------------------------------------------------------------------------------------
// OOP INTERFACE ROUTINES
// --------------------------------------------------------------------------------------------------------------

OSStatus TextDragTracking(WindowPtr pWindow, void *pData, DragReference theDragRef, short message)
{
#pragma unused(pWindow)

	unsigned long	attributes;
	RgnHandle		hilightRgn;
	Point			localMouseLoc, dragMouseLoc;
	short			textOffset;
	long			theTime = TickCount();
	WindowDataPtr	theData = (WindowDataPtr) pData;
	Rect 			pRect; // (DP) Needed because of Opacity Issues


	GetDragAttributes(theDragRef, &attributes);

	switch(message)
		{
		case kDragTrackingEnterWindow:

			gCanAccept = IsOnlyThisFlavor(theDragRef, 'TEXT');

			gCaretTime   = theTime;
			gCaretOffset = gLastOffset = -1;
			gCaretShow   = true;

			gCursorInContent = false;
			gAutoScrollTicks = 0;

			break;		

		case kDragTrackingInWindow:

			if (gCanAccept)
				{			
				GetDragMouse(theDragRef, &dragMouseLoc, 0L);
				localMouseLoc = dragMouseLoc;
				GlobalToLocal(&localMouseLoc);

				if (attributes & kDragInsideSenderWindow)
					{
					short deltaV = 0;

					if ((localMouseLoc.v < 16) && (localMouseLoc.v > 0))
						deltaV = theData->vScrollAmount;

					if (localMouseLoc.v > GetPortBounds(GetWindowPort(pWindow), &pRect)->bottom - 16)
						deltaV -= theData->vScrollAmount;

					if (deltaV == 0)
						{
						gAutoScrollTicks = 0;
						}
					else
						{
						if (gAutoScrollTicks == 0)
							{
							gAutoScrollTicks = theTime;
							}
						else
							{
							if (theTime - gAutoScrollTicks > 10)	// 10 ticks to start is what the H.I. doc says
								{
								// remove the drag-destination caret if it's showing
								if (gCaretOffset != -1)
									{
									DrawCaret(gCaretOffset, ((TextDataPtr) pData)->hTE);
									gCaretOffset = -1;
									}
	
								SetControlAndClipAmount(theData->vScroll, &deltaV);
								if (deltaV != 0)
									{
									DragPreScroll(theDragRef, 0, deltaV);
									DoScrollContent(pWindow, theData, 0, deltaV);
									DragPostScroll(theDragRef);
									}
								
								gAutoScrollTicks = theTime - 7;	// let's wait 3 more ticks until next jump
								}
							}
						}
					}

				if (attributes & kDragHasLeftSenderWindow)
					{
					if (PtInRect(localMouseLoc, &(theData->contentRect)))
						{
						if (!gCursorInContent)
							{
							hilightRgn = NewRgn();
							RectRgn(hilightRgn, &theData->contentRect);
							ShowDragHilite(theDragRef, hilightRgn, true);					
							DisposeRgn(hilightRgn);
							}
						gCursorInContent = true;
						}
					else
						{
						if (gCursorInContent)
							{
							HideDragHilite(theDragRef);
							gCursorInContent = false;
							}
						}
					}
				}

			textOffset = HitTest(dragMouseLoc, ((TextDataPtr)pData)->hTE);

			//	If this application is the sender, do not allow tracking through
			//	the selection in the window that sourced the drag.

			if (attributes & kDragInsideSenderWindow)
				{
				if ((textOffset >= (**((TextDataPtr)pData)->hTE).selStart) &&
					(textOffset <= (**((TextDataPtr)pData)->hTE).selEnd))
					{
						textOffset = -1;
					}
				}

			gInsertPosition = textOffset;

			//	Reset the flashing counter if the offset has moved. This makes the
			//	caret blink only after the caret has stopped moving long enough.

			if (textOffset != gLastOffset)
				{
				gCaretTime = theTime;
				gCaretShow = true;
				}
			
			gLastOffset = textOffset;

			//	Flash the caret, blinky-blinky-blinky.

			if (theTime - gCaretTime > GetCaretTime())
				{
				gCaretShow = !gCaretShow;
				gCaretTime = theTime;
				}
			
			if (!gCaretShow)
				textOffset = -1;

			//	If the caret offset has changed, move the caret on the screen.

			if (textOffset != gCaretOffset)
				{
				if (gCaretOffset != -1)
					DrawCaret(gCaretOffset, ((TextDataPtr)pData)->hTE);

				if (textOffset != -1)
					DrawCaret(textOffset, ((TextDataPtr)pData)->hTE);
				}

			gCaretOffset = textOffset;
			break;

		case kDragTrackingLeaveWindow:
	
			//	If the caret is on the screen, remove it.

			if (gCaretOffset != -1)
				{
				DrawCaret(gCaretOffset, ((TextDataPtr)pData)->hTE);
				gCaretOffset = -1;
				}

			// Remove the window hilighting, if any.
	
			if ((gCursorInContent) && (attributes & kDragHasLeftSenderWindow))
				HideDragHilite(theDragRef);

			break;
			
	  	} // switch (message)
	
	return noErr;

} // TextDragTracking


// --------------------------------------------------------------------------------------------------------------

OSStatus TextDragReceive(WindowPtr pWindow, void *pData, DragReference theDragRef)
{

	OSStatus			error;
	unsigned short	items, index;
	DragAttributes	attributes;
	ItemReference	theItem;
	Ptr				textData;
	StScrpHandle	styleHand;
	Size			textSize, styleSize, totalTextSize;
	short			mouseDownModifiers, mouseUpModifiers, moveText, selStart, selEnd;
	long			totalTextStart;
	long			additionalChars;
	TEHandle		hTE;
	Boolean			wasActive;
	
	if ((!gCanAccept)  || (gInsertPosition == -1))
		return dragNotAcceptedErr;

	hTE = ((TextDataPtr) pData)->hTE;

	// We're going to try our best to insert some text, so first save off the beginning of where
	// we'll do it.

	totalTextStart = gInsertPosition;
	totalTextSize = 0L;

	// draw in this window, and activate the text editing record so that selections
	// happen properly

	SetPortWindowPort (pWindow);

	wasActive = (*hTE)->active != 0;	// can't test window == FrontWindow (might not be front app)
	if (!wasActive)
		TEActivate(hTE);

	GetDragAttributes(theDragRef, &attributes);
	GetDragModifiers(theDragRef, 0L, &mouseDownModifiers, &mouseUpModifiers);

	moveText = (attributes & kDragInsideSenderWindow) &&
			   (!((mouseDownModifiers & optionKey) | (mouseUpModifiers & optionKey)));

	//	Loop through all of the drag items contained in this drag and snag all of the 'TEXT'.

	CountDragItems(theDragRef, &items);
	
	for (index = 1; index <= items; index++)
		{
		//	Get the item's reference number, so we can refer to it.

		GetDragItemReferenceNumber(theDragRef, index, &theItem);

		//	Try to get the size for a 'TEXT' flavor. If this returns noErr,
		//	then we know that a 'TEXT' flavor exists in the item.
		
		error = GetFlavorDataSize(theDragRef, theItem, 'TEXT', &textSize);

		if (error == noErr)
			{		
			// If the current length, plus the drag data would make the document too large, say so.

			if (((*hTE)->teLength + textSize) > kMaxLength)
				return eDocumentTooLarge;

			textData = NewPtr(textSize);
	
			// If we couldn't get a chunk of memory for the text, bail.

			if(textData == 0L)
				return memFullErr;

			GetFlavorData(theDragRef, theItem, 'TEXT', textData, &textSize, 0);

			// Let's see if there is an optional 'styl' flavor.

			styleHand = 0L;
			
			error = GetFlavorDataSize(theDragRef, theItem, 'styl', &styleSize);

			// If there was no 'styl' data, or it somehow was zero in length, don't
			// attempt to insert it along with the text, 'cause we'd fail miserably.

			if ((error == noErr) && (styleSize != 0))
				{
				styleHand = (StScrpHandle) NewHandle(styleSize);
			
				// If we couldn't get a chunk of memory for the styles, also bail.

				if (styleHand == 0L)
					{	
					DisposePtr(textData);
					return memFullErr;
					}

				HLock((Handle) styleHand);
				GetFlavorData(theDragRef, theItem, 'styl', *styleHand, &styleSize, 0L);
				HUnlock((Handle) styleHand);			
				}

			//	If the caret or highlighting is on the screen, remove it/them.
	
			if (gCaretOffset != -1)
				{
				DrawCaret(gCaretOffset, hTE);
				gCaretOffset = -1;
				}
			
			if (attributes & kDragHasLeftSenderWindow)
				HideDragHilite(theDragRef);

			// save away any changes, so that we can undo them
			SaveCurrentUndoState(pData, cTypingCommand);
			
			// If this window is also the sender, delete the source selection text if the
			// option key is not being held down.

			if (moveText)
				{
				selStart = (**hTE).selStart;
				selEnd   = (**hTE).selEnd;
				
				if ( WhiteSpaceAtOffset(selStart - 1, hTE) &&
					!WhiteSpaceAtOffset(selStart,     hTE) &&
					!WhiteSpaceAtOffset(selEnd - 1,   hTE) &&
					 WhiteSpaceAtOffset(selEnd,       hTE))
					{
					 
					 if (GetCharAtOffset(selEnd, hTE) == ' ')
						(**hTE).selEnd++;
					}
				
				if (gInsertPosition > selStart)
					{
					selEnd = (**hTE).selEnd;
					gInsertPosition -= (selEnd - selStart);
					totalTextStart -= (selEnd - selStart);
					}
				
				TEDelete(hTE);	
				}

			// We can finally insert the text and style data into our record.

			additionalChars = InsertTextAtOffset(gInsertPosition, textData, textSize, styleHand, hTE);
			
			// In case we're inserting multiple chunks of text, we need to update the location of where we
			// need to insert the next block.

			gInsertPosition += textSize + additionalChars;
			totalTextSize += textSize + additionalChars;
			
			DisposePtr(textData);
			if (styleHand)
				DisposeHandle((Handle) styleHand);
			}
		}

	// Select everything we've just inserted.

	TESetSelect(totalTextStart, totalTextStart + totalTextSize, hTE);

	AdjustTE(pData, false);
	AdjustScrollBars(pWindow, false, false, nil);
	
	SetDocumentContentChanged( (WindowDataPtr) pData, true );

	// if we had to activate the edit record, deactivate it after we are all done
	if (!wasActive)
		TEDeactivate(hTE);

	return noErr;

} // TextDragReceive


// --------------------------------------------------------------------------------------------------------------

Boolean DragText(WindowPtr pWindow, void *pData, EventRecord *pEvent, RgnHandle hilightRgn)
{

	Point			theLoc = {0,0};
	RgnHandle		dragRegion, tempRegion;
	DragReference	theDragRef;
	StScrpHandle	theStyleHand = 0L;
	OSStatus			error;
	AEDesc			dropLocation;
	DragAttributes	attributes;
	short			mouseDownModifiers, mouseUpModifiers, copyText;

	//	Copy the hilight region into dragRegion and offset it into global coordinates.

	CopyRgn(hilightRgn, dragRegion = NewRgn());
	LocalToGlobal(&theLoc);
	OffsetRgn(dragRegion, theLoc.h, theLoc.v);

	//	Wait for the mouse to move or the mouse button to be released. If the mouse button was
	//	released before the mouse moves, return false. Returing false from DragText means that
	//	a drag operation did not occur.

	if (!WaitMouseMoved(pEvent->where))
		return false;

	NewDrag(&theDragRef);

	// add the flavor for the window title, errors can be ignored as this
	// is a cosmetic addition
	{
	enum
		{
		kFlavorTypeClippingName = 'clnm'
		};
	Str255	windowTitle;
	
	GetWTitle(pWindow, windowTitle);
	(void) 	AddDragItemFlavor(theDragRef, 1, kFlavorTypeClippingName, &windowTitle, windowTitle[0]+1, flavorNotSaved);
	}

	AddDragItemFlavor(theDragRef, 1, 'TEXT', GetSelectedTextPtr(((TextDataPtr)pData)->hTE), GetSelectionSize(((TextDataPtr)pData)->hTE), 0);

	theStyleHand = TEGetStyleScrapHandle(((TextDataPtr)pData)->hTE);

	// Just be a little paranoid and see if we did get a handle.

	if (theStyleHand)
		{
		HLock((Handle) theStyleHand);
		AddDragItemFlavor(theDragRef, 1, 'styl', (Ptr) *theStyleHand, GetHandleSize((Handle) theStyleHand), 0);
		DisposeHandle((Handle) theStyleHand);
		}

	//	Set the item's bounding rectangle in global coordinates.
		{
		Rect	dragBounds;
		
		SetDragItemBounds(theDragRef, 1, GetRegionBounds( dragRegion, &dragBounds ));
		}
		
	//	Prepare the drag region.

	tempRegion = NewRgn();
	CopyRgn(dragRegion, tempRegion);
	InsetRgn(tempRegion, 1, 1);
	DiffRgn(dragRegion, tempRegion, dragRegion);
	DisposeRgn(tempRegion);

	//	Drag the text. TrackDrag will return userCanceledErr if the drop whooshed back for any reason.

	error = TrackDrag(theDragRef, pEvent, dragRegion);
	
	if ((error != noErr) && (error != userCanceledErr))
		return true;

	//	Check to see if the drop occurred in the Finder's Trash. If the drop occurred
	//	in the Finder's Trash and a copy operation wasn't specified, delete the
	//	source selection. Note that we can continute to get the attributes, drop location
	//	modifiers, etc. of the drag until we dispose of it using DisposeDrag.

	GetDragAttributes(theDragRef, &attributes);
	if (!(attributes & kDragInsideSenderApplication))
		{
		GetDropLocation(theDragRef, &dropLocation);

		GetDragModifiers(theDragRef, 0L, &mouseDownModifiers, &mouseUpModifiers);
		copyText = (mouseDownModifiers | mouseUpModifiers) & optionKey;

		if ((!copyText) && (IsDropInFinderTrash(&dropLocation)))
			{
			TEDelete(((TextDataPtr)pData)->hTE);
			AdjustTE(pData, false);
			AdjustScrollBars(pWindow, false, false, nil);
			SetDocumentContentChanged( (WindowDataPtr) pData, true );
			}

		AEDisposeDesc(&dropLocation);
		}

	// Dispose of this drag, 'cause we're done.

	DisposeDrag(theDragRef);

	DisposeRgn(dragRegion);

	return true;

} // DragText
