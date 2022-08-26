// Must be exlicitly included in C files using Project Builder;
// Otherwise generates build error if specified as prefix file... (DaveM)

#define TARGET_OS_MAC 1
#define TARGET_OS_WIN32 0
#define TARGET_API_MAC_CARBON 1

// Define for using stand-alone vs QuickTime Framework app (DaveM)
#define USE_QTFRAMEWORK 1

// Include additional defines from QuickTime Framework too (DaveM)
#if USE_QTFRAMEWORK
#include "MacPrefix.h"
#endif