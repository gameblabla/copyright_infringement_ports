#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <dos.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <go32.h>
#include <malloc.h>
#include <unistd.h>
#include <stdint.h>
#include <dpmi.h>

#include "graph.h"
#include "music.h"
#include "sound.h"
#include "input.h"

BITMAP bmp;
VideoDevice mainvideo;
MusicDevice mainmusic;
SoundDevice mainsound;
struct WaveData *Voice;

int ismusicdriver;

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
	"GO FOR IT  ",
	"FINISH !   ",
};

/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

static unsigned char bcd[4];
static unsigned char hibcd[4];
unsigned short delay_game;
unsigned char toggle_sound_status;
unsigned long frame_offset;

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


unsigned char music, voices_enabled;
unsigned char action_status;

void Check_Music_Driver()
{
	if (mainmusic.Init_Music(0) == 1)
	{
		ismusicdriver = 1;
	}

	if (ismusicdriver == 1)
	{
		mainsound = PMDPCM_device;
		if (mainsound.Sound_Init() == 1)
		{
			if (mainsound.Sound_Load("GAME.P86", Voice, 0) == 0)
			{
			
			}
		}
	 }
	 
	if (ismusicdriver == 0)
	{
		 mainmusic = NullMus_device;
		 mainsound = NullPCMdevice;
	}
}

#define PMD_Play_Voice(vc) mainsound.Sound_Play(Voice, vc);
#define PMD_Stop() mainsound.Sound_Play(Voice, 120);

int main(int argc,char **argv) 
{
	unsigned char done;
	
	mainvideo = Normal_PC9821_PGC;
	mainmusic = PMD_device;
	mainvideo.SetVideoMode(640, 400, 0, argc, argv);
	
	ismusicdriver = 0;
	voices_enabled = 0;
	music = 0;
	Check_Music_Driver();
	
	hibcd[0] = 48;
	hibcd[1] = 48;
	hibcd[2] = 48;
	hibcd[3] = '\0';
	
	switch_gamemode(0);
	done = 0;

	while (!done)
	{
		delay_game++;
		Get_Input();
		
		if (inputs[ESC_KEY] == 1)
		{
			done = 1;
		}
		
		switch(game_mode)
		{
			default:
				if (inputs[SPACE_KEY] && delay_game > 60)
				{
					switch_gamemode(jump[game_mode]);
				}
			break;
			case 1:
				time_game+=1;

				// If Standby
				if (action_status == 0)
				{
					frame_offset = 0;
					if (inputs[SPACE_KEY] == 1)
					{
						PMD_Play_Voice(5);
						
						if (toggle_sound_status == 1)
						{
							if (status_level1[status] == KEEPITUP_GAME_STATUS)
							{
								PMD_Play_Voice(6);
								toggle_sound_status = 0;
							}
						} 
						action_status = 1;
					}
				}
				if (action_status == 1)
				{
					if (status_level1[status] != END_GAME_STATUS)
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
						
						frame_offset = (640*360) * FRAME_CURRENT;
						mainvideo.Print_text(bcd, 10, 23);
						
						FRAME_CURRENT++;
						if (FRAME_CURRENT > 6)
						{
							FRAME_CURRENT = 0;
							action_status = 0;
						}
					}
				}
				
				if (time_game > 60)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						mainvideo.Print_text(ingame_quote[status_level1[status]], 0, 24);
						toggle_sound_status = 1;
					}
				}
				
				mainvideo.DrawBMP_static(&bmp, 0, 0, frame_offset);
				mainvideo.FlipVideo();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				
			break;
			case 3:
				//
				if (inputs[SPACE_KEY] && delay_game > 30)
				{
					text_progress++;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						mainvideo.ClearVideo();
						PMD_Play_Voice(text_progress);
						switch(text_progress)
						{
							case 1:
								frame_offset = 640*400;
								mainvideo.Print_text("You're going to be the perfect victim for my games!", 0, 22);
							break;
							case 2:
								frame_offset = 0;
								mainvideo.Print_text("I'm Rikuto, i offer you the chance to prove your worth in a trial.", 0, 22);
							break;
							case 3:
								frame_offset = 640*400;
								mainvideo.Print_text("Don't be scared, it'll be fun!", 0, 22);
							break;
							case 4:
								frame_offset = 0;
								mainvideo.Print_text("Your first challenge is going to be this:", 0, 22);
								mainvideo.Print_text("You'll be on the other end and punish them !", 0, 23);
							break;
						}
					}
					delay_game = 0;
				}
				mainvideo.DrawBMP_static(&bmp, 0, 0, frame_offset);
				mainvideo.FlipVideo(); mainvideo.Vsyncwait();
				

			break;
		}
		

		
	}
	
	mainvideo.FreeBMP(&bmp);
	mainvideo.FreeVideo();
	PMD_Stop();

	return 0;
}

void switch_gamemode(unsigned char mode)
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;
	delay_game = 0;
	FRAME_CURRENT = 0;
	action_status = 0;
	toggle_sound_status = 0;
	frame_offset = 0;
	
	mainvideo.ClearVideo();
	
	switch(mode)
	{
		case 0:
			if (ismusicdriver == 1)
			{
				if (mainmusic.Load_Music("TITLE.M", 1568) == 1)
				{
					mainmusic.Play_Music();
				}
			}
			
			unsigned char res = mainvideo.LoadBMP("TITLE.SIF", &bmp, 640, 400, 1);
			if (res) mainvideo.SetPalette();
			mainvideo.Print_text("COPYRIGHT INFRINGEMENT", 30, 0);
			mainvideo.Print_text("BY GAMEBLABLA", 34, 1);
			
			mainvideo.Print_text("HISCORE", 0, 20);
			mainvideo.Print_text(hibcd, 0, 21);
			
			mainvideo.Print_text("PRESS KEY TO START", 31, 22);
			
			if (res) mainvideo.DrawBMP_static(&bmp, 0, 0, 1);
			
			mainvideo.FlipVideo(); mainvideo.Vsyncwait();
		break;
		case 1:
			
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
			
			mainvideo.ClearVideo();
			mainvideo.Print_text("LOADING... PLEASE WAIT !", 1, 2);


			mainvideo.LoadBMP("PORN.SIF", &bmp, 640, 360, 1);
			mainvideo.SetPalette();
			
			mainvideo.ClearVideo();
			mainvideo.Print_text("SCORE", 0, 23);
			mainvideo.Print_text(bcd, 10, 23);
			mainvideo.Print_text(ingame_quote[status_level1[status]], 0, 24);
			mainmusic.Stop_Music();
		break;
		case 2:
		
			mainvideo.LoadBMP("RIKU.SIF", &bmp, 640, 400, 1);
			mainvideo.DrawBMP_static(&bmp, 0, 0, frame_offset);
			mainvideo.SetPalette();
			
			mainvideo.Print_text("Your SCORE was ", 0, 0);
			mainvideo.Print_text(bcd, 15, 0);
			
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
			
			if (bcd[0] == 48)
			{
				mainvideo.Print_text("Guess you'll stay there for a while", 0, 22);
			}
			else if (bcd[1] < 48+3)
			{
				mainvideo.Print_text("Not bad but you can do better!", 0, 22);
				mainvideo.Print_text("Otherwise you can't leave this place!", 0, 23);
			}
			else
			{
				if (mainmusic.Load_Music("WIN.M", 176) == 1)
				{
					mainmusic.Play_Music();
				}
				mainvideo.Print_text("Ugh !", 0, 22);
				mainvideo.Print_text("This won't be the last of me!", 0, 23);
			}
			mainvideo.FlipVideo(); mainvideo.Vsyncwait();
		break;
		case 3:
			mainvideo.LoadBMP("RIKU.SIF", &bmp, 640, 400, 1);
			mainvideo.SetPalette();
			
			mainmusic.Stop_Music();
			
			PMD_Play_Voice(0);
			
			mainvideo.Print_text("Oh my, what do we have here?", 0, 22);
			mainvideo.Print_text("You look so innocent.", 0, 23);
			
		break;
		case 4:
			if (mainmusic.Load_Music("MIDSCR.M", 299) == 1)
			{
				mainmusic.Play_Music();
			}
			mainvideo.Print_text("Your goal is to SCREW Zhongli!", 0, 22);
			mainvideo.Print_text("Follow the instructions to get points.", 0, 23);
			mainvideo.Print_text("PRESS KEY TO START PLAYING", 0, 24);
			PMD_Stop();
		break;
	}
	
}

