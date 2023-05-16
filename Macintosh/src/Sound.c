/* 
	File Sound.c:  Sound routines for Gravity Well
	
	Copyright (c) 1986-88 Randall H. Wilson
	
	This approach to sound uses the Sound Driver, which is no longer
	supported by Apple.  It will probably work less and less over
	the years.  I need to switch to the Sound Manager.  As well as
	a newer version of Think C.  - Randy, 6/21/93
*/

//#include <VRetraceMgr.h>
//#include <SoundDvr.h>
#include <Sound.h>
#include "GW.h"

#define	HARDSNDBUF	(MemTop - 0x300)
#define	SNDBUFLEN	370

#define	NO_PRIOR	0	/* priorities of various sounds	*/
#define	FIRE_PRIOR	70
#define	EXP1_PRIOR	90	/* bunker explosion	*/
#define	THRU_PRIOR	35
#define	BUNK_PRIOR	40
#define	SOFT_PRIOR	30
#define	SHLD_PRIOR	70
#define	FUEL_PRIOR	80
#define	EXP2_PRIOR	100	/* ship explosion		*/
#define	EXP3_PRIOR	50	/* alien explosion	*/
#define	CRACK_PRIOR	92
#define	FIZZ_PRIOR	93
#define	ECHO_PRIOR	94

#define	FUELBEEPFREQ	26	/* frequency of beeps in fuel pickup */
#define	SHLD_FREQ	50	/* frequency of shield sound		*/

extern int shielding, thrusting, Screen_Type;


typedef struct 
{
	int mode;
	long count;
	char waveBytes[SNDBUFLEN*2];
} myFFSynthRec;
myFFSynthRec *synthptr;		/* buffer for calls to Mac II */
char *soundbuffer;		/* holds free form sound description */
char *sndswitch;		/* for turning sound on and off */

int soundlock;			/* true when changing current sound */
	/* do_sound does nothing in the rare event that soundlock is true */
int currentsound, priority;
int phase, freq;		/* for ship firing sound	*/
int period, amp, ampchange;	/* for bunk firing sound	*/
int brightlen, brightphase, brightamp, brightdir; /* for bright sounds	*/
int fuelcount;			/* for fuel sound		*/
int crackcount;			/* for mission complete		*/
int fizzcount;			/* for planet fizz-out		*/
int echocount;			/* for echoing away		*/

char sine_wave[256];

char expl_rands[128];
char thru_rands[128];
char hiss_rands[256];

VBLTask taskrec;

static int priorities[] = {NO_PRIOR, FIRE_PRIOR, EXP1_PRIOR, THRU_PRIOR,
			BUNK_PRIOR, SOFT_PRIOR, SHLD_PRIOR, FUEL_PRIOR,
			EXP2_PRIOR, EXP3_PRIOR, CRACK_PRIOR, FIZZ_PRIOR,
			ECHO_PRIOR };



open_sound()
{
	int level;
	
	sndswitch = *((char **) 0x1D4);
	GetSoundVol(&level);
	realsound = level > 0;
	
	if (realsound)
	{
		soundlock = 0;
		clear_sound();
		if (Screen_Type == MACHINE_PLUS)
		{
			soundbuffer = HARDSNDBUF;
			*sndswitch &= ~0x80;
		}
		else
		{
			synthptr = (void *) NewPtr((long) sizeof(myFFSynthRec));
			if (synthptr == NULL)
				memory_error();
			synthptr->mode = ffMode;
			synthptr->count = 2L << 16;
				/* ==2 to emulate normal buffer */
			soundbuffer = synthptr->waveBytes;
		}
		taskrec.qType = vType;
		taskrec.vblAddr = (ProcPtr) do_sound;
		taskrec.vblPhase = 0;
		//show_sound();
	}
}
