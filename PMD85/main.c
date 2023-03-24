#include <stdio.h>
#include <conio.h>
#include <compress/zx0.h>
#include <graphics.h>
#include <games.h>
#include <string.h>

/*
    0000 až 7FFF RAM
    8000 až 8FFF ROM (Monitor - 4KB)
    9000 až 9FFF nothing
    A000 až AFFF ROM (Monitor - mirrored 8000-8FFF, maybe simply address decoder)
    B000 až BFFF nothing
    C000 až FFFF Video RAM 
*/

// zcc +pmd85 -O3 --opt-code-speed=all -Cc-unsigned main.c data.asm dzx0.asm -create-app -pragma-define:CRT_ENABLE_STDIO=0 -pragma-define:CRT_INITIALIZE_BSS=0 -pragma-define:CLIB_EXIT_STACK_SIZE=0

#define FRAME_CURRENT text_progress

extern unsigned char bakura_data[];
extern unsigned char title_data[];

extern unsigned char frame1_data[];
extern unsigned char frame2_data[];
extern unsigned char frame3_data[];
extern unsigned char frame4_data[];

extern void dzx0_decompress(void* src, void* dst);
static unsigned char k, game_mode, text_progress;
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

static unsigned char status;
static unsigned char i;
static unsigned short time_game;

static const char ingame_quote[5][9] =
{
	"GO ON   ",
	"STOP    ",
	"FUCK HIM",
	"FINISH  ",
};

static const unsigned char text_minigame_1[] = 
{
	"Your goal is to FUCK Zhongli!\nFollow the instructions to get points.\nHold a key then pull out by releasing it."
};



/*
#define jump_size 64
#define copy_size 64
static void memcpy_jump_64(void* dest, const void* src) {

    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (uint16_t i = 0; i < 5120; i += copy_size) {
        memcpy(d, s, copy_size);
        d += jump_size + copy_size;
        s += copy_size;
    }
}
*/

/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };
static unsigned char bcd[3];
static const unsigned char* const frame_porn_pattern[]=
{
	frame1_data, frame2_data, frame3_data, frame4_data,
};
static const unsigned char text1[] = "You're going to be the perfect victim for my games!";
static const unsigned char text2[] = "I'm Rikuto, i offer you the chance to prove yourworth in a trial";
static const unsigned char text3[] = "Don't be scared, it'll be fun!";
static const unsigned char text4[] = "Your challenge will be this: you will be on the other end and punish them";
static const unsigned char* const story_text[]=
{
	text1, text1, text2, text3, text4,
};


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



int main()
{
	switch_gamemode(0);
	
	while(1)
	{
		k = getk();
		time_game+=1;
		switch(game_mode)
		{
			default:
				if (k > 0 && time_game > 10)
				{
					switch_gamemode(jump[game_mode]);
				}
			break;
			case 1:
				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (status_level1[status] == STOP_GAME_STATUS)
					{
						if (!(bcd[2] < 48+4 && bcd[0] == 48 && bcd[1] == 48)) 
						{
							decrease_score(4);
						}
					}
					else
					{
						add_score(1);
					}
					
					FRAME_CURRENT++;
					if (FRAME_CURRENT > 3) FRAME_CURRENT = 0;
					
					memcpy(0xC000 + 0x1500, frame_porn_pattern[FRAME_CURRENT], 5144);
					//memcpy_jump_64(0xC000 + 0x1000, frame_porn_pattern[FRAME_CURRENT]);
					gotoxy(10,29);
					puts_cons("SCORE");
					gotoxy(10,30);
					puts_cons(bcd);
					time_game+= 24;
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (k > 0)
					{
						FRAME_CURRENT = 1;
					}
				}
				

				
				if (time_game > 128*9)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						gotoxy(19,7);
						puts_cons(ingame_quote[status_level1[status]]);
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
						memset((volatile void *) 0xFFFF-2048, 0, 2048);
						gotoxy(0,28);
						puts_cons(story_text[text_progress]);
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
	
	memset((volatile void *) 0xC000, 0, 16384);
	
	gotoxy(0,0);
	switch(mode)
	{
		case 0:
			#asm
			lxi	d,_title_data
			push	d
			lxi	b,0C000h
			CALL _dzx0_decompress
			pop d
			#endasm;
			gotoxy(13,25);
			puts_cons("COPYRIGHT INFRINGEMENT");
			gotoxy(1,29);
			puts_cons("By gameblabla 2023");
			gotoxy(14,20);
			puts_cons("PRESS KEY TO START");
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			//memcpy_jump_64(0xC000 + 0x1000, frame_porn_pattern[FRAME_CURRENT]);
			memcpy(0xC000 + 0x1500, frame_porn_pattern[FRAME_CURRENT], 5144);
			gotoxy(19,7);
			puts_cons(ingame_quote[status_level1[status]]);
			gotoxy(10,29);
		break;
		case 2:
			#asm
			lxi	d,_bakura_data
			push d
			lxi	b,0C000h
			CALL _dzx0_decompress
			pop d
			#endasm;
			gotoxy(0,28);
			if (bcd[0] < 48+1)
			{
				puts_cons("Guess you'll stay there for a while.");
			}
			else
			{
				puts_cons("Congrats! I'll leave you alone...\nFor now, anyway.");
			}
		break;
		case 3:
			#asm
			lxi	d,_bakura_data
			push	d
			lxi	b,0C000h
			CALL _dzx0_decompress
			pop d
			#endasm;
			gotoxy(0,28);
			puts_cons("Oh my, What do we have here?");
		break;
		case 4:
			puts_cons(text_minigame_1);
		break;
	}
}

