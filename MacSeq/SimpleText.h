/*
	File:		SimpleText.h

	Contains:	defines input for both .c and .r files.
				structured so as to compile correctly in both.

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

#ifndef __SIMPLETEXT__
#define __SIMPLETEXT__


//#define kScrollBarSize				15		// size of a scroll bar, in pixels
#ifndef REZ
extern SInt16	kScrollBarSize;
#endif

// Internal errors -- WARNING: some also appear in SimpleText.r
#define eActionAlreadyHandled		100		// the action was handled by the object, don't report an error
#define eUserCanceled			101		// user canceled an operation

#define eDocumentTooLarge		200		// document too large to be opened
#define eDocumentWrongKind		201		// document cannot be opened by this application
#define eErrorWhileDrawing		202		// error while drawing the document
#define eDocumentContainsPS		203		// document contains PostScript, it may not display properly
#define eMachineToOld			204
#define eDocumentNotModifiable		205		// document can't be modified
#define eDocumentAlreadyOpen		206
#define eDocumentContainsNoPages	207

#define eDocumentHasNoContentsEntries 208	

// icon IDs -- WARNING: Also appear in SimpleText.r
#define kTextIcon				129
#define kReadOnlyIcon				130
#define kPICTIcon				131
#define kStationeryIcon				132

// menu and item defines -- define 'MENU' 'MCMD' 'hmnu' and 'STR#' IDs for menus!
#define rMenuBar			128

#define	mApple				128
#define	mFile				129
#define	mEdit				130
#define mRun				131		
#define mFont				132
#define mSize				133
#define mStyle				134
#define	mSound				135
#define mVoices				136
#define mContents			137

#define mLastMenu			mContents
#define mNumberMenus			10

#define mFontSubMenusStart		200

// Command numbers
#define cNull				0
#define cAbout				1
#define cDeskAccessory			2
#define cTypingCommand			3

#define cNew				10
#define cOpen				11
#define cClose				12
#define cSave				13
#define cSaveAs				14
#define cPageSetup			15
#define cPrint				16
#define cPrintOneCopy			17
#define cQuit				18

#define cUndo				20
#define cCut				21
#define cCopy				22
#define cPaste				23
#define cClear				24
#define cSelectAll			25
#define cFind				26
#define cFindAgain			27
#define cFindSelection			28
#define cReplace			29
#define cReplaceAgain			30
#define cNextPage			31
#define cPreviousPage			32
#define cGotoPage			33
	#define cGotoLast			32767
	#define cGotoFirst			-32767
#define cShowClipboard			34

#define cSelectFont			35
#define cSelectFontStyle		36
#define cExecute			37	// for AppleScript

#define cSize9				40
#define cSize10				41
#define cSize12				42
#define cSize14				43
#define cSize18				44
#define cSize24				45
#define cSize36				46

#define cPlain				50
#define cBold				51
#define cItalic				52
#define cUnderline			53
#define cOutline			54
#define cShadow				55
#define cCondensed			56
#define cExtended			57

#define cRecord				60
#define cPlay				61
#define cErase				62
#define cSpeak				63
#define cStopSpeaking			64
#define cSelectVoice			65
#define cSelectVoiceSubMenu		66

#define cSelectContents			75

#define cRunCamera			81
#define cRunProjector			82
#define cRunSequencer			83
#define cRunScript			84
#define cRunLoop			85
#define cRunRecord			86
#define cRunCounter			87
#define cRunSerial			88
#define cRunKeypad			89		

// windows, dialogs, alerts, and other items of that sort
#define kDefaultWindowID		128
#define kSaveChangesWindowID		129
#define kFindWindowID			130
#define kReplaceWindowID		131
	#define iFindEdit				4
	#define iCaseSensitive				5
	#define iWrapAround				6
	#define iReplaceEdit				8
	#define iReplaceAll				9
#define kSequencerWindowID		132
#define kSequencerQuitWindowID		133
#define kSerialPortWindowID		134
#define kSerialDetectWindowID		135
#define kScriptEmptyWindowID		136
#define kFrameCounterWindowID		137
#define kRunFrameCounterWindowID	138
#define kRunLoopWindowID		139
#define kKeyMapWindowID			140
	
// Error STR# base
#define kErrorBaseID			1000

// Help for window STR#
#define kWindowHelpID			2000
	#define iDidTheBalloon				-1
	#define iNoBalloon				0
	
	#define iHelpActiveScroll			1
	#define iHelpDimHorizScroll			2
	#define iHelpDimVertScroll			3
	#define iHelpGrowBox				4
	#define iHelpGenericContent			5

	#define iHelpPictContent			6
	#define iHelpPictSelection			7
	#define iHelpTextContent			8

// Miscellaneous strings (STR#)
#define kMiscStrings			3000
#define iFirstNewDocumentTitle	1
#define iHelpMenuCommand		2
#define iSelectAllCommand		3
#define iSelectNoneCommand		4

// Patterns used for selections
#define kPatternListID		128

// for disabling menu items
#define AllItems	0b1111111111111111111111111111111	/* 31 flags */
#define NoItems		0b0000000000000000000000000000000
#define MenuItem1	0b0000000000000000000000000000001
#define MenuItem2	0b0000000000000000000000000000010
#define MenuItem3	0b0000000000000000000000000000100
#define MenuItem4	0b0000000000000000000000000001000
#define MenuItem5	0b0000000000000000000000000010000
#define MenuItem6	0b0000000000000000000000000100000
#define MenuItem7	0b0000000000000000000000001000000
#define MenuItem8	0b0000000000000000000000010000000
#define MenuItem9	0b0000000000000000000000100000000
#define MenuItem10	0b0000000000000000000001000000000
#define MenuItem11	0b0000000000000000000010000000000
#define MenuItem12	0b0000000000000000000100000000000
#define MenuItem13	0b0000000000000000001000000000000
#define MenuItem14	0b0000000000000000010000000000000
#define MenuItem15	0b0000000000000000100000000000000
#define MenuItem16	0b0000000000000001000000000000000
#define MenuItem17	0b0000000000000010000000000000000
#define MenuItem18	0b0000000000000100000000000000000
#define MenuItem19	0b0000000000001000000000000000000

// Size resource information
#define kPreferredSize		1024*1024
#define kMinimumSize		512*1024

#ifndef REZ

	enum {
		// Keyboard virtual keycode constants
		kHome		= 0x73,
		kEnd		= 0x77,
		kPageUp		= 0x74,
		kPageDown	= 0x79,
		kUpArrow	= 0x7E,
		kDownArrow	= 0x7D,
		kLeftArrow	= 0x7B,
		kRightArrow	= 0x7C,
		kF1		= 0x7A,
		kF2		= 0x78,
		kF3		= 0x63,
		kF4		= 0x76,
	
		kFromTopTipOffset = 20,						// offset from top/left of balloons
		kFromBottomTipOffset = 7,					// offset from bottom/right of balloons
		
		kRAMNeededForNew = 64*1024,					// amount of RAM free before we allow New documents
		kGrowScrollAdjust = 13,						// amount to cull scroll bar to make room for growbox
		
		kMinDocSize		= 	128						// min window size in pixels
		};

	enum
		{
		kMaxWaitTime	=	5*60*60				// maximum time to pass to WaitNextEvent()
		};
		
	struct MachineInfoRec
		{
		Boolean			amInBackground;			// are we running in the background or foreground
		Boolean			isQuitting;			// App is quitting, Nav Services dialogs are up for saving
		Boolean			isClosing;			// All windows being closed, Nav dialogs are up for saving
		long			documentCount;			// # of new docs we have made
		short			lastBalloonIndex;		// identifier of last balloon we displayed

		Boolean			haveQuickTime;			// do we have QuickTime installed?
		Boolean			haveRecording;			// do we have sound input?
		Boolean			haveTTS;			// do we have text to speech?
		Boolean			haveTSM;			// do we have text services?
		Boolean			haveTSMTE;			// have inline support for TE?
		Boolean			haveDragMgr;			// do we have the Drag Manager?
		Boolean			haveThreeD;			// do we have 3D on this machine?
		Boolean			haveAppleGuide;			// do we have AppleGuide?
		Boolean			haveThreads;			// do we have threads?
		Boolean			haveAppearanceMgr;		// do we have appearance manager?
		Boolean			haveProxyIcons;			// do we have document proxy icons?
		Boolean			haveFloatingWindows;		// do we have floating windows?
		
		Boolean			haveNavigationServices;		// do we have Navigation Services?
		};
	typedef struct MachineInfoRec MachineInfoRec, *MachineInfoPtr;

	#ifndef CompilingMain
	extern MachineInfoRec 	gMachineInfo;
	extern EventRecord		gEvent;
	extern Str255			gFindString, gReplaceString;
	extern Boolean			gWrapAround, gCaseSensitive;
	#endif
	
	
	struct PreflightRecord
		{
		Boolean		continueWithOpen;	// continue with the opening of the window
		Boolean		needResFork;		// create resfork if none exists
		SignedByte	openKind;		// kind of opening of this file
		short		resourceID;		// resource ID of the 'WIND' resource
		Boolean		wantHScroll;		// want a horizontal scroll bar
		Boolean		wantVScroll;		// want a vertical scroll bar
		Boolean		doZoom;			// want window opened large
		long		storageSize;		// data for the window's refCon
		void *		makeProcPtr;		// pointer to the make proc
		OSType		fileType;		// file type of document
		};
	typedef struct PreflightRecord PreflightRecord, *PreflightPtr;
	
	struct LongRect
		{
		long	top;
		long	left;
		long	bottom;
		long	right;
		};
	typedef struct LongRect LongRect;

	typedef OSStatus 	(*UpdateWindowProc)			(WindowPtr pWindow, void* refCon);
	typedef OSStatus	(*CommandProc)				(WindowPtr pWindow, void* refCon, short commandID, long menuResult);
	typedef OSStatus 	(*PreflightWindowProc)			(PreflightPtr pPreflightData);
	typedef OSStatus 	(*MakeWindowProc)			(WindowPtr pWindow, void* refCon);
	typedef OSStatus	(*PreMenuAccessProc)			(WindowPtr pWindow, void* refCon);
	typedef OSStatus 	(*AdjustMenusProc)			(WindowPtr pWindow, void* refCon);
	typedef OSStatus 	(*CloseWindowProc)			(WindowPtr pWindow, void* refCon);
	typedef OSStatus 	(*GetDocumentRectProc)			(WindowPtr pWindow, void* refCon, LongRect * documentRectangle, Boolean forGrow);
	typedef OSStatus 	(*ContentClickProc)			(WindowPtr pWindow, void* refCon, EventRecord * event);
	typedef OSStatus 	(*ScrollContentProc)			(WindowPtr pWindow, void* refCon, short deltaH, short deltaV);
	typedef OSStatus	(*ActivateEventProc)			(WindowPtr pWindow, void* refCon, Boolean activating);
	typedef OSStatus	(*KeyEventProc)				(WindowPtr pWindow, void* refCon, EventRecord * event, Boolean isMotionKey);
	typedef OSStatus	(*AdjustSizeProc)			(WindowPtr pWindow, void* refCon, Boolean *didResize);
	typedef OSStatus	(*AdjustCursorProc)			(WindowPtr pWindow, void* refCon, Point * localMouse, RgnHandle globalRgn);
	typedef OSStatus	(*PrintPageProc)			(WindowPtr pWindow, void* refCon, Rect* pageRect, long* pageNum);
	typedef OSStatus	(*GetBalloonProc)			(WindowPtr pWindow, void* refCon, Point *localMouse, short * returnedBalloonIndex, Rect *returnedRectangle);
	typedef OSStatus	(*DragTrackingProc)			(WindowPtr pWindow, void *refCon, DragReference theDragRef, short message);
	typedef OSStatus	(*DragReceiveProc)			(WindowPtr pWindow, void *refCon, DragReference theDragRef);
	typedef OSStatus	(*DragAddFlavorsProc)			(WindowPtr pWindow, void *refCon, DragReference theDragRef);
	typedef OSStatus	(*GetCoachRectangleProc)		(WindowPtr pWindow, void* refCon, Rect *pRect, Ptr name);
	typedef OSStatus	(*SaveToProc)				(WindowPtr pWindow, void* refCon, FSRef *fileRef, Boolean isStationery );

	typedef Boolean		(*FilterEventProc)			(WindowPtr pWindow, void* refCon, EventRecord * event);
	typedef long		(*CalculateIdleTimeProc)		(WindowPtr pWindow, void* refCon);

	struct WindowDataRecord
		{
		WindowPtr			theWindow;
		ResType				windowKind;		// and kind to identify window type
		OSType				originalFileType;	// original source of file
		AliasHandle			fileAlias;		// alias handle of the file
		FSRef				fileRef;		// file reference of the file
		short				dataRefNum;		// data fork refNum
		short				resRefNum;		// res fork refNum
		Boolean				isWritable;		// can you write to this file?
		Boolean				bumpUntitledCount;	// does this kind of document use up an "untitled" number?
		Boolean				openAsNew;		// open as an untitled document?
		Boolean				changed;		// has content changed?
		Boolean				documentAcceptsText;	// document allows typing
		Boolean				dragWindowAligned;	// drag using DragAlignedWindow
		Boolean				isClosing;		// window is in the process of being closed,
									// waiting on a Nav Services dialog

		// Standard procedure entry points, may be NIL to get default behavior
		PreflightWindowProc		pPreflightWindow;
		MakeWindowProc			pMakeWindow;
		CloseWindowProc			pCloseWindow;
		
		GetDocumentRectProc		pGetDocumentRect;
		ScrollContentProc		pScrollContent;
		AdjustSizeProc			pAdjustSize;
		AdjustCursorProc		pAdjustCursor;
		GetBalloonProc			pGetBalloon;
		DragTrackingProc		pDragTracking;
		DragReceiveProc			pDragReceive;
		DragAddFlavorsProc		pDragAddFlavors;

		FilterEventProc			pFilterEvent;
		ActivateEventProc		pActivateEvent;
		UpdateWindowProc		pUpdateWindow;
		PreMenuAccessProc		pPreMenuAccess;
		AdjustMenusProc			pAdjustMenus;
		KeyEventProc			pKeyEvent;
		ContentClickProc		pContentClick;
		CommandProc			pCommand;
		PrintPageProc			pPrintPage;
		CalculateIdleTimeProc		pCalculateIdleTime;
		GetCoachRectangleProc		pGetCoachRectangle;
		SaveToProc			pSaveTo;

		// controls that the default implementations use
		PMPrintSession			printSession;		// new carbon print session record
		Handle				hPageFormat;		// new carbon print record for the page format object (flattened)
		//DMG850 Handle				hPrintSettings;		// new carbon print record for the print settings object (flattened)
		Boolean				hasGrow;		// has a grow box?
		Rect				contentRect;		// content area, minus the scroll bars
		short				minHSize;		// minimum window X size, 0 default
		short				minVSize;		// minimum window Y size, 0 default
		ControlHandle			hScroll;		// horizontal scroll bar
		ControlHandle			vScroll;		// vertical scroll bar
		short				hScrollAmount;		// amount to scroll in left/right arrow
		short				vScrollAmount;		// amount to scroll in up/down arrow
		short				hScrollOffset;		// offset of scrollbar from left side of window
		short				vScrollOffset;		// offset of scrollbar from top of window
		short				oldVValue;		// previous vertical value
		short				oldHValue;		// previous horizontal value
		
		// variables having to do with input support
		TSMTERecHandle			docTSMRecHandle;	// TSM info stored here
		TSMDocumentID			docTSMDoc;		// TSM document id
		
		// Info needed for NavServices state.
		NavDialogRef			navDialog;		// The sheet window attached to this document, if any

		// Custom data follows here
		};
	typedef struct WindowDataRecord WindowDataRecord, *WindowDataPtr;	

	void SetDocumentContentChanged( WindowDataPtr pData, Boolean changed );

        // MacSeq Text insertion/extraction routines
        void InsertScriptText(Str255 ScriptText);
        void ExtractScriptText(char **pText, short *pSize, short *pNumLines, short **pLineStarts);

	// Window adjustment routines
	OSStatus AdjustScrollBars(WindowPtr pWindow, Boolean moveControls, Boolean didGrow, Boolean *needInvalidate);
	void SetControlAndClipAmount(ControlHandle control, short * amount);
	OSStatus DoScrollContent(WindowPtr pWindow, WindowDataPtr pData, short deltaH, short deltaV);

	// document rectangle utilities
	void LongRectToRect(LongRect* longRect, Rect *rect);
	void RectToLongRect(Rect *rect, LongRect *longRect);
	void MovableModalDialog(ModalFilterProcPtr filterProc, short * pItem);
	void BeginMovableModal(void);
	void EndMovableModal(void);
	void GetPICTRectangleAt72dpi(PicHandle hPicture, Rect *pictureRect);
	
	// menu command utilities
	void EnableCommand(short commandID);
	void EnableCommandCheck(short commandID, Boolean check);
	void EnableCommandCheckStyle(short commandID, Boolean check, short style);
	void ChangeCommandName(short commandID, short resourceID, short resourceIndex);
	Boolean IsCommandEnabled(short commandID);

	// printing utility routines
	OSStatus	DoDefault(WindowDataPtr 	pData);
	OSStatus	DoPageSetup(WindowPtr pWindow);

	// event handling routines
	void HandleEvent(EventRecord *);
	void DragAndDropArea(WindowPtr pWindow, WindowDataPtr pData, EventRecord* event, Rect *pFrameRect);
	short ConductFindOrReplaceDialog(short dialogID);
	Boolean PerformSearch(
			Handle	h,			// handle to search
			long start,			// offset to begin with
			Str255 searchString,		// string to search for
			Boolean isCaseSensitive,	// case sensitive search
			Boolean isBackwards,		// search backwards from starting point
			Boolean isWraparound,		// wrap search around from end->begining
			long * pNewStart,		// returned new selection start
			long * pNewEnd);		// returned new selection end

	// Drag utils
	Boolean IsOnlyThisFlavor(DragReference theDragRef, FlavorType theType);
	Boolean IsDropInFinderTrash(AEDesc *dropLocation);
	Boolean DragText(WindowPtr pWindow, void *pData, EventRecord *pEvent, RgnHandle hilightRgn);
	OSStatus TextDragTracking(WindowPtr pWindow, void *pData, DragReference theDragRef, short message);
	OSStatus TextDragReceive(WindowPtr pWindow, void *pData, DragReference theDragRef);
	
	// TEClick utilities
	pascal void TextClickLoop(void);
	pascal TEClickLoopUPP GetOldClickLoop(void);
	void AdjustTE(WindowDataPtr pData, Boolean doScroll);

	// error utilities
	void ConductErrorDialog(OSStatus error, short commandID, short alertType);

	// generic utilities
	OSStatus DoAdjustCursor(WindowPtr pWindow, Point *where);
	void LocalToGlobalRgn(RgnHandle rgn);
	void GlobalToLocalRgn(RgnHandle rgn);
	void SetWatchCursor(void);
	OSStatus SaveCurrentUndoState(WindowDataPtr pData, short newCommandID);
	pascal void MyDrawHook ( unsigned short offset, unsigned short textLen,
			Ptr textPtr, TEPtr tePtr, TEHandle teHdl );
	
	// Preflight routines for the windows we support
	OSStatus AboutPreflightWindow(PreflightPtr pPreflightData);
	OSStatus PICTPreflightWindow(PreflightPtr pPreflightData);
	OSStatus MoviePreflightWindow(PreflightPtr pPreflightData);
	OSStatus ClipboardPreflightWindow(PreflightPtr pPreflightData);
	OSStatus TextPreflightWindow(PreflightPtr pPreflightData);
	OSStatus ThreeDPreflightWindow(PreflightPtr pPreflightData);
	
	// File type routines for the document kinds we support
	void AboutGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes);
	void PICTGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes);
	void MovieGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes);
	void ClipboardGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes);
	void TextGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes);
	void ThreeDGetFileTypes(OSType * pFileTypes, OSType * pDocumentTypes, short * numTypes);
	
	// selection rectangle utilities
	#define MOVESELECTION(X) (((X) & 0x7) == 0x4)
	void DrawSelection(WindowDataPtr pData, Rect *pSelection, short * pPhase, Boolean bumpPhase);
	OSStatus SelectContents(WindowPtr pWindow, WindowDataPtr pData, EventRecord *pEvent, Rect *pSelection, Rect *pContent, short *pPhase);

	Boolean	AdjustMenus(WindowPtr pWindow, Boolean editDialogs, Boolean forceTitlesOn);
	
#endif

// defined window kinds and resource ranges for windows we support
#define kAboutWindow		'ABOT'
#define kAboutBaseID		200

#define kPICTWindow		'PICT'
#define kPICTBaseID		300

#define kMovieWindow		'MooV'
#define kMovieBaseID		400

#define kClipboardWindow	'Clip'
#define kClipboardBaseID	500

#define kTextWindow		'TEXT'
#define kTextBaseID		600

#define kThreeDWindow		'3DMF'
#define kThreeDBaseID		800

#endif
