#ifndef SB_H
#define SB_H


typedef struct SoundDevice {
	/* * * */
	/* The name of this video driver */
	const char *name;

	unsigned char (*Sound_Init)();

	unsigned char (*Sound_Load)(const char* WaveFile, unsigned char index);
	void (*Sound_Play)( unsigned char index);
	void (*Sound_Unload)( unsigned char index);
	
	void (*Sound_Update)();
	
	void (*Sound_Close)();
} SoundDevice;


extern SoundDevice SBDMA_device;
extern SoundDevice Tandy_device;
extern SoundDevice PCK_device;
extern SoundDevice ADLIBPCM_device;
extern SoundDevice NULLWAV_device;

#endif
