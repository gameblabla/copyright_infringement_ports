#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <i86.h>
#include <time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "generic.h"
#include "sound.h"

static uint16_t OPL2_PORT_REG;
static uint16_t OPL2_PORT_DATA;

#define INPUT_FREQ 4000

#define TIMER_CONTROL_PORT 0x43
#define TIMER_DATA_PORT 0x40

static void (interrupt *old_isr)(); // Declare a global pointer to store the original ISR

static volatile uint16_t ad_current_sample = 0;
static uint8_t *ad_samples;
static uint16_t ad_sample_count; // The number of samples, should be set according to your data
uint8_t adlib_pcm = 0;
uint8_t adlib_music = 0;

extern unsigned char *imf_data;
extern unsigned long imf_size;
extern unsigned char imf_loop;
extern uint16_t delay_imf;
extern uint16_t pos;

uint8_t adlib_init = 0;

void write_adlib_register(unsigned char reg, unsigned char value) 
{
    uint8_t cx;
    outp(OPL2_PORT_REG, reg);
    for (cx = 6; cx > 0; cx--) {
        inp(OPL2_PORT_REG);
    }
    outp(OPL2_PORT_DATA, value);
    for (cx = 35; cx > 0; cx--) {
        inp(OPL2_PORT_REG);
    }
}

void reset_all_registers()
{
	uint8_t reg;
    for (reg = 0; reg <= 0xF5; ++reg) write_adlib_register(reg, 0);
}

void init_adlib_for_pcm()
{
    write_adlib_register(0x20, 0x21);
    write_adlib_register(0x60, 0xF0);
    write_adlib_register(0x80, 0xF0);
    write_adlib_register(0xC0, 0x01);
    write_adlib_register(0xE0, 0x00);
    write_adlib_register(0x43, 0x3F);
    write_adlib_register(0xB0, 0x01);
    write_adlib_register(0xA0, 0x8F);
    write_adlib_register(0xB0, 0x2E);
}


/****************************************************************************
** IRQ service routine - this is called when the DSP has finished playing  **
** a block                                                                 **
****************************************************************************/

#define ADLIB_DELAY_PAUSE() \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \

#define ADLIB_DELAY_PAUSE2() \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \
	inp(OPL2_PORT_REG); \


// Can't play both at the same time
// I wonder if i should just separate the two... 
void interrupt ADLIB_IRQ() 
{
	uint8_t cx;
	uint8_t sample;
	uint8_t temp;
	
	if (adlib_pcm == 1)
	{
		outp(OPL2_PORT_REG, 0x40);
		ADLIB_DELAY_PAUSE();
		outp(OPL2_PORT_DATA, ad_samples[ad_current_sample]);
		ADLIB_DELAY_PAUSE2();
		ad_current_sample++;

		if (ad_current_sample > ad_sample_count)
		{
			ad_current_sample = 0;
			ad_sample_count = 0;
			adlib_pcm = 0;
		}
	}
	
	if (adlib_music == 1)
	{
		if (pos >= imf_size)
		{
			if (imf_loop == 1)
			{
				pos = 0;
				return;
			}
			
			for (cx = 0xB0; cx <= 0xB8; ++cx) {
				write_adlib_register(cx, 0);
			}
			
			adlib_music = 0;
			return;
		}
		
		if (delay_imf > 0) {
			delay_imf--;
			return;
		}
		
		outp(OPL2_PORT_REG, imf_data[pos]);
		
		ADLIB_DELAY_PAUSE();
		
		outp(OPL2_PORT_DATA, imf_data[pos + 1]);
		
		ADLIB_DELAY_PAUSE2();
		
		delay_imf = *((uint16_t *)&imf_data[pos + 2]);
		pos+=4;	
	}
	
#ifdef __DJGPP__
	//Acknowledge hardware interrupt
	outportb (0x20, 0x20);
#endif
}


unsigned char detect_adlib(uint16_t regi) {
	unsigned char a, b;
    write_adlib_register(0x04, 0x60); // Reset both timers
    write_adlib_register(0x04, 0x80); // Enable timer interrupts
	a = inp(regi);
	write_adlib_register(0x02, 0xFF);
	write_adlib_register(0x04, 0x21);
	delay(1);
	b = inp(regi);
	write_adlib_register(0x04, 0x60);
	write_adlib_register(0x04, 0x00);
    write_adlib_register(4, 0); // Disable timer interrupts
    return (a) == 6 && (b) == 6;
}


unsigned char ADLIBPCM_Init()
{
	uint16_t tries[4] = {0x378, 0x379, 0x3BC, 0x388};
	uint32_t count;
	uint8_t detc, i;
	clock_t ticks_to_wait, start_ticks;
	
	OPL2_PORT_REG = 0x388;
	for(i=0;i<2;i++)
	{
		detc = detect_adlib(tries[i]);
		if (detc)
		{
			OPL2_PORT_REG = tries[i];
			break;
		}
	}
	
	OPL2_PORT_DATA = OPL2_PORT_REG + 1;
	adlib_pcm = 0;
	adlib_init = 1;
	
	_disable();
    old_isr = _dos_getvect(0x08);
    _enable();
	
	reset_all_registers();
    init_adlib_for_pcm();
	
	// Init adlib
	ticks_to_wait = 2386 * (CLOCKS_PER_SEC / 1193180);
    start_ticks = clock();
    while (clock() < start_ticks + ticks_to_wait) {}

	// Set the timer value according to your desired sample rate
	// For example, for 4000 Hz:
	// count = 1193180 / 4000 = 298 (in decimal) = 12A (in hex)
	count = 1193180 / INPUT_FREQ;
	outp(TIMER_CONTROL_PORT, 0x36);
	outp(TIMER_DATA_PORT, count & 0xFF);
	outp(TIMER_DATA_PORT, count >> 8);

	_disable();
	_dos_setvect(0x08, ADLIB_IRQ); // Set IRQ0 handler
	_enable();

    write_adlib_register(0xB0, 0x20);
    write_adlib_register(0xA0, 0);
	
	return 0;
}

static unsigned char ADLIBPCM_Load(const char* WaveFile, struct WaveData *vc, unsigned char index)
{
	// Cancel out audio
	ad_current_sample = 0;
	ad_sample_count = 1;
	adlib_pcm = 0;
	
	return Sound_Load_File(WaveFile, vc, index);
}

static void ADLIBPCM_Play(struct WaveData *vc, unsigned char index)
{

	if (adlib_pcm == 1) return;
	
	ad_samples = vc->Sample;
	ad_sample_count = vc->SoundLength;
	ad_current_sample = 0;
	adlib_pcm = 1;
}

static void ADLIBPCM_Unload(struct WaveData *vc, unsigned char index)
{
	if (vc->Sample)
	{
		free(vc->Sample);
	}
}

static void ADLIBPCM_Update()
{
}

static void ADLIBPCM_Close()
{
	_disable();
	_dos_setvect(0x08, old_isr);
	_enable();
	adlib_init = 0;
	adlib_pcm = 0;
}


SoundDevice ADLIBPCM_device = {
	"ADLIB PCM driver",
	ADLIBPCM_Init,
	ADLIBPCM_Load,
	ADLIBPCM_Play,
	ADLIBPCM_Unload,
	ADLIBPCM_Update,
	ADLIBPCM_Close
};
