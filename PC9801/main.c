#include <stdio.h>
#include <conio.h> /* this is where Open Watcom hides the outp() etc. functions */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <dos.h>
#include <ctype.h>
#include <i86.h>
#include <fcntl.h>
#include <math.h>

#include "gdc.h"
#include "bios.h"

#include "gfx.h"
#include "input.h"

const unsigned char bakura_palette[] = {
    0x00, 0x00, 0x00,
    0x02, 0x02, 0x03,
    0x03, 0x02, 0x02,
    0x06, 0x05, 0x04,
  
	0x05, 0x05, 0x09,
    0x05, 0x05, 0x07,
    0x09, 0x07, 0x09,
    0x08, 0x09, 0x0C,
    
    0x0B, 0x09, 0x07,
    0x0A, 0x0A, 0x0B,
    0x0C, 0x0B, 0x06,
    0x0C, 0x0C, 0x0C,
    
    0x0E, 0x0C, 0x0B,
    0x0D, 0x0C, 0x0E,
    0x0E, 0x0E, 0x0E,
    0x0F, 0x0F, 0x0F,
};

const unsigned char porn_palette[] = {
    0x05, 0x02, 0x01,
    0x07, 0x04, 0x02,
    0x09, 0x04, 0x02,
    0x09, 0x06, 0x03,
    0x0C, 0x05, 0x02,
    0x0B, 0x06, 0x04,
    0x0D, 0x06, 0x03,
    0x0C, 0x08, 0x05,
    0x0E, 0x07, 0x04,
    0x0F, 0x08, 0x05,
    0x0D, 0x0A, 0x07,
    0x0F, 0x0A, 0x07,
    0x0E, 0x0B, 0x09,
    0x0F, 0x0C, 0x09,
    0x0F, 0x0D, 0x0B,
    0x0F, 0x0E, 0x0D,
};

unsigned char title_palette[] = {
    0x02, 0x02, 0x02,
    0x02, 0x04, 0x03,
    0x02, 0x05, 0x03,
    0x02, 0x05, 0x06,
    0x02, 0x05, 0x0B,
    0x05, 0x05, 0x03,
    0x03, 0x06, 0x06,
    0x02, 0x07, 0x02,
    0x02, 0x0A, 0x05,
    0x03, 0x09, 0x0D,
    0x05, 0x0A, 0x02,
    0x09, 0x09, 0x02,
    0x03, 0x0A, 0x0A,
    0x02, 0x0C, 0x03,
    0x06, 0x0D, 0x0D,
    0x06, 0x0E, 0x09,
};

unsigned short offset_frames[4][4];

extern void zx0_decompress();
extern void lzsa1_decompress();

#define SIZE 32000
unsigned char* hold_graph[4];
unsigned char font_graph, index_t, dst;

// _dos_allocmem
// _dos_freemem

void Load_Bitmap_Part(const char* filename, unsigned char* hold_graph[], unsigned long size, unsigned char index)
{
	int fd;
	char* mem = NULL;
	uint32_t fsz;
	size_t totalsize;

	_dos_open (filename, O_RDONLY, &fd);
	
	if (!hold_graph[index])
	{
		free(hold_graph[index]);
		hold_graph[index] = NULL;
	}
	hold_graph[index] = (unsigned char*) malloc(size);

	_dos_read (fd, (char*)hold_graph[index], size, &totalsize);
	_dos_close (fd);
}

void Load_Bitmap(const char* filename, unsigned char* hold_graph[], unsigned long size, unsigned char num_graphs)
{
	int fd;
	char* mem = NULL;
	uint32_t fsz;
	size_t totalsize;
    uint_fast8_t i;

    _dos_open (filename, O_RDONLY, &fd);

    for (i = 0; i < num_graphs; i++) {
        if (!hold_graph[i]) {
            hold_graph[i] = (unsigned char*) malloc(size);
        }
        _dos_read (fd, (char*)hold_graph[i], size, &totalsize);
    }
	_dos_close (fd);
}

void Clear_Bitmap(unsigned char *hold_graph[])
{
	unsigned short i;
	for (i = 0; i < 4; i++)
	{
		if (hold_graph[i])
		{
            free(hold_graph[i]);
            hold_graph[i] = NULL;
        }
	}	
}

static void CAL_Zx0Expand (uint8_t *source, uint8_t *dest)
{
	unsigned sourceseg,sourceoff,destseg,destoff;

	source++;	// normalize
	source--;
	dest++;
	dest--;

	sourceseg = FP_SEG(source);
	sourceoff = FP_OFF(source);
	destseg = FP_SEG(dest);
	destoff = FP_OFF(dest);

	_asm
	{
			mov	si,[sourceoff]
			mov	di,[destoff]
			mov	es,[destseg]
			mov	ds,[sourceseg]
	}
	zx0_decompress();

	_asm
	{
		mov	ax,ss
		mov	ds,ax
	}
}


static void CAL_LZSAExpand (uint8_t *source, uint8_t *dest)
{
	unsigned sourceseg,sourceoff,destseg,destoff;

	source++;	// normalize
	source--;
	dest++;
	dest--;

	sourceseg = FP_SEG(source);
	sourceoff = FP_OFF(source);
	destseg = FP_SEG(dest);
	destoff = FP_OFF(dest);

	_asm
	{
			mov	si,[sourceoff]
			mov	di,[destoff]
			mov	es,[destseg]
			mov	ds,[sourceseg]
	}
	lzsa1_decompress();

	_asm
	{
		mov	ax,ss
		mov	ds,ax
	}
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
unsigned short delay_game;

void DrawString(const char* str, unsigned char x, unsigned char y)
{
	Cursor_SetXY(x,y);
	Print_text(str);
}

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


void Refresh_screen()
{
	gfx_set_plane( GFX_PLANE_DISPLAY,   1 - gfx_get_plane(GFX_PLANE_DISPLAY));
	gfx_set_plane( GFX_PLANE_RENDERING, 1 - gfx_get_plane(GFX_PLANE_RENDERING));
	gfx_wait_vsync();	
}

static inline Draw_Frame()
{
	CAL_LZSAExpand(hold_graph[FRAME_CURRENT], GDC_PLANE_1);
	CAL_LZSAExpand(hold_graph[FRAME_CURRENT] + offset_frames[FRAME_CURRENT][0], GDC_PLANE_2);
	CAL_LZSAExpand(hold_graph[FRAME_CURRENT] + (offset_frames[FRAME_CURRENT][0]+offset_frames[FRAME_CURRENT][1]), GDC_PLANE_3);
	CAL_LZSAExpand(hold_graph[FRAME_CURRENT] + (offset_frames[FRAME_CURRENT][0]+offset_frames[FRAME_CURRENT][1]+offset_frames[FRAME_CURRENT][2]), GDC_PLANE_4);	
}

int main() 
{
	unsigned char done;
	gfx_init();
	
	hibcd[0] = 48;
	hibcd[1] = 48;
	hibcd[2] = 48;
	hibcd[3] = '\0';
	
	switch_gamemode(0);
	done = 0;

	while (!done)
	{
		Get_Input();
		
		if (inputs[ESC_KEY] == 1)
		{
			done = 1;
		}
		
		switch(game_mode)
		{
			default:
				if (inputs[SPACE_KEY])
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
					
					Draw_Frame();
					
					FRAME_CURRENT++;
					if (FRAME_CURRENT > 3) FRAME_CURRENT = 0;
					
					DrawString("SCORE", 0, 20);
					DrawString(bcd, 0, 21);
					Refresh_screen();
					
					time_game += 2;
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (inputs[SPACE_KEY] == 1)
					{
						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > 60*2)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						DrawString(ingame_quote[status_level1[status]], 0, 24);
					}
				}
				
				gfx_wait_vsync();
				
			break;
			case 3:
				delay_game++;
				if (inputs[SPACE_KEY] && delay_game > 25)
				{
					text_progress++;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						CLEAR_TXT();
						
						switch(text_progress)
						{
							case 1:
								DrawString("You're going to be the perfect victim for my games!", 0, 22);
							break;
							case 2:
								DrawString("I'm Rikuto, i offer you the chance to prove your worth in a trial.", 0, 22);

							break;
							case 3:
								DrawString("Don't be scared, it'll be fun!", 0, 22);
							break;
							case 4:
								DrawString("Your challenge will be this: you'll be on the other end and punish them !", 0, 22);
							break;
						}
					}
					delay_game = 0;
				}
				
				gfx_wait_vsync();
			break;
		}
		

		
	}
	
	gfx_end();
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
	
	memset(GDC_PLANE_1, 0, 32000);
	memset(GDC_PLANE_2, 0, 32000);
	memset(GDC_PLANE_3, 0, 32000);
	memset(GDC_PLANE_4, 0, 32000);
	
	CLEAR_TEXT_VRAM();
	
	switch(mode)
	{
		case 0:
			Load_Bitmap_Part("TITLE.ZX0", hold_graph, 15182+13796+10395+6496, 0);
			CAL_Zx0Expand(hold_graph[0], GDC_PLANE_1);
			CAL_Zx0Expand(hold_graph[0] + 15182, GDC_PLANE_2);
			CAL_Zx0Expand(hold_graph[0] + (15182+13796), GDC_PLANE_3);
			CAL_Zx0Expand(hold_graph[0] + (15182+13796+10395), GDC_PLANE_4);
		
			DrawString("COPYRIGHT INFRINGEMENT", 30, 0);
			DrawString("BY GAMEBLABLA", 34, 1);
			
			DrawString("HISCORE", 0, 20);
			DrawString(hibcd, 0, 21);
			
			DrawString("PRESS KEY TO START", 31, 22);
			
			gfx_fill_palette(title_palette, sizeof(title_palette) / 3);
			
			Refresh_screen();
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';

			DrawString(ingame_quote[status_level1[status]], 0, 160);
			
			offset_frames[0][0] = 10862;
			offset_frames[0][1] = 11708;
			offset_frames[0][2] = 9179;
			offset_frames[0][3] = 6026;
			Load_Bitmap_Part("FRAME1.BIN", hold_graph, 10259+12609+9495+6455, 0);

			offset_frames[1][0] = 11234;
			offset_frames[1][1] = 12305;
			offset_frames[1][2] = 9760;
			offset_frames[1][3] = 6322;
			Load_Bitmap_Part("FRAME2.BIN", hold_graph, 12517+14051+11218+7352, 1);
			
			offset_frames[2][0] = 10940;
			offset_frames[2][1] = 12142;
			offset_frames[2][2] = 9252;
			offset_frames[2][3] = 6051;
			Load_Bitmap_Part("FRAME3.BIN", hold_graph, 12755+13016+9861+6648, 2);
			
			offset_frames[3][0] = 10383;
			offset_frames[3][1] = 9778;
			offset_frames[3][2] = 8943;
			offset_frames[3][3] = 5968;
			Load_Bitmap_Part("FRAME4.BIN", hold_graph, 12051+12714+9422+6386, 3);
			
			gfx_fill_palette(porn_palette, sizeof(porn_palette) / 3);
			
			Draw_Frame();
			
			DrawString("SCORE", 0, 20);
			DrawString(bcd, 0, 21);
			DrawString(ingame_quote[status_level1[status]], 0, 24);
			
			Refresh_screen();
		break;
		case 2:
			Load_Bitmap_Part("BPF.ZX0", hold_graph, 9966+9865+9078+6458, 0);
			CAL_Zx0Expand(hold_graph[0], GDC_PLANE_1);
			CAL_Zx0Expand(hold_graph[0] + 9966, GDC_PLANE_2);
			CAL_Zx0Expand(hold_graph[0] + (9966+9865), GDC_PLANE_3);
			CAL_Zx0Expand(hold_graph[0] + (9966+9865+9078), GDC_PLANE_4);
			
			gfx_fill_palette(bakura_palette, sizeof(bakura_palette) / 3);
			
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
			
			if (bcd[0] < 48+1)
			{
				DrawString("Guess you'll stay there for a while", 0, 22);
			}
			else if (bcd[1] < 48+2)
			{
				DrawString("Not bad but you can do better!", 0, 22);
				DrawString("Otherwise you can't leave this place!", 0, 23);
			}
			else
			{
				DrawString("Congrats! I'll leave you alone.", 0, 22);
				DrawString("This won't be the last of me!", 0, 23);
			}
			Refresh_screen();
		break;
		case 3:
			Load_Bitmap_Part("BPF.ZX0", hold_graph, 9966+9865+9078+6458, 0);
			CAL_Zx0Expand(hold_graph[0], GDC_PLANE_1);
			CAL_Zx0Expand(hold_graph[0] + 9966, GDC_PLANE_2);
			CAL_Zx0Expand(hold_graph[0] + (9966+9865), GDC_PLANE_3);
			CAL_Zx0Expand(hold_graph[0] + (9966+9865+9078), GDC_PLANE_4);
			
			gfx_fill_palette(bakura_palette, sizeof(bakura_palette) / 3);
			
			DrawString("Oh my, what do we have here?", 0, 22);
			DrawString("You look so innocent", 0, 23);
			Refresh_screen();
		break;
		case 4:
			DrawString("Your goal is to FUCK Zhongli!", 0, 0);
			DrawString("Follow the instructions to get points", 0, 1);
			DrawString("Penetrate him by pressing a key", 0, 2);
			DrawString("Good luck !", 0, 5);
			Refresh_screen();
		break;
	}
	
}

