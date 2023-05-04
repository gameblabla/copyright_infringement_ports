
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

#define TIMER_CONTROL_PORT 0x43
#define TIMER_DATA_PORT 0x40
#define SPEAKER_CONTROL_PORT 0x61
#define IRQ0_FREQUENCY 1193180
#define TIMER_DATA_PORT_2 0x42

#define BUFFER_SIZE 32768
#define BLOCK_SIZE 8192
#define PSG_PORT 0x00C0

static uint8_t *samples;
static uint16_t sample_count;
static unsigned short sample_prg;
static uint8_t pc_play;
static unsigned int bit_index;

static uint32_t frequency;

static uint8_t psg_command; // PCjr does not like WORD writes so this is to ensure this doesn't happen

//#define toggle_speaker(state) outp(SPEAKER_CONTROL_PORT, (inp(SPEAKER_CONTROL_PORT) & 0xFD) | (state << 1));
static inline void toggle_speaker(unsigned char state) {
    outp(SPEAKER_CONTROL_PORT, (inp(SPEAKER_CONTROL_PORT) & 0xFD) | (state << 1));
}

static void (interrupt *old_isr)(); // Declare a global pointer to store the original ISR

static void interrupt PCK_IRQ ()
{
	uint8_t temp;
	temp = (inp(SPEAKER_CONTROL_PORT) & 0xFD) | (((samples[sample_prg] >> bit_index) & 1) << 1);
	__asm 
	{
		mov al, temp
		out SPEAKER_CONTROL_PORT, al
	};
	bit_index = (bit_index + 1) % 8;
	sample_prg += (bit_index == 0);
	
	if (sample_prg >= sample_count)
	{
		sample_prg = 0;
		_disable();
		_dos_setvect(0x08, old_isr);
		_enable();
		pc_play = 0;
	}
	
#ifdef __DJGPP__
	//Acknowledge hardware interrupt
	outportb (0x20, 0x20);
#endif
}


static unsigned char PCK_Init()
{
	// Store the original interrupt vector for IRQ 0 (timer)
	_disable();
    old_isr = _dos_getvect(0x08);
    _enable();
	
	return 0;
}

static unsigned char PCK_Load(const char* WaveFile, struct WaveData *vc, unsigned char index)
{
	return Sound_Load_File(WaveFile, vc, index);
}

static void PCK_Play(struct WaveData *vc, unsigned char index)
{
	samples = vc->Sample;
	sample_count = vc->SoundLength;
	pc_play = 1;
	sample_prg = 0;
	
    frequency = IRQ0_FREQUENCY / 16000;
    outp(TIMER_CONTROL_PORT, 0x36); // Set Timer 0 mode to rate generator
    outp(TIMER_DATA_PORT, frequency & 0xFF);
    outp(TIMER_DATA_PORT, frequency >> 8);
    
    _disable();
	_dos_setvect(0x08, PCK_IRQ);
	_enable();
}

static void PCK_Unload(struct WaveData *vc, unsigned char index)
{
	if (vc->Sample)
	{
		free(vc->Sample);
	}
}

static void PCK_Update()
{

}

static void PCK_Close()
{
	_disable();
	_dos_setvect(0x08, old_isr);
	_enable();
	pc_play = 0;
}


SoundDevice PCK_device = {
	"PC Speaker driver",
	PCK_Init,
	PCK_Load,
	PCK_Play,
	PCK_Unload,
	PCK_Update,
	PCK_Close
};
