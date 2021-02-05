/*
 *  SerialPortIO.h
 *  MacSeq
 *
 *  Created by Dave Milici on Sat Feb 14 2004.
 *  Copyright (c) 2004 Ironic Research Labs. All rights reserved.
 *
 */

int InitSio(char* portname);
int OutSio(char* outbuf);
int InSio(char* inbuf);
int ExitSio(void);

 
