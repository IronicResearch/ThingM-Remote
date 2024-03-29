/*
	File:		SimpleText.r

	Contains:	resources for a simple document editing application for shipping
				with system software.

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

#ifndef SystemSevenOrLater
	#define SystemSevenOrLater 1
#endif

#ifdef USE_UMBRELLA_HEADERS && USE_UMBRELLA_HEADERS
#include "Carbon.r"
#else
#include "MacTypes.r"
#include "Dialogs.r"
#include "Balloons.r"
#include "Menus.r"
#include "Finder.r"
#include "Quickdraw.r"
#include "Icons.r"
#include "Processes.r"
#endif

#include "SimpleText.h"
#include "Localize.r"

// --------------------------------------------------------------------------------------------------------------
// This is Carbonized application, so 'carb' 0 resource indicates this
// --------------------------------------------------------------------------------------------------------------
data 'carb' (0) {
};

// --------------------------------------------------------------------------------------------------------------
// MENUS AND MENU BARS
// --------------------------------------------------------------------------------------------------------------

type 'MCMD' {
	integer = $$CountOf(Commands);
	array Commands {
		integer;			// list of commands
		};
};

resource 'MBAR' (rMenuBar, preload) {
#if 0
	{ mApple, mFile, mEdit, mFont, mSize, mStyle, mSound };	/* our menus */
#else        
	{ mApple, mFile, mEdit, mRun }; /* our menus -- beware of typos in new items */
#endif
};

resource 'MENU' (mApple, "Apple", preload) {
	mApple, textMenuProc,
	AllItems & ~MenuItem2,
	enabled, apple,
	{
	ABOUTITEM,			noicon, nokey, nomark, plain;
	"-",				noicon, nokey, nomark, plain;
	}
};

resource 'MCMD' (mApple, purgeable) {
	{
		cAbout;
	cNull;
		cDeskAccessory;
	}
};

resource 'MENU' (mFile, FILEMENU, preload) {
	mFile, textMenuProc,
	AllItems & ~MenuItem3 & ~MenuItem7 & ~MenuItem10,
	enabled, FILEMENU,
	{
		FILENEWITEM, noicon, 			FILENEWKEY, nomark, plain;
		FILEOPENITEM, noicon, 			FILEOPENKEY, nomark, plain;
		"-", noicon,		 			nokey, nomark, plain;
		FILECLOSEITEM, noicon, 			FILECLOSEKEY, nomark, plain;
		FILESAVEITEM, noicon, 			FILESAVEKEY, nomark, plain;
		FILESAVEASITEM, noicon, 		FILESAVEASKEY, nomark, plain;
		"-", noicon,		 			nokey, nomark, plain;
		FILEPAGESETUPITEM, noicon, 		FILEPAGESETUPKEY, nomark, plain;
		FILEPRINTITEM, noicon, 			FILEPRINTKEY, nomark, plain;
		FILEPRINTONECOPYITEM, noicon, 	FILEPRINTONECOPYKEY, nomark, plain;
	}
};

resource 'MCMD' (mFile, purgeable) {
	{
		cNew;
		cOpen;
	cNull;
		cClose;
		cSave;
		cSaveAs;
	cNull;
		cPageSetup;
		cPrint;
		cPrintOneCopy;
	cNull;
		cQuit;
	}
};

resource 'MENU' (mEdit, EDITMENU, preload) {
	mEdit, textMenuProc,
	AllItems & ~MenuItem2 & ~MenuItem7 & ~MenuItem9 & ~MenuItem14 & ~MenuItem18,
	enabled, EDITMENU,
	{
		EDITUNDOITEM, noicon, 			EDITUNDOKEY, nomark, plain;
		"-", noicon,		 			nokey, nomark, plain;
		EDITCUTITEM, noicon, 			EDITCUTKEY, nomark, plain;
		EDITCOPYITEM, noicon, 			EDITCOPYKEY, nomark, plain;
		EDITPASTEITEM, noicon, 			EDITPASTEKEY, nomark, plain;
		EDITCLEARITEM, noicon, 			EDITCLEARKEY, nomark, plain;
		"-", noicon,		 			nokey, nomark, plain;
		EDITSELECTALLITEM, noicon, 		EDITSELECTALLKEY, nomark, plain;
		"-", noicon,		 			nokey, nomark, plain;
		FINDITEM, noicon, 				FINDKEY, nomark, plain;
		FINDAGAINITEM, noicon, 			FINDAGAINKEY, nomark, plain;
		USESELECTIONFORFIND, noicon, 	SELECTIONFORFINDKEY, nomark, plain;
		REPLACEITEM, noicon, 			REPLACEKEY, nomark, plain;
		REPLACEAGAINITEM, noicon, 		REPLACEAGAINKEY, nomark, plain;
#if 1
		"-", noicon,		 			nokey, nomark, plain;
		EXECUTEITEM, noicon,			EXECUTEKEY, nomark, plain;			// for AppleScript
		"-", noicon,		 			nokey, nomark, plain;			// for AppleScript
		EDITNEXTPAGEITEM, noicon, 		EDITNEXTPAGEKEY, nomark, plain;
		EDITPREVPAGEITEM, noicon, 		EDITPREVPAGEKEY, nomark, plain;
		EDITGOTOPAGEITEM, noicon, 		EDITGOTOPAGEKEY, nomark, plain;
		"-", noicon,		 			nokey, nomark, plain;
		EDITSHOWCLIPBOARDITEM, noicon,	EDITSHOWCLIPBOARDKEY, nomark, plain;
#endif
	}
};

resource 'MCMD' (mEdit, purgeable) {
	{
		cUndo;
	cNull;
		cCut;
		cCopy;
		cPaste;
		cClear;
	cNull;
		cSelectAll;
	cNull;
		cFind;
		cFindAgain;
		cFindSelection;
		cReplace;
		cReplaceAgain;
	cNull;
				// for AppleScript
		cExecute;																				// for AppleScript
	cNull;
		cNextPage;
		cPreviousPage;
		cGotoPage;
	cNull;
		cShowClipboard;
	}
};

resource 'MENU' (mFont, FONTMENU, preload) {
	mFont, textMenuProc,
	AllItems,
	enabled, FONTMENU,
	{
	}
};

resource 'MCMD' (mFont, purgeable) {
	{
		cSelectFont;
	}
};

resource 'MENU' (mSize, SIZEMENU, preload) {
	mSize, textMenuProc,
	AllItems,
	enabled, SIZEMENU,
	{
	SIZE9POINT, noicon, 			SIZE9POINTKEY, nomark, plain;
	SIZE10POINT, noicon, 			SIZE10POINTKEY, nomark, plain;
	SIZE12POINT, noicon, 			SIZE12POINTKEY, nomark, plain;
	SIZE14POINT, noicon, 			SIZE14POINTKEY, nomark, plain;
	SIZE18POINT, noicon, 			SIZE18POINTKEY, nomark, plain;
	SIZE24POINT, noicon, 			SIZE24POINTKEY, nomark, plain;
	SIZE36POINT, noicon, 			SIZE36POINTKEY, nomark, plain;
	}
};

resource 'MCMD' (mSize, purgeable) {
	{
		cSize9;
		cSize10;
		cSize12;
		cSize14;
		cSize18;
		cSize24;
		cSize36;
	}
};

resource 'MENU' (mStyle, STYLEMENU, preload) {
	mStyle, textMenuProc,
	AllItems,
	enabled, STYLEMENU,
	{
	STYLEPLAIN, noicon, 			STYLEPLAINKEY, nomark, plain;
	STYLEBOLD, noicon, 				STYLEBOLDKEY, nomark, bold;
	STYLEITALIC, noicon, 			STYLEITALICKEY, nomark, italic;
	STYLEUNDERLINE, noicon, 		STYLEUNDERLINEKEY, nomark, underline;
	STYLEOUTLINE, noicon, 			STYLEOUTLINEKEY, nomark, outline;
	STYLESHADOW, noicon, 			STYLESHADOWKEY, nomark, shadow;
	STYLECONDENSED, noicon, 		STYLECONDENSEDKEY, nomark, condense;
	STYLEEXTENDED, noicon, 			STYLEEXTENDEDKEY, nomark, extend;
	}
};

resource 'MCMD' (mStyle, purgeable) {
	{
		cPlain;
		cBold;
		cItalic;
		cUnderline;
		cOutline;
		cShadow;
		cCondensed;
		cExtended;
	}
};

resource 'MENU' (mSound, SOUNDMENU, preload) {
	mSound, textMenuProc,
	AllItems,
	enabled, SOUNDMENU,
	{
		SOUNDRECORDITEM, noicon, 		SOUNDRECORDKEY, nomark, plain;
		SOUNDPLAYITEM, noicon, 			SOUNDPLAYKEY, nomark, plain;
		SOUNDERASEITEM, noicon, 		SOUNDERASEKEY, nomark, plain;
		"-", noicon,		 			nokey, nomark, plain;
		SOUNDSPEAKALLITEM, noicon, 		SOUNDSPEAKKEY, nomark, plain;
		SOUNDSTOPSPEAKINGITEM, noicon, 	SOUNDSTOPSPEAKINGKEY, nomark, plain;
		SOUNDVOICESITEM, noicon, 		SOUNDVOICESKEY, nomark, plain;
		
	}
};

resource 'MCMD' (mSound, purgeable) {
	{
		cRecord;
		cPlay;
		cErase;
	cNull;
		cSpeak;
		cStopSpeaking;
		cSelectVoice;
	}
};

resource 'MENU' (mVoices, SOUNDVOICESITEM, preload) {
	mVoices, textMenuProc,
	AllItems,
	enabled, SOUNDVOICESITEM,
	{
		NOVOICESITEM, noicon, 		NOVOICESKEY, nomark, plain;
	}
};

resource 'MCMD' (mVoices, purgeable) {
	{
	cSelectVoiceSubMenu;
	}
};

resource 'MCMD' (mContents, purgeable) {
	{
	cSelectContents;
	}
};

resource 'MENU' (mRun, RUNMENU, preload) {
	mRun, textMenuProc,
	AllItems,
	enabled, RUNMENU,
	{
	RUNCAM, noicon, 			RUNCAMKEY, nomark, plain;
	RUNPRJ, noicon, 			RUNPRJKEY, nomark, plain;
	RUNSEQ, noicon, 			RUNSEQKEY, nomark, plain;
        "-", noicon,		 		nokey, nomark, plain;
	RUNSCRIPT, noicon, 			RUNSCRIPTKEY, nomark, plain;
	RUNLOOP, noicon, 			RUNLOOPKEY, nomark, plain;
        "-", noicon,		 		nokey, nomark, plain;
	RUNRECORD, noicon, 			RUNRECORDKEY, nomark, plain;
	RUNCOUNTER, noicon, 			RUNCOUNTERKEY, nomark, plain;
        "-", noicon,		 		nokey, nomark, plain;
	RUNSERIAL, noicon, 			RUNSERIALKEY, nomark, plain;
	RUNKEYPAD, noicon, 			RUNKEYPADKEY, nomark, plain;
	}
};

resource 'MCMD' (mRun, purgeable) {
	{
		cRunCamera;
		cRunProjector;
		cRunSequencer;
	cNull;
		cRunScript;
		cRunLoop;
	cNull;
		cRunRecord;
		cRunCounter;
	cNull;
		cRunSerial;
		cRunKeypad;
	}
};

// --------------------------------------------------------------------------------------------------------------
// windows, dialogs, alerts, and other items of that sort
// --------------------------------------------------------------------------------------------------------------

resource 'WIND' (kDefaultWindowID) {
	{40, 40, 508, 400}, zoomDocProc, invisible, goAway, 0, NEWDOCUMENTTITLE, staggerMainScreen
};
resource 'STR#' (kMiscStrings) {
	{
	FIRSTNEWDOCUMENTTITLE,
	HELPMENUCOMMAND,
	EDITSELECTALLITEM,
	EDITSELECTNONEITEM,
	}
};

// --------------------------------------------------------------------------------------------------------------
// "do you want to save?" dialog
// --------------------------------------------------------------------------------------------------------------

resource 'DLOG' (kSaveChangesWindowID) {
	{94, 80, 211, 494},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kSaveChangesWindowID,
	"",
	alertPositionParentWindowScreen
};

resource 'dlgx' (kSaveChangesWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kSaveChangesWindowID) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{88, 308, 108, 379},
		Button {
			enabled,
			SAVESAVEBUTTON
		},
		/* [2] */
		{88, 220, 108, 299},
		Button {
			enabled,
			SAVECANCELBUTTON
		},
		/* [3] */
		{88, 44, 108, 155},
		Button {
			enabled,
			SAVEDONTSAVEBUTTON
		},
		/* [4] */
		{10, 20, 42, 52},
		Icon {
			disabled,
			2
		},
		/* [5] */
		{10, 72, 76, 380},
		StaticText {
			disabled,
			SAVESTATICTEXT
		}
	}
};

resource 'STR ' (kSaveChangesWindowID) {
	SAVEBUTTONSHORTCUTS
};

// --------------------------------------------------------------------------------------------------------------
// "find and replace" dialogs
// --------------------------------------------------------------------------------------------------------------
#define BUTTONY	92

resource 'DLOG' (kFindWindowID) {
	{0, 0, BUTTONY+20+8, 352+14},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	kFindWindowID,
	"",
	alertPositionParentWindow
};

resource 'dlgx' (kFindWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kFindWindowID, purgeable) {
	{	
		/* [1] */
		{BUTTONY, 278, BUTTONY+20, 356},
			Button { enabled, FINDBUTTON },
			
		/* [2] */
		{BUTTONY, 168, BUTTONY+20, 257},
			Button { enabled, FINDCANCELBUTTON },

		/* [3] */
		{8, 12, 26, 356},
			StaticText { disabled, FINDSTATIC },
			
		/* [4] */
		{32, 12, 48, 356},
			EditText { disabled, "" },
			
		/* [5] */
		{60, 8, 78, 160},
			CheckBox { enabled, FINDCASESENSITIVE },
		
		/* [6] */
		{60, 184, 78, 356},
			CheckBox { enabled, FINDWRAPAROUND },
						
	}
};

#define BUTTONY	150

resource 'DLOG' (kReplaceWindowID) {
	{0, 0, BUTTONY+24+12, 400},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	kReplaceWindowID,
	"",
	alertPositionParentWindow
};

resource 'dlgx' (kReplaceWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kReplaceWindowID, purgeable) {
	{	
		/* [1] */
		{BUTTONY, 296, BUTTONY+20, 387},
			Button { enabled, REPLACEBUTTON },
	
		/* [2] */
		{BUTTONY, 180, BUTTONY+20, 272},
			Button { enabled, FINDCANCELBUTTON },

		/* [3] */
		{8, 8, 28, 380},
			StaticText { disabled, FINDSTATIC },

		/* [4] */
		{36, 8, 52, 380},
			EditText { disabled, "" },
			
		/* [5] */
		{120, 8, 138, 148},
			CheckBox { enabled, FINDCASESENSITIVE},
			
		/* [6] */
		{120, 192, 138, 384},
			CheckBox { enabled, FINDWRAPAROUND },

		// ITEMS ABOVE THIS POINT SHOULD MATCH UP WITH THE FIND DIALOG
		
		/* [7] */
		{64, 8, 84, 384},
			StaticText { disabled, REPLACESTATIC },

		/* [8] */
		{92, 8, 108, 380},
			EditText { disabled, "" },
			
		/* [9] */
		{BUTTONY, 10, BUTTONY+20, 124},
			Button { enabled, REPLACEALLBUTTON },

	}
};


// --------------------------------------------------------------------------------------------------------------

// Error strings for the various commands

resource 'STR#'	(kErrorBaseID + cNull, purgeable) {
	{
	"204";		ERRSTARTUPVERSION;
	"0";		ERRSTARTUPFAILED;
	};
};


resource 'STR#'	(kErrorBaseID + cNew, purgeable) {
	{
	"0";		ERRNEWFAILEDRAM;
	};
};

resource 'STR#'	(kErrorBaseID + cOpen, purgeable) {
	{
	"-108";		ERROPENOUTOFMEM;
	"-1708";	ERROPENANOTHERFAILED;
	"200";		ERROPENSIZE;
	"201";		ERROPENTYPE;
	"202";		ERROPENDRAW;
	"203";		ERROPENPS;
	"207";		ERROPENNOPAGES;
	"0";		ERROPENFAILED;	
	};
};

resource 'STR#'	(kErrorBaseID + cSave, purgeable) {
	{
	"201";		ERRSAVETYPE;
	"0";		ERRSAVEFAILED;
	};
};

resource 'STR#' (kErrorBaseID + cSize36, purgeable) {
	{
	"0";		ERRSIZECHANGE;
	};
};

resource 'STR#'	(kErrorBaseID + cTypingCommand, purgeable) {
	{
	"200";		ERRTYPESIZE;
	"0";		ERRMODFAILED;
	};
};

resource 'STR#'	(kErrorBaseID + cCopy, purgeable) {
	{
	"0";		ERRCOPYTOOBIG;
	};
};

resource 'STR#'	(kErrorBaseID + cPaste, purgeable) {
	{
	"0";		ERRPASTESIZE;
	};
};

resource 'STR#'	(kErrorBaseID + cPageSetup, purgeable) {
	{
	"-108";		ERRPAGESETUPMEM;
	"-27999";	ERRPAGESETUPMEM;
	"0";		ERRSETUPFAILED;
	};
};

resource 'STR#'	(kErrorBaseID + cPrint, purgeable) {
	{
	"-108";		ERRPRINTMEM;
	"-27999";	ERRPRINTMEM;
	"-34";		ERRPRINTDISKFULL;
	"-1708";	ERRPRINTANOTHERFAILED;
	"201";		ERRPRINTTYPE;
	"0";		ERRPRINTFAILED;
	};
};

resource 'STR#'	(kErrorBaseID + cRecord, purgeable) {
	{
	"-108";		ERRRECORDFULL;
	"0";		ERRRECORDERR;
	};
};

resource 'STR#'	(kErrorBaseID + cSpeak, purgeable) {
	{
	"-108";		ERRSPEAKFULL;
	"0";		ERRSPEAKERR;
	};
};

resource 'STR#'	(kErrorBaseID + cSelectVoiceSubMenu, purgeable) {
	{
	"0";		ERRCHANGEVOICE;
	};
};

resource 'DLOG' (kErrorBaseID+1, purgeable) {
	{93, 62, 197, 450},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kErrorBaseID+1,
	"",
	alertPositionMainScreen
};

resource 'dlgx' (kErrorBaseID+1) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};


resource 'DITL' (kErrorBaseID+1) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{72, 296, 92, 376},
		Button {
			enabled,
			ERROKBUTTON
		},
		/* [2] */
		{12, 76, 60, 376},
		StaticText {
			disabled,
			"^0"
		},
		/* [3] */
		{12, 22, 44, 54},
		Icon {
			disabled,
			0
		}
	}
};

resource 'DLOG' (kErrorBaseID+2, purgeable) {
	{93, 62, 197, 450},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kErrorBaseID+2,
	"",
	alertPositionMainScreen
};

resource 'dlgx' (kErrorBaseID+2) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kErrorBaseID+2) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{72, 296, 92, 376},
		Button {
			enabled,
			ERRCANCELBUTTON
		},
		/* [2] */
		{12, 76, 60, 376},
		StaticText {
			disabled,
			"^0"
		},
		/* [3] */
		{12, 22, 44, 54},
		Icon {
			disabled,
			0
		}
	}
};

resource 'DLOG' (kSequencerWindowID) {
	{0, 0, 170, 280},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kSequencerWindowID,
	"Sequencer",
	alertPositionParentWindow
};

resource 'dlgx' (kSequencerWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground + kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kSequencerWindowID) {
	{
/* [1] */ {10, 190, 30, 270}, Button {enabled, "OK"},
/* [2] */ {40, 190, 60, 270}, Button {enabled, "Cancel"},
/* [3] */ {10, 10, 30, 120}, StaticText {disabled, "Frame Count"},
/* [4] */ {40, 10, 56, 100}, EditText {enabled, "0"},
/* [5] */ {80, 10, 95, 120}, RadioButton {enabled, "Camera Only"},
/* [6] */ {100, 10, 115, 120}, RadioButton {enabled, "Projector Only"},
/* [7] */ {120, 10, 135, 120}, RadioButton {enabled, "Sequencer"},
/* [8] */ {120, 130, 136, 150}, EditText {enabled, "1"},
/* [9] */ {120, 160, 136, 180}, EditText {enabled, "1"},
	}
};

resource 'DLOG' (kSequencerQuitWindowID) {
	{0, 0, 70, 280},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kSequencerQuitWindowID,
	"Quit Sequence",
	alertPositionParentWindow
};

resource 'dlgx' (kSequencerQuitWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kSequencerQuitWindowID) {
	{
/* [1] */ {10, 190, 30, 270}, Button {enabled, "OK"},
/* [2] */ {40, 190, 60, 270}, Button {enabled, "Cancel"},
/* [3] */ {10, 10, 30, 170}, StaticText {disabled, "Quit Running Sequence?"},
	}
};

resource 'DLOG' (kSerialPortWindowID) {
	{0, 0, 100, 280},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kSerialPortWindowID,
	"Serial Ports",
	alertPositionParentWindow
};

resource 'dlgx' (kSerialPortWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kSerialPortWindowID) {
	{
/* [1] */ {10, 190, 30, 270}, Button {enabled, "OK"},
/* [2] */ {40, 190, 60, 270}, Button {enabled, "Cancel"},
/* [3] */ {40, 10, 55, 170}, RadioButton {enabled, "Modem"},
/* [4] */ {60, 10, 75, 170}, RadioButton {enabled, "Printer"},
/* [5] */ {10, 10, 32, 170}, StaticText {disabled, "Select Serial Port:"},
	}
};

resource 'ALRT' (kSerialDetectWindowID) {
	{0, 0, 110, 356},
	kSerialDetectWindowID,
	{
/* [1] */ OK, visible, 0,
/* [2] */ OK, visible, sound1,
/* [3] */ OK, visible, sound1,
/* [4] */ OK, visible, sound1
	},
	alertPositionParentWindow
};

resource 'DITL' (kSerialDetectWindowID) {
	{
/* [1] */ {78, 273, 98, 341}, Button {enabled, "OK"},
/* [2] */ {13, 65, 61, 348}, StaticText {disabled, "Unable to initialize serial port."},
	}
};

resource 'ALRT' (kScriptEmptyWindowID) {
	{0, 0, 110, 356},
	kScriptEmptyWindowID,
	{
/* [1] */ OK, visible, 0,
/* [2] */ OK, visible, sound1,
/* [3] */ OK, visible, sound1,
/* [4] */ OK, visible, sound1
	},
	alertPositionParentWindow
};

resource 'DITL' (kScriptEmptyWindowID) {
	{
/* [1] */ {78, 273, 98, 341}, Button {enabled, "OK"},
/* [2] */ {13, 65, 61, 348}, StaticText {disabled, "No ThingM commands in script text."},
	}
};

resource 'DLOG' (kFrameCounterWindowID) {
	{0, 0, 80, 400},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kFrameCounterWindowID,
	"Frame Counters",
	alertPositionParentWindow
};

resource 'dlgx' (kFrameCounterWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground + kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kFrameCounterWindowID) {
	{
/* [1] */ {10, 290, 30, 370}, Button {enabled, "OK"},
/* [2] */ {40, 290, 60, 370}, Button {enabled, "Cancel"},
/* [3] */ {10, 10, 30, 120}, StaticText {disabled, "Camera Frame"},
/* [4] */ {40, 10, 56, 100}, EditText {enabled, "0"},
/* [5] */ {10, 130, 30, 230}, StaticText {disabled, "Projector Frame"},
/* [6] */ {40, 130, 56, 230}, EditText {enabled, "0"},
	}
};

resource 'DLOG' (kRunFrameCounterWindowID) {
	{0, 0, 80, 400},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kRunFrameCounterWindowID,
	"Frame Counters",
	alertPositionParentWindow
};

resource 'dlgx' (kRunFrameCounterWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground /* + kDialogFlagsUseControlHierarchy */ + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kRunFrameCounterWindowID) {
	{
/* [1] */ {10, 290, 30, 370}, Button {enabled, "Pause"},
/* [2] */ {40, 290, 60, 370}, Button {enabled, "Cancel"},
/* [3] */ {10, 10, 30, 120}, StaticText {disabled, "Camera Frame"},
/* [4] */ {40, 10, 56, 100}, StaticText {disabled, "0"},
/* [5] */ {10, 130, 30, 230}, StaticText {disabled, "Projector Frame"},
/* [6] */ {40, 130, 56, 230}, StaticText {disabled, "0"},
	}
};

resource 'DLOG' (kRunLoopWindowID) {
	{0, 0, 80, 280},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kRunLoopWindowID,
	"Run Script Loop",
	alertPositionParentWindow
};

resource 'dlgx' (kRunLoopWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground + kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kRunLoopWindowID) {
	{
/* [1] */ {10, 190, 30, 270}, Button {enabled, "OK"},
/* [2] */ {40, 190, 60, 270}, Button {enabled, "Cancel"},
/* [3] */ {20, 10, 36, 120}, RadioButton {enabled, "Continuous"},
/* [4] */ {40, 10, 56, 120}, RadioButton {enabled, "Loop Count"},
/* [5] */ {40, 130, 56, 170}, EditText {enabled, ""},
	}
};

resource 'DLOG' (kKeyMapWindowID) {
	{0, 0, 170, 280},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kKeyMapWindowID,
	"Keypad Mapping",
	alertPositionParentWindow
};

resource 'dlgx' (kKeyMapWindowID) {
	versionZero {
		kDialogFlagsUseThemeBackground + kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'DITL' (kKeyMapWindowID) {
	{
/* [1] */ {10, 190, 30, 270}, Button {enabled, "OK"},
/* [2] */ {40, 190, 60, 270}, Button {enabled, "Cancel"},
/* [3] */ {30, 50, 48, 68}, RadioButton {enabled, ""},
/* [4] */ {30, 70, 48, 88}, RadioButton {enabled, ""},
/* [5] */ {30, 90, 48, 108}, RadioButton {enabled, ""},
/* [6] */ {30, 110, 48, 128}, RadioButton {enabled, ""},
/* [7] */ {50, 50, 68, 68}, RadioButton {enabled, ""},
/* [8] */ {50, 70, 68, 88}, RadioButton {enabled, ""},
/* [8] */ {50, 90, 68, 108}, RadioButton {enabled, ""},
/* [10] */ {50, 110, 68, 128}, RadioButton {enabled, ""},
/* [11] */ {10, 10, 28, 170}, StaticText {disabled, "Select Keypad Button:"},
/* [12] */ {90, 10, 108, 190}, StaticText {disabled, "ThingM Keypad Command:"},
/* [13] */ {120, 10, 136, 190}, StaticText {disabled, ""},
	}
};

// --------------------------------------------------------------------------------------------------------------
// Alternate location of guide files - extensions folder

type 'gdir' {
	longint;
	pstring[32];
};

resource 'gdir' (-16384) {
	'extn',
	"";
};

#if CALL_NOT_IN_CARBON
// --------------------------------------------------------------------------------------------------------------
// Balloon info for the user to see in the Finder
resource 'hfdr' (kHMHelpID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	{
	HMStringItem { FINDERHELPSTRING }
	}
};
#endif

// Our creator resource
type 'ttxt' as 'STR ';
resource 'ttxt' (0) { "" };

resource 'vers' (1) {
	MAJORVERSION, 
	MINORVERSION, 
	STAGE, 
	RELEASE, 
	COUNTRYVERSION,
	SHORTVERSIONSTRING,
	LONGVERSIONSTRING
};

// -------------------------------------------------------------------------------------------------------------
// Pattern list for selection rectangles

resource 'PAT#' (kPatternListID) {
	{	/* array PatArray: 8 elements */
		/* [1] */
		$"F0E1 C387 0F1E 3C78",
		/* [2] */
		$"78F0 E1C3 870F 1E3C",
		/* [3] */
		$"3C78 F0E1 C387 0F1E",
		/* [4] */
		$"1E3C 78F0 E1C3 870F",
		/* [5] */
		$"0F1E 3C78 F0E1 C387",
		/* [6] */
		$"870F 1E3C 78F0 E1C3",
		/* [7] */
		$"C387 0F1E 3C78 F0E1",
		/* [8] */
		$"E1C3 870F 1E3C 78F0"
	}
};

// -------------------------------------------------------------------------------------------------------------
// Our BNDL resource and icons
// WARNING: ID #s are also in SimpleText.h

resource 'BNDL' (128) {
	'ttxt', 0, {
		'ICN#', {0,128; 1,129; 2,130; 3,131; 4,131; 5,132; 6,133; 7,134; 8,135; 9,136; 10,134; 11,138};
		'FREF', {0,128; 1,129; 2,130; 3,139; 4,131; 5,132; 6,133; 7,134; 8,135; 9,136; 10,137; 11,138}
	}
};

resource 'FREF' (128) { 'APPL', 0, "" };
resource 'FREF' (129) { 'TEXT', 1, "" };
resource 'FREF' (130) { 'ttro', 2, "" };
resource 'FREF' (139) { 'qtif', 3, "" }; // this one shares icons with PICT.  I want it to take precedence over PICT, too.
resource 'FREF' (131) { 'PICT', 4, "" };
resource 'FREF' (132) { 'sEXT', 5, "" };
resource 'FREF' (133) { 'MooV', 6, "" };
resource 'FREF' (134) { 'tjob', 7, "" };
resource 'FREF' (135) { 'qjob', 8, "" };
resource 'FREF' (136) { 'sjob', 9, "" };
resource 'FREF' (138) { '3DMF', 11, "" };

// these share icons with the 'tjob'
resource 'FREF' (137) { 'rjob', 10, "" };

// Kinds of documents SimpleText can open.  The order is in the preferred order.
resource 'open' (128, purgeable) {
	'ttxt',
	{
		'ttro',
		'PICT',
		'MooV',
		'3DMF',
		'tjob',
		'qjob',
		'sjob',
		'rjob',
		'sEXT',
		'TEXT'
	}
};

resource 'kind' (128, purgeable)
{
	'ttxt',
	0,
	{
		'apnm',		APPNAME,
		'ttro',		READONLYDOC,
		'PICT',		PICTDOC,
		'MooV', 	MOVIEDOC,
		'tjob',		PRINTDOC,
		'qjob',		PRINTDOC,
		'sjob',		PDDDOC,
		'rjob',		PRINTDOC,
		'sEXT',		STATIONERYDOC,
		'TEXT',		TEXTDOC
	}	
};


// -------------------------------------------------------------------------------------------------------------
// application icon

resource 'ICN#' (128, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"0001 0000 0002 8000 0004 4000 0008 2000"
		$"0010 101C 0021 0822 0042 0441 0084 42A1"
		$"0108 8151 0211 12AA 0422 2554 0800 4AA8"
		$"1088 1550 2111 2AA8 4202 5544 8444 AA82"
		$"4088 9501 2110 CA02 1020 E404 0840 F808"
		$"0400 0010 0200 0020 0100 0040 0080 0080"
		$"0040 0100 0020 0200 0010 0400 0008 0800"
		$"0004 1000 0002 2000 0001 4000 0000 80",
		/* [2] */
		$"0001 0000 0003 8000 0007 C000 000F E000"
		$"001F F01C 003F F83E 007F FC7F 00FF FEFF"
		$"01FF FFFF 03FF FFFE 07FF FFFC 0FFF FFF8"
		$"1FFF FFF0 3FFF FFF8 7FFF FFFC FFFF FFFE"
		$"7FFF FFFF 3FFF FFFE 1FFF FFFC 0FFF FFF8"
		$"07FF FFF0 03FF FFE0 01FF FFC0 00FF FF80"
		$"007F FF00 003F FE00 001F FC00 000F F800"
		$"0007 F000 0003 E000 0001 C000 0000 80"
	}
};

resource 'icl4' (128, purgeable) {
	$"0000 0000 0000 000F 0000 0000 0000 0000"
	$"0000 0000 0000 00FC F000 0000 0000 0000"
	$"0000 0000 0000 0FC0 CF00 0000 0000 0000"
	$"0000 0000 0000 FC0C 0CF0 0000 0000 0000"
	$"0000 0000 000F C0C0 C0CF 0000 000F FF00"
	$"0000 0000 00FC 0C0F 0C0C F000 00F5 DCF0"
	$"0000 0000 0FC0 C0F0 C0C0 CF00 0F5D CC5F"
	$"0000 0000 FC0C 0F0C 0F0C 0CF0 F5DC C5DF"
	$"0000 000F C0C0 F0C0 F0C0 C0CF 5DCC 5D5F"
	$"0000 00FC 0C0F 0C0F 0C0F 0CF5 DCC5 D5F0"
	$"0000 0FC0 C0F0 C0F0 C0F0 CF5D CC5D 5F00"
	$"0000 FC0C 0C0C 0C0C 0F0C F5DC C5D5 F000"
	$"000F C0C0 F0C0 F0C0 C0CF 5DCC 5D5F 0000"
	$"00FC 0C0F 0C0F 0C0F 0CF5 DCC5 D5FC F000"
	$"0FC0 C0F0 C0C0 C0F0 CF5D CC5D 5FC0 CF00"
	$"FC0C 0F0C 0F0C 0F0C F5DC C5D5 FC0C 0CF0"
	$"0FC0 C0C0 F0C0 F0C0 F2CC 5D5F C0C0 C0CF"
	$"00FC 0C0F 0C0F 0C0C FC25 D5FC 0C0C 0CF0"
	$"000F C0C0 C0F0 C0C0 F2C2 5FC0 C0C0 CF00"
	$"0000 FC0C 0F0C 0C0C FFFF FC0C 0C0C F000"
	$"0000 0FC0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"0000 00FC 0C0C 0C0C 0C0C 0C0C 0CF0 0000"
	$"0000 000F C0C0 C0C0 C0C0 C0C0 CF00 0000"
	$"0000 0000 FC0C 0C0C 0C0C 0C0C F000 0000"
	$"0000 0000 0FC0 C0C0 C0C0 C0CF 0000 0000"
	$"0000 0000 00FC 0C0C 0C0C 0CF0 0000 0000"
	$"0000 0000 000F C0C0 C0C0 CF00 0000 0000"
	$"0000 0000 0000 FC0C 0C0C F000 0000 0000"
	$"0000 0000 0000 0FC0 C0CF 0000 0000 0000"
	$"0000 0000 0000 00FC 0CF0 0000 0000 0000"
	$"0000 0000 0000 000F CF00 0000 0000 0000"
	$"0000 0000 0000 0000 F0"
};

resource 'icl8' (128, purgeable) {
	$"0000 0000 0000 0000 0000 0000 0000 00FF"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 FFF5"
	$"FF00 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 00FF F5F5"
	$"F5FF 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 FFF5 F5F5"
	$"F5F5 FF00 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 00FF F5F5 F5F5"
	$"F5F5 F5FF 0000 0000 0000 00FF FFFF 0000"
	$"0000 0000 0000 0000 0000 FFF5 F5F5 F5FF"
	$"F5F5 F5F5 FF00 0000 0000 FFAB 5C2A FF00"
	$"0000 0000 0000 0000 00FF F5F5 F5F5 FFF5"
	$"F5F5 F5F5 F5FF 0000 00FF AB5C 2A54 ABFF"
	$"0000 0000 0000 0000 FFF5 F5F5 F5FF F5F5"
	$"F5FF F5F5 F5F5 FF00 FFAB 5C2A 54AB 5CFF"
	$"0000 0000 0000 00FF F5F5 F5F5 FFF5 F5F5"
	$"FFF5 F5F5 F5F5 F5FF AB5C 2A54 AB5C ABFF"
	$"0000 0000 0000 FFF5 F5F5 F5FF F5F5 F5FF"
	$"F5F5 F5FF F5F5 FFAB 5C2A 54AB 5CAB FF00"
	$"0000 0000 00FF F5F5 F5F5 FFF5 F5F5 FFF5"
	$"F5F5 FFF5 F5FF AB5C 2A54 AB5C ABFF 0000"
	$"0000 0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5FF F5F5 FFAB 5C2A 54AB 5CAB FF00 0000"
	$"0000 00FF F5F5 F5F5 FFF5 F5F5 FFF5 F5F5"
	$"F5F5 F5FF AB5C 2A54 AB5C ABFF 0000 0000"
	$"0000 FFF5 F5F5 F5FF F5F5 F5FF F5F5 F5FF"
	$"F5F5 FFAB 5C2A 54AB 5CAB FFF5 FF00 0000"
	$"00FF F5F5 F5F5 FFF5 F5F5 F5F5 F5F5 FFF5"
	$"F5FF AB5C 2A54 AB5C ABFF F5F5 F5FF 0000"
	$"FFF5 F5F5 F5FF F5F5 F5FF F5F5 F5FF F5F5"
	$"FFAB 5C2A 54AB 5CAB FFF5 F5F5 F5F5 FF00"
	$"00FF F5F5 F5F5 F5F5 FFF5 F5F5 FFF5 F5F5"
	$"FF08 2A54 AB5C ABFF F5F5 F5F5 F5F5 F5FF"
	$"0000 FFF5 F5F5 F5FF F5F5 F5FF F5F5 F5F5"
	$"FF08 08AB 5CAB FFF5 F5F5 F5F5 F5F5 FF00"
	$"0000 00FF F5F5 F5F5 F5F5 FFF5 F5F5 F5F5"
	$"FF08 0808 ABFF F5F5 F5F5 F5F5 F5FF 0000"
	$"0000 0000 FFF5 F5F5 F5FF F5F5 F5F5 F5F5"
	$"FFFF FFFF FFF5 F5F5 F5F5 F5F5 FF00 0000"
	$"0000 0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 0000 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 FF00 0000 0000"
	$"0000 0000 0000 00FF F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000 0000"
	$"0000 0000 0000 0000 FFF5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 0000 0000 0000"
	$"0000 0000 0000 0000 00FF F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5FF 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 FFF5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 00FF F5F5 F5F5"
	$"F5F5 F5F5 F5FF 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 FFF5 F5F5"
	$"F5F5 F5F5 FF00 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 00FF F5F5"
	$"F5F5 F5FF 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 FFF5"
	$"F5F5 FF00 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 00FF"
	$"F5FF 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FF"
};

resource 'ics#' (128, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"0100 0280 0446 0929 1215 242A 4854 92AA"
		$"44D1 20E2 1004 0808 0410 0220 0140 0080",
		/* [2] */
		$"0100 0380 07C6 0FEF 1FFF 3FFE 7FFC FFFE"
		$"7FFF 3FFE 1FFC 0FF8 07F0 03E0 01C0 0080"
	}
};

resource 'ics4' (128, purgeable) {
	$"0000 000F 0000 0000 0000 00FC F000 0000"
	$"0000 0FC0 CF00 0FF0 0000 FC0F 0CF0 FDCF"
	$"000F C0F0 C0CF DC5F 00FC 0F0C 0CFD C5F0"
	$"0FC0 F0C0 CFDC 5F00 FC0F 0CFC FDC5 FCF0"
	$"0FC0 CFC0 F25F C0CF 00FC 0C0C FFFC 0CF0"
	$"000F C0C0 C0C0 CF00 0000 FC0C 0C0C F000"
	$"0000 0FC0 C0CF 0000 0000 00FC 0CF0 0000"
	$"0000 000F CF00 0000 0000 0000 F0"
};

resource 'ics8' (128, purgeable) {
	$"0000 0000 0000 00FF 0000 0000 0000 0000"
	$"0000 0000 0000 FFF5 FF00 0000 0000 0000"
	$"0000 0000 00FF F5F5 F5FF 0000 00FF FF00"
	$"0000 0000 FFF5 F5FF F5F5 FF00 FF5C 54FF"
	$"0000 00FF F5F5 FFF5 F5F5 F5FF 5C54 ABFF"
	$"0000 FFF5 F5FF F5F5 F5F5 FF5C 54AB FF00"
	$"00FF F5F5 FFF5 F5F5 F5FF 5C54 ABFF 0000"
	$"FFF5 F5FF F5F5 FFF5 FF5C 54AB FFF5 FF00"
	$"00FF F5F5 F5FF F5F5 FF08 ABFF F5F5 F5FF"
	$"0000 FFF5 F5F5 F5F5 FFFF FFF5 F5F5 FF00"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"0000 0000 FFF5 F5F5 F5F5 F5F5 FF00 0000"
	$"0000 0000 00FF F5F5 F5F5 F5FF 0000 0000"
	$"0000 0000 0000 FFF5 F5F5 FF00 0000 0000"
	$"0000 0000 0000 00FF F5FF 0000 0000 0000"
	$"0000 0000 0000 0000 FF"
};

// -------------------------------------------------------------------------------------------------------------
// text document icon

resource 'ICN#' (129, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FC00 1000 0600 1000 0500 1000 0480"
		$"1000 0440 1000 0420 1000 07F0 1000 0010"
		$"1000 0010 1000 0010 11DD DB10 1000 0010"
		$"1000 0010 11DE F710 1000 0010 1000 0010"
		$"11BF BB10 1000 0010 1000 0010 1000 0010"
		$"1000 0010 1000 0010 1000 0010 1000 0010"
		$"1000 0010 1000 0010 1000 0010 1000 0010"
		$"1000 0010 1000 0010 1000 0010 1FFF FFF0",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80"
		$"1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};

resource 'icl4' (129, purgeable) {
	$"000F FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0FF0 0000 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 CFCF 0000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0FCC F000 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 CFCC CF00 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0FCC CCF0 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 CFFF FFFF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0CF FFCF FFCF FFCF F0FF C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0F FF0F FFFC FFFF 0FFF 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0CF F0FF FFFF F0FF F0FF C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'icl8' (129, purgeable) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF 0000 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2BFF 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B FF00 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2BFF 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2B2B FF00 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FFFF FFFF FFFF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5FF FFFF F5FF FFFF F5FF"
	$"FFFF F5FF FFF5 FFFF F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5FF FFFF F5FF FFFF FFF5"
	$"FFFF FFFF F5FF FFFF F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5FF FFF5 FFFF FFFF FFFF"
	$"FFF5 FFFF FFF5 FFFF F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'ics#' (129, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FE0 4030 4028 403C 4004 4004 4B64 4004"
		$"4F64 4004 4DA4 4004 4004 4004 4004 7FFC",
		/* [2] */
		$"7FE0 7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC"
		$"7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	}
};

resource 'ics4' (129, purgeable) {
	$"0FFF FFFF FFF0 0000 0F0C 0C0C 0CFF 0000"
	$"0FC0 C0C0 C0FC F000 0F0C 0C0C 0CFF FF00"
	$"0FC0 C0C0 C0C0 CF00 0F0C 0C0C 0C0C 0F00"
	$"0FC0 F0FF CFF0 CF00 0F0C 0C0C 0C0C 0F00"
	$"0FC0 FFFF CFF0 CF00 0F0C 0C0C 0C0C 0F00"
	$"0FC0 FFCF F0F0 CF00 0F0C 0C0C 0C0C 0F00"
	$"0FC0 C0C0 C0C0 CF00 0F0C 0C0C 0C0C 0F00"
	$"0FC0 C0C0 C0C0 CF00 0FFF FFFF FFFF FF"
};

resource 'ics8' (129, purgeable) {
	$"00FF FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 FFFF 0000 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 FF2B FF00 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF F5F5 FFF5 FFFF F5FF FFF5 F5FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF F5F5 FFFF FFFF F5FF FFF5 F5FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF F5F5 FFFF F5FF FFF5 FFF5 F5FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};

// -------------------------------------------------------------------------------------------------------------
// read-only text document icon

resource 'ICN#' (130, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 0000 0000 0000 0000 7FFF FFFE"
		$"4000 0002 5E20 007B 404D DB03 5EAD 5B7B"
		$"5E08 027B 4000 0003 7FFF FFFF 4000 0003"
		$"4E97 98CB 4820 252B 4042 0963 4A92 252B"
		$"4000 0003 7FFF FFFF 4000 0003 551F F96B"
		$"5BA8 04B3 403F FC03 5FB5 55FB 402A AC03"
		$"5FB5 55FB 402A AC03 5FB5 55FB 401F F803"
		$"5F80 01FB 400F F003 4000 0001 3FFF FFFE",
		/* [2] */
		$"0000 0000 0000 0000 0000 0000 7FFF FFFE"
		$"7FFF FFFE 7FFF FFFF 7FFF FFFF 7FFF FFFF"
		$"7FFF FFFF 7FFF FFFF 7FFF FFFF 7FFF FFFF"
		$"7FFF FFFF 7FFF FFFF 7FFF FFFF 7FFF FFFF"
		$"7FFF FFFF 7FFF FFFF 7FFF FFFF 7FFF FFFF"
		$"7FFF FFFF 7FFF FFFF 7FFF FFFF 7FFF FFFF"
		$"7FFF FFFF 7FFF FFFF 7FFF FFFF 7FFF FFFF"
		$"7FFF FFFF 7FFF FFFF 7FFF FFFF 3FFF FFFE"
	}
};

resource 'icl4' (130, purgeable) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0FFF FFFF FFFF FFFF FFFF FFFF FFFF FFF0"
	$"0FC0 C0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0F0"
	$"0F0D DDDC 0CFC 0C0C 0C0C 0C0C 0DDD DCFF"
	$"0FC0 C0C0 CF00 FFCF FFCF F0FF C0C0 C0FF"
	$"0F0D DDDC FCFC FF0F 0F0F FCFF 0DDD DCFF"
	$"0FCD DDD0 C0C0 F0C0 C0C0 C0F0 CDDD D0FF"
	$"0F0C 0C0C 0C0C 0C0C 0C0C 0C0C 0C0C 0CFF"
	$"0FFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"0F0C 0C0C 0C0C 0C0C 0C0C 0C0C 0C0C 0CFF"
	$"0FC0 FFF0 F0CF CFFF F0CF F0C0 FFC0 F0FF"
	$"0F0C FC0C 0CFC 0C0C 0CFC 0F0F 0CFC FCFF"
	$"0FC0 C0C0 CFC0 C0F0 C0C0 F0CF CFF0 C0FF"
	$"0F0C FCFC FC0F 0CFC 0CFC 0F0F 0CFC FCFF"
	$"0FC0 C0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0FF"
	$"0FFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"0FC0 C0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0FF"
	$"0F0D DDDD DC0F FFFF FFFF FC0D DDDD DCFF"
	$"0FCD DDDD D0F0 F000 0000 0FCD DDDD D0FF"
	$"0F0C 0C0C 0CFF FFFF FFFF FF0C 0C0C 0CFF"
	$"0FCD DDDD D0FD DDDD DDDD DFCD DDDD D0FF"
	$"0F0C 0C0C 0CFD DDDD DDDD DF0C 0C0C 0CFF"
	$"0FCD DDDD D0FD DDDD DDDD DFCD DDDD D0FF"
	$"0F0C 0C0C 0CFD DDDD DDDD DF0C 0C0C 0CFF"
	$"0FCD DDDD D0FD DDDD DDDD DFCD DDDD D0FF"
	$"0F0C 0C0C 0C0F FFFF FFFF FC0C 0C0C 0CFF"
	$"0FCD DDDD D0C0 C0C0 C0C0 C0CD DDDD D0FF"
	$"0F0C 0C0C 0C0C DDDD DDDD 0C0C 0C0C 0CFF"
	$"0FC0 C0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0CF"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FFF0"
};


resource 'icl8' (130, purgeable) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FF00"
	$"00FF F5FA FAFA FAF5 F500 FFF5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FA FAFA FAF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5FF F5F5 FFFF F5FF"
	$"FFFF F5FF FFF5 FFFF F5F5 F5F5 F5F5 FFFF"
	$"00FF F5FA FAFA FAF5 FFF5 FFF5 FFFF F5FF"
	$"F5FF F5FF FFF5 FFFF F5FA FAFA FAF5 FFFF"
	$"00FF F5FA FAFA FAF5 F5F5 F5F5 FFF5 F5F5"
	$"F5F5 F5F5 F5F5 FFF5 F5FA FAFA FAF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF F5F5 FFFF FFF5 FFF5 F5FF F5FF FFFF"
	$"FFF5 F5FF FFF5 F5F5 FFFF F5F5 FFF5 FFFF"
	$"00FF F5F5 FFF5 F5F5 F5F5 FFF5 F5F5 F5F5"
	$"F5F5 FFF5 F5FF F5FF F5F5 FFF5 FFF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5FF F5F5 F5F5 FFF5"
	$"F5F5 F5F5 FFF5 F5FF F5FF FFF5 F5F5 FFFF"
	$"00FF F5F5 FFF5 FFF5 FFF5 F5FF F5F5 FFF5"
	$"F5F5 FFF5 F5FF F5FF F5F5 FFF5 FFF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF F5FA FAFA FAFA FAF5 F5FF FFFF FFFF"
	$"FFFF FFFF FFF5 F5FA FAFA FAFA FAF5 FFFF"
	$"00FF F5FA FAFA FAFA FAF5 FF00 FF00 0000"
	$"0000 0000 00FF F5FA FAFA FAFA FAF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 FFFF FFFF FFFF"
	$"FFFF FFFF FFFF F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF F5FA FAFA FAFA FAF5 FFF9 F9F9 F9F9"
	$"F9F9 F9F9 F9FF F5FA FAFA FAFA FAF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 FFF9 F9F9 F9F9"
	$"F9F9 F9F9 F9FF F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF F5FA FAFA FAFA FAF5 FFF9 F9F9 F9F9"
	$"F9F9 F9F9 F9FF F5FA FAFA FAFA FAF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 FFF9 F9F9 F9F9"
	$"F9F9 F9F9 F9FF F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF F5FA FAFA FAFA FAF5 FFF9 F9F9 F9F9"
	$"F9F9 F9F9 F9FF F5FA FAFA FAFA FAF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF FFFF FFFF"
	$"FFFF FFFF FFF5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF F5FA FAFA FAFA FAF5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5FA FAFA FAFA FAF5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 FAFA FAFA"
	$"FAFA FAFA F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};


resource 'ics#' (130, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 FFFE 888B A223 FFFF AAAB AAAB FFFF"
		$"8003 A7CB 8AA3 AD6B 8AA3 A7CB 8003 7FFE",
		/* [2] */
		$"0000 FFFE FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF 7FFE"
	}
};

resource 'ics4' (130, purgeable) {
	$"0000 0000 0000 0000 FFFF FFFF FFFF FFF0"
	$"F0C0 D0C0 D0C0 D0FF FCDC 0CDC 0CDC 0CFF"
	$"FFFF FFFF FFFF FFFF FCDC DCD0 DCDC DCFF"
	$"F0D0 D0D0 D0D0 D0FF FFFF FFFF FFFF FFFF"
	$"F0C0 C0C0 C0C0 C0FF FCDC 0FFF FF0C DCFF"
	$"F0C0 FDDD DDF0 C0FF FCDC FDDD DDFC DCFF"
	$"F0C0 FDDD DDF0 C0FF FCDC 0FFF FF0C DCFF"
	$"F0C0 C0C0 C0C0 C0FF 0FFF FFFF FFFF FFF0"
};

resource 'ics8' (130, purgeable) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00"
	$"FFF5 F5F5 FAF5 F5F5 FAF5 F5F5 FAF5 FFFF"
	$"FFF5 FAF5 F5F5 FAF5 F5F5 FAF5 F5F5 FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFF5 F9F5 FAF5 FAF5 FAF5 FAF5 FAF5 FFFF"
	$"FFF5 F9F5 FAF5 FAF5 FAF5 FAF5 FAF5 FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"FFF5 FAF5 00FF FFFF FFFF F5F5 FAF5 FFFF"
	$"FFF5 F5F5 FFF9 F9F9 F9F9 FFF5 F5F5 FFFF"
	$"FFF5 FAF5 FFF9 F9F9 F9F9 FFF5 FAF5 FFFF"
	$"FFF5 00F5 FFF9 F9F9 F9F9 FFF5 F5F5 FFFF"
	$"FFF5 FAF5 F5FF FFFF FFFF F5F5 FAF5 FFFF"
	$"FF00 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
// -------------------------------------------------------------------------------------------------------------
// picture document icon

resource 'ICN#' (131, purgeable)
	{ {
	$"1FFFFC00 10000600 10000500 10000480"
	$"10000440 10000420 100007F0 10000010"
	$"10000010 10000010 11FE0010 11021C10"
	$"11022210 11024110 11FE4110 10004110"
	$"10022210 10051C10 10088010 10104010"
	$"10202010 10104010 10088010 10050010"
	$"10020010 10000010 10000010 10000010"
	$"10000010 10000010 10000010 1FFFFFF0";

	$"1FFFFC00 1FFFFE00 1FFFFF00 1FFFFF80"
	$"1FFFFFC0 1FFFFFE0 1FFFFFF0 1FFFFFF0"
	$"1FFFFFF0 1FFFFFF0 1FFFFFF0 1FFFFFF0"
	$"1FFFFFF0 1FFFFFF0 1FFFFFF0 1FFFFFF0"
	$"1FFFFFF0 1FFFFFF0 1FFFFFF0 1FFFFFF0"
	$"1FFFFFF0 1FFFFFF0 1FFFFFF0 1FFFFFF0"
	$"1FFFFFF0 1FFFFFF0 1FFFFFF0 1FFFFFF0"
	$"1FFFFFF0 1FFFFFF0 1FFFFFF0 1FFFFFF0"
	} };

resource 'icl4' (131, purgeable)
	{
	$"000FFFFFFFFFFFFFFFFFFF0000000000"
	$"000FC0C0C0C0C0C0C0C0CFF000000000"
	$"000F0C0C0C0C0C0C0C0C0FCF00000000"
	$"000FC0C0C0C0C0C0C0C0CFCCF0000000"
	$"000F0C0C0C0C0C0C0C0C0FCCCF000000"
	$"000FC0C0C0C0C0C0C0C0CFCCCCF00000"
	$"000F0C0C0C0C0C0C0C0C0FFFFFFF0000"
	$"000FC0C0C0C0C0C0C0C0C0C0C0CF0000"
	$"000F0C0C0C0C0C0C0C0C0C0C0C0F0000"
	$"000FC0C0C0C0C0C0C0C0C0C0C0CF0000"
	$"000F0C0FFFFFFFFC0C0C0C0C0C0F0000"
	$"000FC0CF707070F0C0CFFFC0C0CF0000"
	$"000F0C0F070707FC0CF4C4FC0C0F0000"
	$"000FC0CF707070F0CF4C4C4FC0CF0000"
	$"000F0C0FFFFFFFFC0FC4C4CF0C0F0000"
	$"000FC0C0C0C0C0C0CF4C4C4FC0CF0000"
	$"000F0C0C0C0C0CFC0CF4C4FC0C0F0000"
	$"000FC0C0C0C0CF5FC0CFFFC0C0CF0000"
	$"000F0C0C0C0CF5D5FC0C0C0C0C0F0000"
	$"000FC0C0C0CF5D5D5FC0C0C0C0CF0000"
	$"000F0C0C0CF5D5D5D5FC0C0C0C0F0000"
	$"000FC0C0C0CF5D5D5FC0C0C0C0CF0000"
	$"000F0C0C0C0CF5D5FC0C0C0C0C0F0000"
	$"000FC0C0C0C0CF5FC0C0C0C0C0CF0000"
	$"000F0C0C0C0C0CFC0C0C0C0C0C0F0000"
	$"000FC0C0C0C0C0C0C0C0C0C0C0CF0000"
	$"000F0C0C0C0C0C0C0C0C0C0C0C0F0000"
	$"000FC0C0C0C0C0C0C0C0C0C0C0CF0000"
	$"000F0C0C0C0C0C0C0C0C0C0C0C0F0000"
	$"000FC0C0C0C0C0C0C0C0C0C0C0CF0000"
	$"000F0C0C0C0C0C0C0C0C0C0C0C0F0000"
	$"000FFFFFFFFFFFFFFFFFFFFFFFFF0000"
	};

resource 'icl8' (131, purgeable)
	{
	$"000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FFFF000000000000000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF2BFF0000000000000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF2B2BFF00000000000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF2B2B2BFF000000000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF2B2B2B2BFF0000000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FFFFFFFFFFFFFF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5FFFFFFFFFFFFFFFFF5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5FF484848484848FFF5F5F5F5FFFFFFF5F5F5F5F5FF00000000"
	$"000000FFF5F5F5FF484848484848FFF5F5F5FF131313FFF5F5F5F5FF00000000"
	$"000000FFF5F5F5FF484848484848FFF5F5FF1313131313FFF5F5F5FF00000000"
	$"000000FFF5F5F5FFFFFFFFFFFFFFFFF5F5FF1313131313FFF5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5FF1313131313FFF5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5FFF5F5F5FF131313FFF5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5FF7FFFF5F5F5FFFFFFF5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5FF7F7F7FFFF5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5FF7F7F7F7F7FFFF5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5FF7F7F7F7F7F7F7FFFF5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5FF7F7F7F7F7FFFF5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5FF7F7F7FFFF5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5FF7FFFF5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5FFF5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFF5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5F5FF00000000"
	$"000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000"
	};

resource 'ics#' (131, purgeable)
	{ {
	$"7FE0 4030 4028 403C 5E04 5264 5E94 4094"
	$"4264 4504 4884 4504 4204 4004 4004 7FFC";

	$"7FE0 7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC"
	$"7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	} };

resource 'ics4' (131, purgeable)
	{
	$"0FFFFFFFFFF00000"
	$"0FC0C0C0C0FF0000"
	$"0F0C0C0C0CFCF000"
	$"0FC0C0C0C0FFFF00"
	$"0F0FFFFC0C0C0F00"
	$"0FCF07F0CFF0CF00"
	$"0F0FFFFCF4CF0F00"
	$"0FC0C0C0FC4FCF00"
	$"0F0C0CFC0FFC0F00"
	$"0FC0CF5FC0C0CF00"
	$"0F0CF5D5FC0C0F00"
	$"0FC0CF5FC0C0CF00"
	$"0F0C0CFC0C0C0F00"
	$"0FC0C0C0C0C0CF00"
	$"0F0C0C0C0C0C0F00"
	$"0FFFFFFFFFFFFF00"
	};

resource 'ics8' (131, purgeable)
	{
	$"00FFFFFFFFFFFFFFFFFFFF0000000000"
	$"00FFF5F5F5F5F5F5F5F5FFFF00000000"
	$"00FFF5F5F5F5F5F5F5F5FF2BFF000000"
	$"00FFF5F5F5F5F5F5F5F5FFFFFFFF0000"
	$"00FFF5FFFFFFFFF5F5F5F5F5F5FF0000"
	$"00FFF5FF4848FFF5F5FFFFF5F5FF0000"
	$"00FFF5FFFFFFFFF5FF1313FFF5FF0000"
	$"00FFF5F5F5F5F5F5FF1313FFF5FF0000"
	$"00FFF5F5F5F5FFF5F5FFFFF5F5FF0000"
	$"00FFF5F5F5FF7FFFF5F5F5F5F5FF0000"
	$"00FFF5F5FF7F7F7FFFF5F5F5F5FF0000"
	$"00FFF5F5F5FF7FFFF5F5F5F5F5FF0000"
	$"00FFF5F5F5F5FFF5F5F5F5F5F5FF0000"
	$"00FFF5F5F5F5F5F5F5F5F5F5F5FF0000"
	$"00FFF5F5F5F5F5F5F5F5F5F5F5FF0000"
	$"00FFFFFFFFFFFFFFFFFFFFFFFFFF0000"
	};
// -------------------------------------------------------------------------------------------------------------
// text-stationary document icon

resource 'ICN#' (132, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FFC0 1000 0040 1000 0070 1000 0050"
		$"1000 0050 1000 0050 1000 0050 1000 0050"
		$"1000 0050 1000 0050 11DD DC50 1000 0050"
		$"1000 0050 11DD B850 1000 0050 1000 0050"
		$"11BD DC50 1000 0050 1000 0050 1000 0050"
		$"1000 0050 1000 0050 1000 0050 1000 0050"
		$"1000 0FD0 1000 0890 1000 0910 1000 0A10"
		$"1000 0C10 1FFF F810 0400 0010 07FF FFF0",
		/* [2] */
		$"1FFF FFC0 1FFF FFC0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 07FF FFF0 07FF FFF0"
	}
};

resource 'icl4' (132, purgeable) {
	$"000F FFFF FFFF FFFF FFFF FFFF FF00 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0F00 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFFF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0CF FFCF FFCF FFCF FFC0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0F FF0F FF0F FCFF FC0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0CF F0FF FFCF FFCF FFC0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0FDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 FFFF FFDF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C FCCC FDDF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 FCCF DDCF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C FCFD DCCF 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 FFDD CCCF 0000"
	$"000F FFFF FFFF FFFF FFFF FDDC CCCF 0000"
	$"0000 0FDD DDDD DDDD DDDD DDCC CCCF 0000"
	$"0000 0FFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'icl8' (132, purgeable) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF FFFF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5FF FFFF F5FF FFFF F5FF"
	$"FFFF F5FF FFFF F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5FF FFFF F5FF FFFF F5FF"
	$"FFF5 FFFF FFF5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5FF FFF5 FFFF FFFF F5FF"
	$"FFFF F5FF FFFF F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 FFFF FFFF FFFF F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 FF2B 2B2B FFF9 F9FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 FF2B 2BFF F9F9 2BFF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 FF2B FFF9 F92B 2BFF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 FFFF F9F9 2B2B 2BFF 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFF9 F92B 2B2B 2BFF 0000 0000"
	$"0000 0000 00FF F9F9 F9F9 F9F9 F9F9 F9F9"
	$"F9F9 F9F9 F9F9 2B2B 2B2B 2BFF 0000 0000"
	$"0000 0000 00FF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'ics#' (132, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4010 401C 4B94 4014 4F14 4014 4D94"
		$"4014 4014 40F4 40A4 40C4 7F84 1004 1FFC",
		/* [2] */
		$"7FF0 7FF0 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
		$"7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 1FFC 1FFC"
	}
};

resource 'ics4' (132, purgeable) {
	$"0FFF FFFF FFFF 0000 0F0C 0C0C 0C0F 0000"
	$"0FC0 C0C0 C0CF FF00 0F0C FCFF FC0F DF00"
	$"0FC0 C0C0 C0CF DF00 0F0C FFFF 0C0F DF00"
	$"0FC0 C0C0 C0CF DF00 0F0C FF0F FC0F DF00"
	$"0FC0 C0C0 C0CF DF00 0F0C 0C0C 0C0F DF00"
	$"0FC0 C0C0 FFFF DF00 0F0C 0C0C FCFD DF00"
	$"0FC0 C0C0 FFDD CF00 0FFF FFFF FDDC CF00"
	$"000F DDDD DDCC CF00 000F FFFF FFFF FF00"
};

resource 'ics8' (132, purgeable) {
	$"00FF FFFF FFFF FFFF FFFF FFFF 0000 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF FFFF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000"
	$"00FF F5F5 FFF5 FFFF F5FF F5FF F9FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000"
	$"00FF F5F5 FFFF FFFF F5FF F5FF F9FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000"
	$"00FF F5F5 FFFF F5FF FFF5 F5FF F9FF 0000"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5FF F9FF 0000"
	$"00FF F5F5 F5F5 F5F5 FFFF FFFF F9FF 0000"
	$"00FF F5F5 F5F5 F5F5 FFF6 FFF9 F9FF 0000"
	$"00FF F5F5 F5F5 F5F5 FFFF F9F9 F6FF 0000"
	$"00FF FFFF FFFF FFFF FFF9 F9F6 F6FF 0000"
	$"0000 00FF F9F9 F9F9 F9F9 F6F6 F6FF 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF"
};

// -------------------------------------------------------------------------------------------------------------
// Movie icon

resource 'ICN#' (133, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FC00 1200 0600 1200 0500 1E00 0480"
		$"1E00 0440 1200 0420 1200 07F0 1E00 00F0"
		$"1E00 00F0 1200 0090 13FF FF90 1E00 00F0"
		$"1E00 00F0 1200 0090 1200 0090 1E00 00F0"
		$"1E00 00F0 1200 0090 1200 0090 1E00 00F0"
		$"1E00 00F0 13FF FF90 1200 0090 1E00 00F0"
		$"1E00 00F0 1200 0090 1200 0090 1E00 00F0"
		$"1E00 00F0 1200 0090 1200 0090 1FFF FFF0",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80"
		$"1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};

resource 'icl4' (133, purgeable) {
	$"000F FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"000F 00FC 0C0C 0C0C 0C0C 0FF0 0000 0000"
	$"000F 00F0 C0C0 C0C0 C0C0 CFCF 0000 0000"
	$"000F FFFC 0C0C 0C0C 0C0C 0FCC F000 0000"
	$"000F FFF0 C0C0 C0C0 C0C0 CFCC CF00 0000"
	$"000F 00FC 0C0C 0C0C 0C0C 0FCC CCF0 0000"
	$"000F 00F0 C0C0 C0C0 C0C0 CFFF FFFF 0000"
	$"000F FFFC 0C0C 0C0C 0C0C 0C0C FFFF 0000"
	$"000F FFF0 C0C0 C0C0 C0C0 C0C0 FFFF 0000"
	$"000F 00FC 0C0C 0C0C 0C0C 0C0C F00F 0000"
	$"000F 00FF FFFF FFFF FFFF FFFF F00F 0000"
	$"000F FFFC 0C0C 0C0C 0C0C 0C0C FFFF 0000"
	$"000F FFF0 C0C0 C0C0 C0C0 C0C0 FFFF 0000"
	$"000F 00FC 0C0C 0C0C 0C0C 0C0C F00F 0000"
	$"000F 00F0 C0C0 C0C0 C0C0 C0C0 F00F 0000"
	$"000F FFFC 0C0C 0C0C 0C0C 0C0C FFFF 0000"
	$"000F FFF0 C0C0 C0C0 C0C0 C0C0 FFFF 0000"
	$"000F 00FC 0C0C 0C0C 0C0C 0C0C F00F 0000"
	$"000F 00F0 C0C0 C0C0 C0C0 C0C0 F00F 0000"
	$"000F FFFC 0C0C 0C0C 0C0C 0C0C FFFF 0000"
	$"000F FFF0 C0C0 C0C0 C0C0 C0C0 FFFF 0000"
	$"000F 00FF FFFF FFFF FFFF FFFF F00F 0000"
	$"000F 00F0 C0C0 C0C0 C0C0 C0C0 F00F 0000"
	$"000F FFFC 0C0C 0C0C 0C0C 0C0C FFFF 0000"
	$"000F FFF0 C0C0 C0C0 C0C0 C0C0 FFFF 0000"
	$"000F 00FC 0C0C 0C0C 0C0C 0C0C F00F 0000"
	$"000F 00F0 C0C0 C0C0 C0C0 C0C0 F00F 0000"
	$"000F FFFC 0C0C 0C0C 0C0C 0C0C FFFF 0000"
	$"000F FFF0 C0C0 C0C0 C0C0 C0C0 FFFF 0000"
	$"000F 00FC 0C0C 0C0C 0C0C 0C0C F00F 0000"
	$"000F 00F0 C0C0 C0C0 C0C0 C0C0 F00F 0000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF"
};


resource 'icl8' (133, purgeable) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF 0000 0000 0000 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FF00 0000 0000 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2BFF 0000 0000 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B FF00 0000 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2BFF 0000 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2B2B FF00 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FFFF FFFF FFFF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F500 FF00 00FF 0000 0000"
	$"0000 00FF 0000 FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FF00 00FF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF 0000 FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FF00 00FF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF FFFF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FFFF FFFF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF 0000 FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 FF00 00FF 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'ics#' (133, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FE0 7030 5028 703C 5014 7FFC 5014 701C"
		$"5014 701C 5014 7FFC 5014 701C 5014 7FFC",
		/* [2] */
		$"7FE0 7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC"
		$"7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	}
};

resource 'ics4' (133, purgeable) {
	$"0FFF FFFF FFF0 0000 0FFF 0C0C 0CFF 0000"
	$"0F0F C0C0 C0FC F000 0FFF 0C0C 0CFF FF00"
	$"0F0F C0C0 C0CF 0F00 0FFF FFFF FFFF FF00"
	$"0F0F C0C0 C0CF 0F00 0FFF 0C0C 0C0F FF00"
	$"0F0F C0C0 C0CF 0F00 0FFF 0C0C 0C0F FF00"
	$"0F0F C0C0 C0CF 0F00 0FFF FFFF FFFF FF00"
	$"0F0F C0C0 C0CF 0F00 0FFF 0C0C 0C0F FF00"
	$"0F0F C0C0 C0CF 0F00 0FFF FFFF FFFF FF"
};

resource 'ics8' (133, purgeable) {
	$"00FF FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"00FF FFFF F5F5 F5F5 F5F5 FFFF 0000 0000"
	$"00FF 00FF F5F5 F5F5 F5F5 FF2B FF00 0000"
	$"00FF FFFF F5F5 F5F5 F5F5 FFFF FFFF 0000"
	$"00FF 00FF F5F5 F5F5 F5F5 F5FF 00FF 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF 0000"
	$"00FF 00FF F5F5 F5F5 F5F5 F5FF 00FF 0000"
	$"00FF FFFF F5F5 F5F5 F5F5 F5FF FFFF 0000"
	$"00FF 00FF F5F5 F5F5 F5F5 F5FF 00FF 0000"
	$"00FF FFFF F5F5 F5F5 F5F5 F5FF FFFF 0000"
	$"00FF 00FF F5F5 F5F5 F5F5 F5FF 00FF 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF 0000"
	$"00FF 00FF F5F5 F5F5 F5F5 F5FF 00FF 0000"
	$"00FF FFFF F5F5 F5F5 F5F5 F5FF FFFF 0000"
	$"00FF 00FF F5F5 F5F5 F5F5 F5FF 00FF 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};


// -------------------------------------------------------------------------------------------------------------
// GX spool file icon


resource 'ICN#' (134, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FC00 1000 0600 1000 0500 1000 0480"
		$"1000 0440 1000 0420 1000 07F0 1007 E010"
		$"101E 1810 103C 1C10 1078 0E10 10F0 0E10"
		$"11F0 0F10 11E0 0F10 13E0 0F10 13C1 CF10"
		$"17C2 2F10 17C4 7710 1784 AA10 1785 C410"
		$"1783 A210 1781 1110 1780 8890 1380 4450"
		$"1381 A250 10C3 D0D0 107F 09D0 1000 07D0"
		$"1000 0010 1000 0010 1000 0010 1FFF FFF0",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80"
		$"1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};

resource 'icl4' (134, purgeable) {
	$"000F FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"000F 0000 0000 0000 0000 0FF0 0000 0000"
	$"000F 0000 0000 0000 0000 0FCF 0000 0000"
	$"000F 0000 0000 0000 0000 0FCC F000 0000"
	$"000F 0000 0000 0000 0000 0FCC CF00 0000"
	$"000F 0000 0000 0000 0000 0FCC CCF0 0000"
	$"000F 0000 0000 0000 0000 0FFF FFFF 0000"
	$"000F 0000 0000 0767 6760 0000 000F 0000"
	$"000F 0000 0006 7670 0006 7000 000F 0000"
	$"000F 0000 0067 6700 0007 6700 000F 0000"
	$"000F 0000 0676 7000 0000 7670 000F 0000"
	$"000F 0000 6767 0000 0000 6760 000F 0000"
	$"000F 0006 7676 0000 0000 7676 000F 0000"
	$"000F 0007 6760 0000 0000 6767 000F 0000"
	$"000F 0076 7670 0000 0000 7676 000F 0000"
	$"000F 0067 6700 0002 2200 6767 000F 0000"
	$"000F 0676 7600 0022 22F0 7676 000F 0000"
	$"000F 0767 6700 0222 2DEF 6767 000F 0000"
	$"000F 0676 7000 0222 CCD1 2670 000F 0000"
	$"000F 0767 6000 022D CC10 1260 000F 0000"
	$"000F 0676 7000 00AE D121 0120 000F 0000"
	$"000F 0767 6000 000A 1012 1012 000F 0000"
	$"000F 0676 7000 0000 2101 2101 200F 0000"
	$"000F 0067 6000 0000 6210 1210 000F 0000"
	$"000F 0076 7000 0006 7621 0120 000F 0000"
	$"000F 0000 6700 0067 6702 1000 0F0F 0000"
	$"000F 0000 0676 7677 0000 2000 FF0F 0000"
	$"000F 0000 0000 0000 0000 000F FF0F 0000"
	$"000F 0000 0000 0000 0000 0000 000F 0000"
	$"000F 0000 0000 0000 0000 0000 000F 0000"
	$"000F 0000 0000 0000 0000 0000 000F 0000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'icl8' (134, purgeable) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF 0000 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2BFF 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B FF00 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2BFF 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2B2B FF00 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FFFF FFFF FFFF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5C0 ECC0"
	$"ECC0 ECF5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5EC C0EC C0F5"
	$"F5F5 F5EC C0F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 ECC0 ECC0 F5F5"
	$"F5F5 F5C0 ECC0 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5EC C0EC C0F5 F5F5"
	$"F5F5 F5F5 C0EC C0F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 ECC0 ECC0 F5F5 F5F5"
	$"F5F5 F5F5 ECC0 ECF5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5EC C0EC C0EC F5F5 F5F5"
	$"F5F5 F5F5 C0EC C0EC F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5C0 ECC0 ECF5 F5F5 F5F5"
	$"F5F5 F5F5 ECC0 ECC0 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 C0EC C0EC C0F5 F5F5 F5F5"
	$"F5F5 F5F5 C0EC C0EC F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 ECC0 ECC0 F5F5 F5F5 F516"
	$"1616 F5F5 ECC0 ECC0 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC C0EC F5F5 F5F5 1616"
	$"1616 FFF5 C0EC C0EC F5F5 F5FF 0000 0000"
	$"0000 00FF F5C0 ECC0 ECC0 F5F5 F516 1616"
	$"16F9 FBFF ECC0 ECC0 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC C0F5 F5F5 F516 1616"
	$"F7F6 F905 16EC C0F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5C0 ECC0 ECF5 F5F5 F516 16F9"
	$"F8F7 0500 0516 ECF5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC C0F5 F5F5 F5F5 FDFB"
	$"F905 1605 0005 16F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5C0 ECC0 ECF5 F5F5 F5F5 F5FD"
	$"0500 0516 0500 0516 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC C0F5 F5F5 F5F5 F5F5"
	$"1605 0005 1605 0005 16F5 F5FF 0000 0000"
	$"0000 00FF F5F5 ECC0 ECF5 F5F5 F5F5 F5F5"
	$"EC16 0500 0516 0500 00F5 F5FF 0000 0000"
	$"0000 00FF F5F5 C0EC C0F5 F5F5 F5F5 F5EC"
	$"C0EC 1605 0005 1600 0000 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 ECC0 F5F5 F5F5 ECC0"
	$"ECC0 F516 0500 0000 00FF F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5EC C0EC C0EC C0C0"
	$"F5F5 F5F5 1600 0000 FFFF F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 00FF FFFF F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'ics#' (134, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4038 402C 43FC 4764 4E34 5C34 5DB4"
		$"7BF4 7BF4 79D4 78AC 59DC 4F3C 4004 7FFC",
		/* [2] */
		$"7FE0 7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC"
		$"7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	}
};

resource 'ics4' (134, purgeable) {
	$"0FFF FFFF FFFF 0000 0F00 0000 00FF F000"
	$"0F00 0000 00FC FF00 0F00 0077 76FF FF00"
	$"0F00 0777 0770 0F00 0F00 7770 0077 0F00"
	$"0F07 7700 0077 0F00 0F07 7702 2077 0F00"
	$"0F77 7022 DF77 0F00 0F77 702D CD67 0F00"
	$"0F77 700E D212 0F00 0F67 7000 6121 2F00"
	$"0F07 7007 7212 FF00 0F00 6777 002F FF00"
	$"0F00 0000 0000 0F00 0FFF FFFF FFFF FF"
};

resource 'ics8' (134, purgeable) {
	$"00FF FFFF FFFF FFFF FFFF FFFF 0000 0000"
	$"00FF 0000 0000 0000 0000 FFFF FF00 0000"
	$"00FF 0000 0000 0000 0000 FF2B FFFF 0000"
	$"00FF 0000 0000 ECEC ECEC FFFF FFFF 0000"
	$"00FF 0000 00EC ECEC 00EC EC00 00FF 0000"
	$"00FF 0000 ECEC EC00 0000 ECEC 00FF 0000"
	$"00FF 00EC ECEC 0000 0000 ECEC 00FF 0000"
	$"00FF 00EC ECEC 0017 1700 ECEC 00FF 0000"
	$"00FF ECEC EC00 1717 F9FF ECEC 00FF 0000"
	$"00FF ECEC EC00 17F9 2BF9 ECEC 00FF 0000"
	$"00FF ECEC EC00 00FC F917 0517 00FF 0000"
	$"00FF ECEC EC00 0000 EC05 1705 17FF 0000"
	$"00FF 00EC EC00 00EC EC17 0517 FFFF 0000"
	$"00FF 0000 ECEC ECEC 0000 17FF FFFF 0000"
	$"00FF 0000 0000 0000 0000 0000 00FF 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};


// -------------------------------------------------------------------------------------------------------------
// GX failed spool file icon


resource 'ICN#' (135, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"DFFF FC03 7000 0606 3000 050C 1800 0498"
		$"1C00 0470 1600 0460 1300 07F0 1187 E190"
		$"10DE 1B10 107C 1E10 1078 0E10 10F8 1E10"
		$"11FC 3F10 11E6 6F10 13E3 CF10 13C1 CF10"
		$"17C3 EF10 17C6 7710 178C BA10 179D DC10"
		$"17B3 AE10 17E1 1710 17C0 8B90 1380 45D0"
		$"1381 A2D0 16C3 D0F0 1C7F 09F0 1800 07D8"
		$"3000 001C 7000 0016 D000 0013 9FFF FFF1",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80"
		$"1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};

resource 'icl4' (135, purgeable) {
	$"FF0F FFFF FFFF FFFF FFFF FF00 0000 00FF"
	$"0FFF 0000 0000 0000 0000 0FF0 0000 0FF0"
	$"00FF 0000 0000 0000 0000 0FCF 0000 FF00"
	$"000F F000 0000 0000 0000 0FCC F00F F000"
	$"000F FF00 0000 0000 0000 0FCC CFFF 0000"
	$"000F 0FF0 0000 0000 0000 0FCC CFF0 0000"
	$"000F 00FF 0000 0000 0000 0FFF FFFF 0000"
	$"000F 000F F000 0767 6760 000F F00F 0000"
	$"000F 0000 FF06 7670 0006 70FF 000F 0000"
	$"000F 0000 0FF7 6700 0007 6FF0 000F 0000"
	$"000F 0000 06FF 7000 0000 FF70 000F 0000"
	$"000F 0000 676F F000 000F F760 000F 0000"
	$"000F 0006 7676 FF00 00FF 7676 000F 0000"
	$"000F 0007 6760 0FF0 0FF0 6767 000F 0000"
	$"000F 0076 7670 00FF FF00 7676 000F 0000"
	$"000F 0067 6700 000F F200 6767 000F 0000"
	$"000F 0676 7600 00FF FFF0 7676 000F 0000"
	$"000F 0767 6700 0FF2 2FFF 6767 000F 0000"
	$"000F 0676 7000 FF22 CCFF 2670 000F 0000"
	$"000F 0767 600F F22D CC1F F260 000F 0000"
	$"000F 0676 70FF 00AE D121 FF20 000F 0000"
	$"000F 0767 6FF0 000A 1012 1FF2 000F 0000"
	$"000F 0676 FF00 0000 2101 21FF 200F 0000"
	$"000F 006F F000 0000 6210 121F F00F 0000"
	$"000F 00FF 7000 0006 7621 0120 FF0F 0000"
	$"000F 0FF0 6700 0067 6702 1000 0FFF 0000"
	$"000F FF00 0676 7677 0000 2000 FFFF 0000"
	$"000F F000 0000 0000 0000 000F FF0F F000"
	$"00FF 0000 0000 0000 0000 0000 000F FF00"
	$"0FFF 0000 0000 0000 0000 0000 000F 0FF0"
	$"FF0F 0000 0000 0000 0000 0000 000F 00FF"
	$"F00F FFFF FFFF FFFF FFFF FFFF FFFF 000F"
};

resource 'icl8' (135, purgeable) {
	$"FFFF 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF 0000 0000 0000 0000 FFFF"
	$"00FF FFFF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FF00 0000 0000 00FF FF00"
	$"0000 FFFF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2BFF 0000 0000 FFFF 0000"
	$"0000 00FF FFF5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B FF00 00FF FF00 0000"
	$"0000 00FF FFFF F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2BFF FFFF 0000 0000"
	$"0000 00FF F5FF FFF5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B 2BFF FF00 0000 0000"
	$"0000 00FF F5F5 FFFF F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FFFF FFFF FFFF 0000 0000"
	$"0000 00FF F5F5 F5FF FFF5 F5F5 F5C0 ECC0"
	$"ECC0 ECF5 F5F5 F5FF FFF5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 FFFF F5EC C0EC C0F5"
	$"F5F5 F5EC C0F5 FFFF F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5FF FFC0 ECC0 F5F5"
	$"F5F5 F5C0 ECFF FFF5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5EC FFFF C0F5 F5F5"
	$"F5F5 F5F5 FFFF C0F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 ECC0 ECFF FFF5 F5F5"
	$"F5F5 F5FF FFC0 ECF5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5EC C0EC C0EC FFFF F5F5"
	$"F5F5 FFFF C0EC C0EC F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5C0 ECC0 ECF5 F5FF FFF5"
	$"F5FF FFF5 ECC0 ECC0 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 C0EC C0EC C0F5 F5F5 FFFF"
	$"FFFF F5F5 C0EC C0EC F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 ECC0 ECC0 F5F5 F5F5 F5FF"
	$"FF16 F5F5 ECC0 ECC0 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC C0EC F5F5 F5F5 FFFF"
	$"FFFF FFF5 C0EC C0EC F5F5 F5FF 0000 0000"
	$"0000 00FF F5C0 ECC0 ECC0 F5F5 F5FF FF16"
	$"16FF FFFF ECC0 ECC0 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC C0F5 F5F5 FFFF 1616"
	$"F7F6 FFFF 16EC C0F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5C0 ECC0 ECF5 F5FF FF16 16F9"
	$"F8F7 05FF FF16 ECF5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC C0F5 FFFF F5F5 FDFB"
	$"F905 1605 FFFF 16F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5C0 ECC0 ECFF FFF5 F5F5 F5FD"
	$"0500 0516 05FF FF16 F5F5 F5FF 0000 0000"
	$"0000 00FF F5EC C0EC FFFF F5F5 F5F5 F5F5"
	$"1605 0005 1605 FFFF 16F5 F5FF 0000 0000"
	$"0000 00FF F5F5 ECFF FFF5 F5F5 F5F5 F5F5"
	$"EC16 0500 0516 05FF FFF5 F5FF 0000 0000"
	$"0000 00FF F5F5 FFFF C0F5 F5F5 F5F5 F5EC"
	$"C0EC 1605 0005 1600 FFFF F5FF 0000 0000"
	$"0000 00FF F5FF FFF5 ECC0 F5F5 F5F5 ECC0"
	$"ECC0 F516 0500 0000 00FF FFFF 0000 0000"
	$"0000 00FF FFFF F5F5 F5EC C0EC C0EC C0C0"
	$"F5F5 F5F5 1600 0000 FFFF FFFF 0000 0000"
	$"0000 00FF FFF5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 00FF FFFF F5FF FF00 0000"
	$"0000 FFFF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF FFFF 0000"
	$"00FF FFFF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 00FF FF00"
	$"FFFF 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 FFFF"
	$"FF00 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF 0000 00FF"
};

resource 'ics#' (135, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"FFF3 603E 702C 5BFC 4F74 4E74 5FF4 5DB4"
		$"7BF4 7FF4 7DF4 78BC 79DC 6F3E C007 FFFD",
		/* [2] */
		$"7FE0 7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC"
		$"7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	}
};

resource 'ics4' (135, purgeable) {
	$"FFFF FFFF FFFF 00FF 0FF0 0000 00FF FFF0"
	$"0FFF 0000 00FC FF00 0F0F F077 76FF FF00"
	$"0F00 FF77 07FF 0F00 0F00 7FF0 0FF7 0F00"
	$"0F07 77FF FF77 0F00 0F07 770F F077 0F00"
	$"0F77 70FF FF77 0F00 0F77 7FFD CFF7 0F00"
	$"0F77 FF0E D2FF 0F00 0F6F F000 612F FF00"
	$"0FFF 7007 7212 FF00 0FF0 6777 002F FFF0"
	$"FF00 0000 0000 0FFF FFFF FFFF FFFF FF0F"
};

resource 'ics8' (135, purgeable) {
	$"FFFF FFFF FFFF FFFF FFFF FFFF 0000 FFFF"
	$"00FF FF00 0000 0000 0000 FFFF FFFF FF00"
	$"00FF FFFF 0000 0000 0000 FF2B FFFF 0000"
	$"00FF 00FF FF00 ECEC ECEC FFFF FFFF 0000"
	$"00FF 0000 FFFF ECEC 00EC FFFF 00FF 0000"
	$"00FF 0000 ECFF FF00 00FF FFEC 00FF 0000"
	$"00FF 00EC ECEC FFFF FFFF ECEC 00FF 0000"
	$"00FF 00EC ECEC 00FF FF00 ECEC 00FF 0000"
	$"00FF ECEC EC00 FFFF FFFF ECEC 00FF 0000"
	$"00FF ECEC ECFF FFF9 2BFF FFEC 00FF 0000"
	$"00FF ECEC FFFF 00FC F917 FFFF 00FF 0000"
	$"00FF ECFF FF00 0000 EC05 17FF FFFF 0000"
	$"00FF FFFF EC00 00EC EC17 0517 FFFF 0000"
	$"00FF FF00 ECEC ECEC 0000 17FF FFFF FF00"
	$"FFFF 0000 0000 0000 0000 0000 00FF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF 00FF"
};


// -------------------------------------------------------------------------------------------------------------
// GX portable digital document icon

resource 'ICN#' (136, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"003F FC00 007F FE00 007F FE00 0070 0E00"
		$"FFFF FFFF 8070 0E01 8004 0081 8004 0081"
		$"81FC 3F81 8000 0001 81FD 5151 810D 5151"
		$"816D 5051 81FC 4541 81ED 5551 8125 5111"
		$"810C 1451 81FC 5451 8000 0001 8000 0001"
		$"8150 FF51 8150 9B51 8040 BF51 8150 9F11"
		$"8154 9351 FD14 9341 4554 8F51 2454 FF51"
		$"1400 0001 0C00 0001 07FF FFFF",
		/* [2] */
		$"003F FC00 007F FE00 007F FE00 007F FE00"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF 7FFF FFFF 3FFF FFFF"
		$"1FFF FFFF 0FFF FFFF 07FF FFFF"
	}
};

resource 'icl4' (136, purgeable) {
	$"0000 0000 00FF FFFF FFFF FF00 0000 0000"
	$"0000 0000 0FDD DDDD DDDD DDF0 0000 0000"
	$"0000 0000 0FDF FFFF FFFF FDF0 0000 0000"
	$"0000 0000 0FDF 0000 0000 FDF0 0000 0000"
	$"FFFF FFFF FFDF FFFF FFFF FDFF FFFF FFFF"
	$"FC0C 0C00 0FDF 0C0C 0C00 FDF0 0C0C 0C0F"
	$"F0C0 C0C0 C0C0 CDC0 C000 C0C0 D0C0 C0CF"
	$"FC0C 0C00 0C0C 0D0C 0C0C 0C0C DC0C 0C0F"
	$"F0C0 C0CD DDDD DDC0 C0DD DDDD D0C0 C0CF"
	$"FC0C 0C0C 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F"
	$"F0C0 C0CE EEEE EECE CECE C0CE CECE C0CF"
	$"FC0C 0C0E CCC0 DF0E 0E0E 0C0E 0E0E 0C0F"
	$"F0C0 C0CE CEE0 DFCE CECE C0C0 CECE C0CF"
	$"FC0C 0C0E EEEE DF0C 0E0C 0E0E 0E0C 0C0F"
	$"F0C0 C0CE EEE0 EECE CECE CECE CECE C0CF"
	$"FC0C 0C0E C0E0 0E0E 0E0E 0C0E 0C0E 0C0F"
	$"F0C0 C0CE CC00 DEC0 C0CE CEC0 CECE C0CF"
	$"FC0C 0C0E EEEE EE0C 0E0E 0E0C 0E0E 0C0F"
	$"F0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0C0 C0CF"
	$"FC0C 0C0C 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F"
	$"F0C0 C0CE CECE C0C0 EEEE EEEE CECE C0CF"
	$"FC0C 0C0E 0E0E 0C0C ECCE D0DE 0E0E 0C0F"
	$"F0C0 C0C0 CEC0 C0C0 ECEE EEEE CECE C0CF"
	$"FC0C 0C0E 0E0E 0C0C ECDE EEEE 0C0E 0C0F"
	$"F0C0 C0CE CECE CEC0 ECDE 00DE CECE C0CF"
	$"FFFF FF0E 0C0E 0E0C ECCE 00EE 0E0C 0C0F"
	$"0FCC CFCE CECE CEC0 ECCC EEEE CECE C0CF"
	$"00FC CF0C 0E0E 0E0C EEEE EEEE 0E0E 0C0F"
	$"000F CFC0 C0C0 C0C0 C0C0 C0C0 C0C0 C0CF"
	$"0000 FF0C 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F"
	$"0000 0FFF FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'icl8' (136, purgeable) {
	$"0000 0000 0000 0000 0000 FFFF FFFF FFFF"
	$"FFFF FFFF FFFF 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 00FF FAFA FAFA FAFA"
	$"FAFA FAFA FAFA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 00FF FAFF FFFF FFFF"
	$"FFFF FFFF FFFA FF00 0000 0000 0000 0000"
	$"0000 0000 0000 0000 00FF FAFF 0000 0000"
	$"0000 0000 FFFA FF00 0000 0000 0000 0000"
	$"FFFF FFFF FFFF FFFF FFFF FAFF FFFF FFFF"
	$"FFFF FFFF FFFA FFFF FFFF FFFF FFFF FFFF"
	$"FFF5 F5F5 F5F5 F500 00FF FAFF 0000 0000"
	$"0000 0000 FFFA FF00 F5F5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F500 F5F5 F5F5 F5FA F5F5"
	$"F5F5 00F5 F5F5 F5F5 FAF5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F500 F5F5 F5F5 F5FA F5F5"
	$"F5F5 00F5 F5F5 F5F5 FAF5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FA FAFA FAFA FAFA F5F5"
	$"F5F5 FAFA FAFA FAFA FAF5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB FBFB FBFB FBFB F5FB"
	$"F5FB F5FB F5F5 F5FB F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB 4F4F 4F01 FAFE F5FB"
	$"F5FB F5FB F5F5 F5FB F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB 4FFC FC01 FAFE F5FB"
	$"F5FB F5FB F5F5 F5F5 F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB FCFC FCFC FAFE F5F5"
	$"F5FB F5F5 F5FB F5FB F5FB F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB FCFC FC00 FCFB F5FB"
	$"F5FB F5FB F5FB F5FB F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB 4F00 FC00 01FB F5FB"
	$"F5FB F5FB F5F5 F5FB F5F5 F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB 4F4F 0101 FAFB F5F5"
	$"F5F5 F5FB F5FB F5F5 F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB FBFB FBFB FBFB F5F5"
	$"F5FB F5FB F5FB F5F5 F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB F5FB F5FB F5F5 F5F5"
	$"FBFB FBFB FBFB FBFB F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB F5FB F5FB F5F5 F5F5"
	$"FB4F 4FFC FA01 FDFB F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5F5 F5FB F5F5 F5F5 F5F5"
	$"FB4F FCFC FCFC FCFB F5FB F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB F5FB F5FB F5F5 F5F5"
	$"FB4F F9FC FCFC FCFB F5F5 F5FB F5F5 F5FF"
	$"FFF5 F5F5 F5F5 F5FB F5FB F5FB F5FB F5F5"
	$"FB4F F9FC 0000 FAFB F5FB F5FB F5F5 F5FF"
	$"FFFF FFFF FFFF F5FB F5F5 F5FB F5FB F5F5"
	$"FB4F 4FFC 0000 FCFB F5FB F5F5 F5F5 F5FF"
	$"00FF F6F6 F6FF F5FB F5FB F5FB F5FB F5F5"
	$"FB4F 4F07 FCFC FCFB F5FB F5FB F5F5 F5FF"
	$"0000 FFF6 F6FF F5F5 F5FB F5FB F5FB F5F5"
	$"FBFB FBFB FBFB FBFB F5FB F5FB F5F5 F5FF"
	$"0000 00FF F6FF F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"0000 0000 FFFF F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"0000 0000 00FF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
};

resource 'ics#' (136, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"07E0 0FF0 0C30 FFFF 8001 9EA9 9AA1 9229"
		$"9EA9 8001 85E9 8569 F129 55E9 3001 1FFF",
		/* [2] */
		$"07E0 0FF0 0FF0 FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF 7FFF 3FFF 1FFF"
	}
};

resource 'ics4' (136, purgeable) {
	$"0000 0FFF FFF0 0000 0000 FFFF FFFF 0000"
	$"0000 FF00 00FF 0000 FFFF FFFF FFFF FFFF"
	$"F0C0 C0C0 C0C0 C0CF FC0E EEFC ECEC EC0F"
	$"F0CE E0F0 E0E0 C0CF FC0E C0EC 0CEC EC0F"
	$"F0CE EEE0 E0E0 E0CF FC0C 0C0C 0C0C 0C0F"
	$"F0C0 CECE EEE0 E0CF FC0C 0E0E CFEC EC0F"
	$"FFFF C0CE C0E0 E0CF 0FCF 0E0E EEEC EC0F"
	$"00FF C0C0 C0C0 C0CF 000F FFFF FFFF FFFF"
};

resource 'ics8' (136, purgeable) {
	$"0000 0000 00FF FFFF FFFF FF00 0000 0000"
	$"0000 0000 FFFF FFFF FFFF FFFF 0000 0000"
	$"0000 0000 FFFF 0000 0000 FFFF 0000 0000"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FFF5 F5FB FBFB FEF5 FBF5 FBF5 FBF5 F5FF"
	$"FFF5 F5FB FC02 FEF5 FBF5 FBF5 F5F5 F5FF"
	$"FFF5 F5FB 4F01 FBF5 F5F5 FBF5 FBF5 F5FF"
	$"FFF5 F5FB FBFB FBF5 FBF5 FBF5 FBF5 F5FF"
	$"FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"FFF5 F5F5 F5FB F5FB FBFB FBF5 FBF5 F5FF"
	$"FFF5 F5F5 F5FB F5FB 4FFE FBF5 FBF5 F5FF"
	$"FFFF FFFF F5F5 F5FB 0701 FBF5 FBF5 F5FF"
	$"00FF F7FF F5FB F5FB FBFB FBF5 FBF5 F5FF"
	$"0000 FFFF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};

// -------------------------------------------------------------------------------------------------------------
// Three D Meta File

resource 'ICN#' (138, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FC00 1000 0600 1000 0500 1000 0480"
		$"103F 0440 1041 8420 1082 87F0 11FC 8010"
		$"1104 8010 1104 8010 1104 8010 1105 0010"
		$"1106 0010 11FC 0010 1000 1E10 1000 2110"
		$"1000 4090 1070 8050 1050 8050 1088 8050"
		$"1088 8050 1104 4090 1104 2110 1202 1E10"
		$"1202 0010 1401 0010 1401 0010 1306 0010"
		$"10F8 0010 1000 0010 1000 0010 1FFF FFF0",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80"
		$"1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};

resource 'icl4' (138, purgeable) {
	$"000F FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0FF0 0000 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 CFCF 0000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0FCC F000 0000"
	$"000F C0C0 C0DE EEEE C0C0 CFCC CF00 0000"
	$"000F 0C0C 0E07 070E EC0C 0FCC CCF0 0000"
	$"000F C0C0 E070 70E7 E0C0 CFFF FFFF 0000"
	$"000F 0C0D EEEE ED76 EC0C 0C0C 0C0F 0000"
	$"000F C0CE 7777 7E67 E0C0 C0C0 C0CF 0000"
	$"000F 0C0E 7777 7E76 EC0C 0C0C 0C0F 0000"
	$"000F C0CE 7777 7E67 D0C0 C0C0 C0CF 0000"
	$"000F 0C0E 7777 7E7E 0C0C 0C0C 0C0F 0000"
	$"000F C0CE 7777 7EE0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0D EEEE ED0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0CF FFF0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0DF2 222F DC0F 0000"
	$"000F C0C0 C0C0 C0C0 CF2C C223 F0CF 0000"
	$"000F 0C0C 0DFD 0C0C F2C0 0D23 4F0F 0000"
	$"000F C0C0 CF8F C0C0 F2C0 C233 4FCF 0000"
	$"000F 0C0C F88D FC0C F22D 2233 4F0F 0000"
	$"000F C0C0 F80D F0C0 F222 3334 4FCF 0000"
	$"000F 0C0F 8808 DF0C 0F33 3344 FC0F 0000"
	$"000F C0CF 8088 DFC0 CDF4 444F D0CF 0000"
	$"000F 0CF8 8088 8DFC 0C0F FFFC 0C0F 0000"
	$"000F C0F8 0888 8DF0 C0C0 C0C0 C0CF 0000"
	$"000F 0E88 0888 88DE 0C0C 0C0C 0C0F 0000"
	$"000F CE88 8888 88DE C0C0 C0C0 C0CF 0000"
	$"000F 0CFE 8888 8EFC 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 FFFF F0C0 C0C0 C0C0 C0CF 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0C0F 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF 0000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF"
};


resource 'icl8' (138, purgeable) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF 0000 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF FF00 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2BFF 0000 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5FF 2B2B FF00 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 7FAB ABAB ABAB"
	$"54F5 F5F5 F5FF 2B2B 2BFF 0000 0000 0000"
	$"0000 00FF F5F5 F5F5 F5AB 002A 002A 00AB"
	$"ABF5 F5F5 F5FF 2B2B 2B2B FF00 0000 0000"
	$"0000 00FF F5F5 F5F5 AB00 2A00 2A00 AB54"
	$"ABF5 F5F5 F5FF FFFF FFFF FFFF 0000 0000"
	$"0000 00FF F5F5 F57F ABAB ABAB AB7F 542A"
	$"ABF5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5AB 2A2A 2A2A 2AAB 2A54"
	$"ABF5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5AB 2A2A 2A2A 2AAB 542A"
	$"ABF5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5AB 2A2A 2A2A 2AAB 2A54"
	$"7FF5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5AB 2A2A 2A2A 2AAB 54AB"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5AB 2A2A 2A2A 2AAB ABF5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F57F ABAB ABAB AB7F F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5FF FFFF FFF5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F9 FF16 1616 16FF F9F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5FF 1608 0816 16D8 FFF5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F59F FF9F F5F5 F5F5"
	$"FF16 0800 0133 16D8 69FF F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5FF 92FF F5F5 F5F5"
	$"FF16 0801 0816 D8D8 69FF F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 FF92 929F FFF5 F5F5"
	$"FF16 1633 1616 D8D8 69FF F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 FF92 009F FFF5 F5F5"
	$"FF16 1616 D8D8 D869 69FF F5FF 0000 0000"
	$"0000 00FF F5F5 F5FF 9292 00E3 9FFF F5F5"
	$"F5FF D8D8 D8D8 6969 FFF5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5FF 9200 92E3 9FFF F5F5"
	$"F5F9 FF69 6969 69FF F9F5 F5FF 0000 0000"
	$"0000 00FF F5F5 FF92 9200 9292 E39F FFF5"
	$"F5F5 F5FF FFFF FFF5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 FF92 0092 9292 E39F FFF5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF 00A5 9292 0092 9292 92E3 9FA5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5A5 9292 9292 9292 92E3 9FA5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 FFA5 9292 9292 92A5 FFF5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 FFFF FFFF FFF5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 F5FF 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF"
};


resource 'ics#' (138, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4028 4024 5C3C 5404 5CE4 4114 4114"
		$"4114 48E4 4C04 5404 5604 4804 4004 7FFC",
		/* [2] */
		$"7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
		$"7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	}
};

resource 'ics4' (138, purgeable) {
	$"0FFF FFFF FFFF 0000 0F0C 0C0C 0CFC F000"
	$"0FC0 C0C0 C0FC CF00 0F06 660C 0CFF FF00"
	$"0FC6 66C0 C0CC CF00 0F06 660C 0C0C 0F00"
	$"0FC0 C0C0 22C0 CF00 0F0C 0C02 022C 0F00"
	$"0FC0 C0C2 2220 CF00 0F0C 8C0C 220C 0F00"
	$"0FC0 88C0 C0C0 CF00 0F08 880C 0C0C 0F00"
	$"0FC8 8880 C0C0 CF00 0F0C 8C0C 0C0C 0F00"
	$"0FC0 C0C0 C0C0 CF00 0FFF FFFF FFFF FF"
};

resource 'ics8' (138, purgeable) {
	$"00FF FFFF FFFF FFFF FFFF FFFF 0000 0000"
	$"00FF 2B2B 2B2B 2B2B 2B2B FF2B FF00 0000"
	$"00FF 2B2B 2B2B 2B2B 2B2B FF2B 2BFF 0000"
	$"00FF 2BEC ECEC 2B2B 2B2B FFFF FFFF 0000"
	$"00FF 2BEC ECEC 2B2B 2B2B 2B2B 2BFF 0000"
	$"00FF 2BEC ECEC 2B2B 2B2B 2B2B 2BFF 0000"
	$"00FF 2B2B 2B2B 2B2B 1717 2B2B 2BFF 0000"
	$"00FF 2B2B 2B2B 2B17 0017 172B 2BFF 0000"
	$"00FF 2B2B 2B2B 2B17 1717 D82B 2BFF 0000"
	$"00FF 2B2B E32B 2B2B 17D8 2B2B 2BFF 0000"
	$"00FF 2B2B E3E3 2B2B 2B2B 2B2B 2BFF 0000"
	$"00FF 2BE3 E39F 2B2B 2B2B 2B2B 2BFF 0000"
	$"00FF 2BE3 E3E3 9F2B 2B2B 2B2B 2BFF 0000"
	$"00FF 2B2B 9F2B 2B2B 2B2B 2B2B 2BFF 0000"
	$"00FF 2B2B 2B2B 2B2B 2B2B 2B2B 2BFF 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};


// -------------------------------------------------------------------------------------------------------------

#if CALL_NOT_IN_CARBON
// HELP RESOURCES FOR MENUS

resource 'hmnu' (mApple, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem { /* no missing string */
		},
		{
			HMSkipItem {		/* Apple Menu Title is handled separately	*/
			},

			HMStringResItem { 	/* About� */
				mApple,1,
				mApple,1,
				mApple,1,
				0,0
			},
		}
};

resource 'STR#' (mApple, purgeable) {
	{
	HELPABOUTITEM;
	};
};


resource 'hmnu' (mFile, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem { /* no missing string */
		},
		{
			HMStringResItem {						/* help type of HMStringResItem = STR255	*/
				mFile,1,						/* menu title message		*/
				mFile,1,						/* disabled title message	*/
				0,0,
				0,0
			},

			HMStringResItem { /* New */
				mFile,2,
				mFile,3,
				0,0,
				0,0
			},

			HMStringResItem { /* Open� */
				mFile,4,
				mFile,5,
				0,0,
				0,0
			},

			HMSkipItem { 	/* Dash */
			},

			HMStringResItem { /* Close */
				mFile,6,
				mFile,7,
				0,0,
				0,0
			},

			HMStringResItem { /* Save */
				mFile,8,
				mFile,9,
				0,0,
				0,0
			},

			HMStringResItem { /* Save As� */
				mFile,10,
				mFile,11,
				0,0,
				0,0
			},

			HMSkipItem { 	/* Dash */
			},

			HMStringResItem { /* Page Setup� */
				mFile,12,
				mFile,13,
				0,0,
				0,0
			},

			HMStringResItem { /* Print� */
				mFile,14,
				mFile,15,
				0,0,
				0,0
			},
			
			HMStringResItem { /* Print One Copy */
				mFile,16,
				mFile,17,
				0,0,
				0,0
			},

			HMSkipItem { 	/* Dash */
			},

			HMStringResItem { /* Quit */
				mFile,18,
				mFile,19,
				0,0,
				0,0
			},
		}
};

resource 'STR#' (mFile, purgeable) {
	{
	HELPFILEMENU;

	HELPNEWE;
	HELPNEWD;

	HELPOPENE;
	HELPOPEND;

	HELPCLOSEE;
	HELPCLOSED;

	HELPSAVEE;
	HELPSAVED;

	HELPSAVEASE;
	HELPSAVEASD;

	HELPPAGESETUPE;
	HELPPAGESETUPD;

	HELPPRINTE;
	HELPPRINTD;

	HELPPRINTONEE;
	HELPPRINTONED;

	HELPQUITE;
	HELPQUITD;
	};
};


resource 'hmnu' (mEdit, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem { /* nomEdit missing string */
		},
		{
			HMStringResItem {						/* help type of HMStringResItem = STR255	*/
				mEdit,1,						/* menu title message		*/
				mEdit,1,						/* disabled title message	*/
				0,0,
				0,0
			},

			HMStringResItem { /* Undo */
				mEdit,2,
				mEdit,3,
				0,0,
				0,0
			},

			HMSkipItem { /* Dash */
			},

			HMStringResItem { /* Cut */
				mEdit,4,
				mEdit,5,
				0,0,
				0,0
			},

			HMStringResItem { /* Copy */
				mEdit,6,
				mEdit,7,
				0,0,
				0,0
			},

			HMStringResItem { /* Paste */
				mEdit,8,
				mEdit,9,
				0,0,
				0,0
			},

			HMStringResItem { /* Clear */
				mEdit,10,
				mEdit,11,
				0,0,
				0,0
			},

			HMSkipItem { 	/* Dash */
			},

			HMStringResItem { /* Select All */
				mEdit,12,
				mEdit,13,
				0,0,
				0,0
			},

			HMSkipItem { 	/* Dash */
			},

			HMStringResItem { /* Find */
				mEdit,22,
				mEdit,23,
				0,0,
				0,0
			},

			HMStringResItem { /* FindAgain */
				mEdit,24,
				mEdit,25,
				0,0,
				0,0
			},

			HMStringResItem { /* FindSelection */
				mEdit,26,
				mEdit,27,
				0,0,
				0,0
			},

			HMStringResItem { /* Replace */
				mEdit,28,
				mEdit,29,
				0,0,
				0,0
			},

			HMStringResItem { /* ReplaceAgain */
				mEdit,30,
				mEdit,31,
				0,0,
				0,0
			},
#if 0
			HMSkipItem { 	/* Dash */
			},
			
			HMStringResItem { /* Next page */
				mEdit,14,
				mEdit,15,
				0,0,
				0,0
			},

			HMStringResItem { /* Prev page */
				mEdit,16,
				mEdit,17,
				0,0,
				0,0
			},

			HMStringResItem { /* Goto page */
				mEdit,18,
				mEdit,19,
				0,0,
				0,0
			},

			HMSkipItem { 	/* Dash */
			},
			
			HMCompareItem {		/* Show Clipboard */
				EDITSHOWCLIPBOARDITEM,	
				HMStringResItem {
					mEdit,20,
					mEdit,20,
					mEdit,20,
					0,0
				}
			},
			HMCompareItem {		/* Hide Clipboard */
				CLIPBOARDHIDE,	
				HMStringResItem {
					mEdit,21,
					mEdit,21,
					mEdit,21,
					0,0
				}
			}
#endif
		}
};


resource 'STR#' (mEdit, purgeable) {
	{
	HELPEDITMENU;

	HELPUNDOE;
	HELPUNDOD;

	HELPCUTE;
	HELPCUTD;

	HELPCOPYE;
	HELPCOPYD;

	HELPPASTEE;
	HELPPASTED;

	HELPCLEARE;
	HELPCLEARD;

	HELPSELECTALLE;
	HELPSELECTALLD;

	HELPNEXTPAGEE;
	HELPNEXTPAGED;

	HELPPREVPAGEE;
	HELPPREVPAGED;

	HELPGOTOPAGEE;
	HELPGOTOPAGED;

	HELPSHOWCLIPBOARDE;
	
	HELPHIDECLIPBOARDE;
	
	/* 22 */
	HELPFINDE;
	HELPFINDD;

	HELPFINDAGAINE;
	HELPFINDAGAIND;

	HELPFINDSELECTIONE;
	HELPFINDSELECTIOND;

	HELPREPLACEE;
	HELPREPLACED;

	HELPREPLACEAGAINE;
	HELPREPLACEAGAIND;
	};
};

resource 'hmnu' (mFont, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
	HMStringResItem {
		mFont, 3,
		mFont, 4,
		mFont, 5,
		0, 0
	},
	{	/* array HMenuArray: 1 elements */
		/* [1] */
		HMStringResItem {
			mFont, 1,
			mFont, 2,
			0, 0,
			0, 0
		}
	}
};

resource 'STR#' (mFont, purgeable) {
	{	
	HELPFONTMENUE;
	HELPFONTMENUD;
	
	HELPFONTITEME;
	HELPFONTITEMD;
	HELPFONTITEMC;
	}
};


resource 'hmnu' (mStyle, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
	HMStringResItem {
		mStyle, 3,
		mStyle, 4,
		mStyle, 5,
		0, 0
	},
	{	/* array HMenuArray: 1 elements */
		/* [1] */
		HMStringResItem {
			mStyle, 1,
			mStyle, 2,
			0, 0,
			0, 0
		}
	}
};


resource 'STR#' (mStyle, purgeable) {
	{	
	HELPSTYLEMENUE;
	HELPSTYLEMENUD;
	
	HELPSTYLEITEME;
	HELPSTYLEITEMD;
	HELPSTYLEITEMC;
	}
};


resource 'hmnu' (mSize, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
	HMStringResItem {
		mSize, 3,
		mSize, 4,
		mSize, 5,
		0, 0
	},
	{	/* array HMenuArray: 1 elements */
		/* [1] */
		HMStringResItem {
			mSize, 1,
			mSize, 2,
			0, 0,
			0, 0
		}
	}
};


resource 'STR#' (mSize, purgeable) {
	{	
	HELPSIZEMENUE;
	HELPSIZEMENUD;
	
	HELPSIZEITEME;
	HELPSIZEITEMD;
	HELPSIZEITEMC;
	}
};


resource 'hmnu' (mSound, purgeable) {
		HelpMgrVersion,							/* Help Version */
		0,										/* options */
		0,										/* theProc */
		0,										/* variant */
		HMSkipItem { /* no missing string */
		},
		{
			HMStringResItem {						/* help type of HMStringResItem = STR255	*/
				mSound,1,						/* menu title message		*/
				mSound,1,						/* disabled title message	*/
				0,0,
				0,0
			},

			HMStringResItem { /* Record */
				mSound,2,
				mSound,3,
				0,0,
				0,0
			},

			HMStringResItem { /* Play */
				mSound,4,
				mSound,5,
				0,0,
				0,0
			},

			HMStringResItem { /* Erase */
				mSound,6,
				mSound,7,
				0,0,
				0,0
			},

			HMSkipItem { 	/* Dash */
			},

			HMCompareItem {		/* Speak All */
				SOUNDSPEAKALLITEM,	
				HMStringResItem {
					mSound,8,
					mSound,9,
					0,0,
					0,0
				}
			},
				
			HMCompareItem {		/* Speak Selection */
				SOUNDSPEAKSELECTIONITEM,	
				HMStringResItem {
					mSound,10,
					mSound,11,
					0,0,
					0,0
				}
			},
				
			HMStringResItem { /* stop speaking */
				mSound,12,
				mSound,13,
				0,0,
				0,0
			},

			HMStringResItem { /* voices */
				mSound,14,
				mSound,15,
				0,0,
				0,0
			},
				
		}
};

resource 'STR#' (mSound, purgeable) {
	{
	HELPSOUNDMENU;

	HELPRECORDE;
	HELPRECORDD;

	HELPPLAYE;
	HELPPLAYD;

	HELPERASEE;
	HELPERASED;

	HELPSPEAKALLE;
	HELPSPEAKALLD;
	
	HELPSPEAKSELECTIONE;
	HELPSPEAKSELECTIOND;

	HELPSTOPSPEAKINGE;
	HELPSTOPSPEAKINGD;

	HELPVOICESE;
	HELPVOICESD;
	};
};

resource 'hmnu' ( mVoices, purgeable )
{
	HelpMgrVersion,							// Help Manager Version
	hmDefaultOptions,						// Options
	0,										// Use default Balloon definition function
	0,										// Use default 
		
	HMStringResItem							// Help for missing items.  Used for voice names.
	{
		mVoices, 1,		// Voice name enabled
		0, 0,			// Voice names are never dimmed
		mVoices, 2,		// Voice name enabled and checked
		0, 0,			// Voice names will never be marked
	},
	{
	}
};

resource 'STR#' (mVoices, purgeable) {
	{
	HELPVOICESELE;
	HELPVOICESELC;
	};
};

resource 'STR#' (kWindowHelpID, purgeable) {
	{
	HELPACTIVESCROLL;
	HELPDIMHORIZSCROLL;
	HELPDIMVERTSCROLL;
	
	HELPGROWBOX;
	
	HELPGENERICCONTENT;

	HELPPICTCONT;
	HELPPICTSEL;
	
	HELPTEXTCONT;
	};
};

#endif	// CALL_NOT_IN_CARBON

