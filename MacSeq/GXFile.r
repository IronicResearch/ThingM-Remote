/*
	File:		GXFile.r

	Contains:	GX print file support for simple text application

	Version:	SimpleText 1.4 or later
	
	Written by:	Tom Dowdy

	Copyright:	� 1993, 1995, 1997 by Apple Computer, Inc., all rights reserved.

	File Ownership:

		DRI:				Tom Dowdy

		Other Contact:		Jim Negrette

		Technology:			Macintosh Graphics Group

	Writers:

		(TD)	Tom Dowdy

	Change History (most recent first):

	$Log: GXFile.r,v $
	Revision 1.2  1998/08/26 16:04:12  mkellner
	Make work with new (built) headers
	
	Revision 1.1.1.1  1998/03/18 22:56:10  ivory
	Initial checkin of SimpleText.
	
 * 
 * 2     7/29/97 6:28 PM Tom Dowdy
 * Added appearance
 * 
 * 1     7/28/97 11:18 AM Duane Byram
 * first added to Source Safe project

		 <4>	 10/2/95	TD		adding in other selections for editing
		 <3>	 9/11/95	TD		adding more markup graphics
		 <2>	  9/8/95	TD		fixing zoom in icons
		 <1>	 8/21/95	TD		First checked in.

*/

#include "GXFile.h"
#include "Icons.r"

type 'LSTR' {
	string;
};

resource 'LSTR' (kLabelString) {
	PAGELABELSTRING
};

resource 'STR#' (kPageControlStrings) {
	{
	GOTOPAGESLIDERSTRING;
	};
};

resource 'MENU' (kGXPopUpMenu, GXPOPUPMENU) {
	kGXPopUpMenu, textMenuProc,
	AllItems,
	enabled, GXPOPUPMENU,
	{
		GXFIFTY, noicon, nokey, nomark, plain;
		GXONEHUNDRED, noicon, nokey, nomark, plain;
		GXONEHUNDREDTWELVE, noicon, nokey, nomark, plain;
		GXONEHUNDREDFIFTY, noicon, nokey, nomark, plain;
		GXTWOHUNDRED, noicon, nokey, nomark, plain;
		GXFOURHUNDRED, noicon, nokey, nomark, plain;
		"-", noicon, nokey, nomark, plain;
		GXSCALETOFIT, noicon, nokey, nomark, plain;
		"-", noicon, nokey, nomark, plain;
		GXDONTSHOWMARGINS, noicon, nokey, nomark, plain;
	}
};


#define BUTTONX		125
#define BUTTONY		45

resource 'DITL' (kGotoPageDialogID, purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{BUTTONY, BUTTONX+60+15, BUTTONY+20, BUTTONX+60+15+60},
		Button {
			enabled,
			GOTOOKBUTTON
		},
		/* [2] */
		{BUTTONY, BUTTONX, BUTTONY+20, BUTTONX+60},
		Button {
			enabled,
			GOTOCANCELBUTTON
		},
		/* [3] */
		{12, 18, 28, 100},
		StaticText {
			disabled,
			GOTOSTATICTEXT
		},
		/* [4] */
		{12, 110, 28, 178},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{12, 190, 28, 260},
		StaticText {
			disabled,
			GOTOOFSTATICTEXT
		},
	}
};


resource 'DLOG' (kGotoPageDialogID, purgeable) {
	{105, 150, 179, 420},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	kGotoPageDialogID,
	"",
	alertPositionParentWindowScreen
};

resource 'dlgx' (kGotoPageDialogID) {
	versionZero {
		kDialogFlagsUseThemeBackground 	+ kDialogFlagsUseControlHierarchy + kDialogFlagsUseThemeControls
	};
};

resource 'ICN#' (kZoomControlPlain, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"FFFF FFE0 8004 0020 8004 0020 8004 00A0"
		$"8004 01A0 8004 03A0 8004 07A0 8004 8FA0"
		$"8045 DFA0 80E5 FFA0 89F5 FFA0 9FF5 FFA0"
		$"BFF5 FFA0 8004 0020 8004 0020",
		/* [2] */
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0"
	}
};

resource 'ICN#' (kZoomControlLeft, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"FFFF FFE0 FFFC 0020 FFFC 0020 FFFC 00A0"
		$"FFFC 01A0 FFFC 03A0 FFFC 07A0 FFFC 8FA0"
		$"FFBD DFA0 FF1D FFA0 F60D FFA0 E00D FFA0"
		$"C00D FFA0 FFFC 0020 FFFC 0020",
		/* [2] */
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0"
	}
};

resource 'ICN#' (kZoomControlRight, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"FFFF FFE0 8007 FFE0 8007 FFE0 8007 FF60"
		$"8007 FE60 8007 FC60 8007 F860 8007 7060"
		$"8046 2060 80E6 0060 89F6 0060 9FF6 0060"
		$"BFF6 0060 8007 FFE0 8007 FFE0",
		/* [2] */
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0 FFFF FFE0"
		$"FFFF FFE0 FFFF FFE0 FFFF FFE0"
	}
};


resource 'ICN#' (kPageControlPlain, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"3F9F 83F8 4010 C004 8410 A042 8C10 F062"
		$"9F10 11F2 BF10 11FA 9F10 11F2 8C10 1062"
		$"8410 1042 4010 1004 3F9F F3F8",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};

resource 'ICN#' (kPageControlRight, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"3F9F 83F8 4010 C3FC 8410 A3BE 8C10 F39E"
		$"9F10 120E BF10 1206 9F10 120E 8C10 139E"
		$"8410 13BE 4010 13FC 3F9F F3F8",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};

resource 'ICN#' (kPageControlLeft, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"3F9F 83F8 7F90 C004 FB90 A042 F390 F062"
		$"E090 11F2 C090 11FA E090 11F2 F390 1062"
		$"FB90 1042 7F90 1004 3F9F F3F8",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};

// ------------------------------------------------------------------------------------
// Tool information
// ------------------------------------------------------------------------------------
resource 'MENU' (kGXToolMenu, "") {
	kGXToolMenu, textMenuProc,
	AllItems,
	enabled, "",
	{
		" ", kSelectionTool, nokey, nomark, plain;
		" ", kRedMarkerTool, nokey, nomark, plain;
	}
};

data 'crsr' (kIconBase + kRedMarkerTool) {
	$"8001 0000 0060 0000 0092 0000 0000 0000"            /* �....`...�...... */
	$"0000 0000 0004 001E 002F 0046 008A 0114"            /* ........./.F.�.. */
	$"0228 0450 08A0 1540 2280 3500 3A00 7C00"            /* .(.P.�.@"�5.:.|. */
	$"E000 0000 001E 003F 007F 00FF 01FF 03FE"            /* �......?...�.�.� */
	$"07FC 0FF8 1FF0 3FE0 7FC0 7F80 7F00 3E00"            /* .�.�.�?�.�.�..>. */
	$"7C00 E000 000F 0000 0000 0000 0000 0000"            /* |.�............. */
	$"0000 0000 8004 0000 0000 0010 0010 0000"            /* ....�........... */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0002 0001 0002 0000 0000 0000 00D2 0000"            /* .............�.. */
	$"0000 0000 0030 0000 03FC 0000 0DFF 0000"            /* .....0...�..��.. */
	$"357C 0000 D5DC 0003 5770 000D 5DC0 0035"            /* 5|..��..Wp.�]�.5 */
	$"7700 00D5 DC00 0377 7000 0D5D C000 0F77"            /* w..��..wp.�]�..w */
	$"0000 0FDC 0000 3FF0 0000 FC00 0000 0000"            /* ...�..?�..�..... */
	$"0000 0000 0000 0000 0002 0000 FFFF FFFF"            /* ............���� */
	$"FFFF 0001 DDDD 0000 0000 0003 0000 0000"            /* ��..��.......... */
	$"0000"                                               /* .. */
};

data 'cicn' (kIconBase + kRedMarkerTool) {
	$"0000 0000 8008 0000 0000 0020 0020 0000"            /* ....�...... . .. */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0002 0001 0002 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0004 0000 0000 0020 0020"            /* ............. .  */
	$"0000 0000 0004 0000 0000 0020 0020 0000"            /* ........... . .. */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 1E00 0000 3F00 0000 7F00 0000"            /* ........?....... */
	$"FF00 0001 FF00 0003 FE00 0007 FC00 000F"            /* �...�...�...�... */
	$"F800 001F F000 003F E000 007F C000 007F"            /* �...�..?�...�... */
	$"8000 007F 0000 00FE 0000 01FC 0000 01E0"            /* �......�...�...� */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0400 0000 1E00 0000 2F00 0000"            /* ............/... */
	$"4600 0000 8A00 0001 1400 0002 2800 0004"            /* F...�.......(... */
	$"5000 0008 A000 0015 4000 0022 8000 0035"            /* P...�...@.."�..5 */
	$"0000 003A 0000 007C 0000 00E0 0000 0000"            /* ...:...|...�.... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0002 0000 FFFF FFFF"            /* ............���� */
	$"FFFF 0001 DDDD 0000 0000 0003 0000 0000"            /* ��..��.......... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0030 0000 0000 0000 03FC"            /* .......0.......� */
	$"0000 0000 0000 0DFF 0000 0000 0000 357C"            /* ......��......5| */
	$"0000 0000 0000 D5DC 0000 0000 0003 5770"            /* ......��......Wp */
	$"0000 0000 000D 5DC0 0000 0000 0035 7700"            /* .....�]�.....5w. */
	$"0000 0000 00D5 DC00 0000 0000 0377 7000"            /* .....��......wp. */
	$"0000 0000 0D5D C000 0000 0000 0F77 0000"            /* ....�]�......w.. */
	$"0000 0000 0FDC 0000 0000 0000 3FF0 0000"            /* .....�......?�.. */
	$"0000 0000 FC00 0000 0000 0000 0000 0000"            /* ....�........... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000"                                               /* .. */
};

data 'cicn' (kIconBase + kSelectionTool) {
	$"0000 0000 8004 0000 0000 0020 0020 0000"            /* ....�...... . .. */
	$"0000 0000 0000 0048 0000 0048 0000 0000"            /* .......H...H.... */
	$"0001 0001 0001 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0004 0000 0000 0020 0020"            /* ............. .  */
	$"0000 0000 0004 0000 0000 0020 0020 0000"            /* ........... . .. */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0001 0000 0003 8000 0003"            /* ............�... */
	$"8000 0003 8000 0003 8000 003F F800 007F"            /* �...�...�..?�... */
	$"FC00 003F F800 0003 8000 0003 8000 0003"            /* �..?�...�...�... */
	$"8000 0003 8000 0001 0000 0000 0000 0000"            /* �...�........... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0001 0000 0001"            /* ................ */
	$"0000 0001 0000 0001 0000 0001 0000 003F"            /* ...............? */
	$"F800 0001 0000 0001 0000 0001 0000 0001"            /* �............... */
	$"0000 0001 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0001 0000 FFFF FFFF"            /* ............���� */
	$"FFFF 0001 0000 0000 0000 0000 0000 0000"            /* ��.............. */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0001 0000 0001 0000 0001 0000 0001"            /* ................ */
	$"0000 0001 0000 003F F800 0001 0000 0001"            /* .......?�....... */
	$"0000 0001 0000 0001 0000 0001 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000"                           /* .......... */
};

