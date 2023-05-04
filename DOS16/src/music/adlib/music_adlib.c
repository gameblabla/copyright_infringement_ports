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
extern uint8_t adlib_pcm;
extern uint8_t adlib_music;

#define TIMER_CONTROL_PORT 0x43
#define TIMER_DATA_PORT 0x40
#define IRQ0_FREQUENCY 1193180
#define OPL2_PORT_REG 0x388
#define OPL2_PORT_DATA 0x389

unsigned char *imf_data;
unsigned long imf_size;
unsigned char imf_loop;
uint16_t delay_imf = 0;
uint16_t pos = 0;

extern unsigned char ADLIBPCM_Init();
extern uint8_t adlib_init;
extern void write_adlib_register(unsigned char reg, unsigned char value) ;

static unsigned char IMF_Init(unsigned short flags)
{
	if (adlib_init == 1)
	{
		return 0;
	}
	// It's done like this because they share the two same IRQ 
	ADLIBPCM_Init();
	return 1;
}

static unsigned char IMF_Load(const char* filename, unsigned long size)
{
	int fd;
	unsigned ssize;
	if (imf_data)
	{
		free(imf_data);
		imf_data = NULL;
	}
	_dos_open(filename, O_RDONLY, &fd);
	imf_size = lseek(fd, 0, SEEK_END);
	if (imf_size > 64000)
	{
		imf_size = 64000;
	}
	lseek(fd, 0, SEEK_SET);
	imf_data = malloc(imf_size);
	_dos_read(fd, imf_data, imf_size, &ssize);
	_dos_close(fd);
	return 0;
}

static void IMF_Play(unsigned char loop)
{
	if (!imf_size)
	{
		return;
	}
	imf_loop = loop;
	adlib_music = 1;
}

static void IMF_Stop_Music()
{
	uint8_t cx;
	if (adlib_music == 0) return;
	
	adlib_pcm = 0;
	adlib_music = 0;
	
	for (cx = 0xB0; cx <= 0xB8; ++cx) {
		write_adlib_register(cx, 0);
	}
	
	delay_imf = 0;
	pos = 0;
}

static void IMF_Close_Music()
{
	adlib_music = 0;
	delay_imf = 0;
	pos = 0;
}


MusicDevice IMF_device = {
	"IMF driver",
	IMF_Init,
	IMF_Load,
	IMF_Play,
	IMF_Stop_Music,
	IMF_Close_Music
};
