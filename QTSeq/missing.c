//-----------------------------------------------------------------------
#include "carbonprefix.h"

#include "globals.h"
#include "common.h"

//-----------------------------------------------------------------------

void CreateMonitorWindow(void)
{
	gDialog = GetNewDialog (kMonitorDLOGID, NULL, (WindowPtr) -1L);
	
	gMonitor = GetDialogWindow(gDialog);
}

//-----------------------------------------------------------------------

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