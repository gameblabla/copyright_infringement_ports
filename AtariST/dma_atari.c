/** @file src/audio/dsp_atari.c Atari DMA Sound implementation of the DSP. */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <osbind.h>
#include "sound.h"

typedef struct tagS
{
  unsigned char* data;
  unsigned long size;
} SOUND;


/* STE/TT/Falcon DMA Sound is able to play at :
 * 50066Hz 25033Hz 12517Hz 6258Hz
 * (6258Hz is not available on falcon) */
#define DMASOUND_FREQ 12517
#define DMASOUND_BUFFER_SIZE 48182


extern void set_dma_sound(void * buffer, unsigned int len);
extern void set_dma_sound_noloop(void * buffer, unsigned int len);
extern void stop_dma_sound(void);	/* needs to be called in supervisor mode */
//extern unsigned int get_dma_status(void);	/* needs to be called in supervisor mode */

static unsigned char *s_stRamBuffer;
static uint32_t s_stRamBufferSize;

static SOUND snd[4];

static inline void memset32(void *ptr, uint32_t value, size_t num) {
    uint32_t* p = (uint32_t*) ptr;
    while(num--) {
        *p++ = value;
    }
}

static void DMA_free(uint8_t index)
{
	if (snd[index].data) Mfree(snd[index].data);
}

void DMA_Stop(void)
{
	Supexec(stop_dma_sound);
}

void DMA_Load(const char* path, uint8_t index)
{
	int fp;
	unsigned short	handle;
	
	DMA_Stop();
	DMA_free(index);

	fp = Fopen(path, 0);
	Fseek(0, fp,  SEEK_END);
	handle = fp & 0xFFFF;
	snd[index].size = Fseek(0, handle, 2);
	Fseek(0, handle, 0);
	snd[index].data = (unsigned char *)Malloc(snd[index].size);
	Fread(handle, snd[index].size, snd[index].data);
	Fclose(handle);
}




void DMA_Uninit(void)
{
	uint8_t i;
	
	DMA_Stop();
	if (s_stRamBuffer) 
	{
		Mfree(s_stRamBuffer);
		s_stRamBuffer = NULL;
	}
	s_stRamBufferSize = 0;
	
	for(i=0;i<4;i++)
	DMA_free(i);

}

uint8_t DMA_Init(void) 
{
	/* allocate ST RAM buffer for audio */
	s_stRamBufferSize = DMASOUND_BUFFER_SIZE;
	s_stRamBuffer = (unsigned char *)Malloc(s_stRamBufferSize);
	if(!s_stRamBuffer) {
		s_stRamBufferSize = 0;
		return 0;
	}
	return 1;
}

void DMA_Play(unsigned char loop, uint8_t index)
{
	//memset32(s_stRamBuffer, 0, DMADMA_BUFFER_SIZE);
	DMA_Stop();
	
	if (loop == 1)
		set_dma_sound(snd[index].data, snd[index].size);

	if (loop == 0)
		set_dma_sound_noloop(snd[index].data, snd[index].size);
}


SoundDevice DMA_device = {
	DMA_Init,
	DMA_Uninit,
	DMA_Load,
	DMA_Play,
	DMA_Stop
};

