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

unsigned char frames_porn;

#if defined(__386__)
#define DATAFOLDER "386\\"
#else
#define DATAFOLDER "DATA\\"
#endif

const unsigned char sound_files[4][7][10+5] =
{
	{
		DATAFOLDER"SP1.WAV", 
		DATAFOLDER"SP2.WAV", 
		DATAFOLDER"SP3.WAV", 
		DATAFOLDER"SP4.WAV", 
		DATAFOLDER"SP5.WAV", 
		DATAFOLDER"FUCK.WAV", 
		DATAFOLDER"FUCK2.WAV"
	},
	{
		DATAFOLDER"SP1.ADW", 
		DATAFOLDER"SP2.ADW", 
		DATAFOLDER"SP3.ADW", 
		DATAFOLDER"SP4.ADW", 
		DATAFOLDER"SP5.ADW", 
		DATAFOLDER"FUCK.ADW", 
		DATAFOLDER"FUCK2.ADW"
	},
	{
		DATAFOLDER"SP1.PCK", 
		DATAFOLDER"SP2.PCK", 
		DATAFOLDER"SP3.PCK", 
		DATAFOLDER"SP4.PCK", 
		DATAFOLDER"SP5.PCK", 
		DATAFOLDER"FUCK.PCK", 
		DATAFOLDER"FUCK2.PCK"
	},
	{
		DATAFOLDER"SP1.TDY", 
		DATAFOLDER"SP2.TDY", 
		DATAFOLDER"SP3.TDY", 
		DATAFOLDER"SP4.TDY", 
		DATAFOLDER"SP5.TDY", 
		DATAFOLDER"FUCK.TDY", 
		DATAFOLDER"FUCK2.TDY"
	},
};

const unsigned char frames_files[14][3 + 9][13+5] =
{
	{
		DATAFOLDER"TDYTIT.SIF", 
		DATAFOLDER"TDYRIK.SIF", 
		DATAFOLDER"TDYRIKL.SIF", 
		DATAFOLDER"TDYF1.SIF", 
		DATAFOLDER"TDYF2.SIF", 
		DATAFOLDER"TDYF3.SIF", 
		DATAFOLDER"TDYF4.SIF",
		DATAFOLDER"TDYF5.SIF", 
		DATAFOLDER"TDYF6.SIF", 
		DATAFOLDER"TDYF7.SIF", 
		DATAFOLDER"TDYF8.SIF",
		DATAFOLDER"TDYF9.SIF",
	},
	{
		DATAFOLDER"VGATIT.SIF", 
		DATAFOLDER"VGARIK.SIF", 
		DATAFOLDER"VGARIKL.SIF", 
		DATAFOLDER"VGAF1.SIF", 
		DATAFOLDER"VGAF2.SIF", 
		DATAFOLDER"VGAF3.SIF", 
		DATAFOLDER"VGAF4.SIF",
		DATAFOLDER"VGAF5.SIF", 
		DATAFOLDER"VGAF6.SIF", 
		DATAFOLDER"VGAF7.SIF", 
		DATAFOLDER"VGAF8.SIF",
		DATAFOLDER"VGAF9.SIF", 
	},
	{
		DATAFOLDER"HGCTIT.SIF", 
		DATAFOLDER"HGCRIK.SIF", 
		DATAFOLDER"HGCRIKL.SIF", 
		DATAFOLDER"HGCF1.SIF", 
		DATAFOLDER"HGCF2.SIF", 
		DATAFOLDER"HGCF3.SIF", 
		DATAFOLDER"HGCF4.SIF",
		DATAFOLDER"HGCF5.SIF", 
		DATAFOLDER"HGCF6.SIF", 
		DATAFOLDER"HGCF7.SIF", 
		DATAFOLDER"HGCF8.SIF",
		DATAFOLDER"HGCF9.SIF",
	},
	{
		DATAFOLDER"CGATIT.SIF", 
		DATAFOLDER"CGARIK.SIF", 
		DATAFOLDER"CGARIKL.SIF", 
		DATAFOLDER"CGAF1.SIF", 
		DATAFOLDER"CGAF2.SIF", 
		DATAFOLDER"CGAF3.SIF", 
		DATAFOLDER"CGAF4.SIF",
		DATAFOLDER"CGAF5.SIF", 
		DATAFOLDER"CGAF6.SIF", 
		DATAFOLDER"CGAF7.SIF", 
		DATAFOLDER"CGAF8.SIF",
		DATAFOLDER"CGAF9.SIF",
	},
	{
		DATAFOLDER"EGATIT.SIF", 
		DATAFOLDER"EGARIK.SIF", 
		DATAFOLDER"EGARIKL.SIF", 
		DATAFOLDER"EGAF1.SIF", 
		DATAFOLDER"EGAF2.SIF", 
		DATAFOLDER"EGAF3.SIF", 
		DATAFOLDER"EGAF4.SIF",
		DATAFOLDER"EGAF5.SIF", 
		DATAFOLDER"EGAF6.SIF", 
		DATAFOLDER"EGAF7.SIF", 
		DATAFOLDER"EGAF8.SIF",
		DATAFOLDER"EGAF9.SIF",
	},
	{
		DATAFOLDER"CNTTIT.SIF", 
		DATAFOLDER"CNTRIK.SIF", 
		DATAFOLDER"CNTRIKL.SIF", 
		DATAFOLDER"CNTF1.SIF", 
		DATAFOLDER"CNTF2.SIF", 
		DATAFOLDER"CNTF3.SIF", 
		DATAFOLDER"CNTF4.SIF",
		DATAFOLDER"CNTF5.SIF", 
		DATAFOLDER"CNTF6.SIF", 
		DATAFOLDER"CNTF7.SIF", 
		DATAFOLDER"CNTF8.SIF",
		DATAFOLDER"CNTF9.SIF",
	},
	
	// CGA B&W
	{
		DATAFOLDER"CBWTIT.SIF", 
		DATAFOLDER"CBWRIK.SIF", 
		DATAFOLDER"CBWRIKL.SIF", 
		DATAFOLDER"CBWF1.SIF", 
		DATAFOLDER"CBWF2.SIF", 
		DATAFOLDER"CBWF3.SIF", 
		DATAFOLDER"CBWF4.SIF",
		DATAFOLDER"CBWF5.SIF", 
		DATAFOLDER"CBWF6.SIF", 
		DATAFOLDER"CBWF7.SIF", 
		DATAFOLDER"CBWF8.SIF",
		DATAFOLDER"CBWF9.SIF",
	},
	{
		DATAFOLDER"CBWTIT.SIF", 
		DATAFOLDER"CBWRIK.SIF", 
		DATAFOLDER"CBWRIKL.SIF", 
		DATAFOLDER"CBWF1.SIF", 
		DATAFOLDER"CBWF2.SIF", 
		DATAFOLDER"CBWF3.SIF", 
		DATAFOLDER"CBWF4.SIF",
		DATAFOLDER"CBWF5.SIF", 
		DATAFOLDER"CBWF6.SIF", 
		DATAFOLDER"CBWF7.SIF", 
		DATAFOLDER"CBWF8.SIF",
		DATAFOLDER"CBWF9.SIF",
	},
	
	{
		DATAFOLDER"HCCTIT.SIF", 
		DATAFOLDER"HCCRIK.SIF", 
		DATAFOLDER"HCCRIKL.SIF", 
		DATAFOLDER"HCCF1.SIF", 
		DATAFOLDER"HCCF2.SIF", 
		DATAFOLDER"HCCF3.SIF", 
		DATAFOLDER"HCCF4.SIF",
		DATAFOLDER"HCCF5.SIF", 
		DATAFOLDER"HCCF6.SIF", 
		DATAFOLDER"HCCF7.SIF", 
		DATAFOLDER"HCCF8.SIF",
		DATAFOLDER"HCCF9.SIF",
	},
	
	// Normal And Special
	{
		DATAFOLDER"VHITIT.SIF", 
		DATAFOLDER"VHIRIK.SIF", 
		DATAFOLDER"VHIRIKL.SIF", 
		DATAFOLDER"VHIF1.SIF", 
		DATAFOLDER"VHIF2.SIF", 
		DATAFOLDER"VHIF3.SIF", 
		DATAFOLDER"VHIF4.SIF",
		DATAFOLDER"VHIF5.SIF", 
		DATAFOLDER"VHIF6.SIF", 
		DATAFOLDER"VHIF7.SIF", 
		DATAFOLDER"VHIF8.SIF",
		DATAFOLDER"VHIF9.SIF",
	},
	{
		DATAFOLDER"VHITIT.SIF", 
		DATAFOLDER"VHIRIK.SIF", 
		DATAFOLDER"VHIRIKL.SIF", 
		DATAFOLDER"VHIF1.SIF", 
		DATAFOLDER"VHIF2.SIF", 
		DATAFOLDER"VHIF3.SIF", 
		DATAFOLDER"VHIF4.SIF",
		DATAFOLDER"VHIF5.SIF", 
		DATAFOLDER"VHIF6.SIF", 
		DATAFOLDER"VHIF7.SIF", 
		DATAFOLDER"VHIF8.SIF",
		DATAFOLDER"VHIF9.SIF",
	},
	
	{
		DATAFOLDER"VMITIT.SIF", 
		DATAFOLDER"VMIRIK.SIF", 
		DATAFOLDER"VMIRIKL.SIF", 
		DATAFOLDER"VMIF1.SIF", 
		DATAFOLDER"VMIF2.SIF", 
		DATAFOLDER"VMIF3.SIF", 
		DATAFOLDER"VMIF4.SIF",
		DATAFOLDER"VMIF5.SIF", 
		DATAFOLDER"VMIF6.SIF", 
		DATAFOLDER"VMIF7.SIF", 
		DATAFOLDER"VMIF8.SIF",
		DATAFOLDER"VMIF9.SIF",
	},
	{
		DATAFOLDER"VMXTIT.SIF", 
		DATAFOLDER"VMXRIK.SIF", 
		DATAFOLDER"VMXRIKL.SIF", 
		DATAFOLDER"VMXF1.SIF", 
		DATAFOLDER"VMXF2.SIF", 
		DATAFOLDER"VMXF3.SIF", 
		DATAFOLDER"VMXF4.SIF",
		DATAFOLDER"VMXF5.SIF", 
		DATAFOLDER"VMXF6.SIF", 
		DATAFOLDER"VMXF7.SIF", 
		DATAFOLDER"VMXF8.SIF",
		DATAFOLDER"VMXF9.SIF",
	},
	{
		DATAFOLDER"V64TIT.SIF", 
		DATAFOLDER"V64RIK.SIF", 
		DATAFOLDER"V64RIKL.SIF", 
		DATAFOLDER"V64F1.SIF", 
		DATAFOLDER"V64F2.SIF", 
		DATAFOLDER"V64F3.SIF", 
		DATAFOLDER"V64F4.SIF",
		DATAFOLDER"V64F5.SIF", 
		DATAFOLDER"V64F6.SIF", 
		DATAFOLDER"V64F7.SIF", 
		DATAFOLDER"V64F8.SIF",
		DATAFOLDER"V64F9.SIF",
	},
};

const unsigned char pal_files[14][4][13+5] =
{
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	
	// CGA stubs
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	{
		DATAFOLDER"VGATIT.PAL", 
		DATAFOLDER"VGARIK.PAL", 
		DATAFOLDER"VGAF1.PAL", 
	},
	
	
	{
		DATAFOLDER"HCCTIT.PAL", 
		DATAFOLDER"HCCRIK.PAL", 
		DATAFOLDER"HCCF1.PAL", 
	},
	
	// Normal and Special
	{
		DATAFOLDER"VHITIT.PAL", 
		DATAFOLDER"VHIRIK.PAL", 
		DATAFOLDER"VHIF1.PAL", 
	},
	{
		DATAFOLDER"VHITIT.PAL", 
		DATAFOLDER"VHIRIK.PAL", 
		DATAFOLDER"VHIF1.PAL", 
	},
	{
		DATAFOLDER"VMITIT.PAL", 
		DATAFOLDER"VMIRIK.PAL", 
		DATAFOLDER"VMIF1.PAL", 
	},
	{
		DATAFOLDER"VMXTIT.PAL", 
		DATAFOLDER"VMXRIK.PAL", 
		DATAFOLDER"VMXF1.PAL", 
	},
	{
		DATAFOLDER"V64TIT.PAL", 
		DATAFOLDER"V64RIK.PAL", 
		DATAFOLDER"V64F1.PAL", 
	},
};

unsigned short vid_width, vid_height;
uint16_t text_line1, text_line2 ;
uint8_t musdef;
uint8_t lowmem;
uint8_t nodecompress_title;
uint8_t nodecompress_bakura;
uint8_t heavy_cpu;

void Display_Ingame_text()
{
	mainvideo.Print_text("SCORE", 0, text_line1, color_text);
	mainvideo.Print_text(ingame_quote[status_level1[status]], 0, text_line2, color_text);
	mainvideo.Print_text(bcd, 64, text_line1, color_text);
}


void Display_Image(unsigned char dec)
{
	if (dec == 0) mainvideo.DrawBMP_static(&bmp[0], 0, 0, 0);
	if (dec == 1) mainvideo.DrawBMP_compressed(&bmp[0], 0, 0);
}

int main(int argc,char **argv) 
{
	unsigned char done;
	unsigned char buttons;
	char valid_choice, choice;
	
	snddef = 0;
	gpu_mode = 1;
	musdef = 0;
	frames_porn = 4;
	
	puts("\nCOPYRIGHT INFRINGEMENT SETUP");

    do {
        puts("\nSelect Video Mode:");
#if defined(__386__)
        puts("0 = Tandy 320x200 16 Colors");
#else
        puts("0 = Tandy/PCJr");
#endif
        puts("1 = MCGA/VGA 320x200 256 Colors");
        puts("2 = Hercules");
        puts("3 = CGA 4-colors");
        puts("4 = EGA 320x200");
        puts("5 = CGA Composite");
        puts("6 = CGA B&W");
        puts("7 = CGA B&W Inverted");
#if defined(__386__)
        puts("8 = Hercules InColor");
        puts("9 = VGA 800x600 16 colors");
        puts("10 = VGA 800x600 16 colors Special/SVGA");
        puts("11 = VGA 360x480 ModeX");
        puts("12 = VGA 320x240 ModeX");
        puts("13 = VGA 640x400 256 colors");
#endif
        puts("Enter your choice: ");
        scanf("%d", &choice);
#if defined(__386__)
        valid_choice = (char)choice >= 0 && (char)choice <= 13;
#else
        valid_choice = (char)choice >= 0 && (char)choice <= 7;
#endif
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


#if defined(__386__)
    do {
        puts("\nUse Low memory mode (less frames) ?:");
        puts("0 = No");
        puts("1 = Yes");
        puts("Enter your choice: ");
        scanf("%d", &choice);
        valid_choice = (char)choice >= 0 && (char)choice <= 1;
        if (!valid_choice) puts("Invalid choice. Please try again.");
        lowmem = choice;
    } while (!valid_choice);
#else
	lowmem = 1;
#endif

	frames_porn = 9;
	if (lowmem)
	{
		frames_porn = 4;
	}

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
	nodecompress_bakura = 1;
	heavy_cpu = 0;
	
	switch(gpu_mode)
	{
		case 0:
			mainvideo = Normal_Tandy;
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
		break;
		case 1:
			mainvideo = Normal_MCGA;
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
			nodecompress_title = 0;
			nodecompress_bakura = 0;
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
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
		break;
		case 4:
			mainvideo = Normal_EGA;
			mainvideo.SetVideoMode(vid_width, vid_height, 0, argc, argv);
			#if defined(__386__)
			nodecompress_bakura = 0;
			#endif
			nodecompress_title = 0;
		break;
		case 5:
			mainvideo = Normal_CGA;
			mainvideo.SetVideoMode(160, 200, 1, argc, argv);
		break;
		case 6:
		case 7:
			mainvideo = Normal_CGA;
			if (gpu_mode == 7) // Inverted graphics
			{
				mainvideo.SetVideoMode(640, 200, 1, argc, argv);
			}
			else
			{
				mainvideo.SetVideoMode(640, 200, 0, argc, argv);
			}
			nodecompress_bakura = 0;
			nodecompress_title = 0;
		break;
		
#if defined(__386__)
		case 8:
			mainvideo = Normal_HGCC;
			mainvideo.SetVideoMode(720, 348, 0, argc, argv);
			vid_width = 720;
			vid_height = 348;
			text_line1 = 320;
			text_line2 = 328;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
		break;
		case 9:
		case 10:
			mainvideo = Planar_VGA;
			if (gpu_mode == 10)
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
		break;
		case 11:
			mainvideo = Planar_VGA;
			mainvideo.SetVideoMode(360, 480, 0, argc, argv);
			vid_width = 360;
			vid_height = 480;
			text_line1 = 400;
			text_line2 = 420;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
		break;
		case 12:
			mainvideo = Planar_VGA;
			mainvideo.SetVideoMode(320, 240, 0, argc, argv);
			vid_width = 320;
			vid_height = 240;
			text_line1 = 200;
			text_line2 = 218;
			color_text = 0xFF;
			nodecompress_title = 0;
			nodecompress_bakura = 0;
		break;
		case 13:
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
		break;
#endif
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
						
						switch(text_progress)
						{
							case 1:
								mainvideo.LoadBMP(frames_files[gpu_mode][2], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text("You're going to be the perfect", 0, text_line1, color_text);
								mainvideo.Print_text("victim for my games!", 0, text_line2, color_text);
							break;
							case 2:
								mainvideo.LoadBMP(frames_files[gpu_mode][1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text("I'm Rikuto, i offer you the chance", 0, text_line1, color_text);
								mainvideo.Print_text("to prove your worth in a trial.", 0, text_line2, color_text);
							break;
							case 3:
								mainvideo.LoadBMP(frames_files[gpu_mode][2], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text("Don't be scared, it'll be fun!", 0, text_line1, color_text);
							break;
							case 4:
								mainvideo.LoadBMP(frames_files[gpu_mode][1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
								Display_Image(nodecompress_bakura);
								mainvideo.Print_text("Your challenge is this: You'll be on", 0, text_line1, color_text);
								mainvideo.Print_text("the other end and punish them!", 0, text_line2, color_text);
							break;
						}
						mainsound.Sound_Unload(0);
						mainsound.Sound_Load(sound_files[snddef][text_progress], 0);
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
	
	Keyboard_Clean();
	Close_Machine();

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
			mainvideo.LoadBMP(frames_files[gpu_mode][0], &bmp[0], vid_width, vid_height, nodecompress_title);
			mainvideo.LoadPalette(pal_files[gpu_mode][0]);
			mainvideo.SetPalette();
			
			Display_Image(nodecompress_title);
			
			mainvideo.Print_text_Center("COPYRIGHT INFRINGEMENT", 0, color_text);
			mainvideo.Print_text_Center("BY GAMEBLABLA", 16, color_text);

			mainvideo.Print_text("HISCORE", 0, text_line1, color_text);
			mainvideo.Print_text(hibcd, 0, text_line2, color_text);
			
			mainvideo.Print_text_Center("PRESS KEY TO START", text_line1-24, color_text);
			
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
			
			mainmusic.Load_Music(DATAFOLDER"TENSE.IMF", 32000);
			mainmusic.Play_Music(1);
			
		break;
		case 1:
			mainvideo.Print_text("LOADING... PLEASE WAIT !", 0, 0, color_text);
#if defined(__386__)
			if (frames_porn == 4)
			{
				mainvideo.LoadBMP(frames_files[gpu_mode][3 + 1], &bmp[0], vid_width, vid_height, 0);
				mainvideo.LoadBMP(frames_files[gpu_mode][3 + 4], &bmp[1], vid_width, vid_height, 0);
				mainvideo.LoadBMP(frames_files[gpu_mode][3 + 6], &bmp[2], vid_width, vid_height, 0);
				mainvideo.LoadBMP(frames_files[gpu_mode][3 + 8], &bmp[3], vid_width, vid_height, 0);
			}
			else
			{
				for(i=0;i<frames_porn;i++)
				{
					mainvideo.LoadBMP(frames_files[gpu_mode][3 + i], &bmp[i], vid_width, vid_height, 0);
				}
			}
#else
			for(i=0;i<frames_porn;i++)
			{
				mainvideo.LoadBMP(frames_files[gpu_mode][3 + i], &bmp[i], vid_width, vid_height, 0);
			}
#endif
			mainvideo.LoadPalette(pal_files[gpu_mode][2]);
			mainvideo.SetPalette();
			
			mainvideo.DrawBMP_static(&bmp[FRAME_CURRENT], 0, 0, 0);

			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
		
			Display_Ingame_text();
			mainsound.Sound_Load(sound_files[snddef][5], 0);
			mainsound.Sound_Load(sound_files[snddef][6], 1);
			
			
			mainmusic.Load_Music(DATAFOLDER"HEAT.IMF", 32000);
			mainmusic.Play_Music(1);

		break;
		case 2:
			mainvideo.LoadPalette(pal_files[gpu_mode][1]);
			mainvideo.SetPalette();
			
			mainvideo.LoadBMP(frames_files[gpu_mode][1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
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
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
		break;
		case 3:


			mainvideo.LoadPalette(pal_files[gpu_mode][1]);
			mainvideo.SetPalette();

			mainvideo.LoadBMP(frames_files[gpu_mode][1], &bmp[0], vid_width, vid_height, nodecompress_bakura);
			Display_Image(nodecompress_bakura);

			
			mainvideo.Print_text("Oh my, what do we have here?", 0, text_line1, color_text);
			mainvideo.Print_text("You look so innocent...", 0, text_line2, color_text);
			
			mainvideo.FlipVideo();
			mainvideo.Vsyncwait();
			

			mainsound.Sound_Load(sound_files[snddef][0], 0);
			mainsound.Sound_Play(0);
			
			mainmusic.Load_Music(DATAFOLDER"OMNI.IMF", 32000);
			mainmusic.Play_Music(1);

		break;
		case 4:
			mainvideo.Print_text("Your goal is to SCREW Zhongli!", 0, 0, color_text);
			mainvideo.Print_text("Follow instructions to get points.", 0, 8, color_text);
			mainvideo.Print_text("GOOD LUCK !", 0, 16, color_text);
			mainvideo.Print_text("PRESS KEY TO START PLAYING", 0, 48, color_text);
			
			mainmusic.Load_Music(DATAFOLDER"DETAILS.IMF", 32000);
			mainmusic.Play_Music(1);
		break;
	}
	
}

