//////////
//
//	File:		MacPrefix.h
//
//	Contains:	Prefix file for our Macintosh projects.
//
//	Written by:	Tim Monroe
//
//	Copyright:	� 1999 by Apple Computer, Inc., all rights reserved.
//
//	Change History (most recent first):
//	   
//	   <1>	 	11/11/99	rtm		first file
//
//////////

#ifndef __Prefix_File__
#define __Prefix_File__


//////////
//
// header files
//
//////////

#define TARGET_API_MAC_CARBON			1

#include <CarbonCore/ConditionalMacros.h>


//////////
//
// compiler macros
//
//////////

#ifndef PASCAL_RTN
#define PASCAL_RTN						pascal
#endif

#if TARGET_CPU_PPC
#define SOUNDSPROCKET_AVAIL				1
#else
#define SOUNDSPROCKET_AVAIL				0
#endif

#endif // __Prefix_File__