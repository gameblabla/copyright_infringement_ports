/* 
	File GW.h:  macros, definitions for Gravity Well in Lightspeed C
	
	Copyright (c) 1986-88 Randall H. Wilson
*/

/*#define	DEBUG
*/

#define	TRUE	1
#define	FALSE	0
#define	NULL		((void *) 0L)

#define	SETRECT(r,l,t,s,b)	\
			(r).left = (l);	\
			(r).top = (t);	\
			(r).right = (s);	\
			(r).bottom = (b)
#define	MIN(a,b)	((a) < (b) ? (a) : (b))
#define	MAX(a,b)	((a) > (b) ? (a) : (b))
#define	ABS(a)	((a) > 0 ? (a) : -(a))
#define	TOLOWER(c)	((c) | ('a' - 'A'))	/* macro to set lowercase */

#define	MOVETOPT(p)		MoveTo((p).h, (p).v)

#define	SCRWTH	512
#define	SCRHT		342
#define	SBARHT	24
#define	VIEWHT	(SCRHT-SBARHT)
#define	SBARSIZE	(SBARHT * 64)
#define	VIEWSIZE	(VIEWHT * 64)
#define 	SCRSIZE	21888

#define	LEFTMARG	180
#define	RIGHTMARG	(SCRWTH - LEFTMARG)
#define	TOPMARG	140
#define	BOTMARG	(VIEWHT - TOPMARG)
#define	SOFTBORDER	200
#define	SHOOTMARG	300		/* distance from screen bunkers fire*/
#define	MAPBORDER	5		/* space around map on screen		*/
#define	MAPWTH	(SCRWTH - 2*MAPBORDER)	/* size of map		*/
#define	MAPHT		(SCRHT - (40 + 2*MAPBORDER))

#define	EDWINHT	284		/* size of editing rectangle in Editor */
#define	EDWINWTH	492
#define	PRECTHT	284		/* size of planet enclosing rectangle */
#define	PRECTWTH	460
#define	SCROLLSPEED	32		/* scrolling speeds */
#define	PAGESPEED	250
#define	FANGLESIZE	50		/* magnitude of firing angle wedges */

#define	FILEHEAD	160		/* planet file sizes */
#define	PLANHEAD	30
#define	PLANSIZE	1540

#define	MAXSTARTLEVEL 100		/* highest starting level		*/
#define	NUMLINES	125		/* maximum number of terrain lines	*/
#define	NUMBULLETS	6		/* number of ship's shots at once	*/
#define	NUMBUNKERS	25		/* largest number of bunkers		*/
#define	NUMFUELS	15		/* largest number of fuel cells	*/
#define	NUMSHOTS	20		/* number of bunker shots at once	*/
#define	NUMSHARDS	15		/* maximum # of concurrent shards	*/
#define	NUMSPARKS	100		/* number of sparks in explosions	*/
#define	NUMSTRAFES	5		/* max strafes at once			*/
#define	NUMCRATERS	50		/* number of craters at a time	*/
#define	NUMPRECRATS	25		/* # craters before bunkers explode	*/
#define	NUMGENS	25		/* # of generator bunkers allowed	*/
#define	CARTOONSIZE	100		/* number of parameters in cartoon	*/
#define	NUMHISCORES	10		/* number of high score entries	*/

#define	SHOTLEN	35		/* cycles bullets keep going		*/
#define	BUNKSHLEN	(SHOTLEN-5)	/* cycles bunk shots keep going	*/

#define	SHIPHT	32		/* height of ship bit map		*/
#define	SCENTER	15		/* distance of ship center from side*/
#define	FCENTER	3		/* center of flame from upleft	*/
#define	SHRADIUS	12		/* effective radius of shield		*/

#define	BUNKHT	48
#define	BUNKKINDS	5
#define	BUNKROTKINDS 2		/* number of kinds that sit on walls*/
#define	WALLBUNK	0		/* normal sit-on-wall bunker		*/
#define	DIFFBUNK	1		/* different at each orientation	*/
#define	GROUNDBUNK	2		/* normal sit-on-ground bunker	*/
#define	FOLLOWBUNK	3		/* tracking bunker			*/
#define	GENERATORBUNK 4		/* generator type of bunker		*/
#define	BUNKFRAMES	8		/* # frames in animated bunkers	*/
#define	BUNKFCYCLES	2		/* time on one frame			*/
#define	BRADIUS	19		/* approx. radius of bunker		*/
#define	SKILLBRADIUS 30		/* for computing suicide kills	*/

#define	FUELHT	32		/* height of fuel definition		*/
#define	FUELCENTER	16		/* center of top from top left	*/
#define	FUELFRAMES	8		/* frames in fuel animation		*/
#define	FUELFCYCLES	3		/* cycles each frame stays		*/
#define	FRADIUS	30		/* distance from fuel to pick it up	*/
#define	BIGFRADIUS	60

#define	SHARDHT	16		/* height of a shard in explosion	*/
#define	SHARDKINDS	7		/* should be >= BUNKKINDS		*/
#define	SH_DISTRIB	20		/* size of shard starting area	*/
#define	SH_LIFE	25		/* cycles shard stays alive		*/
#define	SH_ADDLIFE	15		/* added cycles shard stays alive	*/
#define	EXPLSHARDS	5		/* number of shards in bunker death	*/
#define	EXPLGRAV	5		/* gravity factor for shards		*/
#define	SH_SPEED	32		/* speed factor of shard (*256)	*/
#define	SH_ADDSPEED	16		/* possible amount over above		*/
#define	SH_SLOW	5		/* slow factor (0=stop, 1=1/2...)	*/
#define	SH_SPIN2	64		/* max speed of shard spin (*2*16)	*/

#define	EXPLSPARKS	20		/* number of sparks in bunker death	*/
#define	SPARKLIFE	10		/* minimum life of spark		*/
#define	SPADDLIFE	10		/* possible longer than SPARKLIFE	*/
#define	SPARKSLOW	7		/* slow factor (1=stop, ...)		*/
#define	SP_SPEED16	40		/* speed factor of spark (*16)	*/

#define	SHIPSPARKS	NUMSPARKS	/* number of sparks in ship blowup	*/
#define	SH_SPARKLIFE 35		/* minimum life of ship spark		*/
#define	SH_SPADDLIFE 20		/* possible longer than SPARKLIFE	*/
#define	SH_SP_SPEED16 50		/* speed factor of spark (*16)	*/

#define	STRAFEHT	8		/* height of a strafe			*/
#define	STCENTER	3		/* center of a strafe from upleft	*/
#define	STRAFE_LIFE	4		/* cycles strafes live			*/

#define	CRATERHT	32		/* height of crater image		*/
#define	CRATERCENTER 16		/* center of crater image		*/

#define	DIGHEIGHT	9		/* height of digits			*/
#define	ZEROCHAR	0		/* digit number of zero */
#define	ACHAR		10		/* digit number of character 'A' */
#define	SHIPCHAR	36		/* digit number of little ship character */
#define	SPACECHAR	37		/* digit number of space */

#define	SHIPSTART	2		/* number of ships to start with	*/
#define	FUELSTART	10000		/* starting amount of fuel		*/
#define	MAXFUEL	25000		/* maximum fuel value			*/
#define	FUELBURN	16		/* amount of fuel used each thrust	*/
#define	FUELSHIELD	83		/* amount of fuel used to shield	*/
#define	FUELGAIN	2000		/* amount of fuel gained from cell	*/
#define	CRITFUEL	2000		/* amount of fuel considered critical */
#define	FUELBEEPS	3		/* number of beeps in fuel pickup	*/

#define	SCOREBUNK	200		/* not used: see kill_bunk function */
#define	SCOREFUEL	15		/* score for fuel pickup		*/

#define	DEAD_TIME	80		/* cycles to continue after death	*/
#define	IDLETICKS	(60*60)	/* 60 seconds before cartooning	*/

						/* possible values of currentsound	*/
enum {NO_SOUND = 0, FIRE_SOUND, EXP1_SOUND, THRU_SOUND, BUNK_SOUND,
	SOFT_SOUND, SHLD_SOUND, FUEL_SOUND,
	EXP2_SOUND, EXP3_SOUND, CRACK_SOUND, FIZZ_SOUND, ECHO_SOUND};
	
#define	EXPL_LO_PER	50		/* lowest period in an explosion	*/
#define	EXPL_ADD_PER 206		/* random amount to add to above	*/
#define	THRU_LO_AMP	64
#define	THRU_ADD_AMP 128
#define	MICODELAY	45		/* time to wait before killing planet */

#define	KEY_LEFT	0x01		/* masks for getting keys from keymap*/
#define	KEY_RIGHT	0x02
#define	KEY_SHIELD	0x04
#define	KEY_THRUST	0x08
#define	KEY_SHOOT	0x10

						/* types of screens to deal with: */
enum {MACHINE_PLUS=1,			/* Mac Plus or SE: 2 screens to flip */
	MACHINE_2,				/* Mac II: big enough for a window,
							fast enough to do the copybits */
	MACHINE_SE30,			/* fast but Plus-sized screen: SE30 */
	MACHINE_SMALL};			/* fast, Plus-sized screen: non-SE30 */

						/* menu id's */
enum {APPLEMENU=1, FILEMENU, EDITMENU, GAMEMENU, PLANMENU, OPTSMENU};

#define	VERTSBAR	1		/* scroll bar refcons */
#define	HORIZSBAR	2
						/* next action to be done in main loop */
enum {NEXTQUIT=1, NEXTTITLE, NEXTEDIT, NEXTPLAY};

						/* pointer modes in editor */
enum {ARROWPTR=0, ERASEPTR, CRATERPTR, SHIPPTR, LINEPTR};

						/* types to be selected in the editor */
enum {NONE_T=0, LINE_T, BUNK_T, FUEL_T, CRAT_T};

#define	ROUNDRADIUS	256		/* radius ^2 of corner rounding */
#define	ERASEDIST	64		/* distance ^2 from object to erase */

						/* Miscellaneous resource numbers */
enum {M_CONTROLS=256, M_SINEWAVE, M_CARTOON, M_SBAR, M_FIGS, M_TITLEPAGE,
		M_HISCORES, M_PLANETCLIP, M_README};



typedef struct _linerec
{	int	startx, starty,		/* x,y of left endpoint			*/
		length,			/* the length(longest of x,y)		*/
		endx, endy;			/* x, y of right endpoint		*/
	short	up_down,			/* 1 if down, -1 if up			*/
		type;				/* direction of line: see below	*/
	int	kind,				/* normal, bounce, phantom, etc	*/
		h1, h2,			/* start & end h-vals of xor		*/
		newtype;			/* new type including up-down		*/
	struct _linerec *next,		/* next line of this kind		*/
		*nextwh;			/* next line with white-only drawing*/
} linerec;

				/* Possible values for TYPE field in line record */
enum {LINE_N = 1, LINE_NNE, LINE_NE, LINE_ENE, LINE_E};

				/* Values for up_down field */
#define	L_UP		-1		/* up line	*/
#define	L_DN		1		/* down line*/

				/* Values for kind field */
enum {L_NORMAL=0, L_BOUNCE, L_GHOST, L_EXPLODE, L_NUMKINDS};

				/* Values for newtype field (combo of type & up_down) */
enum {NEW_S=1, NEW_SSE, NEW_SE, NEW_ESE, NEW_E, NEW_ENE, NEW_NE, NEW_NNE};


typedef struct
{	int	x, y;			/* global x and y positions		*/
} craterrec;


typedef struct
{	int	x, y, 		/* current global int x, y of shot	*/
		x8, y8,		/* x, y * 8 (global)			*/
		lifecount,		/* 0 if not going, time to death	*/
		v, h,			/* vert. & horiz speed * 8		*/
		strafedir,		/* rotation of strafe (-1 if none)	*/
		btime;		/* cycles to go after bouncing	*/
	linerec *hitline;		/* pointer to line that it hits	*/
} shotrec;


typedef struct
{	int	x, y,			/* global x and y coordinates		*/
		rot,			/* direction of strafe			*/
		lifecount;		/* remaining life				*/
} straferec;


typedef struct
{	int	low, high;		/* low and high sides of arc (0-511+) */
} rangerec;

typedef struct
{	int	x, y,			/* global x, y, of center of base	*/
		rot,			/* 0-15 for facing up, nne, etc	*/
		kind,			/* which kind of bunker			*/
		alive;		/* true if bunker is alive and well	*/
	rangerec ranges[2];	/* arcs in which bunker will fire	*/
	int	rotcount;		/* in animated, count to next frame	*/
} bunkrec;
		

typedef struct
{	int	x, y,			/* global x, y, of center of top	*/
		alive,		/* true if not picked up yet		*/
		currentfig,		/* 0-1 for which figure is being shown */
		figcount;		/* cycles this figure will be shown */
} fuelrec;



typedef struct
{	int	x, y,			/* global x, y, of upleft of shard 	*/
		h, v,			/* vertical and horizontal speed * 8*/
		rot16,		/* current rotation * 16		*/
		rotspeed,		/* added to rot16 every cycle		*/
		lifecount,		/* countdown to disappearance		*/
		kind;			/* which image (bunker came from)	*/
} shardrec;


/* bookkeeping types of records */

typedef struct			/* holds a key to control the ship */
{
	int			index;
	unsigned char	mask;
	char			name[10];
} controlkey;


typedef struct			/* an entry in the high score file */
{	char	thename[20];
	long	thescore;
	int	thelevel;
} hi_sc_rec;


typedef struct			/* keeps a center of magnetism (gravity?) */
{
	int x, y, str;
} gravrec;



/* bit image definitions */

typedef int Bunker_Pic[16][3*BUNKHT];	/* ([backgr])[kind][rot][data] */

typedef int Fuel_Pic[2*FUELHT];		/* ([backgr])[frame][data] */

typedef int Ship_Pic[2*SHIPHT];		/* [rot][data] */

typedef int Shard_Pic[2][16][SHARDHT];	/* [kind][backgr][rot][data] */

typedef int Crater_Pic[CRATERHT*2];		/* [data] */




