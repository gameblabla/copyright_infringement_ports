#define BANK 2
#pragma bank 2

#include <input.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <psg/wyz.h>

extern wyz_song mysong;
extern wyz_effects myeffects;

static void playmusic(void) __z88dk_fastcall
{
   M_PRESERVE_MAIN;
   M_PRESERVE_INDEX;
   ay_wyz_play();
   M_RESTORE_INDEX;
   M_RESTORE_MAIN;
}

static void setup_int() __z88dk_fastcall {
#ifndef NO_INTERRUPT
#if __SPECTRUM__
   zx_im2_init(0xd300, 0xd4);
   add_raster_int(0x38);
#endif
#ifndef NO_INTERRUPT_INIT
   im1_init();
#endif
	add_raster_int(playmusic);
#endif
}

void sound_init() __z88dk_fastcall
{
	ay_wyz_init(&mysong);
	ay_wyz_effect_init(&myeffects);
	ay_wyz_start(0);
	setup_int();
}

void sound_play(unsigned char p) __z88dk_fastcall
{
	ay_wyz_start_effect(3, p);	
}
