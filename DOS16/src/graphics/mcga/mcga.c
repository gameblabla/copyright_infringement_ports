#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <unistd.h>
#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "mcga.h"

static unsigned char __far *MCGA = (unsigned char __far*)0xA0000000L;
static uint32_t PLANE_SIZE = 0;

/* some video BIOS stuff */
#define _BIOS_SET_MODE			0x0000
#define _BIOS_SET_CURPOS		0x0200
#define _BIOS_GET_MODE			0x0F00
#define _BIOS_SET_PALETTE		0x1000

// Vsync
#define INPUT_STATUS      	0x03da
#define VRETRACE 			0x08


#define plot_pixel(x,y, color) MCGA[x+((y << 8) + (y << 6))] = color;

static void drawChar(char ch, unsigned short x, unsigned char y, unsigned char color_index)
{
	unsigned char i, j;
	unsigned char *charSprite;
	unsigned short plot_x, plot_y;
	unsigned char far *p;
	unsigned char m;
    unsigned int offset;
    unsigned char pixel_mask;
    unsigned char is_bit_set;
    
	charSprite = (ch << 3) + std_font; // Updated line
	// Draw charSprite as monochrome 8*8 image using given color
	i = 8;
	while (i--)
	{
		j = 8;
		while (j--)
		{
			if((charSprite[i] >> j) & 1)
			{
				plot_pixel(x + (7 - j), y + i, color_index);
			}
			else
			{
				plot_pixel(x + (7 - j), y + i, 0);
			}
		}
	}
	x += 8;
}

static unsigned char Init_MCGA(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	unsigned short i;
	
	screen_width = 320;
	screen_height = 200;
	
	/* set mode 13 */
	set_mode(0x13);
	
	// Calculate the size of each plane
	PLANE_SIZE = 64000;

	return 0;
}



static void MCGA_Draw_bitmap(BITMAP *bmp, short x, short y, unsigned long off)
{
	void *dst = MCGA;
    const void *src = bmp->data[0] + off;
    size_t count = bmp->plane_size[0];
    __asm {
        les di, dst
        lds si, src

        mov ax, count
        shr ax, 1
        mov cx, ax
        rep movsw
    }
	//memcpy(MCGA, (uint8_t far*) bmp->data[0] + off, bmp->plane_size[0]);
}

static void MCGA_DrawBMP_compressed(BITMAP *bmp, short x, short y)
{
	CAL_LZSAExpand(bmp->data[0], MCGA);
}

static void MCGA_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress)
{
    Generic_Load_SIF(file, b, s_width, s_height, dontdecompress);
}

void set_text_color(uint8_t color) {
    union REGS regs;

    regs.h.ah = 0x10;  // Video Services
    regs.h.al = 0x08;  // Set Text Color
    regs.h.bl = color; // Color value (0-15)

    int86(0x10, &regs, &regs);
}


static void MCGA_Load_palette(const char *file)
{
	int fd, i;
	unsigned totalsize;
	memset(VGA_8514_GAMEPAL, 0, sizeof(VGA_8514_GAMEPAL));
	_dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, VGA_8514_GAMEPAL, sizeof(VGA_8514_GAMEPAL), &totalsize);
	_dos_close(fd);

}

static void MCGA_Apply_palette()
{
	unsigned short i;
	outp(PALETTE_INDEX,0);	/* tell the MCGA that palette data is coming. */
	for(i=0;i<sizeof(VGA_8514_GAMEPAL);i++)
	{
		outp(PALETTE_DATA,VGA_8514_GAMEPAL[i]);    /* write the data */
	}
}

static void MCGA_Wait_for_retrace(void) 
{
	/* wait until done with vertical retrace */
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	/* wait until done refreshing */
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;
}

static void MCGA_Free_bmp(BITMAP *b)
{
	unsigned char i;
	if (b->data[0])
	{
		free(b->data[0]);
		b->data[0] = NULL;
	}
}


#define _BIOS_SET_CURPOS		0x0200
extern short VideoInt(short, short, short, short);
#pragma aux     VideoInt = \
                "push   bp      ", \
                "int    10h     ", \
                "pop    bp      ", \
                parm caller [ax] [bx] [cx] [dx] value [ax];




static void MCGA_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
    uint16_t screen_width_in_chars = 40;

    x = x >> 3;
    y = y >> 3;
	
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
}

static void MCGA_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
    uint16_t x;
    uint16_t screen_width_in_chars = screen_width >> 3;

    x = (screen_width_in_chars - strlen(str)) >> 1;
    y = y >> 3;
	
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
}

static void Kill_MCGA()
{
	set_mode(TEXT_MODE);
}

static void MCGA_Clear()
{
	memset(MCGA, 0, 64000);
}

static void MCGA_Vsync()
{
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;	
}

static void MCGA_stub()
{
	
}


VideoDevice Normal_MCGA = {
	"MCGA video",
	Init_MCGA,
	MCGA_Draw_bitmap,
	MCGA_DrawBMP_compressed,
	MCGA_stub, //Draw Sprite trans
	MCGA_stub, //Draw sprite non trans
	MCGA_Load_SIF,
	MCGA_Load_palette,
	MCGA_Apply_palette, // Set palette
	MCGA_Free_bmp,
	MCGA_Clear,
	MCGA_stub, // Flip
	MCGA_Vsync,
	Kill_MCGA,
	MCGA_Draw_Text, // Draw Text
	MCGA_Draw_Text_Center, // Draw Text
};

