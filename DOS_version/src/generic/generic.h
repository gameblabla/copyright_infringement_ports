#ifndef _H_GEN_
#define _H_GEN_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "graph.h"

typedef enum
{
	MACHINE_PCXT,
	MACHINE_PCAT,
	MACHINE_PS2,
} MachineType;

#define MAX_PCM 4

#ifdef DJGPP
#include <sys/nearptr.h>
#else
#include <i86.h>
#endif

#if defined(__386__)
#define INT_86 int386
#else
#define INT_86 int86
#endif

#define _BIOS_SET_CURPOS		0x0200
extern short VideoInt(short, short, short, short);
#pragma aux     VideoInt = \
                "push   bp      ", \
                "int    10h     ", \
                "pop    bp      ", \
                parm caller [ax] [bx] [cx] [dx] value [ax];

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

extern uint8_t* PCM_SoundData[MAX_PCM];
extern uint32_t PCM_SoundLength[MAX_PCM];
extern uint16_t PCM_SoundFreq[MAX_PCM];

extern uint32_t __stdcall apl_decompress(const void *Source, void *Destination);

extern void Generic_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress, unsigned char invert);

extern int stringstartswith(char *str, char *start);
extern int strucmp(char *s1, char *s2);

extern unsigned short screen_width, screen_height;

extern unsigned char std_font[];

extern void CAL_LZSAExpand (uint8_t far *source, uint8_t far *dest);
extern unsigned char Sound_Load_File(const char* WaveFile, unsigned char index);

extern MachineType GetMachineType(void);
extern void RestorePICState(MachineType machineType);
extern void SetAutoEOI(MachineType machineType);

extern void Init_Machine();
extern void Close_Machine();

#endif
