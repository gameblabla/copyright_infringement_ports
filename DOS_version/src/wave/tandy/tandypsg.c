
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
static unsigned short sample_prg = 0;
static uint8_t tandy_play;
static uint8_t tandy_init = 0;

static uint8_t psg_command; // PCjr does not like WORD writes so this is to ensure this doesn't happen

// volume is 0-15
static void SetPCjrAudioPeriod(uint8_t chan, uint16_t period)
{
    uint8_t command;
    
/*
  To set a channel, we first send frequency, then volume.
  Frequency:
  76543210 76543210
  1                 - set bit to tell chip we are selecting a register
   xx0              - set channel.  4, 2, and 0 are valid values.
      xxxx          - low 4 bits of period
           0        - clear bit to tell chip more freq. coming
            x       - unused
             xxxxxx - least sig. 6 bits of period

  Sending a word value will not work on PCjr, so send bytes individally.
  (It does work on Tandy, but we want to be nice.)

  Set attenuation (volume):

  76543210
  1                 - set bit to tell chip we are selecting a register
   xx1              - register number (valid values are 1, 3, 5, 7)
      xxxx          - 4-bit volume where 0 is full volume and 15 is silent)

*/
    // build MSB
    command = chan << 5;    // get voice reg in place
    command |= 0x80;        // tell chip we are selecting a reg
    command |= (period & 0xF);    // grab least sig 4 bits of period...
    outp(PSG_PORT,command);
    
    // build LSB
    command = period >> 4;    // isolate upper 6 bits
    //command &= 0x7F;        // clear bit 7 to indicate rest of freq
    outp(PSG_PORT,command);
}

#define SetPCjrAudioVolume(chan, volume) outp(PSG_PORT, chan << 5 | 0x90 | volume);

/****************************************************************************
** IRQ service routine - this is called when the DSP has finished playing  **
** a block                                                                 **
****************************************************************************/


static void (interrupt *old_isr)(); // Declare a global pointer to store the original ISR

uint8_t current_sample;

static void interrupt Tandy_IRQ ()
{
	uint8_t sample;
	uint8_t shift_value = (sample_prg & 1) << 2;
	
	if (sample_prg >= sample_count)
	{
		SetPCjrAudioVolume(0,0xF);
#if defined(__386__)
		outp (0x20, 0x20);
#endif
		return;
	}
	
	sample = 0 << 5 | 0x90 | ((samples[sample_prg] >> shift_value) & 0x0F);
	__asm 
	{
		mov al, sample
		out PSG_PORT, al
	};
	
	//SetPCjrAudioVolume(0, sample);
	sample_prg++;

	
#if defined(__386__)
	//Acknowledge hardware interrupt
	outp (0x20, 0x20);
#endif
}


#ifdef __DJGPP__
//Interrupt vector helpers
static void setvect (int Vector)
{
  //Get location of the new keyboard handler
  MyIRQ.pm_offset = (int)ServiceIRQ;
  MyIRQ.pm_selector = _go32_my_cs ();
  //Save the old interrupt handler
  _go32_dpmi_get_protected_mode_interrupt_vector (Vector, &OldIRQ);
  //Set the new interrupt handler in the chain
  _go32_dpmi_chain_protected_mode_interrupt_vector (Vector, &MyIRQ);
}

static void resetvect (int Vector)
{
  //Set interrupt vector to the BIOS handler
  _go32_dpmi_set_protected_mode_interrupt_vector (Vector, &OldIRQ);
}
#endif

static unsigned char Tandy_Init()
{
	// Store the original interrupt vector for IRQ 0 (timer)
	_disable();
    old_isr = _dos_getvect(0x08);
    _enable();
	
	SetPCjrAudioPeriod(0, 0x000F);
	SetPCjrAudioPeriod(1, 0x0000);
	SetPCjrAudioPeriod(2, 0x0000);
	SetPCjrAudioVolume(0,0xF);
	SetPCjrAudioVolume(1,0xF);
	SetPCjrAudioVolume(2,0xF);

	tandy_play = 0;
	tandy_init = 1;
	
	return 0;
}

static unsigned char Tandy_Load(const char* WaveFile, unsigned char index)
{
	return Sound_Load_File(WaveFile, index);
}

static void Tandy_Play( unsigned char index)
{
	uint32_t frequency;
	
	if (tandy_init == 0) return;

	samples = PCM_SoundData[index];
	sample_count = PCM_SoundLength[index];
	
	SetPCjrAudioPeriod(0, 0x000F);
	SetPCjrAudioVolume(0,0xF);
	
	current_sample = 0;
	sample_prg = 0;
	
	if (tandy_play == 0)
	{
		frequency = IRQ0_FREQUENCY / 11025;
		outp(TIMER_CONTROL_PORT, 0x34); // Set Timer 0 mode to rate generator
		outp(TIMER_DATA_PORT, frequency & 0xFF);
		outp(TIMER_DATA_PORT, frequency >> 8);
		
		_disable();
		_dos_setvect(0x08, Tandy_IRQ);
		_enable();
	}
	
	tandy_play = 1;
}

static void Tandy_Unload(unsigned char index)
{
	if (PCM_SoundData[index])
	{
		free(PCM_SoundData[index]);
		PCM_SoundData[index] = NULL;
		PCM_SoundLength[index] = 0;
	}
}

static void Tandy_Close()
{
	_disable();
	_dos_setvect(0x08, old_isr);
	_enable();
	
	SetPCjrAudioVolume(0,0xF);
	SetPCjrAudioVolume(1,0xF);
	SetPCjrAudioVolume(2,0xF);
	SetPCjrAudioVolume(3,0xF);
	SetPCjrAudioPeriod(0, 0x0000);
	SetPCjrAudioPeriod(1, 0x0000);
	SetPCjrAudioPeriod(2, 0x0000);
	SetPCjrAudioPeriod(3, 0x0000);
	
	
	tandy_play = 0;
	tandy_init = 0;
}

static void Tandy_Update()
{	
	if (sample_prg >= sample_count)
	{
		SetPCjrAudioVolume(0,0xF);
		SetPCjrAudioPeriod(0, 0x0000);
		_disable();
		_dos_setvect(0x08, old_isr);
		_enable();
		tandy_play = 0;
	}
}

SoundDevice Tandy_device = {
	"TANDY PSG driver",
	Tandy_Init,
	Tandy_Load,
	Tandy_Play,
	Tandy_Unload,
	Tandy_Update,
	Tandy_Close
};
