#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include <pc.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <sys/nearptr.h>
#include <sys/types.h>
#include "music.h"

//#define ENABLE_NEAR() if (__djgpp_nearptr_enable() == 0) { printf("Could not get access to first 640K of memory.\n"); exit(-1); }
#define ENABLE_NEAR() __djgpp_nearptr_enable();
#define DISABLE_NEAR() __djgpp_nearptr_disable();


static unsigned char NullMus_Init_Music(unsigned short flags)
{

	return 1;
}

static unsigned char NullMus_Load_Music(const char* filename, unsigned long size)
{

	
	return 1;
}

static void NullMus_Play_Music()
{
}

static void NullMus_Stop_Music()
{
}

static void NullMus_Close_Music()
{

}

MusicDevice NullMus_device =
{
	"NullMus Music",
	NullMus_Init_Music,
	NullMus_Load_Music,
	NullMus_Play_Music,
	NullMus_Stop_Music,
	NullMus_Close_Music
};
