#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "music.h"
#include "generic.h"


static unsigned char NULLMUS_Init(unsigned short flags)
{
	return 1;
}

static unsigned char NULLMUS_Load(const char* filename, unsigned long size)
{
	return 0;
}

static void NULLMUS_Play(unsigned char loop)
{
}

static void NULLMUS_Stop_Music()
{
}

static void NULLMUS_Close_Music()
{
}


MusicDevice NULLMUS_device = {
	"Null Mus driver",
	NULLMUS_Init,
	NULLMUS_Load,
	NULLMUS_Play,
	NULLMUS_Stop_Music,
	NULLMUS_Close_Music
};
