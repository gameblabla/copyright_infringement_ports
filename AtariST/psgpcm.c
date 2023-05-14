#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <osbind.h>
#include "psg.h"
#include "sound.h"

typedef struct tagS
{
  unsigned char* data;
  unsigned long size;
} SOUND;

static SOUND snd[4];

static inline void memset32(void *ptr, uint32_t value, size_t num) {
    uint32_t* p = (uint32_t*) ptr;
    while(num--) {
        *p++ = value;
    }
}

static void PSGPCM_free(uint8_t index)
{
	if (snd[index].data) Mfree(snd[index].data);
}


void PSGPCM_Stop(void)
{

}

void PSGPCM_Load(const char* path, uint8_t index)
{
	int fp;
	unsigned short	handle;
	
	PSGPCM_Stop();
	PSGPCM_free(index);

	fp = Fopen(path, 0);
	Fseek(0, fp,  SEEK_END);
	handle = fp & 0xFFFF;
	snd[index].size = Fseek(0, handle, 2);
	Fseek(0, handle, 0);
	snd[index].data = (unsigned char *)Malloc(snd[index].size);
	Fread(handle, snd[index].size, snd[index].data);
	Fclose(handle);
}


void PSGPCM_Uninit(void)
{
	uint8_t i;
	
	SetVolume(CHANNEL_A, 0x0);
	SetVolume(CHANNEL_B, 0x0);
	
	for(i=0;i<4;i++)
	PSGPCM_free(i);

}

uint8_t PSGPCM_Init(void) 
{
	SetTone(CHANNEL_A, 0);
	SetTone(CHANNEL_B, 0);
	SetTone(CHANNEL_C, 0);
	return 1;
}

void PSGPCM_Play(unsigned char loop, uint8_t index)
{
	uint32_t sample_size, current_sample;
	unsigned char* mysample;

	current_sample = 0;
	sample_size = snd[index].size;
	mysample = snd[index].data;
		
	while(current_sample < sample_size)
	{
		uint8_t shift_value = (current_sample & 1) << 2;
		SetVolume(CHANNEL_A, (mysample[current_sample] >> shift_value) & 0x0F);
		SetVolume(CHANNEL_B, (mysample[current_sample] >> shift_value) & 0x0F);
		SetVolume(CHANNEL_C, (mysample[current_sample] >> shift_value) & 0x0F);
		current_sample++;
	}
	SetVolume(CHANNEL_A, 0x0);
	SetVolume(CHANNEL_B, 0x0);
	SetVolume(CHANNEL_C, 0x0);
}


SoundDevice PSGPCM_device = {
	PSGPCM_Init,
	PSGPCM_Uninit,
	PSGPCM_Load,
	PSGPCM_Play,
	PSGPCM_Stop
};

