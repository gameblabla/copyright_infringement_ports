#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <graphics.h>
#include <games.h>
#include <conio.h>

#pragma output CRT_ORG_CODE = 0x8000
#pragma output CRT_ENABLE_STDIO=0
#pragma output CLIB_EXIT_STACK_SIZE=0
#pragma output CLIB_CONIO_NATIVE_COLOUR=0

// zcc +lviv -SO3 main.c data.asm  dzx1.asm -o main.bin -create-app -pragma-redirect:fputc_cons=fputc_cons_generic

extern void dzx1_decompress(void* src, void* dst);
extern unsigned char bakura_data[];
extern unsigned char frame1_data[];
extern unsigned char frame2_data[];
extern unsigned char frame3_data[];

const unsigned char text_minigame_1[] = 
{
	"Your goal is to FUCK Zhongli!\nYou must follow the instructionsto get more points.\nPenetrate him by pressing a key then pull out by releasing it.\nGood luck !"
};


static void lviv_enable_video_ram() {
    #asm
    ld  a, $fd
    out ($c2), a
    #endasm
}

static void lviv_disable_video_ram() {
    #asm
    ld  a, $ff
    out ($c2), a
    #endasm
}

static void lviv_clear_video_ram() {
	#asm
		; Enable VIDEO RAM
		ld  a, $fd
		out ($c2), a

		; Clear RAM
        ld  hl, $4000  ; starting address of video RAM
        ld  de, $4001  ; next address
        ld  bc, $3FFF  ; count of bytes to clear
        ld  (hl), $00  ; clear first byte
        ldir           ; clear the remaining bytes

		; Disable VIDEO RAM
		ld  a, $ff
		out ($c2), a
    #endasm
}

static void lviv_clear_video_ram_game() {
	#asm
		; Enable VIDEO RAM
		ld  a, $fd
		out ($c2), a

		; Clear RAM
        ld  hl, $4000  ; starting address of video RAM
        ld  de, $4001  ; next address
        ld  bc, $3FFF  ; count of bytes to clear
        ld  (hl), $F0  ; clear first byte
        ldir           ; clear the remaining bytes

		; Disable VIDEO RAM
		ld  a, $ff
		out ($c2), a
    #endasm
}

static void lviv_clear_video_ram_bakura() {
	#asm
		; Enable VIDEO RAM
		ld  a, $fd
		out ($c2), a

		; Clear RAM
        ld  hl, $7000  ; starting address of video RAM
        ld  de, $7001  ; next address
        ld  bc, $0FFF  ; count of bytes to clear
        ld  (hl), $00  ; clear first byte
        ldir           ; clear the remaining bytes

		; Disable VIDEO RAM
		ld  a, $ff
		out ($c2), a
    #endasm
}

unsigned char game_mode, k, text_progress, status, i;
unsigned short time_game, FRAME_CURRENT;
unsigned char bcd[5];

void switch_gamemode(unsigned char mode)  __z88dk_fastcall;
/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

const char ingame_quote[5][12] =
{
	"KEEP IT UP!",
	"STOP       ",
	"FUCK HIM ! ",
	"FINISH !   ",
};


#define add_score(add) \
	bcd[4] += add; \
	if (bcd[4] > 57) { \
		bcd[3]+= 1; \
		bcd[4] = 48; } \
	if (bcd[3] > 57) { \
		bcd[2]+= 1; \
		bcd[3] = 48; } \

#define decrease_score(dec) \
	bcd[4] -= dec; \
	if (bcd[4] < 48) \
	{ \
		bcd[3]-= 1; \
		bcd[4] = 57; \
	} \
	if (bcd[3] < 48) \
	{ \
		bcd[2]-= 1; \
		bcd[3] = 57; \
	} \

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

#define SOUND_ON() *((uint8_t *)0xc1) |= 0x80;
#define SOUND_OFF() *((uint8_t *)0xc1) &= ~ 0x80;

#define GENERATE_BEEP() *((uint8_t *)0xc2) |= 0x01;
#define STOP_BEEP() *((uint8_t *)0xc2) &= ~0x01;

extern void sound_play();
extern void confirm_sound_play();
extern void fuck_sound_play();

int main() {
	

	lviv_clear_video_ram();
	lviv_enable_video_ram();
	
    #asm
	; Initialize 8255 PPI
    and a, $80
    out ($c1), a
    #endasm
    
	switch_gamemode(0);
	
    while (1)
    {
		k = getk();
		time_game++;
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
						if (!(bcd[4] == 48 && bcd[2] == 48 && bcd[3] == 48)) 
						{
							decrease_score(1);
						}
					}
					else
					{
						add_score(1);
					}
					
					gotoxy(11, 23);
					puts_cons(bcd);
					
					FRAME_CURRENT++;
					if (FRAME_CURRENT > 3)
					{
						SOUND_OFF();
						FRAME_CURRENT = 0;
					}
					
					switch(FRAME_CURRENT)
					{
						case 0:
						case 1:
						#asm
						lxi	d,_frame1_data
						push d
						lxi	b,05000h
						push b
						CALL _dzx1_decompress
						pop d
						pop b
						#endasm;
						break;
						case 2:
						#asm
						lxi	d,_frame2_data
						push d
						lxi	b,05000h
						push b
						CALL _dzx1_decompress
						pop d
						pop b
						#endasm;
						break;
						case 3:
						#asm
						lxi	d,_frame3_data
						push d
						lxi	b,05000h
						push b
						CALL _dzx1_decompress
						pop d
						pop b
						#endasm;
						break;
					}
					time_game+=128*1;
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (k > 0)
					{
						fuck_sound_play();
						FRAME_CURRENT = 1;
					}
				}
				

				/*myownprint("SCORE", 5, 0, 28);
				myownprint(bcd, 3, 10, 28);
				
				myownprint(ingame_quote[status_level1[status]], 11, 48, 28);*/
				
				if (time_game > 128*20)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						gotoxy(11, 5);
						puts_cons(ingame_quote[status_level1[status]]);
					}
				}
			

			break;
			case 3:
				if (k > 0 )
				{
					text_progress++;
					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						lviv_clear_video_ram_bakura();
						gotoxy(0, 24);
						switch(text_progress)
						{
							case 1:
								puts_cons("You're going to be the perfect\nvictim for my games!");
							break;
							case 2:
								puts_cons("I'm Rikuto, i offer you the\nchance to prove your\nworth in a trial");
							break;
							case 3:
								puts_cons("Don't be scared, it'll be fun!");
							break;
							case 4:
								puts_cons("Your challenge will be this:\nyou will be on the other end and punish them");
							break;
						}
					}
					confirm_sound_play();
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
	FRAME_CURRENT = 0;
	i = 0;
	
	lviv_clear_video_ram();
	
	textbackground(0);
	textcolor(2);
	gotoxy(0, 24);
	
	SOUND_OFF();
	
	switch(mode)
	{
		case 0:
			#asm
			ld  a, $53
			out ($c1), a
			#endasm
			gotoxy(5, 2);
			puts_cons("COPYRIGHT INFRINGEMENT");
			gotoxy(10, 4);
			puts_cons("BY GAMEBLABLA");
			
			gotoxy(7, 20);
			puts_cons("PRESS KEY TO START");
			
			sound_play();
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = 48;
			bcd[4] = 48;
			#asm
			ld  a, $24
			out ($c1), a
			#endasm
			lviv_clear_video_ram_game();
			
			#asm
			lxi	d,_frame1_data
			push d
			lxi	b,05000h
			push b
			CALL _dzx1_decompress
			pop d
			pop b
			#endasm;
			
			textbackground(2);
			textcolor(1);
			gotoxy(11, 5);
			puts_cons(ingame_quote[status_level1[status]]);
			gotoxy(11, 21);
			puts_cons("SCORE :");
		break;
		case 2:
			// Change Palette to bakura's
			#asm
			ld  a, $53
			out ($c1), a
			#endasm
			
			#asm
			lxi	d,_bakura_data
			push d
			lxi	b,04800h
			CALL _dzx1_decompress
			pop d
			#endasm;

			
			if (bcd[2] < 48+1)
			{
				sound_play();
				puts_cons("Guess you'll stay there for\na while...");

			}
			else if (bcd[3] < 48+2)
			{
				sound_play();
				puts_cons("Not bad but you can do better!\nOtherwise you can't leave this place!");
			}
			else
			{
				fuck_sound_play();
				fuck_sound_play();
				puts_cons("Haha, congrats!\nI'll leave you alone... For now, anyway.\nI'll see you later, bye!");
			}
		break;
		case 3:
			// Change Palette to bakura's
			#asm
			ld  a, $53
			out ($c1), a
			#endasm
			
			#asm
			lxi	d,_bakura_data
			push d
			lxi	b,04800h
			CALL _dzx1_decompress
			pop d
			#endasm;
			gotoxy(0, 24);
			puts_cons("Oh my, what do we have here?\nYou look so innocent.");
#ifdef SOUND_SUPPORT
			bit_fx4(0);
#endif
		break;
		case 4:
			gotoxy(0, 0);
			puts_cons(text_minigame_1);
#ifdef SOUND_SUPPORT
			bit_fx4(5);
#endif
		break;
	}
	
}
