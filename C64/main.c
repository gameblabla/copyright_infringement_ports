
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <c64.h>
#include "font.h"
#include "rikuto_pcm.h"
#include "what_pcm.h"
#include "fun_pcm.h"

#define MCB_COLORS 0xc000
#define MCB_BITMAP 0xe000

#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)    (*(unsigned*) (addr) = (val))
#define PEEK(addr)         (*(unsigned char*) (addr))
#define PEEKW(addr)        (*(unsigned*) (addr))

// set VIC Bank (given the start address)
#define SET_VIC_BANK(_addr) \
  CIA2.pra = (CIA2.pra & ~3) | (((((_addr)>>8)&0xc0)>>6)^3);
  
// set VIC character memory (given the start address)
#define SET_VIC_BITMAP(_addr) \
  VIC.addr = (VIC.addr & 0b11110001) | ((((_addr)>>8)&0x38)>>2);

// set VIC screen memory (given the start address)
#define SET_VIC_SCREEN(_addr) \
  VIC.addr = (VIC.addr & 0b00001111) | ((((_addr)>>8)&0x3c)<<2);
  
void setup_bitmap_multi()
{
	VIC.ctrl1 = 0x38;
	VIC.ctrl2 = 0x18;
	SET_VIC_BANK(MCB_BITMAP);
	SET_VIC_BITMAP(MCB_BITMAP);
	SET_VIC_SCREEN(MCB_COLORS);
}


extern void DrawPict();
extern void Draw_Bakura();

extern void Draw_firstframe();
extern void Draw_firstframe_color();
extern void Draw_frame2();
extern void Draw_frame3();
extern void Draw_frame4();

unsigned char FRAME_CURRENT;
/*
void Copy_Color()
{
	#pragma clang loop unroll(full)
	for (size_t i = 0; i < blockSize1; ++i) {
		((char*)MCB_COLORS)[i] = toframe[i + 8000];
	}
}

void Copy_Bitmap()
{
	// Unrolled loop for the second memcpy
	#pragma clang loop unroll(full)
	for (size_t i = 0; i < blockSize2; ++i) {
		((char*)MCB_BITMAP)[i] = toframe[i];
	}
}
*/

const unsigned char* numberoffset[]=
{
	font + 0,
	font + 16,
	font + 32,
	font + 48,
	font + 64,
	font + 80,
	font + 96,
	font + 112,
	font + 128,
	font + 144
};

void Print_text(char* text, unsigned short addr)
{
	unsigned char t;
	unsigned short i;
	uint8_t *ptr = (uint8_t*)MCB_BITMAP + addr;
	for (i = 0; text[i]; i+=1)
	{
starttextloop:
		// Space
		// If it reaches it, just increment, skip conditions and jump back to start
		if (text[i] == ' ')
		{
			memset(ptr, 0, 16);
			i++;
			ptr+= 16;
			goto starttextloop;
		}
		
		// This is for escape codes
		t = text[i];
		
		// To print text
		if (text[i] >= 'A' && text[i] <= 'Z')
		{
			t -= 55;
		}
		// To print numbers
		if (text[i] <= '9' && text[i] >= '0')
		{
			t -= 48;
		}
		memcpy(ptr, font + (t<<4), 16);
		ptr+= 16;
	}
}

#define JOY_UP    0x01
#define JOY_DOWN  0x02
#define JOY_LEFT  0x04
#define JOY_RIGHT 0x08
#define JOY_FIRE  0x10

static inline unsigned char read_joystick(unsigned char port)
{
    // Read the appropriate register for the selected port
    unsigned char joy_value = port == 1 ? CIA1.prb : CIA1.pra;
    // Invert the bits and mask out irrelevant bits
    joy_value = ~joy_value & 0x1F;
    return joy_value;
}

unsigned char game_mode;

// Voltage based SID PCM sample play
#define PCM_DELAY 196
// 64 for 11025hz
// 96 for 8000hz
// 200 for 4000hz

void Play_PCM(const unsigned char* pcm, unsigned short sizepcm) {
    unsigned short i;
	// Disable the SID voices
    SID.v1.ctrl = 0x41; // Gate bit and triangle waveform
    SID.v1.freq = 0x1000; // Set a constant frequency
    SID.v1.pw = 0; // Pulse width = 0 (unused for triangle waveform)
    SID.v1.ad = 0x00; // Attack = 0, Decay = 0
    SID.v1.sr = 0xF0; // Sustain = max, Release = 0
    // Play the PCM sample
    for (i = 0; i < sizepcm; i++)
    {
        // Set the master volume based on the PCM data value
        SID.amp = (pcm[i] >> 4) & 0x0F; // Convert 8-bit to 4-bit
        // To slow it down enough for 11khz 4-bits
        
        #pragma clang loop unroll(full)
		for (size_t j = 0; j < PCM_DELAY; ++j) {
			asm("NOP");
		}

    }
	SID.v1.ctrl = 0;
    // Reset the SID master volume
	SID.amp = 0;
}

void Play_Sound()
{
	unsigned char i;
    // Configure the SID voice 1
    SID.v1.ctrl = 0x41; // Pulse waveform
    SID.v1.freq = 0x1000; // Set a constant frequency
    SID.v1.ad = 0x00; // Attack = 0, Decay = 0
    SID.v1.sr = 0xF0; // Sustain = max, Release = 0
    SID.amp = 0xFF; // Turn on the master volume
    for (i = 0; i < 60; i++)
    {
        SID.v1.pw  = 0x0F; 
        while (VIC.rasterline != 0);
    }
	SID.v1.ctrl = 0;
    // Reset the SID master volume
	SID.amp = 0;
}

void switch_gamemode(unsigned char m);

#define KEEPITUP_GAME_STATUS 0
#define STOP_GAME_STATUS 1
#define FUCK_GAME_STATUS 2
#define END_GAME_STATUS 3

const unsigned char status_level1[] =
{ 
	FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS,
	END_GAME_STATUS, END_GAME_STATUS 
};

unsigned char status;
unsigned char delay, c;
unsigned short time_game;
unsigned char k, game_mode, text_progress;

char ingame_quote[5][5] =
{
	"GO\x24\x24",
	"STOP",
	"FUCK",
	"END\x24",
};

/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

static unsigned char bcd[4];

#define add_score(add) \
	bcd[2] += add; \
	if (bcd[2] > 9) { \
		bcd[1]+= 1; \
		bcd[2] = 0; } \
	if (bcd[1] > 9) { \
		bcd[0]+= 1; \
		bcd[1] = 0; } \

#define decrease_score(dec) \
	bcd[2] -= dec; \
	if (bcd[2] == 255) \
	{ \
		bcd[1]-= 1; \
		bcd[2] = 9; \
	} \
	if (bcd[1] == 255) \
	{ \
		bcd[0]-= 1; \
		bcd[1] = 9; \
	} \


#define SET_X_Y_TEXT(x, y) ((320 * y) + (x * 16))

static inline void Show_Score()
{
	memcpy((uint8_t*)MCB_BITMAP + SET_X_Y_TEXT(8,23), numberoffset[bcd[0]], 16);
	memcpy((uint8_t*)MCB_BITMAP + SET_X_Y_TEXT(9,23), numberoffset[bcd[1]], 16);
	memcpy((uint8_t*)MCB_BITMAP + SET_X_Y_TEXT(10,23), numberoffset[bcd[2]], 16);	
}

int main(void) {
	setup_bitmap_multi();
	switch_gamemode(0);
    while(1)
    {
		// Vsync
		while (VIC.rasterline != 0);
		
		if (delay < 254) delay++;
		
		switch(game_mode)
		{
			default:
			if ((read_joystick(0) || read_joystick(1)) && delay > 20)
			{
				switch_gamemode(jump[game_mode]);
			}
			break;
			case 1:
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (status_level1[status] == STOP_GAME_STATUS)
					{
						if (!(bcd[2] == 0 && bcd[0] == 0 && bcd[1] == 0)) 
						{
							decrease_score(1);
						}
					}
					else
					{
						add_score(1);
					}
					
					FRAME_CURRENT++;
					if (FRAME_CURRENT > 4) FRAME_CURRENT = 0;
					switch(FRAME_CURRENT)
					{
						case 0:
						case 1:
							Draw_firstframe();
						break;
						case 2:
							Draw_frame2();
						break;
						case 3:
							Draw_frame3();
						break;
						case 4:
							Draw_frame4();
						break;
					}
					Show_Score();
					time_game+= 4;
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (read_joystick(0) || read_joystick(1))
					{
						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > 60*5)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						Print_text(ingame_quote[status_level1[status]], SET_X_Y_TEXT(0,22));
					}
				}
			

				
			break;
			case 3:
				if ((read_joystick(0) || read_joystick(1)) && delay > 30)
				{
					text_progress++;
					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						memset((void*)MCB_BITMAP+6400, 0, 1600);
						switch(text_progress)
						{
							case 1:
								Print_text("THE PERFECT VICTIM  FOR MY GAMES\x24", SET_X_Y_TEXT(0,22));
							break;
							case 2:
								Print_text("IM RIKUTO I OFFER   YOU THE CHANCE TO   PROVE YOUR WORTH", SET_X_Y_TEXT(0,22));
								Play_PCM(rikuto_pcmdata, rikuto_pcmdata_length);
							break;
							case 3:
								Print_text("DONT BE SCARED IT   WILL BE FUN\x24", SET_X_Y_TEXT(0,22));
								Play_PCM(fun_pcmdata, fun_pcmdata_length);
							break;
							case 4:
								Print_text("YOUR CHALLENGE\x26     PUNISH THEM", SET_X_Y_TEXT(0,22));
							break;
						}
					}
					delay = 0;
				}
			break;
		}

		
		//Draw_frame2();
	}
    
	return 0;
}


void switch_gamemode(unsigned char m)
{
	VIC.bgcolor[0] = 0;

	memset((void*)MCB_BITMAP, 0, 8000);
	memset((void*)MCB_COLORS, 0, 2000);
	memset((void*)COLOR_RAM, 0, 40*25);
	FRAME_CURRENT = 0;
	text_progress = 0;
	delay = 0;
	time_game = 0;
	game_mode = m;
	Draw_firstframe_color();
	switch(m)
	{
		case 0:
			Print_text("CR INFRINGEMENT", (320 * 2) + 32);
			Print_text("BY GAMEBLABLA", (320 * 3) + 48);
			
			Print_text("PUSH FIRE TO START", (320 * 20) + 16);
		break;
		case 1:
			bcd[0] = 0;
			bcd[1] = 0;
			bcd[2] = 0;
			bcd[3] = '\0';
			Draw_firstframe();
			Print_text(ingame_quote[status_level1[status]], SET_X_Y_TEXT(0,22));
			Print_text("SCORE\x26", SET_X_Y_TEXT(0,23));
			Show_Score();
		break;
		case 2:
			memset((void*)COLOR_RAM, 11, 40*25);
			Draw_Bakura();
			if (bcd[0] < 1)
			{
				Print_text("GUESS YOU\x25LL STAY   HERE FOR A WHILE", SET_X_Y_TEXT(0,22));
			}
			else if (bcd[0] < 2)
			{
				Print_text("NOT BAD BUT YOU CAN DO BETTER\x24 OTHERWISEYOU\x25LL STAY HERE\x24", SET_X_Y_TEXT(0,22));
			}
			else
			{
				Print_text("CONGRATS\x24 GUESS I WILL LEAVE YOU ALONE   NOW\x24 BYE", SET_X_Y_TEXT(0,22));
			}
		break;
		case 3:
			memset((void*)COLOR_RAM, 11, 40*25);
			Draw_Bakura();
			Print_text("WHAT DO WE HAVE HEREYOU LOOK SO INNOCENT", SET_X_Y_TEXT(0,22));
			Play_PCM(what_pcmdata, what_pcmdata_length);
		break;
		case 4:
			Print_text("GOAL IS TO FUCK     ZHONGLI", (320 * 1) + 16);
			Print_text("WHEN IT SAYS FUCK   PUSH FIRE", (320 * 4) + 16);
			Print_text("WHEN IT DOES NOT    RELEASE", (320 * 8) + 16);
			Print_text("GOOD LUCK", (320 * 11) + 16);
		break;
	}
	
}
