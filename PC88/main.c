#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <games.h>
#include <graphics.h>
#include <arch/pc88.h>
#include <sys/ioctl.h>

#define SetBGColor(c) outp(0x52, c << 4);
#define SetBorderColor(c) outp(0x52, c); // PC8001 only?
#define SETBANK_BLUE() outp(0x5c, 0xff);
#define SETBANK_RED() outp(0x5d, 0xff);
#define SETBANK_GREEN() outp(0x5e, 0xff);
#define SETBANK_MAINRAM() outp(0x5f, 0xff);

#define bit(n) (1 << n)

#define FASTMEM_ON 1 
#define FASTMEM_OFF 0 
#define CLR_BLUE 1

#define SYS_CTL_REGB    0x31
#define V1MODE_FLAG 0x80

#define vu8 volatile unsigned char
#define u8 unsigned char
#define u16 unsigned short
#define GetVRAMPos(x,y) (vu8*)(((y)*80)+0xc000 + (x))

#define EnableALU(fastmem) outp(ALU_MODE_CTRL, 0xC9|(fastmem<<4)); 
#define ExpandedGVRAM_On() outp(EXPANDED_GVRAM_CTRL, 0x80);
#define ExpandedGVRAM_Off() outp(EXPANDED_GVRAM_CTRL, 0); 
#define DisableALU(fastmem) outp(ALU_MODE_CTRL, 0x89|(fastmem<<4)); 

extern unsigned char frame1_b[];
extern unsigned char frame1_g[];
extern unsigned char frame1_r[];

extern unsigned char title_b[];
extern unsigned char title_g[];
extern unsigned char title_r[];

extern unsigned char anim1_g[];
extern unsigned char anim1_r[];

extern unsigned char anim2_g[];
extern unsigned char anim2_r[];

extern unsigned char anim3_g[];
extern unsigned char anim3_r[];

extern unsigned char anim4_g[];
extern unsigned char anim4_r[];

extern void dzx1_mega();
extern void LZ48_decrunch();

unsigned char k, game_mode, text_progress;
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
#define delay i
unsigned char time_game;

const char ingame_quote[5][12] =
{
	"KEEP IT UP!",
	"STOP       ",
	"FUCK HIM ! ",
	"FINISH !   ",
};


const char text_minigame_1[] = 
{
	"Your goal is to FUCK Zhongli!\nYou must follow the instructions to get more points.\nPenetrate him by pressing a key then pull out by releasing it.\nGood luck !"
};

/* This is done to save further space as game_mode 3, 0 , 1 all shared the same code */
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };
static unsigned char bcd[4];

static void myownprint(const char* str, unsigned char x, unsigned char y)
{
	__asm
	ld a, 0x00
    out (0x5F), a
	__endasm;
	gotoxy(x, y);
	puts_cons((const char*)str);
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



static void Clear_Text() __z88dk_fastcall
{
	// Switch to TEXT VRAM
	__asm
	ld a, 0x00
    out (0x5F), a
	__endasm;
	memset((void*)0xC000, 0, 16000);
	
	// Switch to graphics mode
	/*__asm
	ld a, 0x00
    out (0x5c), a
	__endasm;*/
}

static void Draw_Titlescreen() __z88dk_fastcall
{
	__asm
		; ExpandedGVRAM_On
        ld c, 0x35 
        ld a, 0x80 
        out (c), a 

		; EnableALU(FASTMEM_ON);
        ld c, 0x32
        ld a, 16 
        out (c), a 

		; Switch to Blue
		ld a, 0xff
		out (0x5c), a
		
		ld hl, _title_b
		ld de, 0xC000
		call _dzx1_mega
		
		; Switch to Red
		ld a, 0xff
		out (0x5d), a
		
		ld hl, _title_r
		ld de, 0xC000
		call _dzx1_mega
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _title_g
		ld de, 0xC000
		call _dzx1_mega

		; DisableALU(FASTMEM_OFF);
        ld c, 0x32
        ld a, 0 
        out (c), a 

		; ExpandedGVRAM_Off();
        ld c, 0x35 
        ld a, 0 
        out (c), a 

		; SetMAINRAMBANK();
        ld c, 0x55
        ld a, 0xff 
        out (c), a 
	__endasm;
}

static void Draw_Bakura() __z88dk_fastcall
{
	__asm
		; ExpandedGVRAM_On
        ld c, 0x35 
        ld a, 0x80 
        out (c), a 

		; EnableALU(FASTMEM_ON);
        ld c, 0x32
        ld a, 16 
        out (c), a 

		; Switch to Blue
		ld a, 0xff
		out (0x5c), a
		
		ld hl, _frame1_b
		ld de, 0xC000
		call _dzx1_mega
		
		; Switch to Red
		ld a, 0xff
		out (0x5d), a
		
		ld hl, _frame1_r
		ld de, 0xC000
		call _dzx1_mega
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _frame1_g
		ld de, 0xC000
		call _dzx1_mega

		; DisableALU(FASTMEM_OFF);
        ld c, 0x32
        ld a, 0 
        out (c), a 

		; ExpandedGVRAM_Off();
        ld c, 0x35 
        ld a, 0 
        out (c), a 

		; SetMAINRAMBANK();
        ld c, 0x55
        ld a, 0xff 
        out (c), a 
	__endasm;
}
    
static void Draw_Frame1() __z88dk_fastcall
{
	__asm

		; Switch to Red
		ld a, 0xff
		out (0x5d), a
		
		ld hl, _anim1_r
		ld de, 0xC000+4000
		call _LZ48_decrunch
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim1_g
		ld de, 0xC000+4000
		call _LZ48_decrunch
	__endasm;
}

static void Draw_Frame2() __z88dk_fastcall
{
	__asm

		; Switch to Red
		ld a, 0xff
		out (0x5d), a
		
		ld hl, _anim2_r
		ld de, 0xC000+4000
		call _LZ48_decrunch
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim2_g
		ld de, 0xC000+4000
		call _LZ48_decrunch
	__endasm;
}

static void Draw_Frame3() __z88dk_fastcall
{
	__asm

		; Switch to Red
		ld a, 0xff
		out (0x5d), a
		
		ld hl, _anim3_r
		ld de, 0xC000+4000
		call _LZ48_decrunch
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim3_g
		ld de, 0xC000+4000
		call _LZ48_decrunch
	__endasm;
}

static void Draw_Frame4() __z88dk_fastcall
{
	__asm

		; Switch to Red
		ld a, 0xff
		out (0x5d), a
		
		ld hl, _anim4_r
		ld de, 0xC000+4000
		call _LZ48_decrunch
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim4_g
		ld de, 0xC000+4000
		call _LZ48_decrunch
	__endasm;
}

//pc88_fm_addr(); Check for FM, 0 if not found
//pc88_mode(); //2=V2, 1=V1, 0=N Mode
//pc88_8mhz(); Check if CPU is 8Mhz
unsigned char speedup;
void main()  __z88dk_fastcall
{
	speedup = 48+3;
	if (pc88_8mhz() == 1)
	{
		speedup = 48+4;
	}
	
	// Set V1 mode
    outp(0x5C, 0x61);
    outp(0x5D, 0x00);
    
	switch_gamemode(0);

	bit_open();

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
					
					switch(FRAME_CURRENT)
					{
						case 0:
							Draw_Frame1();
						break;
						case 1:
							Draw_Frame1();
						break;
						case 2:
							Draw_Frame2();	
						break;
						case 3:
							Draw_Frame3();
						break;
						case 4:
							Draw_Frame4();	
						break;
					}
					myownprint("SCORE", 0, 18);
					myownprint(bcd, 10, 18);
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (k > 0)
					{
						bit_fx(4);
						FRAME_CURRENT = 1;
					}
				}
				

				
				
				
				if (time_game > 70)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						myownprint(ingame_quote[status_level1[status]], 0, 19);
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
						bit_fx4(2);
						Clear_Text();
						
						switch(text_progress)
						{
							case 1:
								myownprint("You're going to be the perfect victim for my games!", 0, 18);
							break;
							case 2:
								myownprint("I'm Rikuto, i offer you the chance to prove your worth in a trial", 0, 18);
							break;
							case 3:
								myownprint("Don't be scared, it'll be fun!", 0, 18);
							break;
							case 4:
								myownprint("Your challenge will be this: you will be on the other end and punish them", 0, 18);
							break;
						}
					}
				}
			break;
		}
	
		pc88_wait_vblank();
	}
}

void switch_gamemode(unsigned char mode)  __z88dk_fastcall
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;
	clg();
	Clear_Text();
	
	switch(mode)
	{
		case 0:
			Draw_Titlescreen();
			bit_fx4(3);
			//putsprite(0,14, 0, title_spr);
			myownprint("COPYRIGHT INFRINGEMENT", 32, 12);
			myownprint("THE REGRESSIVE RIGHT",  33, 13);
		
			myownprint("PUSH KEY TO START",  34, 17);
			
			myownprint("BY GAMEBLABLA, 2023", 0, 19);
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
			__asm
			; ExpandedGVRAM_On
			ld c, 0x35 
			ld a, 0x80 
			out (c), a 

			; EnableALU(FASTMEM_ON);
			ld c, 0x32
			ld a, 16 
			out (c), a 
			__endasm;
			Draw_Frame1();
			myownprint("SCORE", 0, 18);
			myownprint(bcd, 10, 18);
			myownprint(ingame_quote[status_level1[status]], 0, 19);
		break;
		case 2:
			Clear_Text();
			myownprint("                            ", 0, 18);
			myownprint("                            ", 0, 19);
			Draw_Bakura();
			myownprint("Your SCORE was ", 0, 0);
			myownprint(bcd, 15, 0);
			
			if (bcd[0] < speedup-1)
			{
				bit_fx(5);
				myownprint("Wow, you actually suck! Guess you'll stay there for a while. Haha!", 0, 18);
				
			}
			else if (bcd[0] < speedup)
			{
				bit_fx(5);
				myownprint("Not bad but you can do better! Otherwise you can't leave this place!", 0, 18);
				
			}
			else
			{
				bit_fx(7);
				myownprint("Haha, congrats!\nI'll leave you alone... For now, anyway.\nI'll see you later, bye for now", 0, 18);
				
			}
		break;
		case 3:
			Draw_Bakura();
			myownprint("Oh my, what do we have here? You look so innocent...", 0, 18);
			bit_fx4(0);
		break;
		case 4:
			myownprint(text_minigame_1,  0, 0);
			bit_fx4(5);
		break;
	}
	
}

