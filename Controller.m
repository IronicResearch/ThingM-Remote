#import "Controller.h"
#include <SerialPortIO.h>
#include <sysexits.h>

EntryRec cmdTable[] = {
	{ CAM, INT, "CAM", "Camera" },
	{ PRJ, INT, "PRJ", "Projector" },
	{ AER, INT, "AER", "Aerial" },
	{ SYN, BIN, "SYN", "Sync Projectors" },
	{ FRM, INT, "FRM", "Frame Count" },
	{ CTR, INT, "CTR", "Counter Total" },
	{ ALT, INT, "ALT", "Alternate Sequence" },
	{ SEQ, BIN, "SEQ", "Sequencer" },
	{ STP, INT, "STP", "Step-Frame Sequence" },
	{ SKP, INT, "SKP", "Skip-Frame Sequence" },
	{ AL2, INT, "+TO", "Alternate Sequence" },
	{ SIO, CHR, "---", "ThingM Remote" },
	};

int		iFrameCount[3] = {1, 1, 1};
int		iCounterTotal[3] = {0, 0, 0};
bool	bSequencer = false;
bool	bSyncProj = false;
int		iAltSeq[2] = {1, 1};
int		iStepCam[2] = {1, 1};
int		iSkipPrj[2] = {1, 1};
bool	bRunning = false;
bool	bRunSeq = false;
bool	bRunHold = false;
int		iRunCount = 0;
int		iRunFrame[3] = {0, 0, 0};
char	inbuf[256] = {0};

@implementation Controller

- (IBAction)doClearKey:(id)sender
{
	ival = 0;
	icnt = 0;
	dp = false;
	dpcnt = 0;
	bRunning = false;
	[self display];
	
	// ThingM clear key
	OutSio("\b");
	InSio(inbuf);
	xval.sval = &inbuf[0];
}

- (IBAction)doDigitKey:(id)sender
{
	int x = [[sender selectedCell] tag];
	switch (x) {
		case 11: // minus
			if (type == BIN)
				ival = !ival;
			else
				ival = -ival;
			break;
		case 10: // decimal point
			if (type != DEC)
				break;
			dp = true;
			break;
		default:
			if (type == BIN)
				break;
			// ival = 10 * ival + x;
			if (icnt)
				ival *= 10;
			else
				ival = 0;
			ival += x;	
			icnt++;
		}
	[self display];
	
	// ThingM number key
	switch (x) {
	case 11: OutSio("-"); break;
	case 10: OutSio("."); break;
	case 9: OutSio("9"); break;
	case 8: OutSio("8"); break;
	case 7: OutSio("7"); break;
	case 6: OutSio("6"); break;
	case 5: OutSio("5"); break;
	case 4: OutSio("4"); break;
	case 3: OutSio("3"); break;
	case 2: OutSio("2"); break;
	case 1: OutSio("1"); break;
	case 0: OutSio("0"); break;
	}
	InSio(inbuf);
}

- (IBAction)doEnterKey:(id)sender
{
	switch (fnidx) {
		case FRM: iFrameCount[pridx]   = ival; break;
		case CTR: iCounterTotal[pridx] = ival; break;
		case SYN: bSyncProj  = ival; break;
		case SEQ: bSequencer = ival; break;
		case ALT: iAltSeq[0] = ival; break;
		}
	icnt = 0;
	[self display];
	
	// ThingM Enter key
	OutSio("\r");
	InSio(inbuf);
}

- (IBAction)doFunctionKey:(id)sender
{
	int fkey = [[sender selectedCell] tag];
	switch (fkey) {
		case 1: fnidx = CAM; break;
		case 2: fnidx = PRJ; break;
		case 3: fnidx = AER; break;
		case 4: fnidx = SYN; break;
		case 5: fnidx = FRM; break;
		case 6: fnidx = CTR; break;
		case 7: fnidx = ALT; break;
		case 8: fnidx = SEQ; break;
		}
	switch (fnidx) {
		case CAM:
		case PRJ:
		case AER: ival = iCounterTotal[pridx = fnidx]; break;
		case FRM: ival = iFrameCount[pridx]; break;
		case CTR: ival = iCounterTotal[pridx]; break;
		case SYN: ival = bSyncProj; break;
		case SEQ: ival = bSequencer; break;
		case ALT: ival = iAltSeq[0]; break;
		}
	type = cmdTable[fnidx].type;
	icnt = 0;
	[self display];
	
	// ThingM Function key
	switch (fkey) {
	case 1: OutSio("A"); break;
	case 2: OutSio("B"); break;
	case 3: OutSio("C"); break;
	case 4: OutSio("D"); break;
	case 5: OutSio("E"); break;
	case 6: OutSio("F"); break;
	case 7: OutSio("G"); break;
	case 8: OutSio("H"); break;
	}
	InSio(inbuf);
}

- (IBAction)doProgramKey:(id)sender
{
	// Escape?
	bRunning = false;
	
	// 2nd Program Key?
	fnidx = SIO;
	
	// ThingM Program command
	OutSio("P");
	InSio(inbuf);
	xval.sval = &inbuf[0];
}

- (IBAction)doRunKey:(id)sender
{
	// Load run frame count per selection
	if (!bRunning) {
		iRunCount = abs(iFrameCount[pridx]);
		if (iRunCount == 0)
			return;
		iRunFrame[CAM] = (iFrameCount[CAM] < 0) ? -1 : 1;
		iRunFrame[PRJ] = (iFrameCount[PRJ] < 0) ? -1 : 1;
		iRunFrame[AER] = (iFrameCount[AER] < 0) ? -1 : 1;	
		bRunSeq = false;
		bRunning = true;
		bRunHold = false;
		}
		
	// Start or Stop timer per Run toggle
	if (!timer) {
		timer = [NSTimer scheduledTimerWithTimeInterval:0.25
					target:self 
					selector:@selector(timerTick:)
					userInfo:0
					repeats:YES];
		bRunHold = false;
		}
	else {
		[timer invalidate];
		timer = nil;
		bRunHold = true;
		}
		
	// ThingM Run command
	OutSio("R");
}

- (IBAction)timerTick:(id)sender
{
	// Update run frame count until done
	if (iRunCount <= 0) {
		bRunning = false;
		if (timer) {
			[timer invalidate];
			timer = nil;
			}
		return;
		}
	
	// Update total counters
	if (bSequencer) {
		// Sequence Camera vs Projector(s)
		bRunSeq = !bRunSeq;
		if (bRunSeq) {
			iCounterTotal[CAM] += iRunFrame[CAM];
			if (pridx == CAM)
				iRunCount--;
			}
		else {
			if (bSyncProj) {
				iCounterTotal[PRJ] += iRunFrame[PRJ];
				iCounterTotal[AER] += iRunFrame[AER];
				}
			else if (pridx == AER)
				iCounterTotal[AER] += iRunFrame[AER];
			else
				iCounterTotal[PRJ] += iRunFrame[PRJ];
			if (pridx != CAM)
				iRunCount--;
			}
		}
	else {
		// Run Camera or Projector(s)
		iRunCount--;
		switch (pridx) {
			case CAM: 
				iCounterTotal[CAM] += iRunFrame[CAM]; 
				break;
			case PRJ: 
				iCounterTotal[PRJ] += iRunFrame[PRJ]; 
				if (bSyncProj)
					iCounterTotal[AER] += iRunFrame[AER];
				break;
			case AER: 
				iCounterTotal[AER] += iRunFrame[AER];
				if (bSyncProj)
					iCounterTotal[PRJ] += iRunFrame[PRJ];
				break;
			}
		}
	[self display];
}

- (void)display
{
	id s;
	id a,b,c,d,e,f;
	
	a = [NSString stringWithFormat:@"%s%6d", cmdTable[CAM].prompt, iCounterTotal[CAM]];
	b = [NSString stringWithFormat:@"%s%6d", cmdTable[PRJ].prompt, iCounterTotal[PRJ]];
	c = [NSString stringWithFormat:@"%s%6d", cmdTable[AER].prompt, iCounterTotal[AER]];
	d = [NSString stringWithFormat:@"%s %6s", cmdTable[SEQ].prompt, bSequencer ? "ON" : "OFF"];
	e = [NSString stringWithFormat:@"%s %6s", cmdTable[SYN].prompt, bSyncProj  ? "ON" : "OFF"];
	switch (cmdTable[fnidx].type) {
		case INT: f = [NSString stringWithFormat:@"%-29s %s %6d",    cmdTable[fnidx].hint, cmdTable[fnidx].prompt, ival]; break;
		case DEC: f = [NSString stringWithFormat:@"%-29s %s %3d.%2d",cmdTable[fnidx].hint, cmdTable[fnidx].prompt, ival / 100, ival % 100]; break;
		case BIN: f = [NSString stringWithFormat:@"%-29s %s %6s",    cmdTable[fnidx].hint, cmdTable[fnidx].prompt, ival ? " ON" : "OFF"]; break;
		case CHR: f = [NSString stringWithFormat:@"%-29s %s",		 cmdTable[fnidx].hint, (xval.sval) ? xval.sval : "" ]; break;
		}
	s = [NSString stringWithFormat:@"%s %s %s %s\r%s", 
		[a UTF8String], [b UTF8String],	[c UTF8String], [d UTF8String], [f UTF8String]];
	
	[outLCD setStringValue: s];
}

@end

@implementation Controller(ApplicationNotifications)
-(void)applicationDidFinishLaunching:(NSNotification*)notification
{
	// Init controller state vars and display
	[self doClearKey:self];
	
	// Init serial interface connected to ThingM
	if (InitSio(".AOut") != EX_OK)
		NSBeep();
}
@end
