/* Controller */

#import <Cocoa/Cocoa.h>

typedef enum {
	INT,
	DEC,
	BIN,
	CHR
	} EntryType;
	
typedef union {
	int		ival;
	float	fval;
	bool	bval;
	char*	sval;
	} EntryValue;
	
typedef enum {
	CAM,
	PRJ,
	AER,
	SYN,
	FRM,
	CTR,
	ALT,
	SEQ,
	STP,
	SKP,
	AL2,
	SIO,
	} Function;
	
typedef struct {
	Function	fn;
	EntryType	type;
	char		prompt[4];
	char		hint[30];
	} EntryRec;

@interface Controller : NSObject
{
    IBOutlet id outLCD;
	int			fnidx;
	int			pridx;
	EntryType	type;
	EntryValue	xval;
	int			ival;
	int			icnt;
	bool		dp;
	int			dpcnt;
	NSTimer*	timer;
	}

- (IBAction)doClearKey:(id)sender;
- (IBAction)doDigitKey:(id)sender;
- (IBAction)doEnterKey:(id)sender;
- (IBAction)doFunctionKey:(id)sender;
- (IBAction)doProgramKey:(id)sender;
- (IBAction)doRunKey:(id)sender;
- (IBAction)timerTick:(id)sender;
- (void)display;	
@end

@interface Controller(NSApplicationNotifications)
-(void)applicationDidFinishLaunching:(NSNotification*)notification;
@end
