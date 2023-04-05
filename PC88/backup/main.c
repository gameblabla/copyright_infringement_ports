#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
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

extern void ENABLE_GRAM();
extern void DISABLE_GRAM();

void SetPixel(u16 x, u8 y, u8 c)
{
    u8 po = 7 - ((x & 0xff) % 8);
    vu8* p = (vu8*)(0xc000) + (y * 80) + (u8)(x / 8);
        
    if(c & 0b001){
        SETBANK_BLUE()
        *p |= bit(po);
    }
    if(c & 0b010){
        SETBANK_RED()
        *p |= bit(po);
    }
    if(c & 0b100){
        SETBANK_GREEN()
        *p |= bit(po);
    }

}

extern unsigned char frame1_b[];
extern unsigned char frame1_g[];
extern unsigned char frame1_r[];

extern unsigned char anim1_b[];
extern unsigned char anim1_g[];
extern unsigned char anim1_r[];

extern unsigned char anim2_b[];
extern unsigned char anim2_g[];
extern unsigned char anim2_r[];

extern unsigned char anim3_b[];
extern unsigned char anim3_g[];
extern unsigned char anim3_r[];

extern unsigned char anim4_b[];
extern unsigned char anim4_g[];
extern unsigned char anim4_r[];

extern void dzx0_standard();
extern void dzx1_mega();

static void SetTextVRAM() __z88dk_fastcall
{
	// Switch to TEXT VRAM
	__asm
	ld a, 0x00
    out (0x5F), a
	__endasm;
}

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
		call _dzx1_mega
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim1_g
		ld de, 0xC000+4000
		call _dzx1_mega
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
		call _dzx1_mega
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim2_g
		ld de, 0xC000+4000
		call _dzx1_mega
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
		call _dzx1_mega
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim3_g
		ld de, 0xC000+4000
		call _dzx1_mega
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
		call _dzx1_mega
		
		; Switch to Green
		ld a, 0xff
		out (0x5e), a
		
		ld hl, _anim4_g
		ld de, 0xC000+4000
		call _dzx1_mega
	__endasm;
}


    
//pc88_fm_addr(); Check for FM, 0 if not found
//pc88_mode(); //2=V2, 1=V1, 0=N Mode
//pc88_8mhz(); Check if CPU is 8Mhz

unsigned char FRAME_CURRENT;

int main()
{
    if(pc88_mode() != 2)
    {
        puts_cons("V2 MODE ONLY");
        while(1){};
    }
    
    // Set V2 mode (640x200, 8 colors)
    outp(0x5C, 0x62);
    outp(0x5D, 0x00);
	
	//int TGI_MODE = 2;
	//pc88_v2mode();
	//console_ioctl(IOCTL_GENCON_SET_MODE, &TGI_MODE);
	//pc88_v2mode();
	//ENABLE_GRAM();
	
    outp(0x5F, 0x00); // Switch back to text VRAM
    gotoxy(0, 18);
	puts_cons("Oh my, What do we have here ? You look so innocent...");
	//Clear_Text();
	//clg(); //Graphics only

	Draw_Bakura();
	
	clg();
	
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
	
	FRAME_CURRENT = 0;

	for(;;)
	{
		
		
		FRAME_CURRENT++;
		if (FRAME_CURRENT > 3) FRAME_CURRENT = 0;
		
		switch(FRAME_CURRENT)
		{
			case 0:
				Draw_Frame1();
			break;
			case 1:
				Draw_Frame2();	
			break;
			case 2:
				Draw_Frame3();
			break;
			case 3:
				Draw_Frame4();	
			break;
		}


		pc88_wait_vblank();
	}
	return 0;
}
