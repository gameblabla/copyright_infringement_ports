#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cc65.h>
#include <conio.h>
#include <ctype.h>
#include <joystick.h>

#include <atmos.h>

#include "img.h"

#define poke(addr, value) (*(volatile uint8_t *)(addr) = (value))

/**
BIOS sound routines
void atmos_explode (void);
void atmos_ping (void);
void atmos_shoot (void);
void atmos_tick (void);
void atmos_tock (void);
void atmos_zap(void);
*/

volatile void* display = (volatile void*) 0xA000;

#define HIRES_START 0xA000
#define HIRES_SIZE  8000
#define ORIC1_STATUS_ADDR (*(volatile uint8_t *)0x026A)

/*
 * These were taken from CC65-Chess
*/

void hires_DrawCharA(void);
#define ROP_CPY                 0x40
#define ROP_INV                 0xc0
#define CHAR_HEIGHT             8

extern unsigned char hires_xpos;
extern unsigned char hires_ypos;
extern unsigned char hires_xsize;
extern unsigned char hires_ysize;
extern unsigned char hires_rop;
extern unsigned char hires_clr;
extern unsigned char* hires_src;
extern char hires_CharSet[96][CHAR_HEIGHT];

void plat_DrawChar(unsigned char xpos, unsigned char ypos,  unsigned char rop, char c)
{
	hires_xpos = xpos;
	hires_ypos = ypos;
	hires_rop = rop;
	hires_src = hires_CharSet[c-' '];
	hires_DrawCharA();
}

void plat_DrawString(unsigned char x, unsigned char y,  unsigned char rop, char *s)
{
	while(*s)
	{
		plat_DrawChar(x++,y, rop, *s++);
	}
}

void Clear_Text()
{
	plat_DrawString(0, 180, ROP_CPY, "                                       ");
	plat_DrawString(0, 188, ROP_CPY, "                                       ");	
}

unsigned char k, game_mode, text_progress;
#define FRAME_CURRENT text_progress
signed char diff;

void switch_gamemode(unsigned char mode) ;

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
unsigned char i;
unsigned short time_game;

const char ingame_quote[5][12] =
{
	"KEEP IT UP!",
	"STOP       ",
	"FUCK HIM ! ",
	"FINISH !   ",
};

/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

static unsigned char bcd[4];
static unsigned char hibcd[4];

#define DrawString(str, x, y) plat_DrawString(x, y, ROP_CPY, str);

#define add_score(add) \
	bcd[2] += add; \
	if (bcd[2] > 57) { \
		bcd[1]+= 1; \
		bcd[2] = 48; } \
	if (bcd[1] > 57) { \
		bcd[0]+= 1; \
		bcd[1] = 48; } \

#define decrease_score(dec) \
	bcd[2] -= dec; \
	if (bcd[2] < 48) \
	{ \
		bcd[1]-= 1; \
		bcd[2] = 57; \
	} \
	if (bcd[1] < 48) \
	{ \
		bcd[0]-= 1; \
		bcd[1] = 57; \
	} \


extern void hires();
extern void setup();
extern unsigned char getkey();

// Used to avoid having to multiply
const unsigned char *offset_frames[]=
{
	img,
	img + 5080*1,
	img + 5080*2,
	img + 5080*3,
	img + 5080*4
};

void main() 
{
	//__asm__ ("jsr $EC33");
	setup();
	// Disable blinking cursor and sound
	ORIC1_STATUS_ADDR = (ORIC1_STATUS_ADDR | 0x08) & 0xFE;
	// Zero out the lower portion too
	bzero(0xA000+8000, 128);
	
	hibcd[0] = 48;
	hibcd[1] = 48;
	hibcd[2] = 48;
	hibcd[3] = '\0';
	
	switch_gamemode(0);

	while (1)
	{
		k = getkey();
		switch(game_mode)
		{
			default:
				if (k > 0)
				{
					atmos_tock();
					switch_gamemode(jump[game_mode]);
				}
			break;
			case 1:
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (status_level1[status] == STOP_GAME_STATUS)
					{
						if (!(bcd[2] == 48 && bcd[0] == 48 && bcd[1] == 48)) 
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
					
					memcpy(display, offset_frames[FRAME_CURRENT], 5080);
					
					DrawString("SCORE", 0, 180);
					DrawString(bcd, 0, 188);
					time_game+= 128*3;
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (k > 0)
					{
						if (status_level1[status] != STOP_GAME_STATUS) atmos_shoot();
						else
						{
							atmos_explode();
						}
						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > 128*200)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						if (status_level1[status] != KEEPITUP_GAME_STATUS)
						{
							atmos_ping();
						}
						else
						{
							atmos_tock();
						}
						DrawString(ingame_quote[status_level1[status]], 0, 160);
					}
				}
			break;
			case 3:
				if (k > 0)
				{
					text_progress++;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						atmos_tick();
						Clear_Text();
						
						switch(text_progress)
						{
							case 1:
								DrawString("You're going to be the perfect victim", 0, 180);
								DrawString("for my games!", 0, 188);
							break;
							case 2:
								DrawString("I'm Rikuto, i offer you the chance to", 0, 180);
								DrawString("prove your worth in a trial", 0, 188);
							break;
							case 3:
								DrawString("Don't be scared, it'll be fun!", 0, 180);
							break;
							case 4:
								DrawString("Your challenge will be this: you'll be", 0, 180);
								DrawString("on the other end and punish them", 0, 188);
							break;
						}
					}
				}
			break;
		}
	}
}

void switch_gamemode(unsigned char mode)
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;
	
	switch(mode)
	{
		case 0:
			hires();
			DrawString("COPYRIGHT INFRINGEMENT", 8, 100);
			DrawString("BY GAMEBLABLA", 12, 120);
			
			DrawString("HISCORE", 0, 200-8-8);
			DrawString(hibcd, 0, 200-8);
			
			DrawString("PRESS KEY TO START", 10, 180);
			__asm__ ("jsr _DrawTitle");
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
			memcpy(display, img, 5080);
			DrawString(ingame_quote[status_level1[status]], 0, 160);
		break;
		case 2:
			__asm__ ("jsr _DrawBakura");
			DrawString("Your SCORE was ", 0, 0);
			DrawString(bcd, 15, 0);
			
			//Canonical approach, thanks calcmaniac
			diff = bcd[0] - hibcd[0];
			if (diff == 0)
				diff = bcd[1] - hibcd[1];
			if (diff == 0)
				diff = bcd[2] - hibcd[2];
				
			if (diff > 0)
			{
				hibcd[0] = bcd[0];
				hibcd[1] = bcd[1];
				hibcd[2] = bcd[2];
			}
			
			Clear_Text();
			
			if (bcd[0] < 48+3)
			{
				atmos_explode();
				DrawString("Guess you'll stay there for a while", 0, 180);
			}
			else if (bcd[0] < 48+4)
			{
				atmos_explode();
				DrawString("Not bad but you can do better!", 0, 180);
				DrawString("Otherwise you can't leave this place!", 0, 188);
			}
			else
			{
				atmos_zap();
				DrawString("Congrats! I'll leave you alone.", 0, 180);
				DrawString("This won't be the last of me!", 0, 188);
			}
		break;
		case 3:
			__asm__ ("jsr _DrawBakura");
			Clear_Text();
			DrawString("Oh my, what do we have here?", 0, 180);
			DrawString("You look so innocent", 0, 188);
		break;
		case 4:
			bzero(0xA000, 8000);
			DrawString("Your goal is to FUCK Zhongli!", 0, 8);
			DrawString("Follow the instructions to get points", 0, 16);
			DrawString("Penetrate him by pressing a key", 0, 24);
			DrawString("Good luck !", 0, 64);
		break;
	}
	
}

