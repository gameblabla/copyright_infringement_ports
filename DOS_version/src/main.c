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

BITMAP bmp[10];
VideoDevice mainvideo;
MusicDevice mainmusic;
SoundDevice mainsound;

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

unsigned char frames_porn;

#if defined(__386__)
#define DATAFOLDER "386\\"
#else
#define DATAFOLDER "DATA\\"
#endif

const char *sndfiles[] = {"SP1", "SP2", "SP3", "SP4", "SP5", "FUCK", "FUCK2"};
const char *sif_files[] = {"TIT.SIF", "RIK.SIF", "RIKL.SIF", "F1.SIF", "F2.SIF", "F3.SIF", "F4.SIF", "F5.SIF", "F6.SIF", "F7.SIF", "F8.SIF", "F9.SIF"};
const char *palf[] = {"TIT.PAL", "RIK.PAL", "F1.PAL"};

#define num_sif_files (sizeof(sif_files) / sizeof(sif_files[0]))
#define num_snd_files (sizeof(sndfiles) / sizeof(sndfiles[0]))
#define num_pal_files (sizeof(palf) / sizeof(palf[0]))

unsigned char sound_files[num_snd_files][17];
unsigned char frames_files[num_sif_files][17];
unsigned char pal_files[num_pal_files][17];

unsigned short vid_width, vid_height;
uint16_t text_line1, text_line2 ;
uint8_t musdef;
uint8_t nodecompress_title;
uint8_t nodecompress_bakura;
uint8_t heavy_cpu;

void Display_Ingame_text()
{
	mainvideo.Print_text("SCORE", 0, text_line1, color_text);
	mainvideo.Print_text(ingame_quote[status_level1[status]], 0, text_line2, color_text);
	mainvideo.Print_text(bcd, 64, text_line1, color_text);
}

const char** ingametext;

const char* alt_ingametext[] =
{
	"(C) INFRINGEMENT",
	"BY GAMEBLABLA",
	"What's this here?",
	"You're so innocent",
	"Perfect for games!",
	"My games' victim!",
	"Hope you can",
	"prove your worth.",
	"Don't fear, have fun!",
	"Here's the task:",
	"Punish the other!",
	"Goal: SCREW Zhongli!",
	"Follow Instructions",
	"GOOD LUCK!",
	"SPACE TO PLAY",
	"LOADING, WAIT...",
	"Stay there a bit.",
	"Improve, do better!",
	"Else, can't leave!",
	"Ugh! Nicely done.",
	"I will return!",
};

const char* long_ingametext[] =
{
	"COPYRIGHT INFRINGEMENT",
	"BY GAMEBLABLA",
	"Oh my, what do we have here?",
	"You look so innocent...",
	"You're going to be the perfect",
	"victim for my games!",
	"I'm Rikuto, i offer you the chance",
	"to prove your worth in a trial.",
	"Don't be scared, it'll be fun!",
	"Your challenge is this: You'll be on",
	"the other end and punish them!",
	"Your goal is to SCREW Zhongli!",
	"Follow instructions to get points.",
	"GOOD LUCK !",
	"PRESS KEY TO START PLAYING",
	"LOADING... PLEASE WAIT!",
	"Guess you'll stay there for a while",
	"Not bad but you can do better!",
	"Otherwise you can't leave this place!",
	"Congrats! I'll leave you alone.",
	"This won't be the last of me!",
};

#define Display_Image(dec) \
	if (dec == 0) mainvideo.DrawBMP_static(&bmp[0], 0, 0, 0); \
	if (dec == 1) mainvideo.DrawBMP_compressed(&bmp[0], 0, 0); \

const char tryagain[] = "Invalid choice. Please try again.";
const char enterchoice[] = "Enter your choice :";

unsigned char Choice_Menu(unsigned char d)
{
	char valid_choice, choice;
	unsigned char max_c, c;
	do {
		switch(d)
		{
			case 0:
			
			puts("\nSelect Video Mode:");
			puts("0 = Tandy/PCJr 320x200");
			puts("1 = Tandy/PCJr 160x200");
			puts("2 = MCGA/VGA 320x200 256 Colors");
			puts("3 = Hercules");
			puts("4 = CGA 4-colors");
			puts("5 = EGA 320x200");
			puts("6 = CGA Composite");
			puts("7 = CGA B&W");
			puts("8 = CGA B&W Inverted");
			#if defined(__386__)
			puts("9 = Hercules InColor");
			puts("10 = VGA 800x600 16 colors");
			puts("11 = VGA 800x600 16 colors Special/SVGA");
			puts("12 = VGA 360x480 ModeX");
			puts("13 = VGA 320x240 ModeX");
			puts("14 = VGA 640x400 256 colors");
			max_c = 14;
			#else
			max_c = 8;
			#endif
			break;
			case 1:
			puts("\nSelect Music Driver:");
			puts("0 = No music");
			puts("1 = OPL2 Adlib/SB");
			max_c = 1;
			break;
			case 2:
			puts("\nSelect Sound Driver:");
			puts("0 = Sound Blaster");
			puts("1 = Adlib OPL2");
			puts("2 = PC Speaker");
			puts("3 = Tandy PSG");
			puts("4 = No sound effects");
			max_c = 4;
			break;
			#if !defined(__386__)
			case 3:
			puts("\nUse Low memory mode (less frames) ?:");
			puts("0 = No");
			puts("1 = Yes");
			max_c = 1;
			break;
			#endif
		}
		
        if (scanf("%d", &choice) != 1)
        {
            while ((c = getchar()) != '\n' && c != EOF);
            valid_choice = 0;
            puts(tryagain);
        } else {
            valid_choice = choice >= 0 && choice <= max_c;
            if (!valid_choice) puts(tryagain);
        }
		
	} while (!valid_choice);
	
	return choice;
}

int main(int argc,char **argv) 
{
	unsigned char done;
	unsigned char buttons;
	char choice;
	char tmp[6];
	
	// Sane defaults
	snddef = 4;
	gpu_mode = 4;
	musdef = 0;
	frames_porn = 4;
	ingametext = long_ingametext;
	
	puts("\nCOPYRIGHT INFRINGEMENT SETUP");

    gpu_mode = Choice_Menu(0);
    musdef = Choice_Menu(1);
	snddef = Choice_Menu(2);
	
#if defined(__386__)
	choice = Choice_Menu(3);
	frames_porn = 9;
	if (choice)
	{
		frames_porn = 4;
	}
#else
	frames_porn = 4;
#endif


	Init_Machine();

	switch(snddef)
	{
		case 0:
			mainsound = SBDMA_device;
			strcpy(tmp, ".WAV");
		break;
		case 1:
			mainsound = ADLIBPCM_device;
			strcpy(tmp, ".ADW");
		break;
		case 2:
			mainsound = PCK_device;
			strcpy(tmp, ".PCK");
		break;
		case 3:
			mainsound = Tandy_device;
			strcpy(tmp, ".TDY");
		break;
		default:
			mainsound = NULLWAV_device;
		break;
	}
	
	for(i=0;i<num_snd_files;i++)
	{
		strcpy(sound_files[i], DATAFOLDER);
		strcat(sound_files[i], sndfiles[i]);
		strcat(sound_files[i], tmp);
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
	nodecompress_bakura = 1;
	heavy_cpu = 0;
	
	switch(gpu_mode)
	{
		case 0:
			mainvideo = Normal_Tandy;
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
			strcpy(tmp, "TDY");
		break;
		case 1:
			mainvideo = Normal_Tandy;
			mainvideo.SetVideoMode(160, 200, 0, argc, argv);
			vid_width = 160;
			vid_height = 200;
			strcpy(tmp, "LDY");
			ingametext = alt_ingametext;
		break;
		case 2:
			mainvideo = Normal_MCGA;
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
			nodecompress_title = 0;
			nodecompress_bakura = 0;
			strcpy(tmp, "VGA");
		break;
		case 3:
			mainvideo = Normal_HGC;
			mainvideo.SetVideoMode(720, 348, 0, argc, argv);
			vid_width = 720;
			vid_height = 348;
			text_line1 = 320;
			text_line2 = 328;
			color_text = 0xFF;
			strcpy(tmp, "HGC");
		break;
		case 4:
			mainvideo = Normal_CGA;
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
			strcpy(tmp, "CGA");
		break;
		case 5:
			mainvideo = Normal_EGA;
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
			#if defined(__386__)
			nodecompress_bakura = 0;
			#endif
			nodecompress_title = 0;
			strcpy(tmp, "EGA");
		break;
		case 6:
			mainvideo = Normal_CGA;
			mainvideo.SetVideoMode(160, 200, 1, argc, argv);
			strcpy(tmp, "CNT");
		break;
		case 7:
		case 8:
			mainvideo = Normal_CGA;
			if (gpu_mode == 8) // Inverted graphics
			{
				mainvideo.SetVideoMode(640, 200, 1, argc, argv);
			}
			else
			{
				mainvideo.SetVideoMode(640, 200, 0, argc, argv);
			}
			nodecompress_bakura = 0;
			nodecompress_title = 0;
			strcpy(tmp, "CBW");
		break;
		
#if defined(__386__)
		case 9:
			mainvideo = Normal_HGCC;
			mainvideo.SetVideoMode(720, 348, 0, argc, argv);
			vid_width = 720;
			vid_height = 348;
			text_line1 = 320;
			text_line2 = 328;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
			strcpy(tmp, "HCC");
		break;
		case 10:
		case 11:
			mainvideo = Planar_VGA;
			if (gpu_mode == 11)
			{
				mainvideo.SetVideoMode(800, 600, 1, argc, argv);
				heavy_cpu = 1;
			}
			else
			{
				mainvideo.SetVideoMode(800, 600, 0, argc, argv);
			}
			vid_width = 800;
			vid_height = 600;
			text_line1 = 500;
			text_line2 = 528;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
			strcpy(tmp, "VHI");
		break;
		case 12:
			mainvideo = Planar_VGA;
			mainvideo.SetVideoMode(360, 480, 0, argc, argv);
			vid_width = 360;
			vid_height = 480;
			text_line1 = 400;
			text_line2 = 420;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
			strcpy(tmp, "VMI");
		break;
		case 13:
			mainvideo = Planar_VGA;
			mainvideo.SetVideoMode(320, 240, 0, argc, argv);
			vid_width = 320;
			vid_height = 240;
			text_line1 = 200;
			text_line2 = 218;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
			strcpy(tmp, "VMX");
		break;
		case 14:
			mainvideo = Planar_VGA;
			mainvideo.SetVideoMode(640, 400, 0, argc, argv);
			vid_width = 640;
			vid_height = 400;
			text_line1 = 360;
			text_line2 = 380;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
			heavy_cpu = 1;
			strcpy(tmp, "V64");
		break;
#endif
	}
	
	for(i=0;i<num_sif_files;i++)
	{
		strcpy(frames_files[i], DATAFOLDER);
		strcat(frames_files[i], tmp);
		strcat(frames_files[i], sif_files[i]);
	}
	
	for(i=0;i<num_pal_files;i++)
	{
		strcpy(pal_files[i], DATAFOLDER);
		strcat(pal_files[i], tmp);
		strcat(pal_files[i], palf[i]);
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
		
		buttons = read_gameport_buttons();

		if (keystate[KEY_ESC] == 1)
		{
			done = 1;
		}
		
		switch(game_mode)
		{
			default:
				if ((keystate[KEY_SPACE] || (buttons == 1)) && delay_game > 20)
				{
					switch_gamemode(jump[game_mode]);
					keystate[KEY_SPACE] = 0;
				}
				mainvideo.Vsyncwait(); 
			break;
			case 1:
				time_game+=1;

				// If Standby
				if (action_status == 0)
				{
					frame_offset = 0;
					if ((keystate[KEY_SPACE] || (buttons == 1)))
					{
						if (toggle_sound_status == 0)
						{
							mainsound.Sound_Play(0);
						}
						
						if (toggle_sound_status == 1)
						{
							mainsound.Sound_Play(1);
							toggle_sound_status = 0;
						}
	
						action_status = 1;
						keystate[KEY_SPACE] = 0;
					}
				}
				if (action_status == 1)
				{
					if (status_level1[status] != END_GAME_STATUS)
					{
						if (FRAME_CURRENT >= frames_porn)
						{
							FRAME_CURRENT = 0;
							action_status = 0;
							mainvideo.DrawBMP_static(&bmp[FRAME_CURRENT], 0, 0, 0);
							Display_Ingame_text();
							mainvideo.FlipVideo();
							#if defined(__386__)
							if (frames_porn < 5) 
							#endif
							{ 
								mainvideo.Vsyncwait();
								mainvideo.Vsyncwait(); 
							}
						}
						else
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
							
							#if defined(__386__)
							if (frames_porn < 5) 
							#endif
							{ 
								mainvideo.Vsyncwait();
								mainvideo.Vsyncwait(); 
							}
							
							FRAME_CURRENT++;
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
				
#if defined(__386__)
				if (heavy_cpu == 0)
#endif
				{
					mainvideo.Vsyncwait();
					mainvideo.Vsyncwait();
					mainvideo.Vsyncwait();
				} 

#if defined(__386__)
				if (frames_porn < 5) 
#endif
				{
					mainvideo.Vsyncwait();
					mainvideo.Vsyncwait();
					mainvideo.Vsyncwait();
				}
			break;
			case 3:
				//
				if ((keystate[KEY_SPACE] || (buttons == 1)) && delay_game > 20)
				{
					text_progress++;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						mainvideo.FreeBMP(&bmp[0]);
						switch(text_progress)
						{
							case 1:
								mainvideo.LoadBMP(frames_files[2], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text(ingametext[4], 0, text_line1, color_text);
								mainvideo.Print_text(ingametext[5], 0, text_line2, color_text);
							break;
							case 2:
								mainvideo.LoadBMP(frames_files[1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text(ingametext[6], 0, text_line1, color_text);
								mainvideo.Print_text(ingametext[7], 0, text_line2, color_text);
							break;
							case 3:
								mainvideo.LoadBMP(frames_files[2], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text(ingametext[8], 0, text_line1, color_text);
							break;
							case 4:
								mainvideo.LoadBMP(frames_files[1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text(ingametext[9], 0, text_line1, color_text);
								mainvideo.Print_text(ingametext[10], 0, text_line2, color_text);
							break;
						}
						mainsound.Sound_Unload(0);
						mainsound.Sound_Load(sound_files[text_progress], 0);
						mainsound.Sound_Play(0);
					}
					
					mainvideo.FlipVideo();
					mainvideo.Vsyncwait();
					delay_game = 0;
					keystate[KEY_SPACE] = 0;
				}


			break;
		}
		
		mainsound.Sound_Update();
		mainvideo.Vsyncwait();
		
	}
	
	mainmusic.Stop_Music();
	mainsound.Sound_Close();
	mainmusic.Close_Music();

	mainsound.Sound_Unload(0);
	mainsound.Sound_Unload(1);

	for(i=0;i<9;i++)
	{
		mainvideo.FreeBMP(&bmp[i]);
	}
	mainvideo.FreeVideo();
	
	Keyboard_Clean();
	
	Close_Machine();

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
	keystate[KEY_SPACE] = 0;
	
	mainvideo.ClearVideo();
	mainmusic.Stop_Music();
	
	mainsound.Sound_Unload(0);
	mainsound.Sound_Unload(1);
	
	for(i=0;i<9;i++)
	{
		mainvideo.FreeBMP(&bmp[i]);
	}
	
	i = 0;
	
	switch(mode)
	{
		case 0:
			mainvideo.LoadBMP(frames_files[0], &bmp[0], vid_width, vid_height, nodecompress_title);
			mainvideo.LoadPalette(pal_files[0]);
			mainvideo.SetPalette();
			
			Display_Image(nodecompress_title);
			
			mainvideo.Print_text_Center(ingametext[0], 0, color_text);
			mainvideo.Print_text_Center(ingametext[1], 16, color_text);

			mainvideo.Print_text("HISCORE", 0, text_line1, color_text);
			mainvideo.Print_text(hibcd, 0, text_line2, color_text);
			
			mainvideo.Print_text_Center("PRESS KEY TO START", text_line1-24, color_text);
			
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
			
			mainmusic.Load_Music(DATAFOLDER"TENSE.IMF", 32000);
			mainmusic.Play_Music(1);
			
		break;
		case 1:
			mainvideo.Print_text(ingametext[15], 0, 0, color_text);
#if defined(__386__)
			if (frames_porn == 4)
			{
				mainvideo.LoadBMP(frames_files[3 + 1], &bmp[0], vid_width, vid_height, 0);
				mainvideo.LoadBMP(frames_files[3 + 4], &bmp[1], vid_width, vid_height, 0);
				mainvideo.LoadBMP(frames_files[3 + 6], &bmp[2], vid_width, vid_height, 0);
				mainvideo.LoadBMP(frames_files[3 + 8], &bmp[3], vid_width, vid_height, 0);
			}
			else
			{
				for(i=0;i<frames_porn;i++)
				{
					mainvideo.LoadBMP(frames_files[3 + i], &bmp[i], vid_width, vid_height, 0);
				}
			}
#else
			for(i=0;i<frames_porn;i++)
			{
				mainvideo.LoadBMP(frames_files[3 + i], &bmp[i], vid_width, vid_height, 0);
			}
#endif
			mainvideo.LoadPalette(pal_files[2]);
			mainvideo.SetPalette();
			
			mainvideo.DrawBMP_static(&bmp[FRAME_CURRENT], 0, 0, 0);

			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
		
			Display_Ingame_text();
			mainsound.Sound_Load(sound_files[5], 0);
			mainsound.Sound_Load(sound_files[6], 1);
			
			
			mainmusic.Load_Music(DATAFOLDER"HEAT.IMF", 32000);
			mainmusic.Play_Music(1);

		break;
		case 2:
			mainvideo.LoadPalette(pal_files[1]);
			mainvideo.SetPalette();
			
			mainvideo.LoadBMP(frames_files[1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
			Display_Image(nodecompress_bakura);
			
			mainvideo.Print_text("Your SCORE was ", 0, 0, color_text);
			mainvideo.Print_text(bcd, 17*8, 0, color_text);
			
			mainmusic.Load_Music(DATAFOLDER"BRES.IMF", 32000);
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
				mainvideo.Print_text(ingametext[16], 0, text_line1, color_text);
			}
			else if (bcd[0] < 48+2)
			{
				mainvideo.Print_text(ingametext[17], 0, text_line1, color_text);
				mainvideo.Print_text(ingametext[18], 0, text_line2, color_text);
			}
			else
			{
				mainvideo.Print_text(ingametext[19], 0, text_line1, color_text);
				mainvideo.Print_text(ingametext[20], 0, text_line2, color_text);
			}
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
		break;
		case 3:
			mainvideo.LoadPalette(pal_files[1]);
			mainvideo.SetPalette();

			mainvideo.LoadBMP(frames_files[1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
			Display_Image(nodecompress_bakura);

			
			mainvideo.Print_text(ingametext[2], 0, text_line1, color_text);
			mainvideo.Print_text(ingametext[3], 0, text_line2, color_text);
			
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
			

			mainsound.Sound_Load(sound_files[0], 0);
			mainsound.Sound_Play(0);
			
			mainmusic.Load_Music(DATAFOLDER"OMNI.IMF", 32000);
			mainmusic.Play_Music(1);

		break;
		case 4:
			mainvideo.Print_text(ingametext[11], 0, 0, color_text);
			mainvideo.Print_text(ingametext[12], 0, 8, color_text);
			mainvideo.Print_text(ingametext[13], 0, 16, color_text);
			mainvideo.Print_text(ingametext[14], 0, 48, color_text);
			
			mainmusic.Load_Music(DATAFOLDER"DETAILS.IMF", 32000);
			mainmusic.Play_Music(1);
		break;
	}
	
}

