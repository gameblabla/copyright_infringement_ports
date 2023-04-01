/*	example code for llvm-mos, for NES
 *  writing a full screen from RLE compressed .h file
 *  -can only be done with rendering off
 *	using neslib
 *	Doug Fraker 2018
 */

#include <nesdoug.h>
#include <neslib.h>
#include <bank.h>
#include "bakura.h"
#include "porn1_nametable.h"
#include "porn2_nametable.h"
#include "porn3_nametable.h"
#include "porn4_nametable.h"
#include "porn5_nametable.h"
#include "porn6_nametable.h"

asm (".globl __prg_ram_size\n__prg_ram_size=8\n");

const char bakura_palette[] = { 0x0f, 0x16, 0x24, 0x30, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f };
const unsigned char porn_pal[16] = { 0x0f, 0x16, 0x09, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30, 0x0f, 0x30 };

  static const char palette_sp[16] = {
      0x0f, 0x30, 0x0f, 0x2f // black, black, yellow
  };


  //vram_unrle(bakura_name);
  // bakura_nametable
unsigned char frames = 0;
unsigned char sprid;
unsigned char delayf = 0;
unsigned char pad1;

static const char text[] = "Sprites";
const unsigned char* rlename_table[] =
{
	porn1_nametable, porn2_nametable, porn3_nametable,
	porn4_nametable, porn5_nametable, porn6_nametable,
};

int main(void) {

	ppu_off(); // screen off
	pal_bg(porn_pal); //	load the palette
	pal_spr(palette_sp);

	set_chr_bank_0(1);
	vram_adr(NAMETABLE_A);
	vram_unrle(porn1_nametable);
	ppu_on_all();
				
	oam_size(0);
	bank_spr(1);
	
	oam_clear();
	oam_spr(16,192,'F'-55,0);
	oam_spr(24,192,'U'-56,0);
	oam_spr(32,192,'C'-55,0);
	oam_spr(40,192,'K'-55,0);

	while (1) 
	{
		char pad1 = pad_trigger(0);
		
		delayf++;

		if (delayf > 1)
		{
			ppu_off();
			//set_vram_buffer();
			set_chr_bank_0(frames+1);
			vram_adr(NAMETABLE_A);
			vram_unrle(rlename_table[frames]);
			//set_vram_update(NULL);
			ppu_on_all();
			frames++;
			if (frames > 5) frames = 0;
			delayf = 0;
		}


		ppu_wait_nmi();
	
	}
}
