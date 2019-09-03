#import "Controller.h"

EntryRec cmdTable[] = {
	{ CAM, INT, "CAM", "Camera" },
	{ PRJ, INT, "PRJ", "Projector" },
	{ AER, INT, "AER", "Aerial" },
	{ SYN, BIN, "SYN", "Sync Projectors" },
	{ FRM, INT, "FRM", "Frame Count" },
	{ CTR, INT, "CTR", "Counter Total" },
	{ ALT, INT, "ALT", "Alternate Sequence" },
	{ SEQ, BIN, "SEQ", "Sequencer" }
	};

int		iFrameCount[3];
int		iCounterTotal[3];
bool	bSequencer = false;
bool	bSyncProj = false;
int		iAltSeq[2];
int		iStepCam[2];
int		iSkipPrj[2];
bool	bRunning = false;
bool	bRunSeq = false;
int		iRunCount;
int		iRunFrame[3];

@implementation Controller

- (IBAction)doClearKey:(id)sender
{
	ival = 0;
	dp = false;
	dpcnt = 0;
	[self display];
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
			ival = 10 * ival + x;
			if (dp)
				dpcnt++;
		}
	[self display];
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
	[self display];
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
	[self display];
}

- (IBAction)doProgramKey:(id)sender
{
	// Escape?
	bRunning = false;
	
	// 2nd Program Key?
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
		}
		
	// Start or Stop timer per Run toggle
	if (!timer) {
		timer = [NSTimer scheduledTimerWithTimeInterval:1.0
					target:self 
					selector:@selector(timerTick:)
					userInfo:0
					repeats:YES];
		}
	else {
		[timer invalidate];
		timer = nil;
		}
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
		case DEC: f = [NSString stringWithFormat:@"%-29s %s %3d.2d", cmdTable[fnidx].hint, cmdTable[fnidx].prompt, ival / 100, ival % 100]; break;
		case BIN: f = [NSString stringWithFormat:@"%-29s %s %6s",    cmdTable[fnidx].hint, cmdTable[fnidx].prompt, ival ? " ON" : "OFF"]; break;
		}
	s = [NSString stringWithFormat:@"%s %s %s %s\r%s", 
		[a UTF8String], [b UTF8String],	[c UTF8String], [d UTF8String], [f UTF8String]];
	
	[outLCD setStringValue: s];
}

@end
