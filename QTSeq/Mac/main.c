/*
	File:		main.c
	
	Description: Refer to develop Issue 13, "Video Digitizing Under QuickTime",
				 for details on this code.

	Author:		QuickTime Engineering

	Copyright: 	© Copyright 1992-2001 Apple Computer, Inc. All rights reserved.
	
	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple’s
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

	Revision 1.10   10/5/2001   fixed AE Quit to clean up after itself EA DTS
	Revision 1.9  	4/10/2001	updated for UI3.4 and X EA DTS
	Revision 1.7  	2000/11/20	update menu name while recording KM
	Revision 1.5  	2000/03/01 	add more sized, recording on idle KM	
	Revision 1.4  	2000/02/25 	menu stuff TD
	Revision 1.3  	1999/12/15	carbonized AB
	Revision 1.2  	Original	GW
*/

//-----------------------------------------------------------------------
// Includes

#include "carbonprefix.h"
	
#if (TARGET_OS_MAC && TARGET_API_MAC_CARBON)
	#if __APPLE_CC__
		#include <Carbon/Carbon.h>
	#elif __MWERKS__
		#include <Carbon.h>
	#else
		#error "Ok now what?"
	#endif	
#else
	#include <QTML.h>
	#include <Menus.h>
	#include <MacWindows.h>
	#include <Quickdraw.h>
	#include <Resources.h>
	#include <Fonts.h>
	#include <ToolUtils.h>
	#include <QuickTimeComponents.h>
	#include <Scrap.h>
	#include <Printing.h>
	#include <Errors.h>
	#include <Folders.h>
	#include <Script.h>
	#include <MacMemory.h>
	#include <Gestalt.h>
	#include <Endian.h>
	#include <Devices.h>
	#include <LowMem.h>
	#include <TextUtils.h>
#endif

#include "Globals.h"
#include "Common.h"
	
#define kMenuBarID 128
enum
{
	kAppleID = 128,
	kFileID,
	kEditID,
	kMonitorID,
        kRunID
};
	
// Apple menu items
enum
{
	kAboutItem = 1
};

// File menu items
enum
{
	kPageSetupItem = 1,
	kPrintItem,
	kQuitItem = 4
};

// Edit menu items
enum
{
	kUndoItem = 1,
	kCutItem = 3,
	kCopyItem,
	kPasteItem,
	kClearItem
};

// Monitor menu items
// TODO: Support more screen sizes
enum
{
	kVideoSettingsItem = 1,
	kSoundSettingsItem,
	kRecordVideoItem = 4,
	kRecordSoundItem,
	kSplitTracksItem,
	kQuarterSizeItem = 8,
	kHalfSizeItem,
	kFullSizeItem,
	//k128x96SizeItem,
	//k160x120SizeItem,
	//k176x144SizeItem,
	k320x240SizeItem,
	k360x240SizeItem,
	k352x288SizeItem,
	k640x480SizeItem,
	k720x480SizeItem,
	k704x576SizeItem,
	kCustomSizeItem
};

// Run menu items
enum
{
	kRecordItem = 1,
	kRecordFrameItem,
	//kBreak2,
	kProjectorItem,
	kSequencerItem,
        kBreak2,
        kSlateMarkerItem,
        kFrameMarkerItem,
        kBreak3,
	kSerialItem,
	kProjectorTimingItem,
        kVideoDetectItem
};
	
//-----------------------------------------------------------------------
// Globals
MenuHandle				gAppleMenu=0;
MenuHandle				gFileMenu=0;
MenuHandle				gEditMenu=0;
MenuHandle				gMonitorMenu=0;
MenuHandle				gRunMenu=0;
EventRecord				gTheEvent;

extern Boolean				gSlateMarkers;
extern Boolean				gFrameMarkers;

extern Boolean				bCancelSequence;
extern Boolean				bRunningSequence;

//-----------------------------------------------------------------------
// Prototypes
/* static */ void Initialize (void);
/* static */ void DoMenuSetup (void);
/* static */ void HandleEvent (void);
/* static */ void HandleMouseDown	(void);
/* static */ void AdjustMenus (void);
static void Enable (Handle menu, short item, Boolean ok);
/* static */ void HandleMenu (long menu);
/* static */ void DoQuit (void);

void ShutdownSequenceGrabber(void);

extern void DoAboutDialog(void);
extern void DoOtherDialog(int nDlgId);

extern void DoRecordStart(void);
extern void DoRecordSingleFrame(void);
extern void DoRecordStop(void);
extern void DoSequencerDialog(void);
extern void DoQueryCancelDialog(void);
extern void DoResizeCustomDialog(void);
extern void DoResizeCustom(short width, short height);
extern int ConfigSerialPort(void);
extern int InitSerialPort(void);
extern int ExitSerialPort(void);
extern int UpdateSerialPort(char c);
extern void DoProjectorFrame(void);
extern void DoSerialDialog(void);
extern void DoProjectorTimingDialog(void);

//-----------------------------------------------------------------------

#if !USE_QTFRAMEWORK
int main (int argc, char* argv[])
{
	Initialize();

	// Eat events until done
	do
	{
		HandleEvent();
	}
	while (!gQuitFlag);

        return 0;
}
#endif

//-----------------------------------------------------------------------
// Happy AppleEvent type things

//
//	doAEOpenApp
//
static pascal OSErr doAEOpenApp(const AppleEvent *message, AppleEvent *reply, long refcon)
{
#pragma unused(message, reply, refcon)
	return noErr;
}

//
//	doAEOpenDocument
//
static pascal OSErr doAEOpenDocument(const AppleEvent *message, AppleEvent *reply, long refcon)
{
#pragma unused(message, reply, refcon)
	
	return noErr;
}

//
//	doAEQuit
//
static pascal OSErr doAEQuit(const AppleEvent *message, AppleEvent *reply, long refcon)
{
#pragma unused(message, reply, refcon)
	DoQuit();
	return noErr;
}


//
//	doAEPrintDocuments
//
static pascal OSErr doAEPrintDocuments(const AppleEvent *message, AppleEvent *reply, long refcon)
{
#pragma unused(message, reply, refcon)
	return errAEEventNotHandled;
}


//
//	initAppleEvents
//
static void initAppleEvents(void)
{
	const long noRefCon = 0;
	OSErr aevtErr;
	
	aevtErr = AEInstallEventHandler( kCoreEventClass, kAEOpenApplication, NewAEEventHandlerUPP(doAEOpenApp), noRefCon, false) ;
	aevtErr = AEInstallEventHandler( kCoreEventClass, kAEOpenDocuments,   NewAEEventHandlerUPP(doAEOpenDocument), noRefCon, false) ;
	aevtErr = AEInstallEventHandler( kCoreEventClass, kAEPrintDocuments,  NewAEEventHandlerUPP(doAEPrintDocuments), noRefCon, false) ;
	aevtErr = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(doAEQuit), noRefCon, false) ;
}

//-----------------------------------------------------------------------

void Initialize(void)
{

	// Stock initialization
#if TARGET_API_MAC_CARBON
	InitCursor();
#else
	EventRecord event;
	short count;
	
	InitGraf((Ptr)&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
	
	MaxApplZone();
	for (count = 1; count <= 3; count++)
		EventAvail(everyEvent, &event);		
#endif // TARGET_API_MAC_CARBON

	initAppleEvents();
	
	// Load the menubar
	DoMenuSetup();

	// Initialize QuickTime
	EnterMovies();
	
	// Startup the sequence grabber
	InitializeSequenceGrabber();
        
        // Init Serial port
        InitSerialPort();

        // Menus need to reflect init state
        gChosenSizeItem = kFullSizeItem;
        AdjustMenus();
}

//-----------------------------------------------------------------------

void CreateMonitorWindow(void)
{
	gDialog = GetNewDialog (kMonitorDLOGID, NULL, (WindowPtr) -1L);
	
	gMonitor = GetDialogWindow(gDialog);
}

//-----------------------------------------------------------------------

/* static */ void DoMenuSetup (void)
{	
	Handle	theMenuBar = GetNewMBar (kMenuBarID);
	
	// Set up our menus
	SetMenuBar (theMenuBar);
	DisposeHandle(theMenuBar);
	gAppleMenu = GetMenuHandle (kAppleID);
	gFileMenu = GetMenuHandle (kFileID);
	gEditMenu = GetMenuHandle (kEditID);
	gMonitorMenu = GetMenuHandle (kMonitorID);
	gRunMenu = GetMenuHandle (kMonitorID+1);
#if TARGET_API_MAC_CARBON
{
	long	result = 0;
	
	Gestalt(gestaltMenuMgrAttr, &result);
	if(result & gestaltMenuMgrAquaLayoutMask) {

		// Delete Quit item from the File menu on Mac OS X
//		DeleteMenuItem( gFileMenu, kQuitItem );
//		DeleteMenuItem( gFileMenu, kQuitItem-1 );
	}
}
#else // not TARGET_API_MAC_CARBON
	AppendResMenu(gAppleMenu, 'DRVR');	/* add DA names to Apple menu */
#endif
        // Init menu state
	gChosenSizeItem = kFullSizeItem;
	AdjustMenus();	
#if TARGET_OS_WIN32
	QTMLSetQuitMenuItem(kFileID, kQuitItem);
#endif

}

//-----------------------------------------------------------------------

/* static */ void HandleEvent (void)
{
	ComponentResult	result = noErr;

	// Do system stuff
	HiliteMenu(0);
#if !TARGET_API_MAC_CARBON
	SystemTask();
#endif
	
	// Give some time to the sequence grabber
	if (gSeqGrabber != 0L)
		result = SGIdle (gSeqGrabber);
	
	// Suck an event
	if (WaitNextEvent (everyEvent, &gTheEvent, 0, 0))
	{
		// What was it?
		switch (gTheEvent.what)
		{
			case kHighLevelEvent:
				AEProcessAppleEvent(&gTheEvent);
				break;
			case mouseDown:
			{
				// Handle it
				HandleMouseDown();
				break;
			}
			case keyDown:
			case autoKey:
			{
				char	theChar = gTheEvent.message & charCodeMask;
				long	theMenu = MenuKey (theChar);
                                
                                // Handle Command-Period key
                                if (theChar == '.')
                                    if (gCurrentlyRecording || bRunningSequence)
                                        DoQueryCancelDialog();

				// Handle menu command keys
				HandleMenu(theMenu);				
				break;
			}
			case updateEvt:
			{
				if ((gMonitor != NULL) && ((WindowPtr) (gTheEvent.message) == (WindowPtr) gMonitor))
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
				}
				break;
			}
			default:	// We don't really care about any other events, but you might, so feel free
			{
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------

/* static */ void HandleMouseDown (void)
{	
	WindowPtr	theWindow;
	short		windowCode = MacFindWindow (gTheEvent.where, &theWindow);
	
	// Where was the mouse down?
    switch (windowCode)
	{
#if !TARGET_API_MAC_CARBON
		case inSysWindow:
		{ 
			SystemClick (&gTheEvent, theWindow);
		    break;
	    }
#endif
		case inMenuBar:
		{
			AdjustMenus();
			HandleMenu (0L);
			break;
		}
		case inDrag:
		{
			// Was it the monitor?
			if (theWindow == gMonitor || true)
			{
				ComponentResult	result = noErr;
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
				
				// Pause the sequence grabber
				result = SGPause (gSeqGrabber, true);
				
				if (gVideoChannel != NULL && theWindow == gMonitor)
				{
					// Drag it with the totally cool DragAlignedWindow
	  				result = SGGetChannelBounds (gVideoChannel, &boundsRect);
					DragAlignedWindow (theWindow, gTheEvent.where, &limitRect, &boundsRect, &gSeqGrabberAlignProc);
				}
				else
				{
					DragWindow (theWindow, gTheEvent.where, &limitRect);
				}
				
				// Start up the sequence grabber
				result = SGPause (gSeqGrabber, false);
			}
			break;
		}

		case inGoAway:
	  		if (TrackGoAway(theWindow, gTheEvent.where))
				DoQuit();
			break;

	    default:
	    {
	    	break;
	    }
	}
}

//-----------------------------------------------------------------------

/* static */ void AdjustMenus (void)
{
	Boolean					DA = false;
	ComponentResult			result = noErr;
	short					menuItem=0;

        (void)result;
        
#if !TARGET_API_MAC_CARBON
	register WindowPeek		wp = NULL;
	short					kind = 0;
	
	// What kind of window is frontmost?
	wp = (WindowPeek) FrontWindow();
	kind = wp ? wp->windowKind : 0;
	DA = kind < 0;
#endif
	
	// Set our menu item states appropriately
	
#if !USE_QTFRAMEWORK	
	// Apple menu
	Enable ((Handle) gAppleMenu, kAboutItem, true);	

	// File menu
#if TARGET_API_MAC_CARBON
	Enable ((Handle) gFileMenu, kPageSetupItem, false);
	Enable ((Handle) gFileMenu, kPrintItem, (gVideoChannel != 0L ? false : false));
#else
	Enable ((Handle) gFileMenu, kPageSetupItem, true);
	Enable ((Handle) gFileMenu, kPrintItem, (gVideoChannel != 0L ? true : false));
#endif
#if !TARGET_API_MAC_CARBON
	Enable ((Handle) gFileMenu, kQuitItem, true);
#endif

	// Edit menu
	Enable ((Handle) gEditMenu, kUndoItem, DA);
	Enable ((Handle) gEditMenu, kCutItem, DA || (gVideoChannel != 0L));
	Enable ((Handle) gEditMenu, kCopyItem, DA || (gVideoChannel != 0L));
	Enable ((Handle) gEditMenu, kPasteItem, DA);
	Enable ((Handle) gEditMenu, kClearItem, DA);
#endif

	// Monitor menu
	Enable ((Handle) gMonitorMenu, kVideoSettingsItem, (gVideoChannel != 0L ? true : false));
	Enable ((Handle) gMonitorMenu, kSoundSettingsItem, (gSoundChannel != 0L ? false : false));

	Enable ((Handle) gMonitorMenu, kRecordVideoItem, (gVideoChannel != 0L ? true : false));
	CheckMenuItem (gMonitorMenu, kRecordVideoItem, (gVideoChannel && gRecordVideo)? true : false );
	Enable ((Handle) gMonitorMenu, kRecordSoundItem, (gSoundChannel != 0L ? false : false));
	CheckMenuItem (gMonitorMenu, kRecordSoundItem, (gSoundChannel && gRecordSound)? true : false );
 	Enable ((Handle) gMonitorMenu, kSplitTracksItem, (gSoundChannel && gRecordSound && gVideoChannel && gRecordVideo)? true : false);
 	CheckMenuItem (gMonitorMenu, kSplitTracksItem, gSplitTracks ? false : false);

	Enable ((Handle) gMonitorMenu, kQuarterSizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, kHalfSizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, kFullSizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
//	Enable ((Handle) gMonitorMenu, k128x96SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
//	Enable ((Handle) gMonitorMenu, k160x120SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
//	Enable ((Handle) gMonitorMenu, k176x144SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, k320x240SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, k360x240SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, k352x288SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, k640x480SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, k720x480SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, k704x576SizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	Enable ((Handle) gMonitorMenu, kCustomSizeItem, (gVideoChannel && !gCurrentlyRecording ? true : false));
	for (menuItem = kQuarterSizeItem; menuItem<=kCustomSizeItem; menuItem++) {
		CheckMenuItem (gMonitorMenu, menuItem, menuItem == gChosenSizeItem);
	}

        // Run menu
	Enable ((Handle) gRunMenu, kRecordItem, ((gSoundChannel && gRecordSound) || (gVideoChannel && gRecordVideo) ? true : false));
	Enable ((Handle) gRunMenu, kRecordFrameItem, ((gVideoChannel && gRecordVideo && gCurrentlyRecording) ? true : false));
	{
		Str255 recName;
		GetIndString(recName, 128, gCurrentlyRecording ? 2 : 1);
		SetMenuItemText(gRunMenu,kRecordItem,recName);
	}
	Enable ((Handle) gRunMenu, kProjectorItem, true );
	Enable ((Handle) gRunMenu, kSequencerItem, true );
	Enable ((Handle) gRunMenu, kSlateMarkerItem, true );
 	CheckMenuItem (gRunMenu, kSlateMarkerItem, gSlateMarkers ? true : false);
	Enable ((Handle) gRunMenu, kFrameMarkerItem, true );
 	CheckMenuItem (gRunMenu, kFrameMarkerItem, gFrameMarkers ? true : false);
	Enable ((Handle) gRunMenu, kProjectorTimingItem, true );
	Enable ((Handle) gRunMenu, kSerialItem, true );
	Enable ((Handle) gRunMenu, kVideoDetectItem, true );

	// Draw it
	MacDrawMenuBar();
}

//-----------------------------------------------------------------------

static void Enable (Handle menu, short item, Boolean ok)
{
	// Utility routine to enable and disable menu items
	if (ok)
	{
		MacEnableMenuItem ((MenuHandle) menu, item);
	}
	else
	{
		DisableMenuItem ((MenuHandle) menu, item);
	}
}

//-----------------------------------------------------------------------

/* static */ void HandleMenu (long theMenu)
{	
	long			mSelect;
	short			menuID;
	short			menuItem;
	ComponentResult	result = noErr;	
        
        (void)result;
	
	// Did we get a menu?
	if (theMenu == 0L)
	{
		// Nope, get it from menu select
		mSelect = MenuSelect (gTheEvent.where);
	}
	else
	{
		// Yep, use it
		mSelect = theMenu;
	}
	
	// Decode it
	menuID = HiWord (mSelect);
	menuItem = LoWord (mSelect);
	
	// Which menu is it?
	switch (menuID)
	{
		case kAppleID:
		{
			if (menuItem == kAboutItem)
			{
				// Do the boring about box
				DoAboutDialog();
			}
#if !TARGET_API_MAC_CARBON
			else	// It's a DA
			{
				Str255	name;
				GrafPtr	savedPort;
				
				// Open the DA
				GetPort (&savedPort);
				GetMenuItemText (gAppleMenu, menuItem, name);
				OpenDeskAcc (name);
				SetPort (savedPort);
			}
#endif
			break;
		}
		case kFileID:
		{
			switch (menuItem)
			{
				case kPageSetupItem:
					DoPageSetup();
					break;

				case kPrintItem:
					DoPrint();
					break;
					
				case kQuitItem:
					DoQuit();
					break;
			}
			break;
		}
		case kEditID:
		{
			// Is this a DA kind of thing?
		#if !TARGET_API_MAC_CARBON
			if (!SystemEdit (menuItem - 1))
		#endif
			{
				// We only do cut and copy
				if ((menuItem == kCutItem) || (menuItem == kCopyItem))
					DoCopyToClipboard();
			}
			break;
		}
		case kMonitorID:
		{
			switch (menuItem)
			{
				case kVideoSettingsItem:
					DoVideoSettings();
					break;

				case kSoundSettingsItem:
					DoSoundSettings();
					break;

				case kRecordVideoItem:
					gRecordVideo = !gRecordVideo;
					AdjustMenus();
					break;

				case kRecordSoundItem:
					gRecordSound = !gRecordSound;
					AdjustMenus();
					break;

				case kSplitTracksItem:
					gSplitTracks = !gSplitTracks;
					AdjustMenus();
					break;

				case kQuarterSizeItem:
					gChosenSizeItem = menuItem;
					DoResize(4);
					break;

				case kHalfSizeItem:
					gChosenSizeItem = menuItem;
					DoResize(2);
					break;
					
				case kFullSizeItem:
					gChosenSizeItem = menuItem;
					DoResize(1);
					break;
/*					
				case k128x96SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(128,96);
					break;

				case k160x120SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(160,120);
					break;
					
				case k176x144SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(176, 144);
					break;
*/					
				case k320x240SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(320, 240);
					break;
					
				case k360x240SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(360, 240);
					break;
					
				case k352x288SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(352, 288);
					break;
					
				case k640x480SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(640, 480);
					break;
					
				case k720x480SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(720, 480);
					break;
					
				case k704x576SizeItem:
					gChosenSizeItem = menuItem;
					SetNewSize(704, 576);
					break;
					
				case kCustomSizeItem:
					gChosenSizeItem = menuItem;
					DoResizeCustomDialog();
					break;
                        }
			break;
                }

		case kRunID:
		{
			switch (menuItem)
			{
				case kRecordItem:
					if (!gCurrentlyRecording)
						DoRecordStart();
					else
						DoRecordStop();
					AdjustMenus();
					break;

                                case kRecordFrameItem:
					DoRecordSingleFrame();
					break;

				case kProjectorItem:
					DoProjectorFrame();
					break;

				case kSequencerItem:
					DoSequencerDialog();
					break;
                                        
                                case kSlateMarkerItem:
                                        gSlateMarkers = (gSlateMarkers) ? false : true;
                                        AdjustMenus();
                                        break;
                                        
                                case kFrameMarkerItem:
                                        gFrameMarkers = (gFrameMarkers) ? false : true;
                                        AdjustMenus();
                                        break;
                                        
				case kSerialItem:
					DoSerialDialog();
                                        AdjustMenus();
					break;

				case kProjectorTimingItem:
					DoProjectorTimingDialog();
					break;

                                case kVideoDetectItem:
                                        ShutdownSequenceGrabber();
                                        InitializeSequenceGrabber();
                                        AdjustMenus();
                                        break;

				default:
					break;
			}
		}
		default:
			break;
	}
}

//-----------------------------------------------------------------------

void ShutdownSequenceGrabber(void)
{
	if (gSeqGrabber != 0L)
	{
		CloseComponent (gSeqGrabber);
		gSeqGrabber = 0L;
	}	
	
	if (gMonitor != NULL)
	{
		DisposeWindow(gMonitor);
                gMonitor = NULL;
	}
}

//-----------------------------------------------------------------------

/* static */ void DoQuit (void)
{
        // Intercept quit if recording not saved
        if (gCurrentlyRecording) {
            DoQueryCancelDialog();
            if (bCancelSequence)
                DoRecordStop();
            else
                return;
            }

	// Clean up
        ShutdownSequenceGrabber();
        
        // Close serial port driver
        ExitSerialPort();
	
	// Set quit flag
	gQuitFlag = true;
	
	ExitMovies();
}

//-----------------------------------------------------------------------