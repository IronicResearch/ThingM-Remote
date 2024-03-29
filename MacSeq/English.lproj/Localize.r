/*
	File:		Localize.r

	Contains:	all strings for SimpleText that need to be localized.  Nothing in a .r file outside
				of this file should be language specific.

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

// Basic application information
#define APPNAME					"MacSeq"

// Version number is "MAJOR.MINORHIGHNIBBLE.MINORLONIBBLE"
#define	MAJORVERSION			0x01
#define MINORVERSION			0x40
#define STAGE					final
#define RELEASE					0
#define SHORTVERSIONSTRING		"10.0"
#define COPYRIGHTNOTICE			" � Ironic Research Labs 1989-2004"

#define COUNTRYVERSION			verUS
#define LONGVERSIONSTRING		APPNAME " " SHORTVERSIONSTRING ", " COPYRIGHTNOTICE

// open kinds
#define READONLYDOC		"MacSeq read-only document",
#define PICTDOC			"MacSeq picture",
#define MOVIEDOC		"MacSeq movie",
#define PRINTDOC		"MacSeq print document",
#define PDDDOC			"portable digital document",
#define STATIONERYDOC	"MacSeq stationery file",
#define TEXTDOC			"MacSeq text document"


// Window titles
#define FIRSTNEWDOCUMENTTITLE	"untitled"
#define NEWDOCUMENTTITLE		"untitled ^0"
#define ABOUTBOXTITLE			"About " APPNAME "�"
#define CLIPBOARDTITLE			"Clipboard"

// Misc strings
#define ABOUTSTRING1			APPNAME
#define ABOUTSTRING2			"ThingM Sequencer"
#define ABOUTSTRING3			"for Mac OS X"
#define ABOUTSTRING4			"(Based on Apple Carbon Toolbox)"
#define ABOUTSTRING5			COPYRIGHTNOTICE
#define ABOUTSTRING6			"Version " SHORTVERSIONSTRING
#define CLIPBOARDNONESTRING		"Clipboard contents: none"
#define CLIPBOARDUNKNOWNSTRING	"Clipboard contents: unknown type"
#define CLIPBOARDPICTSTRING		"Clipboard contents: picture"
#define CLIPBOARDTEXTSTRING		"Clipboard contents: text"
#define CLIPBOARDHIDE			"Hide Clipboard"
#define CLIPBOARDSHOW			"Show Clipboard"
#define TEXTSAVEPROMPT			"Save this document as:"
#define TEXTSAVEBUTTON			"Save"
#define TEXTCANCELBUTTON		"Cancel"
#define TEXTEJECTBUTTON			"Eject"
#define TEXTDESKTOPBUTTON		"Desktop"
#define TEXTSTATIONERY			"To save this document as a stationery pad (a template document) click this button before clicking the Save button."
#define TEXTSTATIONERYSELECTED	"When this button is selected, the document you are saving will be saved as a stationery pad (a template document)."
#define TEXTDOCUMENT			"To save your document as a "APPNAME" document, click this button."
#define TEXTDOCUMENTSELECTED	"When this button is selected, your document will be saved as a "APPNAME" document."
#define TEXTPICTMARKER1			"\$CA"
#define TEXTPICTMARKER2			""
#define HELPMENUCOMMAND			"?"

// Save changes alert strings
#define SAVESAVEBUTTON			"Save"
#define SAVECANCELBUTTON		"Cancel"
#define SAVEDONTSAVEBUTTON		"Don�t Save"
#define SAVEBUTTONSHORTCUTS		"Dd"
#define SAVESTATICTEXT			"Save changes to the document �^0� before closing?"

// Go to page alert strings
#define GOTOOKBUTTON			"OK"
#define GOTOCANCELBUTTON		"Cancel"
#define GOTOSTATICTEXT			"Go to page"
#define GOTOOFSTATICTEXT		"of ^0."
#define GOTOPAGESLIDERSTRING	"Go to page: "
#define PAGELABELSTRING			"^0 of ^1 (^2%)"

// Find/Replace alert strings
#define FINDBUTTON				"Find"
#define FINDCANCELBUTTON		"Cancel"
#define REPLACEBUTTON			"Replace"
#define REPLACEALLBUTTON		"Replace All"
#define FINDCASESENSITIVE		"Case Sensitive"
#define FINDWRAPAROUND			"Wrap�Around Search"
#define FINDSTATIC				"Find what?"
#define REPLACESTATIC			"Replace with what?"

// Error alerts
#define ERROKBUTTON			"OK"
#define ERRCANCELBUTTON		"Cancel"
#define ERRSTARTUPVERSION	APPNAME " requires System 7 in order to run."
#define ERRSTARTUPFAILED	APPNAME " is unable to run because of a serious error."
#define ERRNEWFAILEDRAM		APPNAME" cannot make a new document because there is not enough memory."
#define ERROPENFAILED		APPNAME" cannot open this document. It may be in use by someone else."
#define ERRSAVEFAILED		APPNAME" cannot save this document. This disk may be full or locked, or the file may be locked."
#define ERRMODFAILED		"You cannot modify this document; you can only look at it."
#define ERRSETUPFAILED		APPNAME" is unable to setup the page for this document. Please use the Chooser to select a printer."
#define ERRPRINTMEM			APPNAME" cannot print this document because there is not enough memory."
#define ERRPAGESETUPMEM		APPNAME" cannot perform a Page Setup for this document because there is not enough memory."
#define ERRPRINTDISKFULL	APPNAME" cannot print this document because there is not enough room available on the hard disk."
#define ERRPRINTTYPE		APPNAME" cannot print this kind of document."
#define ERRPRINTFAILED		APPNAME" is unable to print this document. Please use the Chooser to select a printer."
#define ERROPENSIZE			"This document is too large to be opened by "APPNAME"."
#define ERRPASTESIZE		"The clipboard content is too large to be pasted in this document."
#define ERRTYPESIZE			"This document is too large to accept any more text."
#define ERRSIZECHANGE		"Your requested change would cause this document to become too long."
#define ERROPENTYPE			APPNAME" cannot display this kind of document."
#define ERRCOPYTOOBIG		APPNAME" cannot copy from this document, the selection is too large."
#define ERRSAVETYPE			APPNAME" cannot replace a different kind of document."
#define ERROPENOUTOFMEM		APPNAME" cannot display this document because there is not enough memory."
#define ERROPENDRAW			APPNAME" cannot display this document because an error occurred while drawing."
#define ERROPENPS			APPNAME" may not be able to display this document properly because it contains PostScript�."
#define ERROPENNOPAGES		APPNAME" cannot display this document because it contains no pages."
#define ERRRECORDFULL		APPNAME" cannot record at this moment because there is not enough memory."
#define ERRRECORDERR		APPNAME" cannot record at this moment.  Another application may be using Sound Input."
#define ERRSPEAKFULL		"There is not enough memory to speak.  Try increasing " APPNAME "'s partition size."
#define ERRSPEAKERR			"An error occurred when " APPNAME " tried to speak."
#define ERRCHANGEVOICE		APPNAME " is unable to change to the new voice.  Try increasing " APPNAME "'s partition size."
#define ERROPENANOTHERFAILED		APPNAME " is unable to open another document at this time.  Cancel the current Print or Page Setup dialog and try again."
#define ERRPRINTANOTHERFAILED		APPNAME " is unable to print another document at this time.  Cancel the current Print or Page Setup dialog and try again."

// Apple menu strings
#define ABOUTITEM				"About " APPNAME "�"

// File menu strings
#define FILEMENU				"File"
#define FILENEWITEM				"New"
	#define FILENEWKEY				"N"
#define FILEOPENITEM			"Open�"
	#define FILEOPENKEY				"O"
#define FILECLOSEITEM			"Close"
	#define FILECLOSEKEY			"W"
#define FILESAVEITEM			"Save"
	#define FILESAVEKEY				"S"
#define FILESAVEASITEM			"Save As�"
	#define FILESAVEASKEY			nokey
#define FILEPAGESETUPITEM		"Page Setup�"
	#define FILEPAGESETUPKEY		nokey
#define FILEPRINTITEM			"Print�"
	#define FILEPRINTKEY			"P"
#define FILEPRINTONECOPYITEM	"Print One Copy"
	#define FILEPRINTONECOPYKEY		nokey
#define FILEQUITITEM			"Quit"
	#define FILEQUITKEY				"Q"

// Edit menu strings
#define EDITMENU				"Edit"
#define EDITUNDOITEM			"Undo"
	#define EDITUNDOKEY				"Z"
#define EDITCUTITEM				"Cut"
	#define EDITCUTKEY				"X"
#define EDITCOPYITEM			"Copy"
	#define EDITCOPYKEY				"C"
#define EDITPASTEITEM			"Paste"
	#define EDITPASTEKEY			"V"
#define EDITCLEARITEM			"Clear"
	#define EDITCLEARKEY			nokey
#define EDITSELECTALLITEM		"Select All"
#define EDITSELECTNONEITEM		"Remove Selection"
	#define EDITSELECTALLKEY		"A"

#define FINDITEM				"Find�"
	#define FINDKEY					"F"
#define FINDAGAINITEM			"Find Again"
	#define FINDAGAINKEY			"G"
#define USESELECTIONFORFIND		"Use Selection For Find"
	#define SELECTIONFORFINDKEY		"E"
#define REPLACEITEM				"Replace�"
	#define REPLACEKEY				"R"
#define REPLACEAGAINITEM		"Replace Again"
	#define REPLACEAGAINKEY			"D"

#define EXECUTEITEM				"Execute"	// for AppleScript
	#define EXECUTEKEY			"="		// for AppleScript

#define EDITNEXTPAGEITEM		"Next Page"
	#define EDITNEXTPAGEKEY			"="
#define EDITPREVPAGEITEM		"Previous Page"
	#define EDITPREVPAGEKEY			"-"
#define EDITGOTOPAGEITEM		"Go to Page�"
	#define EDITGOTOPAGEKEY			nokey
#define EDITSHOWCLIPBOARDITEM	"Show Clipboard  "
	#define EDITSHOWCLIPBOARDKEY	nokey

// Font menu strings
#define FONTMENU				"Font"

// Size menu strings
#define SIZEMENU				"Size"
#define SIZE9POINT				"9 Point"
	#define SIZE9POINTKEY			nokey
#define SIZE10POINT				"10 Point"
	#define SIZE10POINTKEY			nokey
#define SIZE12POINT				"12 Point"
	#define SIZE12POINTKEY			nokey
#define SIZE14POINT				"14 Point"
	#define SIZE14POINTKEY			nokey
#define SIZE18POINT				"18 Point"
	#define SIZE18POINTKEY			nokey
#define SIZE24POINT				"24 Point"
	#define SIZE24POINTKEY			nokey
#define SIZE36POINT				"36 Point"
	#define SIZE36POINTKEY			nokey
	
// Style menu strings
#define STYLEMENU				"Style"
#define STYLEPLAIN				"Plain Text"
	#define STYLEPLAINKEY			"T"
#define STYLEBOLD				"Bold"
	#define STYLEBOLDKEY			"B"
#define STYLEITALIC				"Italic"
	#define STYLEITALICKEY			"I"
#define STYLEUNDERLINE			"Underline"
	#define STYLEUNDERLINEKEY		"U"
#define STYLEOUTLINE			"Outline"
	#define STYLEOUTLINEKEY			nokey
#define STYLESHADOW				"Shadow"
	#define STYLESHADOWKEY			nokey
#define STYLECONDENSED			"Condensed"
	#define STYLECONDENSEDKEY		nokey
#define STYLEEXTENDED			"Extended"
	#define STYLEEXTENDEDKEY		nokey

// Sound menu strings
#define SOUNDMENU				"Sound"
#define SOUNDRECORDITEM			"Record�"
	#define SOUNDRECORDKEY			nokey
#define SOUNDPLAYITEM			"Play"
	#define SOUNDPLAYKEY			nokey
#define SOUNDERASEITEM			"Erase"
	#define SOUNDERASEKEY			nokey
#define SOUNDSPEAKSELECTIONITEM	"Speak Selection"
#define SOUNDSPEAKALLITEM		"Speak All"
	#define SOUNDSPEAKKEY			"J"
#define SOUNDSTOPSPEAKINGITEM	"Stop Speaking"
	#define SOUNDSTOPSPEAKINGKEY	"."
#define SOUNDVOICESITEM			"Voices"
	#define SOUNDVOICESKEY			nokey
#define NOVOICESITEM			"None"
	#define NOVOICESKEY			nokey
	
// Run menu strings
#define RUNMENU					"Run"
#define RUNCAM					"Camera Frame"
	#define RUNCAMKEY			"1"
#define RUNPRJ					"Projector Frame"
	#define RUNPRJKEY			"2"
#define RUNSEQ					"Sequencer..."
	#define RUNSEQKEY			"3"
#define RUNSCRIPT				"Run Script"
	#define RUNSCRIPTKEY			"4"
#define RUNLOOP					"Loop Script"
	#define RUNLOOPKEY			"5"
#define RUNRECORD				"Script Recording"
	#define RUNRECORDKEY			nokey
#define RUNCOUNTER				"Frame Counters..."
	#define RUNCOUNTERKEY			nokey
#define RUNSERIAL				"Serial Ports..."
	#define RUNSERIALKEY			nokey
#define RUNKEYPAD				"Keypad Map..."
	#define RUNKEYPADKEY			nokey
	
	
// items in the GX pop up menu
#define GXPOPUPMENU				"Display Options"

#define GXFIFTY					"50%"
#define GXONEHUNDRED			"100%"
#define GXONEHUNDREDTWELVE		"112%"
#define GXONEHUNDREDFIFTY		"150%"
#define GXTWOHUNDRED			"200%"
#define GXFOURHUNDRED			"400%"
#define GXSCALETOFIT			"Scale image to fit window"
#define GXDONTSHOWMARGINS		"Don�t show margins"

// Balloon Help Strings
#define FINDERHELPSTRING	APPNAME "\n\n" "This is a simple application program that you can use to view Read Me files, text files, movies, 3D files, and certain graphics files, and to write letters and memos."

// Apple menu help
#define HELPABOUTITEM		"Displays information about the version of the "APPNAME" application."

// File menu help
#define HELPFILEMENU		"File menu\n\nUse this menu to open, close, save, and print "APPNAME" documents, and to quit "APPNAME"."
#define HELPNEWE			"Opens a new "APPNAME" document called �untitled.�"
#define HELPNEWD			"Opens a new "APPNAME" document called �untitled.�  Not available because there is a dialog box on the screen."
#define HELPOPENE			"Displays a dialog box that allows you to select an existing "APPNAME" document to open."
#define HELPOPEND			"Displays a dialog box that allows you to select an existing "APPNAME" document to open.  Not available because there is a dialog box on the screen."
#define HELPCLOSEE			"Closes the open "APPNAME" document or window."
#define HELPCLOSED			"Closes the open "APPNAME" document or window.  Not available because no "APPNAME" document is open."
#define HELPSAVEE			"Saves the open "APPNAME" document.  If you have not saved the document before, a dialog box appears in which you assign a name to the document and indicate where to save it."
#define HELPSAVED			"Saves the open "APPNAME" document.  Not available because no "APPNAME" document is open, because no changes have been made to the open document, or because the open document is a special type that cannot be changed or saved."
#define HELPSAVEASE			"Displays a dialog box in which you can assign a name to the document and indicate where to save it."
#define HELPSAVEASD			"Displays a dialog box in which you can name and save the open document.  Not available because no "APPNAME" document is open, because no changes have been made to the open document, or because the open document is a special type that cannot be changed."
#define HELPPAGESETUPE		"Displays a dialog box in which you can select paper size, orientation, and other printing options."
#define HELPPAGESETUPD		"Displays a dialog box in which you can select paper size, orientation, and other printing options.  Not available because the current window cannot be printed."
#define HELPPRINTE			"Displays a dialog box in which you can specify the number of copies you want to print and other printing options."
#define HELPPRINTD			"Displays a dialog box in which you can specify the number of copies you want to print and other printing options.  Not available because no "APPNAME" document is open or because the open "APPNAME" document is empty."
#define HELPPRINTONEE		"Prints a single copy of the "APPNAME" document."
#define HELPPRINTONED		"Prints a single copy of the "APPNAME" document.  Not available because no "APPNAME" document is open or because the open "APPNAME" document is empty."
#define HELPQUITE			"Quits the "APPNAME" application.  If you have not saved changes you made to the open document, you will be asked whether you want to save changes."
#define HELPQUITD			"Quits the "APPNAME" application.  Not available because there is a dialog box on the screen."

// Edit menu help
#define HELPEDITMENU		"Edit menu\n\nUse this menu to undo your last action, to manipulate text or graphics, to select the entire contents of a document, and to show what�s on the Clipboard."
#define HELPUNDOE			"Undoes your last action. In "APPNAME", you can use this command to replace material you have cut or cleared or to remove material you have pasted or typed."
#define HELPUNDOD			"Undoes your last action if it involved cutting, clearing, pasting, or typing. Not available now because your last action cannot be undone."
#define HELPCUTE			"Removes the selected text or graphics and places it temporarily into a storage area called the Clipboard."
#define HELPCUTD			"Removes the selected text or graphics and places it temporarily into a storage area called the Clipboard.  Not available now because nothing is selected."
#define HELPCOPYE			"Copies the selected text or graphics.  The original selection remains where it is. The copy is placed temporarily into a storage area called the Clipboard."
#define HELPCOPYD			"Copies the selected text or graphics. The original selection remains where it is. The copy is placed temporarily into a storage area called the Clipboard. The command is not available now because nothing is selected."
#define HELPPASTEE			"Inserts the contents of the Clipboard at the location of the insertion point."
#define HELPPASTED			"Inserts the contents of the Clipboard at the location of the insertion point. Not available because there is nothing on the Clipboard or because the contents of the Clipboard are of a type "APPNAME" can�t read."
#define HELPCLEARE			"Removes the selected text or graphics without storing it on the Clipboard."
#define HELPCLEARD			"Removes the selected text or graphics without storing it on the Clipboard.  Not available now because nothing is selected."
#define HELPSELECTALLE		"Selects or deselects (for pictures) the entire contents of the open "APPNAME" document."
#define HELPSELECTALLD		"Selects or deselects (for pictures) the entire contents of the open "APPNAME" document.  Not available now because no "APPNAME" document is open or because the open document is empty."
#define HELPNEXTPAGEE		"Displays the next page in the document."
#define HELPNEXTPAGED		"Displays the next page in the document.  Disabled because this document contains no additional pages."
#define HELPPREVPAGEE		"Displays the previous page in the document."
#define HELPPREVPAGED		"Displays the previous page in the document.  Disabled because this document contains no additional pages."
#define HELPGOTOPAGEE		"Displays the specified page in the document."
#define HELPGOTOPAGED		"Displays the specified page in the document.  Disabled because this type of document contains no additional pages, or does not support this command."
#define HELPSHOWCLIPBOARDE	"Displays the contents of the Clipboard (a storage area for the last item cut or copied)."
#define HELPHIDECLIPBOARDE	"Closes the Clipboard window."

#define HELPFINDE			"Allows you to search the current window for particular text."
#define HELPFINDD			"Allows you to search the current window for particular text.  Disabled because the current window is not searchable."
#define HELPFINDAGAINE		"Searches the current window for another occurrence of the found text."
#define HELPFINDAGAIND		"Searches the current window for another occurrence of the found text.  Disabled because the current window is not searchable, or because you have not entered text to find."
#define HELPFINDSELECTIONE	"Allows you to search the current window for another occurrence of the selected text."
#define HELPFINDSELECTIOND	"Allows you to search the current window for another occurrence of the selected text.  Disabled because the current window is not searchable, or because no text is currently selected."
#define HELPREPLACEE		"Allows you to search the current window for particular text and replace it."
#define HELPREPLACED		"Allows you to search the current window for particular text and replace it.  Disabled because the current window is not searchable."
#define HELPREPLACEAGAINE	"Searches the current window for another occurrence of the found text and replaces it."
#define HELPREPLACEAGAIND	"Searches the current window for another occurrence of the found text and replaces it.  Disabled because the current window is not searchable, or because you have not entered text to replace."

// Font menu help
#define HELPFONTMENUE		"Font menu\n\nUse this menu to change the font used for text in a document."
#define HELPFONTMENUD		"Font menu\n\nUse this menu to change the font used for text in a document.  Not available because the active document is not a text document."
#define HELPFONTITEME		"To use this font in the active document, choose it from this menu."
#define HELPFONTITEMD		"To use this font in the active document, choose it from this menu.  Not available because the active document is not a text document."
#define HELPFONTITEMC		"This is the currently selected font.  To change to another font, choose one from this menu."

// Style menu help
#define HELPSTYLEMENUE		"Style menu\n\nUse this menu to change the style used for text in a document."
#define HELPSTYLEMENUD		"Style menu\n\nUse this menu to change the style used for text in a document.  Not available because the active document is not a text document."
#define HELPSTYLEITEME		"To use this style in the active document, choose it from this menu."
#define HELPSTYLEITEMD		"To use this style in the active document, choose it from this menu.  Not available because the active document is not a text document."
#define HELPSTYLEITEMC		"This is the currently selected style.  To change to another style, choose one from this menu."

// Size menu help
#define HELPSIZEMENUE		"Size menu\n\nUse this menu to change the font size used for text in a document."
#define HELPSIZEMENUD		"Size menu\n\nUse this menu to change the font size used for text in a document.  Not available because the active document is not a text document."
#define HELPSIZEITEME		"To use this font size in the active document, choose it from this menu."
#define HELPSIZEITEMD		"To use this font size in the active document, choose it from this menu.  Not available because the active document is not a text document."
#define HELPSIZEITEMC		"This is the currently selected font size.  To change to another size, choose one from this menu."

// Sound menu help
#define HELPSOUNDMENU		"Sound menu\n\nUse this menu to record, play, or erase a voice annotation, or to hear the current text spoken."
#define HELPRECORDE			"Allows you to record a new voice annotation."
#define HELPRECORDD			"Allows you to record a new voice annotation. Not available because there is already a recorded voice annotation, your Macintosh does not support sound input, or this is a read�only document."
#define HELPPLAYE			"Plays the voice annotation."
#define HELPPLAYD			"Plays the voice annotation. Not available because there is no recorded voice annotation."
#define HELPERASEE			"Erases the current voice annotation so that you can record a new one."
#define HELPERASED			"Erases the current voice annotation so that you can record a new one. Not available because there is no recorded voice annotation, or this is a read�only document."
#define HELPSPEAKALLE		"Speaks all the text in the document aloud, using the currently checked voice from the Voices submenu, below."
#define HELPSPEAKALLD		"Speaks text aloud.  Not available, because Text-To-Speech is not installed, this document cannot be spoken, or there is not enough memory to speak (increase " APPNAME "'s partition size)."
#define HELPSPEAKSELECTIONE	"Speaks the currently selected text aloud, using the currently checked voice from the Voices submenu, below."
#define HELPSPEAKSELECTIOND	"Speaks the currently selected text aloud.  Not available, because Text-To-Speech is not installed, this document cannot be spoken, or there is not enough memory to speak (increase " APPNAME "'s partition size)."
#define HELPSTOPSPEAKINGE	"Quiets the speaking you hear right now."
#define HELPSTOPSPEAKINGD	"Quiets any speaking you started.  Not available because there is nothing being spoken at the moment."
#define HELPVOICESE			"Has submenu which allows you to pick the voice used when speaking."
#define HELPVOICESD			"Has submenu which allows you to pick which voice to speak with.  Not available now because speech is not available.  Either speech is not installed or there is not enough memory to speak (increase " APPNAME "'s partition size)."
#define HELPVOICESELE		"One of the possible voices to speak with.  Choose this item to speak with this voice next time you speak."
#define HELPVOICESELC		"The voice that will be used to speak with next time you speak."

// Window content help
#define HELPACTIVESCROLL	"Scroll bar\n\nTo scroll a little at a time, press one of the scroll arrows.  To scroll by the windowful, click in the gray bar.  To scroll to another part of the window, drag the scroll box."
#define HELPDIMHORIZSCROLL	"Scroll bar\n\nUse the scroll bar to see items that are out of view.  This scroll bar is not available because this window contains no items to the left or right of the window�s borders."
#define HELPDIMVERTSCROLL	"Scroll bar\n\nUse the scroll bar to see items that are out of view.  This scroll bar is not available because this window contains no items above or below the window�s borders."
#define HELPGROWBOX			"Size box\n\nTo change the height and width of the window, drag the size box."
#define HELPGENERICCONTENT	"This is a document window.  You cannot type or edit material in this document because the document is a read-only document."
#define HELPPICTCONT		"This is a document window � the area in which you can view and copy images from the "APPNAME" picture document."
#define HELPPICTSEL			"The area inside this rectangle is selected.  You can copy the selected area to the Clipboard by choosing Copy from the Edit menu."
#define HELPTEXTCONT		"This is a document window � the area in which you can type and edit text."
