#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <compress/zx0.h>
#include <compress/zx1.h>
#include <graphics.h>
#include <conio.h>
#include <sys/ioctl.h>
#include <sound.h>

unsigned char k, game_mode, text_progress;
unsigned short delay;
#define FRAME_CURRENT text_progress

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
unsigned short delay;
unsigned short time_game;
int file_handle;

const char ingame_quote[5][12] =
{
	"KEEP IT UP!",
	"STOP       ",
	"FUCK HIM ! ",
	"FINISH !   ",
};


const unsigned char text_minigame_1[] = 
{
	"Your goal is to FUCK Zhongli!\nYou must follow the instructions to get more points.\nPenetrate him by pressing a key\nthen pull out by releasing it.\nGood luck !"
};

extern unsigned char buffer_tocopy[];
extern void Copy_Buffer();
unsigned char tmp[10001];
unsigned char FRAME_CURRENT;
const unsigned char* address_frame[8] =
{
	tmp,
	tmp + 2000,
	tmp + 2000*2,
	tmp + 2000*3,
	tmp + 2000*4,
	tmp + 2000*3,
	tmp + 2000*2,
	tmp + 2000,
};


/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

static unsigned char bcd[3];

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
	bdos(0x0E, 1); // Needed for relative file loading
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
				delay++;
				
				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS && delay > 100)
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
					if (FRAME_CURRENT > 7) FRAME_CURRENT = 0;
					memcpy(buffer_tocopy, address_frame[FRAME_CURRENT], 1920-80);
					Copy_Buffer();
					
					gotoxy(0, 23);
					printk("SCORE : ");
					printk(bcd);
					
					gotoxy(40, 23);
					printk(ingame_quote[status_level1[status]]);
					
					delay = 0;
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

				
				if (time_game > 20000)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						gotoxy(40, 23);
						printk(ingame_quote[status_level1[status]]);
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
						gotoxy(0, 22);
						printk("                                                                                ");
						
						gotoxy(0, 22);
						switch(text_progress)
						{
							case 1:
								printk("You're going to be the perfect victim for my games!");
							break;
							case 2:
								printk("I'm Rikuto, i offer you the chance to prove your worth in a trial");
							break;
							case 3:
								printk("Don't be scared, it'll be fun!");
							break;
							case 4:
								printk("Your challenge will be this: you will be on the other end and punish them");
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
	delay = 0;
	memset(tmp, 0, sizeof(tmp));
	
	textbackground(0);
	clg();
	
	switch(mode)
	{
		case 0:
#ifdef SOUND_SUPPORT
			bit_fx4(3);
#endif
			//putsprite(0,14, 0, title_spr);
			//myownprint("COPYRIGHT INFRINGEMENT", 22, 5, 25);
			gotoxy(30, 5);
			printk("COPYRIGHT INFRINGEMENT");
			gotoxy(34, 8);
			textbackground(2);
			printk("BY GAMEBLABLA");
			textbackground(0);
			
			gotoxy(31, 23);
			printk("PRESS KEY TO START");
		break;
		case 1:
			gotoxy(30, 5);
			printk("NOW LOADING...");
			
			file_handle = open("FRAMES.BIN", O_RDONLY, _IOREAD | _IOUSE);
			read(file_handle, tmp, 10000);
			close(file_handle);
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			
			memcpy(buffer_tocopy, address_frame[FRAME_CURRENT], 1920-80);
			Copy_Buffer();
			
			gotoxy(0, 23);
			printk("SCORE : ");
			printk(bcd);
			
			gotoxy(40, 23);
			printk(ingame_quote[status_level1[status]]);
			
		break;
		case 2:
			file_handle = open("BAKURA.BIN", O_RDONLY, _IOREAD | _IOUSE);
			read(file_handle, tmp, 1442);
			close(file_handle);
			
			textbackground(2);
			putsprite(0, 0, 0, tmp);
			textbackground(0);
			
			//myownprint("Your SCORE was ", 14, 0, 0);
			//myownprint(bcd, 3, 15, 0);
			
			gotoxy(0, 22);
			if (bcd[0] < 48+2)
			{
				printk("Wow, you actually suck! Guess you'll stay there for a while. Haha!");
#ifdef SOUND_SUPPORT
				bit_fx(5);
#endif
			}
			else if (bcd[1] < 48+5)
			{
				printk("Not bad but you can do better! Otherwise you can't leave this place!");
#ifdef SOUND_SUPPORT
				bit_fx(5);
#endif
			}
			else
			{
				printk("Congrats! I'll leave you alone... For now, anyway. I'll see you later.");
#ifdef SOUND_SUPPORT
				bit_fx(7);
#endif
			}
		break;
		case 3:
			file_handle = open("BAKURA.BIN", O_RDONLY, _IOREAD | _IOUSE);
			read(file_handle, tmp, 1442);
			close(file_handle);
			
			textbackground(2);
			putsprite(0, 0, 0, tmp);
			textbackground(0);
			
			gotoxy(0, 22);
			printk("Oh my, what do we have here? You look so innocent.");
#ifdef SOUND_SUPPORT
			bit_fx4(0);
#endif
		break;
		case 4:
			gotoxy(0, 0);
			printk(text_minigame_1);
#ifdef SOUND_SUPPORT
			bit_fx4(5);
#endif
		break;
	}
	
}

