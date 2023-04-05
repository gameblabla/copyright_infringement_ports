#include <ti/getcsc.h>
#include <compression.h>
#include <graphx.h>
#include <ti/screen.h>
#include <stdlib.h>
#include <keypadc.h>
#include <fileioc.h>

#define ARCHIVE_APP 1

/* Include the converted graphics appvar file */
#include "gfx/VARGCI.h"

unsigned char k, game_mode, text_progress;
unsigned char FRAME_CURRENT;

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
unsigned char i, c;
unsigned char delay;
unsigned long time_game;

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

static unsigned short score;
static unsigned short highscore;

#define add_score(add) score += add;

#define decrease_score(dec) score -= dec;

kb_key_t key;
uint8_t prevkey;
gfx_sprite_t *mainimage;
uint8_t filesave;

#ifndef ARCHIVE_APP
const unsigned char* frame_anim[] =
{
	frame1_compressed, frame2_compressed, frame3_compressed, frame4_compressed, frame5_compressed
};
#endif

int main() 
{
	os_ClrHome();
	#ifdef ARCHIVE_APP
    if (VARGCI_init() == 0)
    {
		os_PutStrFull( "Missing VARGCI file");
		os_PutStrFull( "This game needs it to run");
		while (!os_GetCSC());
        return 1;
    }
    #endif
    gfx_Begin();
    gfx_SetDrawBuffer();
	gfx_SetPalette(global_palette, sizeof_global_palette, 0);
	
    gfx_SetTextScale(1, 1);
	gfx_SetTextBGColor(0);
	gfx_SetTextFGColor(1);
	
	mainimage = gfx_MallocSprite(160, 100);
	
	highscore = 0;
	
	filesave = ti_Open("VARCIH", "r");
	if (filesave != 0)
	{
		ti_Read(&highscore, sizeof(highscore), 1, filesave);
		ti_Close(filesave);
	}
	
	switch_gamemode(0);

	while (kb_Data[6] != kb_Clear)
	{
		kb_Scan();
		key = kb_Data[1];

		switch(game_mode)
		{
			case 0:
				delay++;
				if (kb_AnyKey() && delay > 60)
				{
					switch_gamemode(jump[game_mode]);
				}
				
				gfx_ZeroScreen();
			
				gfx_PrintStringXY("COPYRIGHT INFRINGEMENT", 8 * 8, 100);
				gfx_PrintStringXY("BY GAMEBLABLA", 12 * 8, 120);
				
				gfx_PrintStringXY("HISCORE", 0, 200-8-8);
				gfx_SetTextXY(0,200-8);
				gfx_PrintUInt(highscore, 5);
				
				gfx_PrintStringXY("PRESS KEY TO START", 10 * 8, 180);
				
				gfx_BlitBuffer();
			break;
			case 1:
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (status_level1[status] == STOP_GAME_STATUS)
					{
						if (!(score == 0)) 
						{
							decrease_score(1);
						}
					}
					else
					{
						add_score(1);
					}
					
					delay++;
					
					if (delay > 0)
					{
						FRAME_CURRENT++;
						if (FRAME_CURRENT > 4) FRAME_CURRENT = 0;
						
						#ifdef ARCHIVE_APP
						zx0_Decompress(mainimage, VARGCI_appvar[1+FRAME_CURRENT]);
						#else
						zx0_Decompress(mainimage, frame_anim[FRAME_CURRENT]);
						#endif
						delay = 0;
					}
					time_game+= 2;
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (kb_AnyKey())
					{
						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > 128)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
					}
				}
				
					
				gfx_ZeroScreen();
				gfx_ScaledSprite_NoClip(mainimage, 0, 0, 2, 2);
				gfx_PrintStringXY("SCORE", 0, 200);
				gfx_SetTextXY(0,208);
				gfx_PrintUInt(score, 5);
				gfx_PrintStringXY(ingame_quote[status_level1[status]], 160, 200);
				
				gfx_BlitBuffer();
			break;
			case 3:
				delay++;
				if (kb_AnyKey() && delay > 10)
				{
					text_progress++;
					delay = 0;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
				}
				
				gfx_ZeroScreen();
				gfx_ScaledSprite_NoClip(mainimage, 0, 0, 2, 2);
				switch(text_progress)
				{
					case 0:
						gfx_PrintStringXY("Oh my, what do we have here?", 0, 200);
						gfx_PrintStringXY("You look so innocent", 0, 208);
					break;
					case 1:
						gfx_PrintStringXY("You're going to be the perfect victim", 0, 200);
						gfx_PrintStringXY("for my games!", 0, 208);
					break;
					case 2:
						gfx_PrintStringXY("I'm Rikuto, i offer you the chance to", 0, 200);
						gfx_PrintStringXY("prove your worth in a trial", 0, 208);
					break;
					case 3:
						gfx_PrintStringXY("Don't be scared, it'll be fun!", 0, 200);
					break;
					case 4:
						gfx_PrintStringXY("Your challenge will be this: you'll be", 0, 200);
						gfx_PrintStringXY("on the other end and punish them", 0, 208);
					break;
				}
				
				gfx_BlitBuffer();
			break;
			// Results
			case 2:
				delay++;
				if (kb_AnyKey() && delay > 60)
				{
					switch_gamemode(jump[game_mode]);
				}
				
				gfx_ZeroScreen();
				
				gfx_ScaledSprite_NoClip(mainimage, 0, 0, 2, 2);
				
				gfx_PrintStringXY("Your SCORE was ", 0, 0);
				gfx_SetTextXY(104,0);
				gfx_PrintUInt(score, 5);

				if (score < 1)
				{
					gfx_PrintStringXY("Guess you'll stay there for a while...", 0, 200);
					gfx_PrintStringXY("Hahaha !", 0, 208);
				}
				else if (score < 2)
				{
					gfx_PrintStringXY("Not bad but you can do better!", 0, 200);
					gfx_PrintStringXY("Otherwise you can't leave this place!", 0, 208);
				}
				else
				{
					gfx_PrintStringXY("Congrats! I'll leave you alone.", 0, 200);
					gfx_PrintStringXY("This won't be the last of me!", 0, 208);
				}
				
				gfx_BlitBuffer();
			break;
			// Instructions
			case 4:
				delay++;
				if (kb_AnyKey() && delay > 10)
				{
					switch_gamemode(jump[game_mode]);
				}
			
				gfx_ZeroScreen();
				
				gfx_PrintStringXY("Your goal is to FUCK Zhongli!", 16, 16);
				gfx_PrintStringXY("Follow the instructions to get points", 16, 32);
				gfx_PrintStringXY("Penetrate him by pressing a key", 16, 48);
				gfx_PrintStringXY("then pull out by releasing it.", 16, 64);
				gfx_PrintStringXY("Good luck !", 16, 128);
			
				gfx_BlitBuffer();
			break;
		}
		
		prevkey = key;
	}
	
	free(mainimage);
	
	gfx_End();
	return 0;
}

void switch_gamemode(unsigned char mode)
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;
	delay = 0;
	FRAME_CURRENT = 0;
	
	gfx_ZeroScreen();

	switch(mode)
	{
		case 0:


		break;
		case 1:
			score = 0;
			#ifdef ARCHIVE_APP
			zx0_Decompress(mainimage, VARGCI_appvar[1+FRAME_CURRENT]);
			#else
			zx0_Decompress(mainimage, frame_anim[FRAME_CURRENT]);
			#endif
		break;
		case 2:
			zx0_Decompress(mainimage, yami_compressed);
			if (score > highscore)
			{
				highscore = score;
			}
			filesave = ti_Open("VARCIH", "w");
			if (filesave != 0)
			{
				ti_Write(&highscore, sizeof(highscore), 1, filesave);
				ti_Close(filesave);
			}
		break;
		case 3:
			zx0_Decompress(mainimage, yami_compressed);
		break;
		case 4:


		break;
	}
	
}

