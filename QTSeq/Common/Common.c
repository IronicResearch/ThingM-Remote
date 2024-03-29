/*
	File:		Common.c
	
	Description: HackTV cross-platform common code
				 Refer to develop Issue 13, "Video Digitizing Under QuickTime",
				 for details on this code.

	Author:		QuickTime Engineering

	Copyright: 	� Copyright 1992-2001 Apple Computer, Inc. All rights reserved.
	
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
				
	Change History (most recent first):

	Revision 1.11	4/10/2001	updated for UI3.4 and X EA DTS
					2001/02/13	Change the window title to 'preallocating' while calling
								SGStartRecord so the user will know that the allocation is going on TC
	Revision 1.10  	2001/02/13	will now adjust the current bounds when resizing the window --
								now DV previews in full screen TC
	Revision 1.9  	2000/11/20	Fix recording wihtout sitting in a Button() loop with Nav services
								enabled; add more error reporting via the Monitor window title. KM
	Revision 1.7	2000/11/20	Fix the recording state in async record case KM
	Revision 1.6  	2000/03/06	Added #defines to facilitate testing SGOutput APIs BF
	Revision 1.5  	2000/03/01 	add more sized, recording on idle KM	
	Revision 1.4  	2000/02/25 	menu stuff TD
	Revision 1.3  	1999/12/15	carbonized AB
	Revision 1.2  	Original	GW
*/

#include "carbonprefix.h"
	
#if (TARGET_OS_MAC && TARGET_API_MAC_CARBON)
	#if __APPLE_CC__
		#include <Carbon/Carbon.h>
	#elif __MWERKS__
		#include <Carbon.h>
	#else
		#error "I'm confused!"
	#endif	
#else
	#include <QTML.h>
	#include <Endian.h>
	#include <Menus.h>
	#include <Printing.h>
	#include <Script.h>
	#include <Scrap.h>
	#include <QuickTimeComponents.h>
	#include <NumberFormatting.h>
#endif

#include <stdio.h>
#include <string.h>

#include "Globals.h"
#include "Common.h"

#if TARGET_OS_MAC
	#define USE_NAV_SERVICES 1
#else
	#define USE_NAV_SERVICES 0
#endif

#if (USE_NAV_SERVICES && !TARGET_API_MAC_CARBON)
	#include <Navigation.h>
#endif

#define SET_DATA_OUTPUT_MAX_OFFSET 0	// Set to 1 to call SGSetOutputMaximumOffset()
#define LINK_MULTIPLE_OUTPUTS 0			// Set to 1 to link multiple data outputs for split track files
#define CREATE_DATA_OUTPUTS 0			// Set to 1 to create data output for non-split movie.
#define SHOW_SPACE_REMAINING 0			// Set to 1 to call SGGetDataOutputStorageSpaceRemaining64()

// Disable warnings associated with "\p" strings
#if TARGET_OS_WIN32
	#pragma warning(disable: 4129)
#endif

static OSErr XorRectToRgn(Rect *srcRectA, Rect *srcRectB, RgnHandle *destRgn);
void ChangeWindowTitle(WindowPtr pWin, int width, int height, char* szExtraText);
/* ---------------------------------------------------------------------- */

void InitializeSequenceGrabber(void)
{
	ComponentDescription	theDesc;
	ComponentResult			result = noErr;
	GrafPtr					savedPort;
	Component				sgCompID;
	
	gQuitFlag = false;
	gSeqGrabber = 0L;
	gVideoChannel = 0L;
	gSoundChannel = 0L;
	gMonitorPICT = NULL;
#if !TARGET_API_MAC_CARBON
	gPrintRec = (THPrint) NewHandleClear (sizeof (TPrint));
#endif
	gCurrentlyRecording = false;
	
	// Find and open a sequence grabber
	theDesc.componentType = SeqGrabComponentType;
	theDesc.componentSubType = 0L;
	theDesc.componentManufacturer = 'appl';
	theDesc.componentFlags = 0L;
	theDesc.componentFlagsMask = 0L;	
	sgCompID = FindNextComponent (NULL, &theDesc);
	if (sgCompID != 0L)
		gSeqGrabber = OpenComponent (sgCompID);
	
	// If we got a sequence grabber, set it up
	if (gSeqGrabber != 0L)
	{
		// Get the monitor
		CreateMonitorWindow();
		if (gMonitor != NULL)
		{
			// Display the monitor window
			GetPort (&savedPort);
			MacSetPort ((GrafPtr)GetWindowPort(gMonitor));
			MacMoveWindow(gMonitor, 10, 30 + GetMBarHeight(), 0);
			MacShowWindow (gMonitor);		

			// Initialize the sequence grabber
			result = SGInitialize (gSeqGrabber);
			CheckError(result,"SGInitialize");
			if (result == noErr)
			{
				result = SGSetGWorld (gSeqGrabber, GetWindowPort(gMonitor), NULL);
				CheckError(result,"SGSetGWorld");
				
				// Get a video channel
				result = SGNewChannel (gSeqGrabber, VideoMediaType, &gVideoChannel);
				CheckError(result,"SGNewChannel for video");
				if ((gVideoChannel != NULL) && (result == noErr))
				{
					short	width;
					short	height;
					
                                        // Default to full size
					gQuarterSize = false;
					gHalfSize = false;
					gFullSize = true;
					
					result = SGGetSrcVideoBounds (gVideoChannel, &gActiveVideoRect);
					CheckError(result,"SGGetSrcVideoBounds");
					width = (gActiveVideoRect.right - gActiveVideoRect.left) / 1;
					height = (gActiveVideoRect.bottom - gActiveVideoRect.top) / 1;
					SizeWindow (gMonitor, width, height, false);

                                        // Identify Monitor window size
                                        ChangeWindowTitle(gMonitor, width, height, "");		
					
					result = SGSetChannelUsage (gVideoChannel, seqGrabPreview | seqGrabRecord | seqGrabPlayDuringRecord);
					CheckError(result,"SGSetChannelUsage for vid");
				#if TARGET_API_MAC_CARBON
					{
						Rect portRect;
					
						GetPortBounds(GetWindowPort(gMonitor), &portRect);
					
						result = SGSetChannelBounds (gVideoChannel, &portRect);
					}
				#else	
					result = SGSetChannelBounds (gVideoChannel, &(gMonitor->portRect));
				#endif
					CheckError(result,"SGSetChannelBounds");
				}
				
				// Get a sound channel
				result = SGNewChannel (gSeqGrabber, SoundMediaType, &gSoundChannel);
				CheckError(result,"SGNewChannel for sound");

				if ((gSoundChannel != NULL) && (result == noErr))
				{
					if (gSoundChannel != NULL)
					{
						Handle sampleRates = NULL;
						result = SGSetChannelUsage (gSoundChannel, seqGrabPreview | seqGrabRecord);
						CheckError(result,"SGSetChannelUsage for sound");
						
						// Set the volume low to prevent feedback when we start the preview,
						// in case the mic is anywhere near the speaker.
						result = SGSetChannelVolume (gSoundChannel, 0x0010);
						CheckError(result,"SGSetChannelVolume ");
						//result = SGSetSoundRecordChunkSize(gSoundChannel, -((Fixed)(.25*65536.0)));
						sampleRates = NewHandleClear(5*sizeof(Fixed));
						if(sampleRates)
							{
							OSErr tempErr;
							*(long*)(*sampleRates) = 8000<<16; // add 8kHz rate
							*((long*)(*sampleRates)+1) = 11025<<16; // add 11kHz rate
							*((long*)(*sampleRates)+2) = 16000<<16; // add 16kHz rate
							*((long*)(*sampleRates)+3) = 22050<<16; // add 22kHz rate
							*((long*)(*sampleRates)+4) = 32000<<16; // add 32kHz rate
							tempErr = SGSetAdditionalSoundRates(gSoundChannel,sampleRates);
							CheckError(result,"SGSetAdditionalSoundRates ");
							DisposeHandle(sampleRates);
							}	
					}
				}
				
				// Get the alignment proc (for use when dragging the monitor)
				result = SGGetAlignmentProc (gSeqGrabber, &gSeqGrabberAlignProc);
				CheckError(result,"SGGetAlignmentProc ");
			}
			
			// Go!
			if (result == noErr) {
				result = SGStartPreview (gSeqGrabber);
				CheckError(result,"SGStartPreview ");
                            }
			MacSetPort (savedPort);
		}
	}
}

/* ---------------------------------------------------------------------- */

// Specify and setup a file to contain this track's data
static ComponentResult SetTrackFile(SGChannel theChannel, StringPtr prompt, StringPtr defaultName)
{
	FSSpec theFile;
#if USE_NAV_SERVICES
#pragma unused(prompt, defaultName)

	NavReplyRecord reply;
	NavDialogOptions myOptions;
	AEKeyword myKeyword;
	DescType myActualType;
	Size myActualSize;
#else
	StandardFileReply		reply;
#endif
	ComponentResult		err;
	SGOutput	theOutput;
	AliasHandle	alias = 0;

	// Get the destination filename
#if USE_NAV_SERVICES
	err = NavGetDefaultDialogOptions(&myOptions);
	err = NavPutFile(NULL, &reply, &myOptions, NULL, 'MooV', 'TVOD', NULL);
	
	err = AEGetNthPtr(&(reply.selection), 1, typeFSS, &myKeyword, &myActualType, &theFile,
							sizeof(theFile), &myActualSize);
							
	if (err != noErr || !reply.validRecord)
		goto bail;
#else
	StandardPutFile(prompt, defaultName, &reply);
	if (!reply.sfGood)
	{
		err = fnfErr;
		goto bail;
	} else
		theFile = reply.sfFile;
#endif

	// Make an alias from the filename
	if ((err = QTNewAlias(&theFile, &alias, true))) goto bail;
	
	// Create an output from this file
	if ((err = SGNewOutput(gSeqGrabber, (Handle)alias, rAliasType, seqGrabToDisk, &theOutput))) goto bail;

	// Associate this output with the specified channel
	if ((err = SGSetChannelOutput(gSeqGrabber, theChannel, theOutput))) goto bail;

#if SET_DATA_OUTPUT_MAX_OFFSET
	// Set data output maximums
	{
		wide	maxOffset;
		maxOffset.lo = 1000 * 1024;
		maxOffset.hi = 0;
		SGSetOutputMaximumOffset(gSeqGrabber, theOutput, &maxOffset);
	}
#endif

#if LINK_MULTIPLE_OUTPUTS
	// Link multiple outputs
	{
		SGOutput	output2;
		StandardPutFile("\pLink this file", defaultName, &reply);
		if (reply.sfGood) {
			theFile = reply.sfFile;
			if (err = QTNewAlias(&theFile, &alias, true)) goto bail;
			
			// Create an output from this file
			if (err = SGNewOutput(gSeqGrabber, (Handle)alias, rAliasType, seqGrabToDisk, &output2)) goto bail;

#if SET_DATA_OUTPUT_MAX_OFFSET
			// Set data output maximums
			{
				wide	maxOffset;
				maxOffset.lo = 1000 * 1024;
				maxOffset.hi = 0;
				SGSetOutputMaximumOffset(gSeqGrabber, output2, &maxOffset);
			}
#endif
			SGSetOutputNextOutput(gSeqGrabber, theOutput, output2);
		}
	}
#endif

bail:
	if (alias) DisposeHandle((Handle)alias);
#if USE_NAV_SERVICES
	NavDisposeReply(&reply);
#endif
	return err;
}

/* ---------------------------------------------------------------------- */
#if USE_NAV_SERVICES
static	NavReplyRecord reply;
static	NavDialogOptions myOptions;
#endif
static	FSSpec theFile;
static	Str255 szFileName;
// Record a movie
void DoRecord(Boolean useButtonLoop)
{
	long	err;
//	FSSpec theFile;
#if USE_NAV_SERVICES
//	NavDialogOptions myOptions;
	AEKeyword myKeyword;
	DescType myActualType;
	Size myActualSize;
#else
	StandardFileReply	reply;
#endif
	if (!gCurrentlyRecording)
		{
		// Stop everything while the dialogs are up
		SGStop(gSeqGrabber);

		// Get the destination filename
#if USE_NAV_SERVICES
		reply.validRecord = false;
		err = NavGetDefaultDialogOptions(&myOptions);
                // Add file name to default dialog options
                strcpy(&myOptions.savedFileName[0], szFileName);
		err = NavPutFile(NULL, &reply, &myOptions, NULL, 'MooV', 'TVOD', NULL);
		
		err = AEGetNthPtr(&(reply.selection), 1, typeFSS, &myKeyword, &myActualType, &theFile,
								sizeof(theFile), &myActualSize);
								
		if (err == noErr && reply.validRecord)
			if ((err = SGSetDataOutput(gSeqGrabber, &theFile, seqGrabToDisk)))
				goto bail;
#else
		StandardPutFile("\pSave new movie file as:", "\pHack.mov", &reply);
		if (!reply.sfGood)
		{
			err = fnfErr;
			goto bail;
		} else
			theFile = reply.sfFile;
			
		if ((err = SGSetDataOutput(gSeqGrabber, &theFile, seqGrabToDisk)))
			goto bail;
#endif

#if CREATE_DATA_OUTPUTS
		{
		AliasHandle	alias;
		SGOutput	theOutput;

		StandardPutFile("\pPlace data in:", "\p1.trk", &reply);
		if (!reply.sfGood)
		{
			err = fnfErr;
			goto bail;
		} else
			theFile = reply.sfFile;

		// Make an alias from the filename
		if (err = QTNewAlias(&theFile, &alias, true)) goto bail;
		
		// Create an output from this file
		if (err = SGNewOutput(gSeqGrabber, (Handle)alias, rAliasType, seqGrabToDisk, &theOutput)) {
			CheckError(err,"SGNewOutput(gSeqGrabber, ");
			goto bail;
		}

		// Associate this output with the specified channel
		if (gVideoChannel)
			if (err = SGSetChannelOutput(gSeqGrabber, gVideoChannel, theOutput)) {
				CheckError(err,"SGSetChannelOutput vid");
				goto bail;
			}
		if (gSoundChannel)
			if (err = SGSetChannelOutput(gSeqGrabber, gSoundChannel, theOutput)) {
				CheckError(err,"SGSetChannelOutput sound");
				goto bail;
			}
		gOutput = theOutput;

	#if SET_DATA_OUTPUT_MAX_OFFSET
		// Set data output maximums
		{
			wide	maxOffset;
			maxOffset.lo = 1000 * 1024;
			maxOffset.hi = 0;
			SGSetOutputMaximumOffset(gSeqGrabber, theOutput, &maxOffset);
		}
	#endif
		}
#endif
		// Ask use for separate video and sound track files if requested
		if (gSoundChannel && gRecordSound && gVideoChannel && gRecordVideo && gSplitTracks)
		{
			if ((err = SetTrackFile(gVideoChannel, "\pSave video track file as:", "\pHackVideo.trk")))
				goto bail;
			if ((err = SetTrackFile(gSoundChannel, "\pSave sound track file as:", "\pHackSound.trk")))
				goto bail;
		}

		// If not recording sound or video, then "disable" those channels
		if (gSoundChannel && !gRecordSound)
			SGSetChannelUsage(gSoundChannel, 0);
		if (gVideoChannel && !gRecordVideo)
			SGSetChannelUsage(gVideoChannel, 0);

		// Attempt to recover the preview area obscured by dialogs
	#if TARGET_OS_WIN32
		UpdatePort(gMonitor);
	#endif
		SGUpdate(gSeqGrabber, 0);

		// Startup the grab
		SGPause(gSeqGrabber, false);

		// Make the movie file
		DeleteMovieFile(&theFile);
		if ((err = CreateMovieFile(&theFile, 'TVOD', smSystemScript,
			createMovieFileDontOpenFile | createMovieFileDontCreateMovie | createMovieFileDontCreateResFile,
			NULL, NULL))) goto bail;

		// Record!
		// Set the window title so that the user knows that we are recording.
		// The major delay when calling SGStartRecord is the preallocation, so
		// show that.
		SetWTitle(gMonitor, "\pStarting Record... preallocating file...");

		err = SGStartRecord(gSeqGrabber);
		CheckError(err,"SGStartRecord");
		if (err) goto bail;
		
		// Reset the window title to the default.
		SetWTitle(gMonitor, "\pMonitor");
		
		gCurrentlyRecording = true;
		SGUpdate(gSeqGrabber, 0);

		if(useButtonLoop)
			{
			while (!Button() && (err == noErr))
				{
				err = SGIdle(gSeqGrabber);
				CheckError(err,"SGIdle");
#if SHOW_SPACE_REMAINING
				{
				GWorldPtr	wPtr;
				wide		space;
				Str255		str;

				// Note this only works if we are grabbing to the window!
				SGGetGWorld(gSeqGrabber, &wPtr, 0);
				SGGetDataOutputStorageSpaceRemaining64(gSeqGrabber, gOutput, &space);
				NumToString(space.lo, str);
				SetWTitle((WindowPtr)wPtr, str);
				}
#endif
				}
			}
		else 
			{
			return;
			}
		}

	if (gCurrentlyRecording== true)
		{

		// If we recorded until we ran out of space, then allow SGStop to be
		// called to write the movie resource.  The assumption here is that the
		// data output filled up but the disk has enough free space left to
		// write the movie resource.
		if (!((err == dskFulErr) || (err != eofErr)))
			goto bail;
		err = SGStop(gSeqGrabber);
		CheckError(err,"SGStop");

#if USE_NAV_SERVICES
		if ( err == noErr && reply.validRecord) {	
			err = NavCompleteSave(&reply, kNavTranslateInPlace);	//��� Is this needed???
			CheckError(err,"NavCompleteSave");
		}
#endif

		gCurrentlyRecording = false;
		err = SGStartPreview(gSeqGrabber);
		CheckError(err,"SGStartPreview");

		NoteAlert(kMovieHasBeenRecordedAlertID, 0);

#if USE_NAV_SERVICES
		NavDisposeReply(&reply);
		reply.validRecord = false;
#endif	
		}
	return;

bail:
	SetWTitle(gMonitor, "\pRecord Error");
	gCurrentlyRecording = false;
	SGPause(gSeqGrabber, false);
	SGStartPreview(gSeqGrabber);
#if USE_NAV_SERVICES
	NavDisposeReply(&reply);
	reply.validRecord = false;
#endif
}

/* ---------------------------------------------------------------------- */

void DoAboutDialog(void)
{
	short		itemHit;
	DialogPtr	aboutDialog;

	aboutDialog = GetNewDialog(kAboutDLOGID, NULL, (WindowPtr)-1L);

	// Do the boring about dialog
	SetDialogDefaultItem(aboutDialog, 1);
	MacShowWindow(GetDialogWindow(aboutDialog));
	do
	{
		ModalDialog(NULL, &itemHit);
	}
	while (itemHit != 1);
	DisposeDialog(aboutDialog);
}


/* ---------------------------------------------------------------------- */

void DoOtherDialog(int nDlgId);
void DoOtherDialog(int nDlgId)
{
	short		itemHit;
	DialogPtr	aboutDialog;

	aboutDialog = GetNewDialog(nDlgId, NULL, (WindowPtr)-1L);

	// Do the boring about dialog
	SetDialogDefaultItem(aboutDialog, 1);
	MacShowWindow(GetDialogWindow(aboutDialog));
	do
	{
		ModalDialog(NULL, &itemHit);
	}
	while (itemHit != 1);
	DisposeDialog(aboutDialog);
}

/* ---------------------------------------------------------------------- */

void DoPageSetup(void)
{
#if TARGET_API_MAC_CARBON
	DebugStr("\pYou can't call DoPageSetup today");
#else
	PrOpen();
	PrStlDialog(gPrintRec);
	PrClose();
#endif
}

/* ---------------------------------------------------------------------- */

void DoPrint(void)
{
#if TARGET_API_MAC_CARBON
	DebugStr("\pYou can't call DoPrint today");
#else
	TPPrPort	printPort;
	TPrStatus	printStatus;
	ComponentResult		err;
	Rect		tempRect;

	// Copy a frame from the monitor
	if (gMonitorPICT != NULL)
		KillPicture (gMonitorPICT);
	gMonitorPICT = NULL;
	err = SGGrabPict(gSeqGrabber, &gMonitorPICT, NULL, 0, grabPictOffScreen);
	if ((err == noErr) && (gMonitorPICT != NULL))
	{
		// Print it
		HLock((Handle) gMonitorPICT);
		PrOpen();
		if (PrJobDialog (gPrintRec))
		{
			printPort = PrOpenDoc (gPrintRec, NULL, NULL);
			err = PrError();
			PrOpenPage (printPort, 0);
			err = PrError();

			tempRect  = (**gMonitorPICT).picFrame;
			tempRect.left = EndianS16_BtoN(tempRect.left);
			tempRect.top = EndianS16_BtoN(tempRect.top);
			tempRect.right = EndianS16_BtoN(tempRect.right);
			tempRect.bottom = EndianS16_BtoN(tempRect.bottom);
			DrawPicture(gMonitorPICT, &tempRect);

			PrClosePage (printPort);
			err = PrError();
			PrCloseDoc (printPort);
			err = PrError();
			if ((**gPrintRec).prJob.bJDocLoop == bSpoolLoop)
			{
				PrPicFile (gPrintRec, 0, 0, 0, &printStatus);
				err = PrError();
			}
		}
		PrClose();
		err = PrError();
		HUnlock((Handle) gMonitorPICT);
	}
#endif
}

/* ---------------------------------------------------------------------- */

#if TARGET_API_MAC_CARBON
static OSStatus ShimPutScrap (		SInt32 				sourceBufferByteCount,
						ScrapFlavorType 	flavorType,
						const void *		sourceBuffer			)
{
	OSStatus err = noErr;

	do
	{
		ScrapRef scrap;
		err = GetCurrentScrap (&scrap);
		if (err) break;
		err = PutScrapFlavor (scrap,flavorType,kScrapFlavorMaskNone,sourceBufferByteCount,sourceBuffer);
		if (err) break;
	}
	while (false);

	return err;
}
#else
	#define ShimPutScrap(a, b, c)	PutScrap(a, b, c)
#endif

void DoCopyToClipboard(void)
{
	ComponentResult	err;

	// Copy a frame from the monitor
	if (gMonitorPICT != NULL)
		KillPicture (gMonitorPICT);
	gMonitorPICT = NULL;
	err = SGGrabPict (gSeqGrabber, &gMonitorPICT, NULL, 0, grabPictOffScreen);
	if ((err == noErr) && (gMonitorPICT != NULL))
	{
		err = ClearCurrentScrap();
		HLock ((Handle) gMonitorPICT);
		err = ShimPutScrap (GetHandleSize ((Handle) gMonitorPICT), 'PICT', *(Handle)gMonitorPICT);
		HUnlock ((Handle) gMonitorPICT);
	}
}

/* ---------------------------------------------------------------------- */

static pascal Boolean
SeqGrabberModalFilterProc (DialogPtr theDialog, const EventRecord *theEvent,
	short *itemHit, long refCon)
{
#pragma unused(theDialog, itemHit)
	// Ordinarily, if we had multiple windows we cared about, we'd handle
	// updating them in here, but since we don't, we'll just clear out
	// any update events meant for us
	
	// TODO: Need to handle multiple windows if QTFramework is used...
	
	Boolean	handled = false;
	
	if ((theEvent->what == updateEvt) && 
		((WindowPtr) theEvent->message == (WindowPtr) refCon))
	{
		BeginUpdate ((WindowPtr) refCon);
		EndUpdate ((WindowPtr) refCon);
		handled = true;
	}
	return (handled);
}

/* ---------------------------------------------------------------------- */

void DoVideoSettings(void)
{
	Rect	newActiveVideoRect;
	Rect	adjustedActiveVideoRect;
	Rect	curBounds, curVideoRect, newVideoRect, newBounds;
	short	width, height;
	ComponentResult	err;
	GrafPtr	savedPort;
	RgnHandle	deadRgn;
	SGModalFilterUPP	seqGragModalFilterUPP;
	Rect	portRect;			
	
	// Get our current state
	err = SGGetChannelBounds (gVideoChannel, &curBounds);
	err = SGGetVideoRect (gVideoChannel, &curVideoRect);
	
	// Pause
	err = SGPause (gSeqGrabber, true);
	
	// Do the dialog thang
	seqGragModalFilterUPP = (SGModalFilterUPP)NewSGModalFilterUPP(SeqGrabberModalFilterProc);
	err = SGSettingsDialog(gSeqGrabber, gVideoChannel, 0, 
		NULL, 0L, seqGragModalFilterUPP, (long)gMonitor);
	DisposeSGModalFilterUPP(seqGragModalFilterUPP);
	
	// What happened?
	err = SGGetVideoRect (gVideoChannel, &newVideoRect);
	err = SGGetSrcVideoBounds (gVideoChannel, &newActiveVideoRect);

	// Set up our port
	GetPort (&savedPort);
	MacSetPort ((GrafPtr)GetWindowPort(gMonitor));
	
	// Has our active rect changed?
	// If so, it's because our video standard changed (e.g., NTSC to PAL),
	// and we need to adjust our monitor window
	if (!MacEqualRect (&gActiveVideoRect, &newActiveVideoRect))
	{
		if (gFullSize)
		{
			
			width = newActiveVideoRect.right - newActiveVideoRect.left;
			height = newActiveVideoRect.bottom - newActiveVideoRect.top;
			
			gActiveVideoRect = newActiveVideoRect;
			SizeWindow (gMonitor, width, height, false);
			GetPortBounds(GetWindowPort(gMonitor), &portRect);
			
			err = SGSetChannelBounds (gVideoChannel, &portRect);

                        // Identify Monitor window size
                        ChangeWindowTitle(gMonitor, width, height, "");		
		}
		else if (gHalfSize)
		{
			width = (newActiveVideoRect.right - newActiveVideoRect.left) / 2;
			height = (newActiveVideoRect.bottom - newActiveVideoRect.top) / 2;
			
			gActiveVideoRect = newActiveVideoRect;
			SizeWindow (gMonitor, width, height, false);
			GetPortBounds(GetWindowPort(gMonitor), &portRect);

			err = SGSetChannelBounds (gVideoChannel, &portRect);

                        // Identify Monitor window size
                        ChangeWindowTitle(gMonitor, width, height, "");		
		}
		else if (gQuarterSize)
		{
			width = (newActiveVideoRect.right - newActiveVideoRect.left) / 4;
			height = (newActiveVideoRect.bottom - newActiveVideoRect.top) / 4;
			
			gActiveVideoRect = newActiveVideoRect;
			SizeWindow (gMonitor, width, height, false);
			GetPortBounds(GetWindowPort(gMonitor), &portRect);

			err = SGSetChannelBounds (gVideoChannel, &portRect);

                        // Identify Monitor window size
                        ChangeWindowTitle(gMonitor, width, height, "");		
		}
	}
	
	// Has our crop changed?
	// This code shows how to be crop video panel friendly
	// Two important things - 
	// 1) Be aware that you might have been cropped and adjust your
	//    video window appropriately
	// 2) Be aware that you might have been adjusted and attempt to
	//    account for this.  Adjusting refers to using the digitizer
	//    rect to "adjust" the active source rect within the maximum
	//    source rect.  This is useful if you're getting those nasty
	//    black bands on the sides of your video display - you can use
	//    the control-arrow key sequence to shift the active source 
	//    rect around when you're in the crop video panel
	
	adjustedActiveVideoRect = gActiveVideoRect;
	if (!MacEqualRect (&curVideoRect, &newVideoRect))
	{
		GetPortBounds(GetWindowPort(gMonitor), &portRect);

		if ((newVideoRect.left < gActiveVideoRect.left) ||
			(newVideoRect.right > gActiveVideoRect.right) ||
			(newVideoRect.top < gActiveVideoRect.top) ||
			(newVideoRect.bottom > gActiveVideoRect.bottom))
		{
			if (newVideoRect.left < gActiveVideoRect.left)
			{
				adjustedActiveVideoRect.left = newVideoRect.left;
				adjustedActiveVideoRect.right -= (gActiveVideoRect.left - newVideoRect.left);
			}
			if (newVideoRect.right > gActiveVideoRect.right)
			{
				adjustedActiveVideoRect.right = newVideoRect.right;
				adjustedActiveVideoRect.left += (newVideoRect.right - gActiveVideoRect.right);
			}
			if (newVideoRect.top < gActiveVideoRect.top)
			{
				adjustedActiveVideoRect.top = newVideoRect.top;
				adjustedActiveVideoRect.bottom -= (gActiveVideoRect.top - newVideoRect.top);
			}
			if (newVideoRect.bottom > gActiveVideoRect.bottom)
			{
				adjustedActiveVideoRect.bottom = newVideoRect.bottom;
				adjustedActiveVideoRect.top += (newVideoRect.bottom - gActiveVideoRect.bottom);
			}
			newBounds = newVideoRect;
			MapRect (&newBounds, &adjustedActiveVideoRect, &portRect);
		}
		else	// Can't tell if we've been adjusted (digitizer rect is smaller on all sides
				// than the active source rect)
		{
			newBounds = newVideoRect;
			MapRect (&newBounds, &gActiveVideoRect, &portRect);
		}
		width = newBounds.right - newBounds.left;
		height = newBounds.bottom - newBounds.top;
		err = SGSetChannelBounds (gVideoChannel, &newBounds);
	}

	// Clean out the part of the port that isn't being drawn in
	deadRgn = NewRgn();
	if (deadRgn != NULL)
	{
		Rect	boundsRect;

		GetPortBounds(GetWindowPort(gMonitor), &portRect);

		err = SGGetChannelBounds (gVideoChannel, &boundsRect);
		err = XorRectToRgn (&boundsRect, &portRect, &deadRgn);
		EraseRgn (deadRgn);
		DisposeRgn (deadRgn);
	}

	MacSetPort (savedPort);
	
#if !TARGET_OS_MAC
	// This is necessary, for now, to get the grab to start again afer the
	// dialog goes away.  For some reason the video destRect never gets reset to point
	// back to the monitor window.
	SGSetChannelBounds (gVideoChannel, &(gMonitor->portRect));
#endif

	// The pause that refreshes
	err = SGPause (gSeqGrabber, false);
}

/* ---------------------------------------------------------------------- */

void DoSoundSettings(void)
{
	SGModalFilterUPP	seqGragModalFilterUPP;
	ComponentResult		err;

	seqGragModalFilterUPP = (SGModalFilterUPP)NewSGModalFilterUPP(SeqGrabberModalFilterProc);
	err = SGSettingsDialog (gSeqGrabber, gSoundChannel, 0,
		NULL, 0L, seqGragModalFilterUPP, (long)gMonitor);
	DisposeSGModalFilterUPP(seqGragModalFilterUPP);
}

/* ---------------------------------------------------------------------- */

void DoResize(short divisor)
{
	short		width, height;

	// New width and height
	width = (gActiveVideoRect.right - gActiveVideoRect.left) / divisor;
	height = (gActiveVideoRect.bottom - gActiveVideoRect.top) / divisor;
	
	gQuarterSize = (divisor == 4 ? true : false);
	gHalfSize = (divisor == 2 ? true : false);
	gFullSize = (divisor == 1 ? true : false);
	
	SetNewSize(width,height);

}

void SetNewSize(short width, short height)
{
	GrafPtr		savedPort;
	Rect		curBounds, maxBoundsRect;
	RgnHandle	deadRgn;
	ComponentResult	err;
	Rect portRect;
		
	// Resize the monitor
	GetPort (&savedPort);
	MacSetPort ((GrafPtr)GetWindowPort(gMonitor));
	err = SGPause (gSeqGrabber, true);
	err = SGGetChannelBounds (gVideoChannel, &curBounds);

	GetPortBounds(GetWindowPort(gMonitor), &portRect);

	maxBoundsRect = portRect;
	SizeWindow (gMonitor, width, height, false);
	GetPortBounds(GetWindowPort(gMonitor), &portRect);

        // Identify Monitor window size
        ChangeWindowTitle(gMonitor, width, height, "");		

	MapRect (&curBounds, &maxBoundsRect, &portRect);
	err = SGSetChannelBounds (gVideoChannel, &curBounds);

	// Clean out part of port we're not drawing in
	deadRgn = NewRgn();
	if (deadRgn != NULL)
	{
		Rect	boundsRect;

		err = SGGetChannelBounds (gVideoChannel, &boundsRect);
		GetPortBounds(GetWindowPort(gMonitor), &portRect);
		err = XorRectToRgn (&boundsRect, &portRect, &deadRgn);
		EraseRgn (deadRgn);
		DisposeRgn (deadRgn);
	}
		
	MacSetPort (savedPort);
	err = SGPause (gSeqGrabber, false);
}

/* ---------------------------------------------------------------------- */

static OSErr XorRectToRgn (Rect *srcRectA, Rect *srcRectB, RgnHandle *destRgn)
{
	RgnHandle	srcRgnA = NewRgn();
	RgnHandle	srcRgnB = NewRgn();
	OSErr		result = noErr;
	
	if ((destRgn != NULL) && (*destRgn != NULL))
	{
		if ((srcRgnA != NULL) &&
			(srcRgnB != NULL))
		{
			RectRgn (srcRgnA, srcRectA);
			RectRgn (srcRgnB, srcRectB);
			MacXorRgn (srcRgnA, srcRgnB, *destRgn);
			DisposeRgn (srcRgnA);
			DisposeRgn (srcRgnB);
		}
		else
		{
			result = memFullErr;
		}
	}
	else
	{
		result = nilHandleErr;
	}
	return (result);
}

/* ---------------------------------------------------------------------- */

void CheckError(OSErr err,char * message) 
{
	Str255 msg,msg1,msg2;
	if (!err) return;
        // Original routine puts error message in window title
	sprintf((char*)&msg[1],"%d:%s",err,message);
	msg[0] = strlen((char*)&msg[1]);
	sprintf((char*)&msg1[1],"QuickTime Digitizer Error %d",err);
	msg1[0] = strlen((char*)&msg1[1]);
	sprintf((char*)&msg2[1],"%s",message);
	msg2[0] = strlen((char*)&msg2[1]);
	if (msg[0] && gMonitor) {
		//SetWTitle(gMonitor,msg);
                // Fill Monitor window in blue
                Rect portRect;
                RGBColor fg = {0xFFFF, 0xFFFF, 0xFFFF};
                RGBColor bg = {0, 0, 0xFFFF};
                // Make Monitor window decent size
                GetPortBounds(GetWindowPort(gMonitor), &portRect);
                if (EmptyRect(&portRect) || (portRect.bottom - portRect.top < 100))
                    SizeWindow(gMonitor, 360, 240, false);
                // Write white on blue
                RGBForeColor(&fg);
                RGBBackColor(&bg);
                GetPortBounds(GetWindowPort(gMonitor), &portRect);
                EraseRect(&portRect);
                TextSize(18);
                MoveTo(portRect.left, portRect.top+18);
                DrawString(msg1);
                MoveTo(portRect.left, portRect.top+18+18);
                DrawString(msg2);
	}	
}

/* ---------------------------------------------------------------------- */

void ChangeWindowTitle(WindowPtr pWin, int width, int height, char* szExtraText)
{
	Str255	szPTitle;
	memset(szPTitle, 0, sizeof(szPTitle));
	sprintf((char*)&szPTitle[1], "Monitor (%d x %d) %s\0", width, height, szExtraText);
	szPTitle[0] = strlen((char*)&szPTitle[1]);
	SetWTitle(pWin, szPTitle);
}

/* ---------------------------------------------------------------------- */

void DoRecordStart(void);
void DoRecordSingleFrame(void);
void DoRecordStop(void);
void DoSequencerDialog(void);
void DoQueryCancelDialog(void);
void DoResizeCustomDialog(void);
void DoResizeCustom(short width, short height);
int ConfigSerialPort(void);
int InitSerialPort(void);
int ExitSerialPort(void);
int UpdateSerialPort(char c);
void DoProjectorFrame(void);
void DoSerialDialog(void);
void DoProjectorTimingDialog(void);

extern void HandleEvent (void);

DEFINE_API (ComponentResult) MyGrabFrameFunction(
	SGChannel c,
	short bufferNum,
	long refCon);
DEFINE_API (ComponentResult) MyGrabFrameCompleteFunction(
	SGChannel c,
	short bufferNum,
	Boolean *done,
	long refCon);
DEFINE_API (ComponentResult) MyGrabCompressCompleteFunction(
	SGChannel c,
	Boolean *done,
	SGCompressInfo *ci,
	TimeRecord *tr,
	long refCon);
DEFINE_API (ComponentResult) MyAddFrameFunction(
	SGChannel c,
	short bufferNum,
	TimeValue atTime,
	TimeScale scale,
	const SGCompressInfo *ci,
	long refCon);
DEFINE_API (ComponentResult) MyDisplayFrameFunction(
	SGChannel c,
	short bufferNum,
	MatrixRecord *mp,
	RgnHandle clipRgn,
	long refCon);
DEFINE_API (ComponentResult) MyDisplayCompressFunction(
	SGChannel c,
	Ptr dataPtr,
	ImageDescriptionHandle desc,
	MatrixRecord *mp,
	RgnHandle clipRgn,
	long refCon);

/* ---------------------------------------------------------------------- */

Boolean gFrameCaptured = false;
Boolean gFrameGrabbed = false;
CGrafPtr gTempGrafPort = NULL;
int gFrameCount = 0;
int gFPS = 24;
TimeValue gFrameTimeStamp = 0L;
unsigned long gTrigTimeOn = 30L;
unsigned long gTrigTimeOff = 30L;

extern Boolean gSlateMarkers;
extern Boolean gFrameMarkers;

// Grab-Frame Hook
DEFINE_API (ComponentResult) MyGrabFrameFunction(
	SGChannel c,
	short bufferNum,
	long refCon)
{
	ComponentResult cr;
	(void)refCon;
	cr = SGGrabFrame(c, bufferNum);
	return cr;
}
	
// Grab-Frame-Complete Hook
DEFINE_API (ComponentResult) MyGrabFrameCompleteFunction(
	SGChannel c,
	short bufferNum,
	Boolean *done,
	long refCon)
{
	ComponentResult cr;
	(void)refCon;
	cr = SGGrabFrameComplete(c, bufferNum, done);
        gFrameGrabbed = (cr == noErr);
	// Draw something extra on the captured frame
	if (cr == noErr && *done && gTempGrafPort && (gFrameMarkers || gSlateMarkers)) {
		CGrafPtr savePort, tempPort = /* (CGrafPtr)refCon; */ gTempGrafPort;
		GDHandle saveGD;
		PixMapHandle bufferPM, savePM;
		Rect bufferRect;
		OSErr err;
		char	szString[255] = "\pFrame #";
		char* 	pszString = &szString[1];
		
		GetGWorld(&savePort, &saveGD);
		SetGWorld(tempPort, nil);
		err = SGGetBufferInfo(c, bufferNum, &bufferPM, &bufferRect, nil, nil);
		if (!err) {
			//savePM = (PixMapHandle)tempPort->portPixMap;
			savePM = GetPortPixMap(tempPort);
			SetPortPix(bufferPM);
			TextSize(24);
			TextMode(srcXor);	// not 68K code
			if (gSlateMarkers && gRecordVideo > 1 /* && gCurrentlyRecording */) {
				//EraseRect(&bufferRect);
				MoveTo(bufferRect.left, bufferRect.top+24);
                                if (gRecordVideo == 99)
				DrawString("\pEND FRAME");
                                else if (gFrameCount == 0)
                                DrawString("\pSTART FRAME");
				MoveTo(bufferRect.left, bufferRect.top+24+24);
                                DrawString(szFileName);
			}
                        if (gFrameMarkers && gCurrentlyRecording) {
                                MoveTo(bufferRect.left, bufferRect.bottom-24);
                                sprintf(pszString, "Frame #%d", gFrameCount);
                                DrawText((void*)pszString, 0, strlen(szString));
                        }
			TextMode(srcOr);	// not 68K code
			SetPortPix(savePM);
		}
		SetGWorld(savePort, saveGD);
		}
	return cr;	
}
	
// Grab-Compress-Complete Hook
DEFINE_API (ComponentResult) MyGrabCompressCompleteFunction(
	SGChannel c,
	Boolean *done,
	SGCompressInfo *ci,
	TimeRecord *tr,
	long refCon)
{
	ComponentResult cr;
	(void)refCon;
	cr = SGGrabCompressComplete(c, done, ci, tr);
	return cr;
}
	
	
// Add-Frame Hook
DEFINE_API (ComponentResult) MyAddFrameFunction(
	SGChannel c,
	short bufferNum,
	TimeValue atTime,
	TimeScale scale,
	const SGCompressInfo *ci,
	long refCon)
{
	ComponentResult cr;
	TimeValue atTimeAdjust;
	(void)atTime;
	(void)refCon;
	
	// Nothing to do if single frame already captured and saved to file
	if (gFrameCaptured)
		return noErr;
	// change atTime for single-frame operation
	atTimeAdjust = (scale*(TimeValue)gFrameCount)/(TimeValue)gFPS;
	cr = SGAddFrame(c, bufferNum, atTimeAdjust, scale, ci);
	// This is where we identify one single frame captured *and* saved to file
	if (cr == noErr) {
		gFrameCaptured = true;
		gFrameCount++;
		gFrameTimeStamp = atTimeAdjust;
		}

	return cr;
}

// Display-Frame Hook
DEFINE_API (ComponentResult) MyDisplayFrameFunction(
	SGChannel c,
	short bufferNum,
	MatrixRecord *mp,
	RgnHandle clipRgn,
	long refCon)
{
	ComponentResult cr;
	(void)refCon;
	cr = SGDisplayFrame(c, bufferNum, mp, clipRgn);
	return cr;
}

// Display-Compressed-Frame Hook
DEFINE_API (ComponentResult) MyDisplayCompressFunction(
	SGChannel c,
	Ptr dataPtr,
	ImageDescriptionHandle desc,
	MatrixRecord *mp,
	RgnHandle clipRgn,
	long refCon)
{
	ComponentResult cr;
	(void)refCon;
	cr = SGDisplayCompress(c, dataPtr, desc, mp, clipRgn);
	return cr;
}

/* ---------------------------------------------------------------------- */

//	StandardFileReply	reply;

// Setup Sequencer Grabber with Video bottleneck hooks.
void PostSequencerSetup(void);
void PostSequencerSetup(void)
{
	VideoBottles		vb;
	static Boolean 		bSetup = false;

	// Only needs to be done once
	if (bSetup) 
		return;
	bSetup = true;	
	
	// Setup callbacks
	memset(&vb, 0, sizeof(vb));
	vb.procCount = 9;
//	vb.grabProc = NewSGGrabBottleUPP(MyGrabFrameFunction);
	vb.grabCompleteProc = NewSGGrabCompleteBottleUPP(MyGrabFrameCompleteFunction);
//	vb.grabCompressCompleteProc = NewSGGrabCompressCompleteBottleUPP(MyGrabCompressCompleteFunction);
	vb.addFrameProc = NewSGAddFrameBottleUPP(MyAddFrameFunction);
//	vb.displayProc = NewSGDisplayBottleUPP(MyDisplayFrameFunction);
	SGSetVideoBottlenecks(gVideoChannel, &vb);

}

// Start Recording a movie
void DoRecordStart(void)
{

//	long	err;
//	StandardFileReply	reply;
//	ComponentInstance 	videoDigitizer;
//	VideoBottles		vb;
//	UserData			*pud = NULL;
	TimeBase			tb;
	long				flags;
	Boolean				bControlledGrab = false;
	Boolean				bDiscreteSamples = false;
	TimeScale			scale;
	Fixed				framerate;
	static int			iClipNumber = 0;
	Str255				szClipName;
	Rect	portRect;
	

	// Stop everything while the dialogs are up
	SGStop(gSeqGrabber);
	
	// Setup Sequencer hooks
	PostSequencerSetup();
	

#if TARGET_API_MAC_CARBON
	// Setup temporary graphics port for overwriting text on captured video
	if (gTempGrafPort)
		DisposePort(gTempGrafPort);
	gTempGrafPort = (CGrafPtr)CreateNewPort(); 
	SGSetChannelRefCon(gVideoChannel, (long)gTempGrafPort);
	GetPortBounds(GetWindowPort(gMonitor), &portRect);
	SetPortBounds(gTempGrafPort, &portRect);
	// clip and visibility regions already setup?
	PortChanged((GrafPtr)gTempGrafPort);
#else
	// Setup temporary graphics port for overwriting text on captured video
	gTempGrafPort = (CGrafPtr)NewPtr(sizeof(CGrafPort)); // (CGrafPtr)CreateNewPort(); 
	SGSetChannelRefCon(gVideoChannel, (long)gTempGrafPort);
	OpenCPort(gTempGrafPort);
	SetRectRgn(gTempGrafPort->visRgn, 0x8000, 0x8000, 0x7FFF, 0x7FFF);
	CopyRgn(gTempGrafPort->visRgn, gTempGrafPort->clipRgn);
	PortChanged((GrafPtr)gTempGrafPort);
#endif


	// Examine current configuration settings
	SGGetTimeBase(gSeqGrabber, &tb);
	SGGetFlags(gSeqGrabber, &flags);
	if (flags & sgFlagControlledGrab)
		bControlledGrab = true;
	SGGetChannelInfo(gVideoChannel, &flags);
	if (flags & seqGrabHasDiscreteSamples)
	    bDiscreteSamples = true;

        // Get Time base info for adjusting captured time stamps
        SGGetChannelTimeScale(gVideoChannel, &scale); 
	// always valid, selected via Video Settings dialog (default = 2997) 
	SGGetFrameRate(gVideoChannel, &framerate); 
	// default = 0 for "best" frame rate, otherwise computed on fly

	// Set time stamp factor per single captured frame
	gFPS = scale / 100;
	if (gFPS == 29)
		gFPS = 30; // adjust for 29.97 FPS
		
	// Reset capture vars
	gFrameCount = 0;
	gFrameTimeStamp = 0L;

	// Flag to prevent recording first frame until demand
	gFrameCaptured = true;
        gFrameGrabbed = false;

	// Flag for single-frame recording session -- do not pause
	gRecordVideo = 2;
	

	// Create clip file name from clip number
	memset(szClipName, 0, sizeof(szClipName));
	sprintf((char*)&szClipName[1], "Clip%04d.mov", iClipNumber++);
	szClipName[0] = strlen((char*)&szClipName[1]);
        // Sneak into default file dialog options
        strcpy(szFileName, szClipName);


	// Use Default Record File dialog to start recording
	DoRecord(false);
        if (!gCurrentlyRecording) 
            goto bail;
		

	// Update monitor window title bar
	GetPortBounds(GetWindowPort(gMonitor), &portRect);
	ChangeWindowTitle(gMonitor, portRect.right, portRect.bottom, "Frame #0");
	return;
        
bail:
	// Resume grabber to standby operation
	SGPause(gSeqGrabber, false);
	gRecordVideo = 1;
	gFrameCount = 0;
	SGStartPreview(gSeqGrabber);
	return;
}

/* ---------------------------------------------------------------------- */

// Record single frame of a movie
void DoRecordSingleFrame(void)
{
	char	szCText[255];
	Rect	portRect;
		
	// Nothing to do if recording not started
	if (!gCurrentlyRecording)
		return;
		
	// Wait to capture one single frame
	gFrameCaptured = false;
        gFrameGrabbed = false;
	while (!gFrameGrabbed && !gFrameCaptured)
		SGIdle(gSeqGrabber);
		
	// Put frame count for clip in window title bar
	memset(szCText, 0, sizeof(szCText));
	sprintf(szCText, "Frame #%d", gFrameCount);
	GetPortBounds(GetWindowPort(gMonitor), &portRect);
	ChangeWindowTitle(gMonitor, portRect.right, portRect.bottom, szCText);		
	return;
}

/* ---------------------------------------------------------------------- */

// Stop Recording a movie
void DoRecordStop(void)
{

	// SGIdle() must be called during event loop to keep recording
	long	err;
//	StandardFileReply	reply2;
	Movie				theMovie;
	TimeScale			scale;
	TimeValue			duration, endtime, cuttime;
//	FSSpec				spec;
//	FInfo				info;
//	Str255				name;
	short				refMovie, resID;
	Rect	portRect;
	
	// Nothing to do if recording not started
	if (!gCurrentlyRecording)
		return;

	// Flag last frame as "dead frame" due to time duration error
	gRecordVideo = 99;
	
	// Record one last frame
	DoRecordSingleFrame();
	
	// Use Default Record routine to stop recording... 
	DoRecord(false);
	
	GetPortBounds(GetWindowPort(gMonitor), &portRect);
	ChangeWindowTitle(gMonitor, portRect.right, portRect.bottom, "");		

	// Proceed to fixup movie duration info here...	

#if 1 // Adjust the movie file time duration to match last frame time stamp

	// Now adjust time duration of movie
	theMovie = SGGetMovie(gSeqGrabber);
	duration = GetMovieDuration(theMovie);
	resID = 0; // returned resID = -1 indicates movie is in data fork, not resource fork
	SGGetLastMovieResID(gSeqGrabber, &resID);

	// Open movie file for editing
//	OpenMovieFile(&reply.sfFile, &refMovie, fsRdWrPerm);
	OpenMovieFile(&theFile, &refMovie, fsRdWrPerm);
	resID = 0; // returned resID = -1 indicates movie is in data fork, not resource fork
	NewMovieFromFile(&theMovie, refMovie, &resID, nil, newMovieActive, nil);
	if (theMovie == nil)
		goto bail;
		
	// Skipping past the edit steps below preserves all the original movie data.
	// NOTE: Same behavior occurs with QTPro edits changing movie date.
#if 1
	// Movie file time scale seems to be normalized = 600 -- not the same as digitizer time scale!
	scale = GetMovieTimeScale(theMovie); 
	duration = GetMovieDuration(theMovie);
	endtime = (TimeValue)gFrameCount * scale / (TimeValue)gFPS;
	cuttime = duration  - endtime;
#if 0
	// Low-level Movie Edit functions
	err = DeleteMovieSegment(theMovie, endtime, cuttime); // OK
#else	
	// Movie Edit functions
	SetMovieSelection(theMovie, endtime, cuttime); // OK
	ClearMovieSelection(theMovie); // OK
#endif
#endif
	
#if 1 // OK -- same file
	// Save adjusted movie back to original movie file
	err = UpdateMovieResource(theMovie, refMovie, resID, nil); // works even if movie is in data fork
	err = CloseMovieFile(refMovie); // need to call one of the QT "save" functions before closing
	if (err == noErr)	
		NoteAlert(kMovieHasBeenSavedAlertID, 0);
#else	// OK -- different file
// TODO: NAV_FILE_SERVICES method needs to have file name fixed...
	// Need to get the file type for the saved movie file
//	FSpGetFInfo(&reply.sfFile, &info);
	FSpGetFInfo(&theFile, &info);
//	strcpy(&name, &reply.sfFile.name);
	strcpy((char*)&name[1], (char*)&theFile.name[1]);
	strcat((char*)&name[1], ".flat");
	name[0] = (char)strlen((char*)&name[1]);
	FSMakeFSSpec(0, 0, name, &spec);	
#if 1 // OK -- with dialog
	// Create New movie via QT Export dialog
	err = ConvertMovieToFile(theMovie,
		nil, 
		&spec, // &reply.sfFile, 
		0L, // info.fdType,
		'TVOD', // info.fdCreator,  
		smSystemScript,
		nil, 
		createMovieFileDeleteCurFile | showUserSettingsDialog | movieFileSpecValid | movieToFileOnlyExport ,
		nil); // OK
#else // OK -- without dialog
	// Create New movie from old movie without dialog
	FlattenMovie(theMovie,
		flattenAddMovieToDataFork, 
		&spec, // &reply.sfFile, 
		'TVOD', 
		smSystemScript,
		createMovieFileDeleteCurFile,
		NULL,
		NULL); // OK only when using different file name
#endif
	err = CloseMovieFile(refMovie);
	if (err == noErr)	
		NoteAlert(kMovieHasBeenSavedAlertID, 0);
#endif
#endif

bail:
	// Resume grabber to standby operation
	SGPause(gSeqGrabber, false);
	gRecordVideo = 1;
	gFrameCount = 0;
	SGStartPreview(gSeqGrabber);
	return;
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

// Note: All calls to SGSetChannelBounds() will adjust the digitizer source-to-dest transform matrix.

void DoResizeCustom(short width, short height)
{
    SetNewSize(width, height);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

static pascal Boolean
AppDialogModalFilterProc (DialogRef theDialog,
        EventRecord *theEvent,
	DialogItemIndex *itemHit)
{
	Boolean	handled = false;
	
        // TODO: Dialogs should be able to be dragged
        // TODO: Monitor window should be able to be updated
        // TODO: Dialogs need to have Enter key handled for default item

        // What kind of event was it?
        switch (theEvent->what)
        {
            case mouseDown:
            {
                WindowPtr	theWindow;
                short		windowCode = MacFindWindow (theEvent->where, &theWindow);
                // Where was the mouse down?
                if (windowCode == inDrag)
                {
                    Rect			limitRect;
                    RgnHandle		grayRgn = GetGrayRgn();
                    Rect			boundsRect;
                    // Find bounds
                    if (grayRgn != NULL)
                    {
                        GetRegionBounds(grayRgn, &limitRect);
                    }
                    else
                    {
                        BitMap	screenBits;            
                        GetQDGlobalsScreenBits(&screenBits);
                        limitRect = screenBits.bounds;
                    }
                    // Is it the Monitor Window?
                    if (gVideoChannel != NULL && theWindow == gMonitor)
                    {
                        ComponentResult	result = noErr;
                        // Pause the sequence grabber
                        result = SGPause (gSeqGrabber, true);
                        // Drag it with the totally cool DragAlignedWindow
                        result = SGGetChannelBounds (gVideoChannel, &boundsRect);
                        DragAlignedWindow (theWindow, theEvent->where, &limitRect, &boundsRect, &gSeqGrabberAlignProc);
                        // Start up the sequence grabber
                        result = SGPause (gSeqGrabber, false);
                    }
                    // Is it the Dialog Window?
                    else
                    {
                        DragWindow (theWindow, theEvent->where, &limitRect);
                    }
                    handled = true;
                }
                break;
            }
            case updateEvt:
            {	
                // Is it the Monitor Window?
                if ((gMonitor != NULL) && ((WindowPtr) (theEvent->message) == (WindowPtr) gMonitor))
                {
                    RgnHandle updateRgn;
                    updateRgn = NewRgn();            
                #if TARGET_API_MAC_CARBON
                    GetWindowRegion(gMonitor, kWindowUpdateRgn, updateRgn);
                #else
                    GetWindowUpdateRgn(gMonitor, updateRgn);
                #endif                    
                    SGUpdate(gSeqGrabber, updateRgn);
                    DisposeRgn(updateRgn);
        
                    BeginUpdate (gMonitor);
                    EndUpdate (gMonitor);
                    handled = true;
                }
                // Is it the Dialog Window?
                else if ((WindowPtr) theEvent->message == (WindowPtr) theDialog)
                {
                    BeginUpdate ((WindowPtr) theDialog);
                    EndUpdate ((WindowPtr) theDialog);
                    handled = true;
                }
                break;
            }
            case keyDown:
            {
                // Is it the Dialog Window?
                if (1)
                {
                    char	theChar = theEvent->message & charCodeMask;
                    if (theChar == kReturnCharCode || theChar == kEnterCharCode) {
                        *itemHit = 1;
                        handled = true;
                        }
                }
                break;
            }
        }
        return (handled);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */

Boolean bModemPort = true;
Str255 szPortOut = "\p.AOut";
short int iPortRefNum = 0;

extern int InitSio(char *portname);
extern int OutSio(char* outbuf);
extern int InSio(char* inbuf);
extern int ExitSio(void);

int ConfigSerialPort(void)
{
	// Configure serial port = modem or printer
	if (bModemPort)
		strcpy(szPortOut, "\p.AOut");
	else
		strcpy(szPortOut, "\p.BOut");
	return 1;	
}

/* ---------------------------------------------------------------------- */

int InitSerialPort(void)
{
        szPortOut[0] = (char)strlen((char*)&szPortOut[1]);
        if (InitSio(szPortOut) != 0)
            goto Error;
	return 1;	// success	
	
Error:
	// Popup Serial port error message
	NoteAlert(kSerialPortAlertID, 0);	
	return 0;	// failure	
}

/* ---------------------------------------------------------------------- */

int ExitSerialPort(void)
{
	return ExitSio();	
}

/* ---------------------------------------------------------------------- */

int UpdateSerialPort(char c)
{
        char buf[255];
        buf[0] = c;
        buf[1] = '\0';
        return OutSio(buf);
}

/* ---------------------------------------------------------------------- */

void DoProjectorFrame(void)
{
        unsigned long iTriggerTime, iEndTime, iMarkTime;
        char buf[255];
        int n;
        
        // Fire Serial pulse
        iTriggerTime = gTrigTimeOn;
        for (n=0; n<(int)iTriggerTime; n++) {
            buf[n] = 0x55; // 0 bits change line state high 
            }
        buf[n] = '\0';
        iMarkTime = TickCount();
        OutSio(buf);
        // TODO: Replace with GetTime() idle loop...
        //Delay(iTriggerTime, &iEndTime);
        iEndTime = iMarkTime + iTriggerTime;
        do {
            SGIdle(gSeqGrabber);
        } while (TickCount() < iEndTime);
        iTriggerTime = gTrigTimeOff;
        //Delay(iTriggerTime, &iEndTime);
        iMarkTime = TickCount();
        iEndTime = iMarkTime + iTriggerTime;
        do {
            SGIdle(gSeqGrabber);
        } while (TickCount() < iEndTime);
}

/* ---------------------------------------------------------------------- */

void DoSerialDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Handle      	itemModem, itemPrinter;
	DialogItemType	kind;
	Rect        	box;
	Boolean     	bTempModemPort = bModemPort;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;
        
        ModalFilterUPP	uppFilter = NULL;
	
        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kSerialPortDLOGID, nil, (WindowPtr)-1L);

	// Do the serial port dialog with radio buttons
	SetDialogDefaultItem(theDialog, 1);
	GetDialogItem(theDialog, 3, &kind, &itemModem, &box);
	GetDialogItem(theDialog, 4, &kind, &itemPrinter, &box);
	if (bModemPort)
		SetControlValue((ControlHandle)itemModem, 1);
	else
		SetControlValue((ControlHandle)itemPrinter, 1);
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
		ModalDialog(uppFilter, &itemHit);
		switch (itemHit) {
		case 1:	// OK
			bOK = true;
		case 2:	// Cancel
		    bQuitDialog = true;
		    break;
		case 3:	// Modem
			SetControlValue((ControlHandle)itemModem, 1);
			SetControlValue((ControlHandle)itemPrinter, 0);
			bTempModemPort = true;
			break;
		case 4:	// Printer
			SetControlValue((ControlHandle)itemModem, 0);
			SetControlValue((ControlHandle)itemPrinter, 1);
			bTempModemPort = false;
			break;
		}
		
	}
	while (!bQuitDialog);
	DisposeDialog(theDialog);
        DisposeModalFilterUPP(uppFilter);

	// Re-configure serial port
	if (bOK) {
	    bModemPort = bTempModemPort;
	    ExitSerialPort();
	    ConfigSerialPort();
	    InitSerialPort();
	    }
}

/* ---------------------------------------------------------------------- */

static void LocalCopyPascalStringToC(Str255 src, char *dst)
{
        int c;
	int len = (int)((char)src[0]);
	for (c=0; c<len; c++)
		dst[c] = (char)src[c+1];
	dst[c+0] = '\0';	
}

/* ---------------------------------------------------------------------- */

Boolean bCancelSequence = false;
Boolean bRunningSequence = false;

void DoSequencerDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Handle      itemCam, itemPrj, itemSeq;
	Handle      itemFrmCnt, itemCamCnt, itemPrjCnt;
	DialogItemType	kind;
	Rect        box;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;
	Str255		szPText;
	char		szCText[255];
	int			iTemp1, iTemp2, iTemp3;
	
	static int  iSequencer = 3;
	static int  iFrameCount = 0;
	static int	iCamStep = 1;
	static int	iPrjSkip = 1;
	
        ModalFilterUPP	uppFilter = NULL;
	
        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kSequencerDLOGID, nil, (WindowPtr)-1L);

	// Do the sequencer dialog with radio buttons
	SetDialogDefaultItem(theDialog, 1);
	GetDialogItem(theDialog, 5, &kind, &itemCam, &box);
	GetDialogItem(theDialog, 6, &kind, &itemPrj, &box);
	GetDialogItem(theDialog, 7, &kind, &itemSeq, &box);
	GetDialogItem(theDialog, 4, &kind, &itemFrmCnt, &box);
	GetDialogItem(theDialog, 8, &kind, &itemCamCnt, &box);
	GetDialogItem(theDialog, 9, &kind, &itemPrjCnt, &box);
	switch (iSequencer) {
		case 1: SetControlValue((ControlHandle)itemCam, 1); break;
		case 2: SetControlValue((ControlHandle)itemPrj, 1); break;
		case 3: SetControlValue((ControlHandle)itemSeq, 1); break;
		}
	sprintf((char*)&szPText[1], "%d\0", iFrameCount);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemFrmCnt, szPText);
	sprintf((char*)&szPText[1], "%d\0", iCamStep);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemCamCnt, szPText);
	sprintf((char*)&szPText[1], "%d\0", iPrjSkip);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemPrjCnt, szPText);
	iTemp1 = iFrameCount;
	iTemp2 = iCamStep;
	iTemp3 = iPrjSkip;
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
		ModalDialog(uppFilter, &itemHit);
		switch (itemHit) {
		case 1:	// OK
			bOK = true;
		case 2:	// Cancel
		    bQuitDialog = true;
		    break;
		case 5:	// Camera
			SetControlValue((ControlHandle)itemCam, 1);
			SetControlValue((ControlHandle)itemPrj, 0);
			SetControlValue((ControlHandle)itemSeq, 0);
			iSequencer = 1;
			break;
		case 6:	// Projector
			SetControlValue((ControlHandle)itemCam, 0);
			SetControlValue((ControlHandle)itemPrj, 1);
			SetControlValue((ControlHandle)itemSeq, 0);
			iSequencer = 2;
			break;
		case 7:	// Sequencer
			SetControlValue((ControlHandle)itemCam, 0);
			SetControlValue((ControlHandle)itemPrj, 0);
			SetControlValue((ControlHandle)itemSeq, 1);
			iSequencer = 3;
			break;
		case 4:	// Frame Count
		    GetDialogItemText(itemFrmCnt, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp1);
		    break;
		case 8:	// Camera Step Count
		    GetDialogItemText(itemCamCnt, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp2);
		    break;
		case 9:	// Projector Skip Count
		    GetDialogItemText(itemPrjCnt, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp3);
		    break;
		}
		
	}
	while (!bQuitDialog);
	DisposeDialog(theDialog);
        DisposeModalFilterUPP(uppFilter);
	
	// Run camera, projector, or sequencer
	if (bOK) {
                int i, j;
                iFrameCount = iTemp1;
                iCamStep = iTemp2;
                iPrjSkip = iTemp3;
                
                bCancelSequence = false;
                bRunningSequence = true;                
		switch (iSequencer) {
		case 1:	// Camera only
			for (i=0; i<iFrameCount; i++) {
				DoRecordSingleFrame();
//				HandleEvent();
				if (bCancelSequence)
					break;
				}
			break;
		case 2:	// Projector only
			for (i=0; i<iFrameCount; i++) {
				DoProjectorFrame();
//				HandleEvent();
				if (bCancelSequence)
					break;
				}
			break;
		case 3:	// Camera:Projector Sequence
			for (i=0; i<iFrameCount; i++) {
			    for (j=0; j<iCamStep; j++)
					DoRecordSingleFrame();
			    for (j=0; j<iPrjSkip; j++)
					DoProjectorFrame();
//				HandleEvent();
				if (bCancelSequence)
					break;
				}
			break;
			}
                bRunningSequence = false;
		}
}

/* ---------------------------------------------------------------------- */

void DoQueryCancelDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;

        ModalFilterUPP	uppFilter = NULL;
	
        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kQueryCancelDLOGID, nil, (WindowPtr)-1L);

	// Do the cancel sequence dialog
	SetDialogDefaultItem(theDialog, 1);
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
		ModalDialog(nil, &itemHit);
		switch (itemHit) {
		case 1:	// OK
			bOK = true;
		case 2:	// Cancel
		    bQuitDialog = true;
		    break;
		}
	}
	while (!bQuitDialog);
	DisposeDialog(theDialog);
        DisposeModalFilterUPP(uppFilter);
	
        if (bOK)
		bCancelSequence = true;
}

/* ---------------------------------------------------------------------- */

void DoResizeCustomDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Handle      	itemWidth, itemHeight;
	DialogItemType	kind;
	Rect		box;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;
	Str255		szPText;
	char		szCText[255];
	int		iTemp1, iTemp2;
	
	static int  	iDestWidth = 320;
	static int	iDestHeight = 240;
        Rect		portRect;
	
        ModalFilterUPP	uppFilter = NULL;
	
        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kCustomSizeDLOGID, nil, (WindowPtr)-1L);

        // Get current window size for defaults
	GetPortBounds(GetWindowPort(gMonitor), &portRect);
        iDestWidth = portRect.right - portRect.left;
        iDestHeight = portRect.bottom - portRect.top;

	// Do the custom window size dialog
	SetDialogDefaultItem(theDialog, 1);
	GetDialogItem(theDialog, 4, &kind, &itemWidth, &box);
	GetDialogItem(theDialog, 6, &kind, &itemHeight, &box);
	sprintf((char*)&szPText[1], "%d\0", iDestWidth);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemWidth, szPText);
	sprintf((char*)&szPText[1], "%d\0", iDestHeight);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemHeight, szPText);
	iTemp1 = iDestWidth;
	iTemp2 = iDestHeight;
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
		ModalDialog(uppFilter, &itemHit);
		switch (itemHit) {
		case 1:	// OK
			bOK = true;
		case 2:	// Cancel
		    bQuitDialog = true;
		    break;
		case 4:	// Dest Width
		    GetDialogItemText(itemWidth, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp1);
		    break;
		case 6:	// Dest Height
		    GetDialogItemText(itemHeight, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp2);
		    break;
		}
		
	}
	while (!bQuitDialog);
	DisposeDialog(theDialog);
        DisposeModalFilterUPP(uppFilter);
	
	// Do window resize
	if (bOK) {
	    if (iTemp1 > 0 && iTemp1 < 1000)
	    	iDestWidth = iTemp1;
	    if (iTemp2 > 0 && iTemp2 < 1000)
	    	iDestHeight = iTemp2;
	    DoResizeCustom(iDestWidth, iDestHeight);
	}
}

/* ---------------------------------------------------------------------- */

void DoProjectorTimingDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Handle      	itemWidth, itemHeight;
	DialogItemType	kind;
	Rect        	box;
	Boolean		bQuitDialog = false;
        Boolean		bOK = false;
	Str255		szPText;
	char		szCText[255];
	int		iTemp1, iTemp2;

        int		iDestWidth = (int)gTrigTimeOn;
        int		iDestHeight = (int)gTrigTimeOff;
                
        ModalFilterUPP	uppFilter = NULL;
	
        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kProjectorTimingDLOGID, nil, (WindowPtr)-1L);

	// Do the custom window size dialog
	SetDialogDefaultItem(theDialog, 1);
	GetDialogItem(theDialog, 4, &kind, &itemWidth, &box);
	GetDialogItem(theDialog, 6, &kind, &itemHeight, &box);
	sprintf((char*)&szPText[1], "%d\0", iDestWidth);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemWidth, szPText);
	sprintf((char*)&szPText[1], "%d\0", iDestHeight);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemHeight, szPText);
	iTemp1 = iDestWidth;
	iTemp2 = iDestHeight;
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
		ModalDialog(uppFilter, &itemHit);
		switch (itemHit) {
		case 1:	// OK
			bOK = true;
		case 2:	// Cancel
		    bQuitDialog = true;
		    break;
		case 4:	// Dest Width
		    GetDialogItemText(itemWidth, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp1);
		    break;
		case 6:	// Dest Height
		    GetDialogItemText(itemHeight, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp2);
		    break;
		}
		
	}
	while (!bQuitDialog);
	DisposeDialog(theDialog);
        DisposeModalFilterUPP(uppFilter);
	
	// Do window resize
	if (bOK) {
	    if (iTemp1 > 0 && iTemp1 < 1000)
	    	iDestWidth = iTemp1;
	    if (iTemp2 > 0 && iTemp2 < 1000)
	    	iDestHeight = iTemp2;
	    gTrigTimeOn = (unsigned long)iDestWidth;
            gTrigTimeOff = (unsigned long)iDestHeight;
	}
}

/* ---------------------------------------------------------------------- */
