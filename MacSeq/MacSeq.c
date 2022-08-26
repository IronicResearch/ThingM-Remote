/*
 *  MacSeq.c
 *  MacSeq
 *
 *  Created by Dave Milici on Sat Feb 14 2004.
 *  Copyright (c) 2004 Ironic Research Labs. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

#include "MacIncludes.h"
#include "SimpleText.h"
#include "MacSeq.h"
#include "SerialPortIO.h"
#include <sysexits.h>

#define DEBUG_CONSOLE 		0

/* ---------------------------------------------------------------------- */

enum {
    CAM,PRJ,AER,SYN,
    FRM,CTR,ALT,SEQ,
    EXP,SPD,LPS,VSH,
    OUT,WDG,STP,SKP
    };

typedef struct recSeq {
    int  index;
    char key[4];
    char command[16];
    int  entries;
    int  type;
    };
    
struct recSeq cmd[] = {
    {CAM,"CAM","A",0,'0'},
    {PRJ,"PRJ","B",0,'0'},
    {AER,"AER","C",0,'0'},
    {SYN,"SYN","D%c\r",1,'b'},
    {FRM,"FRM","E%d\rR",1,'i'},
    {CTR,"CTR","F%d\r",1,'i'},
    {ALT,"ALT","G%d\r%d\r",2,'i'},
    {SEQ,"SEQ","H%c\r",1,'b'},
    {EXP,"EXP","PA%d.%d\r",1,'d'},
    {SPD,"SPD","PB%d.%d\r",1,'d'},
    {LPS,"LPS","PC%d.%d\r",1,'d'},
    {VSH,"VSH","PD%d\r",1,'i'},
    {OUT,"OUT","PE%d\rR",1,'i'},
    {WDG,"WDG","PF%d.%d\r",1,'d'},
    {STP,"STP","PG%d\r%d\r",2,'i'},
    {SKP,"SKP","PH%d\r%d\r",2,'i'}
    };
    
/* ---------------------------------------------------------------------- */

Boolean bCancelSequence = false;
Boolean bRunningSequence = false;
Boolean bRecordScript = true;
Boolean bSequencerOn = false;
Boolean bLoopContinuous = false;
int	iCameraCounter = 0;
int	iProjectorCounter = 0;
int     iLoopCount = 1;

DialogPtr	theModelessDialog = NULL;
ModalFilterUPP	uppModelessFilter = NULL;
void UpdateModelessFrameCounterDialog(void);

/* ---------------------------------------------------------------------- */

Boolean bSerialEnabled = false;
Boolean bModemPort = true;
Str255 szPortOut = "\p.AOut";
short int iPortRefNum = 0;

/* ---------------------------------------------------------------------- */

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
    int r;
    
    // Classic serial port names are passed to OS X SerialPortIO
    r = InitSio(szPortOut);
    bSerialEnabled = (r == EX_OK);
    if (bSerialEnabled) {
        QueryThingM();
        return 1;	// success	
        }
        
Error:
    // Popup Serial port error message
    NoteAlert(kSerialDetectWindowID, 0);	
    return 0;	// failure	
}

/* ---------------------------------------------------------------------- */

int ExitSerialPort(void)
{
    if (!bSerialEnabled)
        return 0;
        
    ExitSio();
    bSerialEnabled = false;
    return 1;	
}

/* ---------------------------------------------------------------------- */

int OutCmd(char* str)
{
    if (!bSerialEnabled)
        return 0;
        
#if DEBUG_CONSOLE
    fprintf(stdout,"OutSio:  %s\n", str);
#endif

    // OutSio() should always return immediately since outgoing
    // characters will be batched up into IOCTL write buffer.
    OutSio(str);
    return 1;	
}

/* ---------------------------------------------------------------------- */

int InStat(char* str)
{
    int r;

    if (!bSerialEnabled)
        return 0;
        
    // InSio() will return immediately with 0 bytes return value
    // if no characters are ready to be read from IOCTL read buffer.
    // Otherwise it will return with the number of bytes actially read
    // from the IOCTL buffer. The read string will *usually* be
    // completed with an end-of-line character, like LF or CR, 
    // for typical modem style communications.
    r = InSio(str);
    if (r <= 0)
        return 0;
#if DEBUG_CONSOLE
    fprintf(stdout,"InSio:  %s\n", str);
#endif
    return 1;	
}

/* ---------------------------------------------------------------------- */

int WaitReply(char *msg, char *str)
{
//    char str[255];
    
    if (!bSerialEnabled)
        return 0;
        
    // Call InStat() wrapper to check on InSio() status.    
    // This should fall through to exit if nothing to read.
    while (InStat(str)) {
        // Check if we have a match for the current string.
        // All ThingM replies are EOL terminated, like modem.
        if (strstr(str,msg))
            return 1;
        // Update modeless dialog one tick.
        if (theModelessDialog != NULL)
            UpdateModelessFrameCounterDialog();
        }
    return 0;	
}

/* ---------------------------------------------------------------------- */

void QueryThingM(void)
{
    char str[16];
    
    sprintf(str, cmd[CAM].command);
    OutCmd(str);
    WaitReply("CAM", str);
    sscanf(str, "CAM%d", &iCameraCounter);
    
    sprintf(str, cmd[PRJ].command);
    OutCmd(str);
    WaitReply("PRJ", str);
    sscanf(str, "PRJ%d", &iProjectorCounter);

    sprintf(str, cmd[SEQ].command, ' ');
    OutCmd(str);
    WaitReply("SEQ", str);
    if (strstr(str, "ON")) 
        bSequencerOn = true;
    else 
        bSequencerOn = false;
}

/* ---------------------------------------------------------------------- */

void DoCameraFrame(void)
{
    char str[16];
    
    OutCmd(cmd[CAM].command);
    sprintf(str, cmd[FRM].command, 1);
    OutCmd(str);
    WaitReply("RUN", str);
    WaitReply("OK", str);
    iCameraCounter++;
}

/* ---------------------------------------------------------------------- */

void DoProjectorFrame(void)
{
    char str[16];
    
    OutCmd(cmd[PRJ].command);
    sprintf(str, cmd[FRM].command, 1);
    OutCmd(str);
    WaitReply("RUN", str);
    WaitReply("OK", str);
    iProjectorCounter++;
}


/* ---------------------------------------------------------------------- */

void DoSequencerOnOff(Boolean On)
{
    char str[16];
    
    sprintf(str, cmd[SEQ].command, ' ');
    OutCmd(str);
    WaitReply("SEQ", str);
    bSequencerOn = On;
    if (bSequencerOn && strstr(str, "OFF")) {
        sprintf(str, cmd[SEQ].command, '-');
        WaitReply("ON", str);
        }
    else if (!bSequencerOn && strstr(str, "ON")) {
        sprintf(str, cmd[SEQ].command, '-');
        WaitReply("OFF", str);
        }
}

/* ---------------------------------------------------------------------- */

void DoRunCamera(void)
{
    if (bSequencerOn) {
        if (bRecordScript)
            InsertScriptText("\pSEQ OFF\r");
        DoSequencerOnOff(false);
        }

    if (bRecordScript)
        InsertScriptText("\pCAM FRM 1\r");

    DoCameraFrame();
}

/* ---------------------------------------------------------------------- */

void DoRunProjector(void)
{
    if (bSequencerOn) {
        if (bRecordScript)
            InsertScriptText("\pSEQ OFF\r");
        DoSequencerOnOff(false);
        }

    if (bRecordScript)
        InsertScriptText("\pPRJ FRM 1\r");

    DoProjectorFrame();
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

static pascal Boolean
AppDialogModalFilterProc (DialogRef theDialog,
        EventRecord *theEvent,
	DialogItemIndex *itemHit)
{
	Boolean	handled = false;
        WindowPtr	theWindow;
        short		windowCode = MacFindWindow (theEvent->where, &theWindow);
	DialogPtr	theDialogWindow = GetDialogFromWindow( theWindow );
	
        // TODO: Dialogs should be able to be dragged
        // TODO: Other app windows should be able to be updated
        // TODO: Dialogs need to have Enter key handled for default item

        // What kind of event was it?
        switch (theEvent->what)
        {
            case mouseDown:
            {
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
                    // Is it the Dialog Window?
                    if ((WindowPtr) theDialogWindow == (WindowPtr) theDialog)
                    {
                        DragWindow (theWindow, theEvent->where, &limitRect);
                    }
                    handled = true;
                }
                break;
            }
            case updateEvt:
            {	
                // Is it the Dialog Window?
                if ((WindowPtr) theDialogWindow == (WindowPtr) theDialog)
                {
                    BeginUpdate ((WindowPtr) theDialogWindow);
                    EndUpdate ((WindowPtr) theDialogWindow);
                    handled = true;
                }
                break;
            }
            case keyDown:
            {
                // Is it the Dialog Window?
                if ((WindowPtr) theDialogWindow == (WindowPtr) theDialog)
                {
                    char	theChar = theEvent->message & charCodeMask;
                    if (theChar == kReturnCharCode || theChar == kEnterCharCode) {
                        *itemHit = 1;
                        handled = true;
                        }
                    else if (theChar == kEscapeCharCode) {
                        *itemHit = 2;
                        handled = true;
                        }
                }
                break;
            }
        }
        return (handled);
}

/* ---------------------------------------------------------------------- */

void DoFrameCounterDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Handle      	itemCamCnt, itemPrjCnt;
	DialogItemType	kind;
	Rect       	box;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;
	Str255		szPText;
	char		szCText[255];
	int		iTemp1, iTemp2;
        ModalFilterUPP	uppFilter = NULL;
        ControlRef	itemAsControl;
	
        // SimpleText general movable modal dialog handler
        BeginMovableModal();

        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kFrameCounterWindowID, nil, (WindowPtr)-1L);

	// Do the frame counter dialog
	SetDialogDefaultItem(theDialog, 1);
	SetDialogCancelItem(theDialog, 2);
	GetDialogItem(theDialog, 4, &kind, &itemCamCnt, &box);
	GetDialogItem(theDialog, 6, &kind, &itemPrjCnt, &box);
	GetDialogItemAsControl(theDialog, 4, &itemAsControl);
	sprintf((char*)&szPText[1], "%d", iCameraCounter);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText((Handle)itemAsControl, szPText);
	SelectDialogItemText(theDialog, 4, 0, 32767);
	sprintf((char*)&szPText[1], "%d", iProjectorCounter);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemPrjCnt, szPText);
	iTemp1 = iCameraCounter;
	iTemp2 = iProjectorCounter;
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
//		ModalDialog(uppFilter, &itemHit);
		MovableModalDialog(nil, &itemHit);
		switch (itemHit) {
		case 1:	// OK
                    bOK = true;
		case 2:	// Cancel
		    bQuitDialog = true;
		    break;
		case 4:	// Camera Counter
		    GetDialogItemText(itemCamCnt, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp1);
                    break;
		case 6:	// Projector Counter
		    GetDialogItemText(itemPrjCnt, szPText);
		    LocalCopyPascalStringToC(szPText, szCText);
		    sscanf(szCText, "%d", &iTemp2);
                    break;
		}
		
	}
	while (!bQuitDialog);
	DisposeDialog(theDialog);
        DisposeModalFilterUPP(uppFilter);
	
        // SimpleText general movable modal dialog handler
        EndMovableModal();
        
        if (bOK) {
            iCameraCounter = iTemp1;
            iProjectorCounter = iTemp2;
            // Update script
            if (bRecordScript) {
                sprintf((char*)&szPText[0], "\pCAM CTR %d\r\0", iCameraCounter);
                szPText[0] = strlen((char*)&szPText[1]);
                InsertScriptText(szPText);
                sprintf((char*)&szPText[0], "\pPRJ CTR %d\r\r\0", iProjectorCounter);
                szPText[0] = strlen((char*)&szPText[1]);
                InsertScriptText(szPText);
                }
            // Update ThingM
            OutCmd(cmd[CAM].command);
            WaitReply("CAM", szCText);
            sprintf(szCText, cmd[CTR].command, iCameraCounter);
            OutCmd(szCText);
            WaitReply("OK", szCText);
            OutCmd(cmd[PRJ].command);
            WaitReply("PRJ", szCText);
            sprintf(szCText, cmd[CTR].command, iProjectorCounter);
            OutCmd(szCText);
            WaitReply("OK", szCText);
            }
}

/* ---------------------------------------------------------------------- */

void BeginModelessFrameCounterDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Handle      	itemCamCnt, itemPrjCnt;
	DialogItemType	kind;
	Rect       	box;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;
	Str255		szPText;
	char		szCText[255];
	int		iTemp1, iTemp2;
        ModalFilterUPP	uppFilter = NULL;
        ControlRef	itemAsControl;
	
        // SimpleText general movable modal dialog handler
        BeginMovableModal();

        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kRunFrameCounterWindowID, nil, (WindowPtr)-1L);

	// Do the frame counter dialog
	SetDialogDefaultItem(theDialog, 1);
	SetDialogCancelItem(theDialog, 2);
	GetDialogItem(theDialog, 4, &kind, &itemCamCnt, &box);
	GetDialogItem(theDialog, 6, &kind, &itemPrjCnt, &box);
	sprintf((char*)&szPText[1], "%d", iCameraCounter);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemCamCnt, szPText);
	sprintf((char*)&szPText[1], "%d", iProjectorCounter);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemPrjCnt, szPText);
	iTemp1 = iCameraCounter;
	iTemp2 = iProjectorCounter;
	MacShowWindow(GetDialogWindow(theDialog));
        DrawDialog(theDialog);
        
        // Save all necessary globals for modeless dialog ops
        theModelessDialog = theDialog;
        uppModelessFilter = uppFilter;
}

/* ---------------------------------------------------------------------- */

void UpdateModelessFrameCounterDialog(void)
{
	short		itemHit;
	Handle      	itemCamCnt, itemPrjCnt;
	DialogItemType	kind;
	Rect       	box;
	Str255		szPText;
	char		szCText[255];
	int		iTemp1, iTemp2;
	EventRecord	event;
        
 	// Update frame counters
        GetDialogItem(theModelessDialog, 4, &kind, &itemCamCnt, &box);
	GetDialogItem(theModelessDialog, 6, &kind, &itemPrjCnt, &box);
	sprintf((char*)&szPText[1], "%d", iCameraCounter);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemCamCnt, szPText);
	sprintf((char*)&szPText[1], "%d", iProjectorCounter);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemPrjCnt, szPText);
        DrawDialog(theModelessDialog);

        // Single-pass "loop" for modeless dialog
        MovableModelessDialog(uppModelessFilter, &itemHit);
        //if (IsDialogEvent(&event)) {
            //if (DialogSelect(&event,&theModelessDialog,&itemHit)) {
		switch (itemHit) {
		case 1:	// OK
                    DoQueryCancelDialog();
                    break;
		case 2:	// Cancel
		    break;
		}
            //}
        //}
}

/* ---------------------------------------------------------------------- */

void EndModelessFrameCounterDialog(void)
{
        // Remove modeless dialog
	DisposeDialog(theModelessDialog);
        DisposeModalFilterUPP(uppModelessFilter);
        theModelessDialog = NULL;
        uppModelessFilter = NULL;
	
        // SimpleText general movable modal dialog handler
        EndMovableModal();
}

/* ---------------------------------------------------------------------- */

void DoQueryCancelDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;

        ModalFilterUPP	uppFilter = NULL;
	
        // SimpleText general movable modal dialog handler
        BeginMovableModal();

        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kSequencerQuitWindowID, nil, (WindowPtr)-1L);

	// Do the cancel sequence dialog
	SetDialogDefaultItem(theDialog, 1);
	SetDialogCancelItem(theDialog, 2);
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
//		ModalDialog(nil, &itemHit);
//		MovableModalDialog(uppFilter, &itemHit);
		MovableModalDialog(nil, &itemHit);
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
        
        // SimpleText general movable modal dialog handler
        EndMovableModal();
	
        if (bOK)
		bCancelSequence = true;
}

/* ---------------------------------------------------------------------- */

void DoSequencerDialog(void)
{
	short		itemHit;
	DialogPtr	theDialog;
	Handle      	itemCam, itemPrj, itemSeq;
	Handle      	itemFrmCnt, itemCamCnt, itemPrjCnt;
	DialogItemType	kind;
	Rect       	box;
	Boolean		bQuitDialog = false;
	Boolean		bOK = false;
	Str255		szPText;
	char		szCText[255];
	int		iTemp1, iTemp2, iTemp3;
	
	static int  	iSequencer = 3;
	static int  	iFrameCount = 0;
	static int	iCamStep = 1;
	static int	iPrjSkip = 1;
	
        ModalFilterUPP	uppFilter = NULL;
        ControlRef	itemAsControl;
	
        // SimpleText general movable modal dialog handler
        BeginMovableModal();

        uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
	theDialog = GetNewDialog(kSequencerWindowID, nil, (WindowPtr)-1L);

	// Do the sequencer dialog with radio buttons
	SetDialogDefaultItem(theDialog, 1);
	SetDialogCancelItem(theDialog, 2);
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
        // Auto-tabbing and highlighting via kDialogFlagsUseControlHierarchy
        // attribute must use GetDialogItemAsControl() for initial selection
	sprintf((char*)&szPText[1], "%d", iFrameCount);
	szPText[0] = strlen((char*)&szPText[1]);
	GetDialogItemAsControl(theDialog, 4, &itemAsControl);
	SetDialogItemText((Handle)itemAsControl, szPText);
        SelectDialogItemText(theDialog, 4, 0, 32767);
	sprintf((char*)&szPText[1], "%d", iCamStep);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemCamCnt, szPText);
	sprintf((char*)&szPText[1], "%d", iPrjSkip);
	szPText[0] = strlen((char*)&szPText[1]);
	SetDialogItemText(itemPrjCnt, szPText);
	iTemp1 = iFrameCount;
	iTemp2 = iCamStep;
	iTemp3 = iPrjSkip;
	MacShowWindow(GetDialogWindow(theDialog));
	do
	{
//		ModalDialog(uppFilter, &itemHit);
		MovableModalDialog(nil, &itemHit);
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
	
        // SimpleText general movable modal dialog handler
        EndMovableModal();

	// Run camera, projector, or sequencer
	if (bOK) {
                int i, j;
                iFrameCount = iTemp1;
                iCamStep = iTemp2;
                iPrjSkip = iTemp3;
                
                if (bRecordScript) {
                    switch (iSequencer) {
                        case 1:
                            if (bSequencerOn)
                                InsertScriptText("\pSEQ OFF\r");
                            sprintf((char*)&szPText[0], "\pCAM FRM %d\r\r\0", iFrameCount);
                            szPText[0] = strlen((char*)&szPText[1]);
                            InsertScriptText(szPText);
                            break;
                        case 2:
                            if (bSequencerOn)
                                InsertScriptText("\pSEQ OFF\r");
                            sprintf((char*)&szPText[0], "\pPRJ FRM %d\r\r\0", iFrameCount);
                            szPText[0] = strlen((char*)&szPText[1]);
                            InsertScriptText(szPText);
                            break;
                        case 3:
                            InsertScriptText("\pSEQ ON\r");
                            sprintf((char*)&szPText[0], "\pALT %d %d\r\0", iCamStep, iPrjSkip);
                            szPText[0] = strlen((char*)&szPText[1]);
                            InsertScriptText(szPText);
                            sprintf((char*)&szPText[0], "\pPRJ FRM %d\r\r\0", iFrameCount);
                            szPText[0] = strlen((char*)&szPText[1]);
                            InsertScriptText(szPText);
                            break;
                    }
                }

                // Display modeless frame counter dialog
                BeginModelessFrameCounterDialog();
		bCancelSequence = false;
                bRunningSequence = true;                
                switch (iSequencer) {
		case 1:	// Camera only
                        bSequencerOn = false;
                        sprintf(szCText, cmd[SEQ].command, ' ');
                        OutCmd(szCText);
                        WaitReply("SEQ", szCText);
                        if (strstr(szCText,"ON")) {
                            sprintf(szCText, cmd[SEQ].command, '-');
                            OutCmd(szCText);
                            WaitReply("OFF", szCText);
                            }
                        OutCmd(cmd[CAM].command);
                        sprintf(szCText, cmd[FRM].command, iFrameCount);
                        OutCmd(szCText);
                        for (i=0; i<iFrameCount; i++) {
                                if (i == 0)
                                    WaitReply("RUN", szCText);
                                else
                                    WaitReply("\r", szCText);
                                iCameraCounter++;
                                UpdateModelessFrameCounterDialog();
                                if (bCancelSequence)
					break;
				}
			break;
		case 2:	// Projector only
                        bSequencerOn = false;
                        sprintf(szCText, cmd[SEQ].command, ' ');
                        OutCmd(szCText);
                        WaitReply("SEQ", szCText);
                        if (strstr(szCText,"ON")) {
                            sprintf(szCText, cmd[SEQ].command, '-');
                            OutCmd(szCText);
                            WaitReply("OFF", szCText);
                            }
                        OutCmd(cmd[PRJ].command);
                        sprintf(szCText, cmd[FRM].command, iFrameCount);
                        OutCmd(szCText);
                        for (i=0; i<iFrameCount; i++) {
                                if (i == 0)
                                    WaitReply("RUN", szCText);
                                else
                                    WaitReply("\r", szCText);
                                iProjectorCounter++;
                                UpdateModelessFrameCounterDialog();
				if (bCancelSequence)
					break;
				}
			break;
		case 3:	// Camera:Projector Sequence
                        bSequencerOn = true;
                        sprintf(szCText, cmd[SEQ].command, ' ');
                        OutCmd(szCText);
                        WaitReply("SEQ", szCText);
                        if (strstr(szCText,"OFF")) {
                            sprintf(szCText, cmd[SEQ].command, '-');
                            OutCmd(szCText);
                            WaitReply("ON", szCText);
                            }
                        OutCmd(cmd[PRJ].command);
                        sprintf(szCText, cmd[FRM].command, iFrameCount);
                        OutCmd(szCText);
			for (i=0; i<iFrameCount; i++) {
			    for (j=0; j<iCamStep; j++) {
                                if (i == 0)
                                    WaitReply("RUN", szCText);
                                else    
                                    WaitReply("CAM", szCText);
                                iCameraCounter++;
                                UpdateModelessFrameCounterDialog();
                                if (bCancelSequence)
                                    break;
                                }
			    for (j=0; j<iPrjSkip; j++) {
                                WaitReply("PRJ", szCText);
                                iProjectorCounter++;
                                UpdateModelessFrameCounterDialog();
                                if (bCancelSequence)
                                    break;
                                }
                            if (bCancelSequence)
                                break;
				}
			break;
			}
                bRunningSequence = false;
                // Remove modeless frame counter dialog
                EndModelessFrameCounterDialog();
 		}
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
    
    // SimpleText general movable modal dialog handler
    BeginMovableModal();

    uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
    theDialog = GetNewDialog(kSerialPortWindowID, nil, (WindowPtr)-1L);

    // Do the serial port dialog with radio buttons
    SetDialogDefaultItem(theDialog, 1);
    SetDialogCancelItem(theDialog, 2);
    GetDialogItem(theDialog, 3, &kind, &itemModem, &box);
    GetDialogItem(theDialog, 4, &kind, &itemPrinter, &box);
    if (bTempModemPort)
            SetControlValue((ControlHandle)itemModem, 1);
    else
            SetControlValue((ControlHandle)itemPrinter, 1);
    MacShowWindow(GetDialogWindow(theDialog));
    do
    {
//		ModalDialog(uppFilter, &itemHit);
            MovableModalDialog(nil, &itemHit);
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

    // SimpleText general movable modal dialog handler
    EndMovableModal();

    // Re-configure serial port
    if (bOK) {
        bModemPort = bTempModemPort;
        ExitSerialPort();
        ConfigSerialPort();
        InitSerialPort();
        }

}

/* ---------------------------------------------------------------------- */

void RunScript(int nNumLoops)
{
    char	*scriptfile = NULL;
    short	size = 0, lines = 0, *indexes = NULL;
    Str255	scriptline,field,field2;
    short	total, delta, n, s;
    char	*chptr;
    short	c, l, i;
    int		data1, data2;
    Str255	cmdbuf,statbuf;
    Boolean	scriptvalid = false;
    
    // Get TextEdit record for the current window and parse ThingM commands
    ExtractScriptText(&scriptfile, &size, &lines, &indexes);

    if (size == 0 || lines == 0 || scriptfile == NULL || indexes == NULL) {
 	NoteAlert(kScriptEmptyWindowID, 0);	
        return;
        }

    // Display modeless frame counter dialog
    BeginModelessFrameCounterDialog();
    bCancelSequence = false;

    total = 0;
    // Read script file line by line and translate fields to ThingM commands
    //while  (!feof(scriptfile)) {
    for (n = 0; n < nNumLoops; n++) { 
        for (l = 0; l < lines; l++) {
            // Update modeless frame counter dialog as often as possible
            UpdateModelessFrameCounterDialog();
            if (bCancelSequence)
                break;
            // Locate next line via start index
            s = indexes[l];
            //fscanf(scriptfile, "%s\n", &scriptline);
            //fscanf(scriptfile, "%[^\n]\n", &scriptline);
            scriptline[0] = '\0';
            sscanf(&scriptfile[s], "%[^\r]\r", &scriptline);
#if DEBUG_CONSOLE
            fprintf(stdout,"Script:  %s\n", scriptline);
#endif
            if (strlen(scriptline) == 0)
                continue;
            if (scriptline[0] == '*')
                continue;
            if (scriptline[0] == '\r')
                continue;
            if (scriptline[0] == '\n')
                continue;
            //sscanf(scriptline, "%s %s %d %d", &field, &field2, &value, &value2);
            // Parse script line fields incrementally for max format flexibility
            c = 0;
            chptr = &scriptline[0];
            while (sscanf(chptr, "%s", &field) == 1) {
                // Advance character pointer to next non-blank field
                while (*chptr != ' ')
                    chptr++;
                while (*chptr == ' ')
                    chptr++;
                // Search table for valid ThingM command key
                for (i = 0; i < 16; i++) {
                    if (strcmp(field, cmd[i].key) == 0) {
                        switch (cmd[i].type) {
                            case 'i': // integer
                                if (cmd[i].entries > 1) {
                                    sscanf(chptr, "%d %d", &data1 ,&data2);
                                    sprintf(cmdbuf, cmd[i].command, data1, data2);
                                    while (*chptr != ' ')
                                        chptr++;
                                    while (*chptr == ' ')
                                        chptr++;
                                    }
                                else {
                                    sscanf(chptr, "%d", &data1);
                                    sprintf(cmdbuf, cmd[i].command, data1);
                                    }
                                break;
                            case 'd': // decimal
                                sscanf(chptr, "%d.%d", &data1, &data2);
                                sprintf(cmdbuf, cmd[i].command, data1, data2);
                               break;
                            case 'b': // boolean
                                sscanf(chptr, "%s", &field2);
                                sprintf(cmdbuf, cmd[i].command, ' ');
                                OutCmd(cmdbuf);
                                WaitReply(cmd[i].key, statbuf);
                                // Toggle boolean command if response does not match script
                                if (!strstr(statbuf,field2)) {
                                    sprintf(cmdbuf, cmd[i].command, '-');
                                    }
                                break;
                            default:
                                strcpy(cmdbuf, cmd[i].command);
                            }
                        // Output ThingM command
                        OutCmd(cmdbuf);
                        // Wait for reply corresponding to command
                        if (cmd[i].type == '0')
                            WaitReply(cmd[i].key, statbuf);
                        else if (i != FRM && i != OUT)
                            WaitReply("OK", statbuf);
                        else {
                            WaitReply("RUN", statbuf);
                            while (InStat(statbuf)) {
                                if (strstr(statbuf,"CAM"))
                                    sscanf(statbuf,"CAM%d",&iCameraCounter);
                                else if (strstr(statbuf,"PRJ"))
                                    sscanf(statbuf,"PRJ%d",&iProjectorCounter);
                                UpdateModelessFrameCounterDialog();
                                if (bCancelSequence)
                                    break;
                                }
                            }
                        // Advance char ptr to next non-blank field
                        if (cmd[i].type != '0') {
                            while (*chptr != ' ')
                                chptr++;
                            while (*chptr == ' ')
                                chptr++;
                            }
                        scriptvalid = true;
                        break;
                        }
                    }
                }
            }
        // No reloop if no script commands found on 1st pass
        if (!scriptvalid)
            break;
        }

    // Remove modeless frame counter dialog
    EndModelessFrameCounterDialog();

    // Popup empty script alert if no commands found
    if (!scriptvalid)
	NoteAlert(kScriptEmptyWindowID, 0);	

    // Free allocated buffers passed back
    free(indexes);
    free(scriptfile);
}

/* ---------------------------------------------------------------------- */

void DoRunScript(void)
{
    RunScript(1);
}

/* ---------------------------------------------------------------------- */

void DoRunScriptLoop(void)
{
    short		itemHit;
    DialogPtr	theDialog;
    Handle      	itemCont, itemLoop, itemCount;
    ControlRef	itemAsControl;
    DialogItemType	kind;
    Rect        	box;
    Boolean     	bTemp = bLoopContinuous;
    int		iTemp = iLoopCount;
    Boolean		bQuitDialog = false;
    Boolean		bOK = false;
    ModalFilterUPP	uppFilter = NULL;
    Str255		szPText;
    char		szCText[255];
    
    // SimpleText general movable modal dialog handler
    BeginMovableModal();

    uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
    theDialog = GetNewDialog(kRunLoopWindowID, nil, (WindowPtr)-1L);

    // Display Loop Script dialog with radio buttons
    SetDialogDefaultItem(theDialog, 1);
    SetDialogCancelItem(theDialog, 2);
    GetDialogItem(theDialog, 3, &kind, &itemCont, &box);
    GetDialogItem(theDialog, 4, &kind, &itemLoop, &box);
    GetDialogItem(theDialog, 5, &kind, &itemCount, &box);
    GetDialogItemAsControl(theDialog, 5, &itemAsControl);
    if (bTemp)
        SetControlValue((ControlHandle)itemCont, 1);
    else
        SetControlValue((ControlHandle)itemLoop, 1);
    sprintf((char*)&szPText[1], "%d", iLoopCount);
    szPText[0] = (char)strlen((char*)&szPText[1]);
    SetDialogItemText((Handle)itemAsControl, szPText);
    MacShowWindow(GetDialogWindow(theDialog));
    do
    {
//		ModalDialog(uppFilter, &itemHit);
            MovableModalDialog(nil, &itemHit);
            switch (itemHit) {
            case 1:	// OK
                bOK = true;
            case 2:	// Cancel
                bQuitDialog = true;
                break;
            case 3:	// Continuous
                SetControlValue((ControlHandle)itemCont, 1);
                SetControlValue((ControlHandle)itemLoop, 0);
                bTemp = true;
                break;
            case 4:	// Loop Count
                SetControlValue((ControlHandle)itemCont, 0);
                SetControlValue((ControlHandle)itemLoop, 1);
                bTemp = false;
                break;
            case 5: // Loop Count
                GetDialogItemText(itemCount, szPText);
                LocalCopyPascalStringToC(szPText, szCText);
                sscanf(szCText, "%d", &iTemp);
                break;
            }
            
    }
    while (!bQuitDialog);
    DisposeDialog(theDialog);
    DisposeModalFilterUPP(uppFilter);

    // SimpleText general movable modal dialog handler
    EndMovableModal();

    // Run script 
    if (bOK) {
        bLoopContinuous = bTemp;
        iLoopCount = iTemp;
        if (bLoopContinuous)
            RunScript(9999);
        else
            RunScript(iLoopCount);
        }
}

/* ---------------------------------------------------------------------- */

void DoRecordOption(void)
{
    bRecordScript = !bRecordScript;
}

/* ---------------------------------------------------------------------- */

void DoKeypadDialog(void)
{
    short		itemHit,i;
    DialogPtr		theDialog;
    Handle      	item[16];
    Handle		itemKey;
    ControlRef		itemAsControl;
    DialogItemType	kind;
    Rect        	box;
    Boolean		bQuitDialog = false;
    Boolean		bOK = false;
    ModalFilterUPP	uppFilter = NULL;
    Str255		szPText;
    char		szCText[255];
    
    // SimpleText general movable modal dialog handler
    BeginMovableModal();

    uppFilter = NewModalFilterUPP(AppDialogModalFilterProc);
    theDialog = GetNewDialog(kKeyMapWindowID, nil, (WindowPtr)-1L);

    // Display Key Map dialog with radio buttons
    SetDialogDefaultItem(theDialog, 1);
    SetDialogCancelItem(theDialog, 2);
    for (i = 0; i < 8; i++) {
        GetDialogItem(theDialog, i+3, &kind, &item[i], &box);
        if (i == 0)
            SetControlValue((ControlHandle)item[i], 1);
        else
            SetControlValue((ControlHandle)item[i], 0);
        }
    GetDialogItem(theDialog, 13, &kind, &itemKey, &box);
    switch (cmd[0].type) {
        case 'i': strcpy(szCText, "integer"); break;
        case 'd': strcpy(szCText, "decimal"); break;
        case 'b': strcpy(szCText, "boolean"); break;
        default:  strcpy(szCText, "no data");
        }
    sprintf(szPText, "\p%c%c%c = %c, %d, %c (%s)", cmd[0].key[0], cmd[0].key[1], cmd[0].key[2], 
        cmd[0].command[0], cmd[0].entries, cmd[0].type, szCText);
    szPText[0] = strlen((char*)&szPText[1]);
    SetDialogItemText(itemKey, szPText);
    MacShowWindow(GetDialogWindow(theDialog));
    do
    {
        MovableModalDialog(nil, &itemHit);
        switch (itemHit) {
        case 1:	// OK
            bOK = true;
        case 2:	// Cancel
            bQuitDialog = true;
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            for (i = 0; i < 8; i++) {
                if (i+3 == itemHit)
                    SetControlValue((ControlHandle)item[i], 1);
                else
                    SetControlValue((ControlHandle)item[i], 0);
                }
            i = itemHit - 3;
            switch (cmd[i].type) {
                case 'i': strcpy(szCText, "integer"); break;
                case 'd': strcpy(szCText, "decimal"); break;
                case 'b': strcpy(szCText, "boolean"); break;
                default:  strcpy(szCText, "no data");
                }
            sprintf(szPText, "\p%c%c%c = %c, %d, %c (%s)", cmd[i].key[0], cmd[i].key[1], cmd[i].key[2], 
                cmd[i].command[0], cmd[i].entries, cmd[i].type, szCText);
            szPText[0] = strlen((char*)&szPText[1]);
            SetDialogItemText(itemKey, szPText);
            DrawDialog(theDialog);
            break;
        }
    }
    while (!bQuitDialog);
    DisposeDialog(theDialog);
    DisposeModalFilterUPP(uppFilter);

    // SimpleText general movable modal dialog handler
    EndMovableModal();
}

/* ---------------------------------------------------------------------- */
