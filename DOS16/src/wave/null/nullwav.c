
//Include files
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include <math.h>
#include <mem.h>
#include <io.h>

#include <stdlib.h>
#include <string.h>
#include <io.h>      // Include for DOS I/O functions
#include <fcntl.h>   // Include for file access modes
#include <sys/stat.h>// Include for file permission modes
#ifdef DJGPP
#include <go32.h>
#include <dpmi.h>
#include <sys/movedata.h>
#include <sys/nearptr.h>
#include <pc.h>
#endif
#include "generic.h"
#include "sound.h"

static unsigned char NULLWAV_Init()
{
	return 0;
}

static unsigned char NULLWAV_Load(const char* WaveFile, struct WaveData *vc, unsigned char index)
{
	return 0;
}

static void NULLWAV_Play(struct WaveData *vc, unsigned char index)
{

}

static void NULLWAV_Unload(struct WaveData *vc, unsigned char index)
{

}

static void NULLWAV_Close()
{

}

static void NULLWAV_Update()
{	

}

SoundDevice NULLWAV_device = {
	"Null WAV driver",
	NULLWAV_Init,
	NULLWAV_Load,
	NULLWAV_Play,
	NULLWAV_Unload,
	NULLWAV_Update,
	NULLWAV_Close
};
