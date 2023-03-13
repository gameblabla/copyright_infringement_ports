
#include <input.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "zx1.h"
#include <stdio.h>
#include <sys/ioctl.h>
#ifndef __SDCC
#include <psg/wyz.h>
#include <spectrum.h>
#else
#include <arch/zx.h>
#endif
#include <input.h>
#include "text.h"

extern unsigned char rikuto_chr[];
extern unsigned char rikuto_clr[];

extern unsigned char rikuto[];
extern unsigned char titleimg[];
extern unsigned char frame1[];
extern unsigned char frame1_clr[];
extern unsigned char frame2_clr[];
extern unsigned char frame3_clr[];
extern unsigned char frame4_clr[];
extern unsigned char frame5_clr[];

extern unsigned char frame2[];
extern unsigned char frame3[];
extern unsigned char frame4[];
extern unsigned char frame5[];

const unsigned char status_level1[] =
{ 2, 3, 0, 2, 3, 0, 2, 3, 0, 4 };

uint8_t status;
uint8_t time_game;
uint8_t global_score, frame_picture;
uint8_t game_mode;
uint8_t key;
uint8_t temp;
uint8_t joys;

// Reusing variables but setting it as macro
#define story_sofar time_game
#define refresh_pic temp
#define kempston_available temp

void titlescreen() __z88dk_fastcall;
void details_beforemj1() __z88dk_fastcall;
void switch_gamemode(unsigned char mode) __z88dk_fastcall;
void story() __z88dk_fastcall;
void results() __z88dk_fastcall;
void decrease_score() __z88dk_fastcall;
void ingame_fucking() __z88dk_fastcall;

#define my_gotoxy(x,y) zx_movecursorto(y, x)
#define my_puts zx_printf
#define my_textbackground zx_setpaper
#define my_textcolor zx_setink


/* Macros for ingame */

#define add_score(sc) global_score += sc;
#define decrease_score() if (global_score > 2) global_score-=1;

#define add_frame_picture() \
	frame_picture++; \
	if (frame_picture > 7) frame_picture = 0; \
	refresh_pic = 1; \

#define sprite_text(id) \
	my_gotoxy(0, 21); \
	my_puts(ingame_quote[id]); \


static void clear_text() __z88dk_fastcall
{
	my_gotoxy(0, 21);
	my_puts("                                                    ");
	my_gotoxy(0, 22);
	my_puts("                                                    ");
}

// Further helps reduce space
const unsigned char* const frame_porn_pattern[]=
{
	frame2,frame2,frame3,frame3,frame4,
	frame5,frame5,frame1
};
const unsigned char* const frame_porn_clr[]=
{
	frame2_clr,frame2_clr,frame3_clr,frame3_clr,frame4_clr,
	frame5_clr,frame5_clr,frame1_clr
};

#define picture() \
	if (refresh_pic == 1) { \
	dzx1_mega(frame_porn_clr[frame_picture], 16384 + 6144); \
	dzx1_mega(frame_porn_pattern[frame_picture], 16384); \
	refresh_pic = 0; } \
	sprite_text(status_level1[status]); \
	my_gotoxy(0, 22); \
	zx_printf("Score : %d  ",global_score); \
	

#ifndef __SDCC
struct in_UDK joykeys;
#endif

#ifndef __SDCC
uint (*joyfunc)(struct in_UDK *) __z88dk_fastcall;
#else
uint (*joyfunc)(struct in_UDK *);
#endif
#define Key_pressed() (in_FIRE & joys)


void setupcursor()  __z88dk_fastcall
{
#ifndef __SDCC
	joykeys.fire  = in_LookupKey('0');
	joykeys.left  = in_LookupKey('5');
	joykeys.right = in_LookupKey('8');
	joykeys.up    = in_LookupKey('7');
	joykeys.down  = in_LookupKey('6');

	joyfunc = in_JoyKeyboard;
#endif
}

static void test_kemp() __z88dk_fastcall
{
#ifndef __SDCC
	kempston_available = zx_kempston();
	if (kempston_available == 1)
	{
		my_puts("Push button to play");
		while (1)
		{
			joyfunc = in_JoyKeyboard;
			joykeys.fire  = in_LookupKey(' ');
			joys = (joyfunc) (&joykeys);
			if (in_FIRE & joys) return;
			joykeys.fire  = in_LookupKey('m');
			joys = (joyfunc) (&joykeys);
			if (in_FIRE & joys) return;
			joykeys.fire  = in_LookupKey('c');
			joys = (joyfunc) (&joykeys);
			if (in_FIRE & joys) { setupcursor(); return; }
			joyfunc = in_JoySinclair1;
			joys = (joyfunc) (&joykeys);
			if (in_FIRE & joys) return;
			joyfunc = in_JoySinclair2;
			joys = (joyfunc) (&joykeys);
			if (in_FIRE & joys) return;
			if (kempston_available)
			{
				joyfunc = in_JoyKempston;
				joys = (joyfunc) (&joykeys);
				if (in_FIRE & joys) return;
			}
		}
	}	
#endif
}




static void Clear_attributes() __z88dk_fastcall
{
#ifdef __SDCC
	// Clear attributes
	__asm
	xor a
	ld hl, $5800
	ld bc, 768
	ld (hl), a
	push hl
	pop de
	inc de
	ldir
	__endasm;
#else
	// Clear attributes
	#asm
	xor a
	ld hl, $5800
	ld bc, 768
	ld (hl), a
	push hl
	pop de
	inc de
	ldir
	#endasm	
#endif
}

#ifdef SOUND
extern void sound_init() __z88dk_fastcall;
extern void sound_play(unsigned char p) __z88dk_fastcall;
#endif

void main()
{

#ifndef __SDCC
	joykeys.fire  = in_LookupKey(' ');
	joykeys.left  = in_LookupKey('o');
	joykeys.right = in_LookupKey('p');
	joykeys.up    = in_LookupKey('q');
	joykeys.down  = in_LookupKey('a');
	joyfunc = in_JoyKeyboard;
#endif
	zx_border(0);

#ifdef SOUND
	sound_init();
#endif
	my_gotoxy(0,4);
	test_kemp();
	switch_gamemode(0);
#ifdef SOUND
	sound_play(2);
#endif

	bit_open();
	

	while(1)
	{
#ifndef __SDCC
		joys = (joyfunc) (&joykeys);
#endif
		switch(game_mode)
		{
			case 0:
				if (Key_pressed()) switch_gamemode(3); 
			break;
			case 1:
				ingame_fucking();
			break;
			case 2:
				if (Key_pressed()) switch_gamemode(0);
			break;
			case 3:
				story();
			break;
			case 4:
			if (Key_pressed())
			{
				switch_gamemode(1);
			}
			break;
		}
	}

}


void story() __z88dk_fastcall
{
	if (Key_pressed())
	{
		bit_fx4(0);
		if (story_sofar > 3)
		{
			switch_gamemode(4);
		}
		else
		{
			my_textcolor(0);
			clear_text();
			my_textcolor(7);
			my_gotoxy(0,21);
			story_sofar++;
			my_puts(text_introduction_firstline[story_sofar]);
		}
	}
}

void switch_gamemode(unsigned char mode) __z88dk_fastcall
{
	game_mode = mode;
	status = 0;
	time_game = 0;
	
	Clear_attributes();
	my_textbackground(0);
	my_textcolor(7);
	my_gotoxy(0,0);
	switch(mode)
	{
		/* Titlescreen */
		case 0:
			bit_fx4(3);
			dzx1_agile_rcs(titleimg, 16384);
			zx_movecursorto(18, 24);
			zx_printf("COPYRIGHT INFRINGEMENT");
			my_textcolor(4);
			zx_movecursorto(20, 24);
			zx_printf("THE REGRESSIVE RIGHT");
			
		break;
		
		/* In-game (gameplay) */
		case 1:
			frame_picture = 4;
			refresh_pic = 1;
			global_score = 0;
			clear_text();
		break;
		
		case 2:
			if (global_score < 50)
			{
				bit_fx(5);
				my_puts(gameover_mj1);
				
			}
			else if (global_score < 100)
			{
				bit_fx(5);
				my_puts(gameover_mj1_alternate);
			}
			else
			{
				bit_fx(7);
				dzx1_agile_rcs(rikuto, 16384);
				my_gotoxy(0,19);
				my_puts("Congrats ! Haha!\nYou're a real Alpha male !\nI should spare you for now.\nHope you 'enjoyed' my game.\nSee ya.");
			}
		break;

		/* Introduction */
		case 3:
			bit_fx4(0);
			dzx1_agile_rcs(rikuto, 16384);
			story_sofar = 0;
			my_gotoxy(0,21);
			my_puts(text_introduction_firstline[story_sofar]);

		break;

		case 4:
			
			my_puts(text_minigame_1);
			bit_fx4(5);
		break;
	}

}




void ingame_fucking() __z88dk_fastcall
{
	picture();

	time_game+=4;
	
	if (status_level1[status] != 4)
	{
		if (frame_picture != 4)
		{
			if (status_level1[status] == 0 || status_level1[status] == 3)
			{
				add_score(1);
			}
			else if (status_level1[status] == 2)
			{
				decrease_score();
			}
			
			if (frame_picture == 0 || frame_picture != 0)
			{
				add_frame_picture();
			}
		}
		
		if (frame_picture == 4 && Key_pressed())
		{
			bit_fx(4);
			frame_picture = 5;
			refresh_pic = 1;
		}

	}
	
	if (time_game > 123)
	{
		if (status_level1[status] == 4)
		{
			switch_gamemode(2);
		}
		time_game = 0;
		status++;
	}


}

