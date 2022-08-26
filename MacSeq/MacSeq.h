/*
 *  MacSeq.h
 *  MacSeq
 *
 *  Created by Dave Milici on Sat Feb 14 2004.
 *  Copyright (c) 2004 Ironic Research Labs. All rights reserved.
 *
 */

extern Boolean bCancelSequence;
extern Boolean bRunningSequence;
extern Boolean bRecordScript;

OSStatus MacSeqSetDefaultText(WindowPtr pWindow, WindowDataPtr pData);
OSStatus MacSeqInsertText(WindowPtr pWindow, WindowDataPtr pData, Str255 pStr);
OSStatus MacSeqExtractText(WindowDataPtr pData, char **pText, short *pSize, short *pNumLines, short **pLineStarts);

int ConfigSerialPort(void);
int InitSerialPort(void);
int ExitSerialPort(void);
int UpdateSerialPort(char c);

void QueryThingM(void);

void DoQueryCancelDialog(void);
void DoRunCamera(void);
void DoRunProjector(void);
void DoSequencerDialog(void);
void DoRunScript(void);
void DoRunScriptLoop(void);
void DoRecordOption(void);
void DoSerialDialog(void);
void DoKeypadDialog(void);
void DoFrameCounterDialog(void);


 
