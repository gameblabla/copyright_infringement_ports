#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include "music.h"

// Declare functions from playmidi.c
extern int LoadMIDI (const char *fname);
extern void StopMIDI ();
extern void PlayMIDI ();
extern void InitMIDI ();
extern char SetFM ();
extern void UnloadMIDI ();
extern int Playing ();

static unsigned char MIDI_Init_Music(unsigned short flags)
{
	InitMIDI ();
	SetFM (); //or use SetGM here if the PC supports it
	return 0;
}

static unsigned char MIDI_Load_Music(const char* filename)
{
	UnloadMIDI ();
	LoadMIDI (filename);
	return 0;
}

static void MIDI_Play_Music(unsigned char loop)
{
	PlayMIDI();
}

static void MIDI_Stop_Music()
{
	StopMIDI();
}

static void MIDI_Close_Music()
{
	UnloadMIDI ();
}

MusicDevice MIDI_device =
{
	"General/FM Midi",
	MIDI_Init_Music,
	MIDI_Load_Music,
	MIDI_Play_Music,
	MIDI_Stop_Music,
	MIDI_Close_Music
};
