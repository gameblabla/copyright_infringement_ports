
#include <stdio.h>
#include <games.h>
#include <stdlib.h>
#include <compress/zx7.h>
#include <graphics.h>
#include <string.h>
#include <conio.h>
#include <sound.h>

extern const unsigned char title_spr[];
extern const unsigned char bakura_spr[];
extern const unsigned char frame1zx7[];
extern const unsigned char frame2zx7[];
extern const unsigned char frame3zx7[];
extern const unsigned char frame4zx7[];
extern const unsigned char frame5zx7[];

unsigned char k, game_mode, text_progress;
#define FRAME_CURRENT text_progress

volatile unsigned char* display = (volatile unsigned char*) 0xF080;
volatile unsigned char* udg = (volatile unsigned char*) 0xFE00;

void switch_gamemode(unsigned char mode)  __z88dk_fastcall;

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
unsigned char i, c;
#define delay i
unsigned char time_game;

const char ingame_quote[5][12] =
{
	"KEEP IT UP!",
	"STOP       ",
	"FUCK HIM ! ",
	"FINISH !   ",
};


const unsigned char text_minigame_1[] = 
{
	"Your goal is to FUCK Zhongli! You must follow the instructions to get more points. Penetrate him by pressing a key then pull out by releasing it. Good luck !"
};

const unsigned char* const frame_porn_pattern[]=
{
	frame1zx7, frame2zx7, frame3zx7, frame4zx7, frame5zx7,
};

/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

static unsigned char bcd[3];

#define myownprint(str, size, x, y) memcpy(display + (x + (y << 6)), str, size);

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


void main()  __z88dk_fastcall
{
	memset(udg + 210, 0xFF, 32);
	switch_gamemode(0);
#ifdef SOUND_SUPPORT
	bit_open();
#endif
	while (1)
	{
		k = getk();
		switch(game_mode)
		{
			default:
				if (k > 0)
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
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (k > 0)
					{
#ifdef SOUND_SUPPORT
						bit_fx(4);
#endif
						FRAME_CURRENT = 1;
					}
				}
				
				// Make sure to put it before, not after
				dzx7_standard(frame_porn_pattern[FRAME_CURRENT], display);
				
				myownprint("SCORE", 5, 0, 28);
				myownprint(bcd, 3, 10, 28);
				
				myownprint(ingame_quote[status_level1[status]], 11, 48, 28);
				
				if (time_game > 70)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					time_game = 0;
					status++;
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
						memset(display + 512*3+128, 32, 256);
						
						switch(text_progress)
						{
							case 1:
								myownprint("You're going to be the perfect victim for my games!", 51, 0, 26);
							break;
							case 2:
								myownprint("I'm Rikuto, i offer you the chance to prove your worth in a trial", 65, 0, 26);
							break;
							case 3:
								myownprint("Don't be scared, it'll be fun!", 30, 0, 26);
							break;
							case 4:
								myownprint("Your challenge will be this: you will be on the other end and punish them", 73, 0, 26);
							break;
						}
					}
				}
			break;
		}
	}
}

void switch_gamemode(unsigned char mode)  __z88dk_fastcall
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;
	memset(display, 32, 2048);
	switch(mode)
	{
		case 0:
#ifdef SOUND_SUPPORT
			bit_fx4(3);
#endif
			putsprite(0,14, 0, title_spr);
			myownprint("COPYRIGHT INFRINGEMENT", 22, 5, 25);
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
		break;
		case 2:
			putsprite(0, 0, 0, bakura_spr);
			myownprint("Your SCORE was ", 14, 0, 0);
			myownprint(bcd, 3, 15, 0);
			
			if (bcd[0] < 48+1)
			{
				myownprint("Wow, you actually suck! Guess you'll stay there for a while. Haha!", 66, 0, 26);
#ifdef SOUND_SUPPORT
				bit_fx(5);
#endif
			}
			else if (bcd[0] < 48+2)
			{
				myownprint("Not bad but you can do better! Otherwise you can't leave this place!", 28, 0, 26);
#ifdef SOUND_SUPPORT
				bit_fx(5);
#endif
			}
			else
			{
				
				myownprint("Haha, congrats! I'll leave you alone... For now, anyway. I'll see you later, bye for now", 88, 0, 26);
#ifdef SOUND_SUPPORT
				bit_fx(7);
#endif
			}
		break;
		case 3:
			putsprite(0, 0, 0, bakura_spr);
			myownprint("Oh my, what do we have here? You look so innocent", 49, 0, 26);
#ifdef SOUND_SUPPORT
			bit_fx4(0);
#endif
		break;
		case 4:
			myownprint(text_minigame_1, sizeof(text_minigame_1)-1, 0, 0);
#ifdef SOUND_SUPPORT
			bit_fx4(5);
#endif
		break;
	}
	
}

