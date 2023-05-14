#ifndef SOUND_H
#define SOUND_H

typedef struct MusicDevice {
	unsigned char (*Init_Sound)(void);
	void (*Uninit_Sound)(void);
	
	void (*Load_Sound)(const char* path, uint8_t index);
	void (*Play_Sound)(unsigned char loop, uint8_t index);
	void (*Stop_Sound)();
} SoundDevice;

extern SoundDevice PSGPCM_device;
extern SoundDevice DMA_device;

#endif
