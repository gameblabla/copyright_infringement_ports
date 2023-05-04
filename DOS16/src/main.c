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
#include <malloc.h>
#include <unistd.h>
#include <stdint.h>

#include "graph.h"
#include "music.h"
#include "sound.h"
#include "generic.h"
#include "keyboard.h"

BITMAP bmp[4];
VideoDevice mainvideo;
MusicDevice mainmusic;
SoundDevice mainsound;
struct WaveData *Voice;

uint8_t color_text = 255;

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

const char ingame_quote[5][5] =
{
	"GO!!",
	"STOP",
	"FUCK",
	"END!",
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
unsigned char action_status, snddef;
unsigned char gpu_mode;

struct WaveData *sound_voices[2];

const unsigned char sound_files[4][7][10] =
{
	{
		"SP1.WAV", 
		"SP2.WAV", 
		"SP3.WAV", 
		"SP4.WAV", 
		"SP5.WAV", 
		"FUCK.WAV", 
		"FUCK2.WAV"
	},
	{
		"SP1.ADW", 
		"SP2.ADW", 
		"SP3.ADW", 
		"SP4.ADW", 
		"SP5.ADW", 
		"FUCK.ADW", 
		"FUCK2.ADW"
	},
	{
		"SP1.PCK", 
		"SP2.PCK", 
		"SP3.PCK", 
		"SP4.PCK", 
		"SP5.PCK", 
		"FUCK.PCK", 
		"FUCK2.PCK"
	},
	{
		"SP1.TDY", 
		"SP2.TDY", 
		"SP3.TDY", 
		"SP4.TDY", 
		"SP5.TDY", 
		"FUCK.TDY", 
		"FUCK2.TDY"
	},
};

const unsigned char frames_files[6][8][13] =
{
	{
		"TDYTIT.SIF", 
		"TDYRIK.SIF", 
		"TDYRIKL.SIF", 
		"TDYF1.SIF", 
		"TDYF2.SIF", 
		"TDYF3.SIF", 
		"TDYF4.SIF"
	},
	{
		"VGATIT.SIF", 
		"VGARIK.SIF", 
		"VGARIKL.SIF", 
		"VGAF1.SIF", 
		"VGAF2.SIF", 
		"VGAF3.SIF", 
		"VGAF4.SIF"
	},
	{
		"HGCTIT.SIF", 
		"HGCRIK.SIF", 
		"HGCRIKL.SIF", 
		"HGCF1.SIF", 
		"HGCF2.SIF", 
		"HGCF3.SIF", 
		"HGCF4.SIF"
	},
	{
		"CGATIT.SIF", 
		"CGARIK.SIF", 
		"CGARIKL.SIF", 
		"CGAF1.SIF", 
		"CGAF2.SIF", 
		"CGAF3.SIF", 
		"CGAF4.SIF"
	},
	{
		"EGATIT.SIF", 
		"EGARIK.SIF", 
		"EGARIKL.SIF", 
		"EGAF1.SIF", 
		"EGAF2.SIF", 
		"EGAF3.SIF", 
		"EGAF4.SIF"
	},
	{
		"CNTTIT.SIF", 
		"CNTRIK.SIF", 
		"CNTRIKL.SIF", 
		"CNTF1.SIF", 
		"CNTF2.SIF", 
		"CNTF3.SIF", 
		"CNTF4.SIF"
	},
};

const unsigned char pal_files[6][4][13] =
{
	{
		"VGATIT.PAL", 
		"VGARIK.PAL", 
		"VGAF1.PAL", 
	},
	{
		"VGATIT.PAL", 
		"VGARIK.PAL", 
		"VGAF1.PAL", 
	},
	{
		"VGATIT.PAL", 
		"VGARIK.PAL", 
		"VGAF1.PAL", 
	},
	{
		"VGATIT.PAL", 
		"VGARIK.PAL", 
		"VGAF1.PAL", 
	},
	{
		"VGATIT.PAL", 
		"VGARIK.PAL", 
		"VGAF1.PAL", 
	},
	{
		"VGATIT.PAL", 
		"VGARIK.PAL", 
		"VGAF1.PAL", 
	},
};

unsigned short vid_width, vid_height;
uint16_t text_line1, text_line2 ;
uint8_t musdef;
uint8_t nodecompress_title;

void Display_Ingame_text()
{
	mainvideo.Print_text("SCORE", 0, text_line1, color_text);
	mainvideo.Print_text(ingame_quote[status_level1[status]], 0, text_line2, color_text);
	mainvideo.Print_text(bcd, 64, text_line1, color_text);
}



int main(int argc,char **argv) 
{
	unsigned char done;
	char valid_choice, choice;
	
	snddef = 0;
	gpu_mode = 1;
	musdef = 0;
	
    puts("Welcome to the Configuration Menu!");

    do {
        puts("\nSelect Video Mode:");
        puts("0 = Tandy");
        puts("1 = MCGA/VGA");
        puts("2 = Hercules");
        puts("3 = CGA 4-colors");
        puts("4 = EGA");
        puts("5 = CGA Composite");
        puts("Enter your choice: ");
        scanf("%d", &choice);
        valid_choice = (char)choice >= 0 && (char)choice <= 5;
        if (!valid_choice) puts("Invalid choice. Please try again.");
        gpu_mode = choice;
        
    } while (!valid_choice);

    do {
        puts("\nSelect Music Driver:");
        puts("0 = No music");
        puts("1 = OPL2 Adlib/SB");
        puts("Enter your choice: ");
        scanf("%d", &choice);
        valid_choice = (char)choice >= 0 && (char)choice <= 1;
        if (!valid_choice) puts("Invalid choice. Please try again.");
        musdef = choice;
        
    } while (!valid_choice);

    do {
        puts("\nSelect Sound Driver:");
        puts("0 = Sound Blaster");
        puts("1 = Adlib OPL2");
        puts("2 = PC Speaker");
        puts("3 = Tandy PSG");
        puts("4 = No sound effects");
        puts("Enter your choice: ");
        scanf("%d", &choice);
        valid_choice = (char)choice >= 0 && (char)choice <= 4;
        if (!valid_choice) puts("Invalid choice. Please try again.");
        snddef = choice;
    } while (!valid_choice);


	Init_Machine();

	switch(snddef)
	{
		case 0:
			mainsound = SBDMA_device;
		break;
		case 1:
			mainsound = ADLIBPCM_device;
		break;
		case 2:
			mainsound = PCK_device;
		break;
		case 3:
			mainsound = Tandy_device;
		break;
		default:
			mainsound = NULLWAV_device;
		break;
	}
	
	mainsound.Sound_Init();
	
	switch(musdef)
	{
		default:
			mainmusic = NULLMUS_device;
		break;
		case 1:
			mainmusic = IMF_device;
		break;
	}
	
	mainmusic.Init_Music(0);
	
	vid_width = 320;
	vid_height = 200;	
	text_line1 = 180;
	text_line2 = 188;
	nodecompress_title = 1;
	
	switch(gpu_mode)
	{
		case 0:
			mainvideo = Normal_Tandy;
			mainvideo.SetVideoMode(320, 200, 0, argc, argv);
		break;
		case 1:
			mainvideo = Normal_MCGA;
			mainvideo.SetVideoMode(320, 200, 0, argc, argv);
		break;
		case 2:
			mainvideo = Normal_HGC;
			mainvideo.SetVideoMode(720, 348, 0, argc, argv);
			vid_width = 720;
			vid_height = 348;
			text_line1 = 320;
			text_line2 = 328;
			color_text = 0xFF;
		break;
		case 3:
			mainvideo = Normal_CGA;
			mainvideo.SetVideoMode(320, 200, 0, argc, argv);
		break;
		case 4:
			mainvideo = Normal_EGA;
			mainvideo.SetVideoMode(320, 200, 0, argc, argv);
			nodecompress_title = 0;
		break;
		case 5:
			mainvideo = Normal_CGA;
			mainvideo.SetVideoMode(160, 200, 1, argc, argv);
		break;
	}
	
	
	
	ismusicdriver = 0;
	voices_enabled = 0;
	music = 0;
	
	hibcd[0] = 48;
	hibcd[1] = 48;
	hibcd[2] = 48;
	hibcd[3] = '\0';
	
	Keyboard_Init();
	
	switch_gamemode(0);
	done = 0;

	while (!done)
	{
		delay_game++;
		//Get_Input();
		
		if (keystate[KEY_ESC] == 1)
		{
			done = 1;
		}
		
		switch(game_mode)
		{
			default:
				if (keystate[KEY_SPACE] && delay_game > 60)
				{
					switch_gamemode(jump[game_mode]);
					keystate[KEY_SPACE] = 0;
				}
			break;
			case 1:
				time_game+=1;

				// If Standby
				if (action_status == 0)
				{
					frame_offset = 0;
					if (keystate[KEY_SPACE] == 1)
					{
						// This refuses to play for whatever reason
						/*if (toggle_sound_status == 1)
						{
							mainsound.Sound_Play(sound_voices[1], 0);
							toggle_sound_status = 0;
						}
						if (toggle_sound_status == 0)*/
						{
							mainsound.Sound_Play(sound_voices[0], 0);
						}
						
						action_status = 1;
						keystate[KEY_SPACE] = 0;
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
						
						mainvideo.DrawBMP_static(&bmp[FRAME_CURRENT], 0, 0, 0);
						Display_Ingame_text();
						mainvideo.FlipVideo();
						mainvideo.Vsyncwait();
						mainvideo.Vsyncwait();
						
						FRAME_CURRENT++;
						if (FRAME_CURRENT > 3)
						{
							FRAME_CURRENT = 0;
							action_status = 0;
							mainvideo.DrawBMP_static(&bmp[FRAME_CURRENT], 0, 0, 0);
							Display_Ingame_text();
							mainvideo.FlipVideo();
							mainvideo.Vsyncwait();
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
						mainvideo.Print_text(ingame_quote[status_level1[status]], 0, text_line2, color_text);
						toggle_sound_status = 1;
					}
				}
				

				mainvideo.FlipVideo();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
				mainvideo.Vsyncwait();
			break;
			case 3:
				//
				if (keystate[KEY_SPACE] && delay_game > 30)
				{
					text_progress++;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						
						switch(text_progress)
						{
							case 1:
								mainvideo.LoadBMP(frames_files[gpu_mode][2], &bmp[0], vid_width, vid_height, 1);
								mainvideo.DrawBMP_compressed(&bmp[0], 0, 0);
								mainvideo.Print_text("You're going to be the perfect", 0, text_line1, color_text);
								mainvideo.Print_text("victim for my games!", 0, text_line2, color_text);
							break;
							case 2:
								mainvideo.LoadBMP(frames_files[gpu_mode][1], &bmp[0], vid_width, vid_height, 1);
								mainvideo.DrawBMP_compressed(&bmp[0], 0, 0);
								mainvideo.Print_text("I'm Rikuto, i offer you the chance", 0, text_line1, color_text);
								mainvideo.Print_text("to prove your worth in a trial.", 0, text_line2, color_text);
							break;
							case 3:
								mainvideo.LoadBMP(frames_files[gpu_mode][2], &bmp[0], vid_width, vid_height, 1);
								mainvideo.DrawBMP_compressed(&bmp[0], 0, 0);
								mainvideo.Print_text("Don't be scared, it'll be fun!", 0, text_line1, color_text);
							break;
							case 4:
								mainvideo.LoadBMP(frames_files[gpu_mode][1], &bmp[0], vid_width, vid_height, 1);
								mainvideo.DrawBMP_compressed(&bmp[0], 0, 0);
								mainvideo.Print_text("Your challenge is this: You'll be on", 0, text_line1, color_text);
								mainvideo.Print_text("the other end and punish them!", 0, text_line2, color_text);
							break;
						}
						mainsound.Sound_Unload(sound_voices[0], 0);
						mainsound.Sound_Load(sound_files[snddef][text_progress], sound_voices[0], 0);
						mainsound.Sound_Play(sound_voices[0], 0);
					}
					
					mainvideo.FlipVideo();
					mainvideo.Vsyncwait();
					delay_game = 0;
					keystate[KEY_SPACE] = 0;
				}


			break;
		}
		

		mainvideo.Vsyncwait();
		mainsound.Sound_Update();
	}
	
	mainvideo.FreeBMP(&bmp[0]);
	mainvideo.FreeVideo();

	return 0;
}

void switch_gamemode(unsigned char mode)
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	
	delay_game = 0;
	FRAME_CURRENT = 0;
	action_status = 0;
	toggle_sound_status = 0;
	frame_offset = 0;
	
	mainvideo.ClearVideo();
	mainmusic.Stop_Music();
	
	mainsound.Sound_Unload(sound_voices[0], 0);
	mainsound.Sound_Unload(sound_voices[1], 0);
	
	for(i=0;i<4;i++)
	{
		mainvideo.FreeBMP(&bmp[i]);
	}
	
	i = 0;
	
	switch(mode)
	{
		case 0:
			mainvideo.LoadBMP(frames_files[gpu_mode][0], &bmp[0], 320, 200, nodecompress_title);
			mainvideo.LoadPalette(pal_files[gpu_mode][0]);
			mainvideo.SetPalette();
			
			if (nodecompress_title == 0) mainvideo.DrawBMP_static(&bmp[0], 0, 0, 0);
			else
			{
				mainvideo.DrawBMP_compressed(&bmp[0], 0, 0);
			}
			
			mainvideo.Print_text_Center("COPYRIGHT INFRINGEMENT", 0, color_text);
			mainvideo.Print_text_Center("BY GAMEBLABLA", 16, color_text);

			mainvideo.Print_text("HISCORE", 0, text_line1, color_text);
			mainvideo.Print_text(hibcd, 0, text_line2, color_text);
			
			mainvideo.Print_text_Center("PRESS KEY TO START", text_line1-24, color_text);
			
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
			
			mainmusic.Load_Music("TENSE.IMF", 32000);
			mainmusic.Play_Music(1);
			
		break;
		case 1:
			mainvideo.Print_text("LOADING... PLEASE WAIT !", 0, 0, color_text);

			mainvideo.LoadBMP(frames_files[gpu_mode][3], &bmp[0], 320, text_line1, 0);
			mainvideo.LoadBMP(frames_files[gpu_mode][4], &bmp[1], 320, text_line1, 0);
			mainvideo.LoadBMP(frames_files[gpu_mode][5], &bmp[2], 320, text_line1, 0);
			mainvideo.LoadBMP(frames_files[gpu_mode][6], &bmp[3], 320, text_line1, 0);
			mainvideo.LoadPalette(pal_files[gpu_mode][2]);
			mainvideo.SetPalette();
			
			mainvideo.DrawBMP_static(&bmp[FRAME_CURRENT], 0, 0, 0);

			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
		
			Display_Ingame_text();
			mainsound.Sound_Load(sound_files[snddef][5], sound_voices[0], 0);
			//mainsound.Sound_Load(sound_files[snddef][6], sound_voices[1], 1);
			
			mainmusic.Load_Music("HEAT.IMF", 32000);
			mainmusic.Play_Music(1);

		break;
		case 2:
		
			mainvideo.LoadBMP("RIKUN.SIF", &bmp[0], 320, 200, 0);
			mainvideo.DrawBMP_static(&bmp[0], 0, 0, 0);
			mainvideo.LoadPalette(pal_files[gpu_mode][1]);
			mainvideo.SetPalette();
			
			mainvideo.Print_text("Your SCORE was ", 0, 0, color_text);
			mainvideo.Print_text(bcd, 17*8, 0, color_text);
			
			mainmusic.Load_Music("BRES.IMF", 32000);
			mainmusic.Play_Music(0);
			
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
				mainvideo.Print_text("Guess you'll stay there for a while", 0, text_line1, color_text);
			}
			else if (bcd[0] < 48+2)
			{
				mainvideo.Print_text("Not bad but you can do better!", 0, text_line1, color_text);
				mainvideo.Print_text("Otherwise you can't leave this place!", 0, text_line2, color_text);
			}
			else
			{
				mainvideo.Print_text("Congrats! I'll leave you alone.", 0, text_line1, color_text);
				mainvideo.Print_text("This won't be the last of me!", 0, text_line2, color_text);
			}
			mainvideo.FlipVideo(); mainvideo.Vsyncwait();
		break;
		case 3:

			mainmusic.Load_Music("OMNI.IMF", 32000);
			mainmusic.Play_Music(1);
			
			mainvideo.LoadPalette(pal_files[gpu_mode][1]);
			mainvideo.SetPalette();

			mainvideo.LoadBMP(frames_files[gpu_mode][1], &bmp[0], 320, 200, 1);
			mainvideo.DrawBMP_compressed(&bmp[0], 0, 0);

			
			mainvideo.Print_text("Oh my, what do we have here?", 0, text_line1, color_text);
			mainvideo.Print_text("You look so innocent...", 0, text_line2, color_text);
			
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
			

			mainsound.Sound_Load(sound_files[snddef][0], sound_voices[0], 0);
			mainsound.Sound_Play(sound_voices[0], 0);
			
		break;
		case 4:
			mainvideo.Print_text("Your goal is to SCREW Zhongli!", 0, 0, color_text);
			mainvideo.Print_text("Follow instructions to get points.", 0, 8, color_text);
			mainvideo.Print_text("GOOD LUCK !", 0, 16, color_text);
			mainvideo.Print_text("PRESS KEY TO START PLAYING", 0, 48, color_text);
			
			mainmusic.Load_Music("DETAILS.IMF", 32000);
			mainmusic.Play_Music(1);
		break;
	}
	
}

