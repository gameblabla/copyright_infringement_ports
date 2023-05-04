/*
 * Tandy Graphics mode
 * Supported :
 * - 320x200
 * - 160x200 (untested)
 * 
 * This should also support the PCjr.
 * Be careful with the PCJr however, as you are limited by its standard memory configuration !
 * This is less of an issue on the Tandy however.
 * Supports on the fly LZSA1 decompression
*/
#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "tandy.h"

#define _BIOS_SET_MODE			0x0000
#define _BIOS_SET_CURPOS		0x0200

uint8_t far *TANDY=(uint8_t *)0xB8000000L;

static const unsigned char cga_mask[] = {0xC0, 0x30, 0x0C, 0x03};
static const unsigned char cga_shift[] = {6, 4, 2, 0};

static unsigned short PLANE_SIZE;

#define OFFSET_VIDEO 0x2000

/* this is what'll be used to call BIOS interrupt 10 to set the video mode */
extern short VideoInt(short, short, short, short);

#pragma aux     VideoInt = \
                "push   bp      ", \
                "int    10h     ", \
                "pop    bp      ", \
                parm caller [ax] [bx] [cx] [dx] value [ax];

static inline int pixelOffset(unsigned short x, unsigned short y)
{
    unsigned short base = (OFFSET_VIDEO-40) * (y & 0x3);
    return (y << 5) + (y << 3) + (x >> 1) + base;
}

#if 0

void setPixel(unsigned short x, unsigned short y, unsigned char color) {
    int b, m; /* bits and mask */
    unsigned char far *p;
    unsigned char c;

    /* address section differs depending on odd/even scanline */
     p = (char far*)TANDY;
    
    /* start bit (b) and mask (m) for 4-bit pixels */
    if (x & 0x1) {
        b = 0;
        m = 0x0F;
    } else {
        b = 4;
        m = 0xF0;
    }

    p += pixelOffset(x, y);
    c = *p;
    c = c & ~m;
    c = c | (color << b);
    *p = c;
}

static void drawChar(char ch, unsigned short x, unsigned char y, unsigned char color_index)
{
	unsigned char i, j;
	unsigned char *charSprite;
	unsigned short plot_y;
	
	unsigned y_div_4;
	unsigned y_mod_4;
	
	charSprite = (ch << 3) + std_font; // Updated line
	// Draw charSprite as monochrome 8*8 image using given color
	i = 8;
	while (i--)
	{
		j = 8;
		plot_y = y + i;
		while (j--)
		{
			if((charSprite[i] >> j) & 1)
			{
				setPixel(x + (7 - j), plot_y, color_index);
			}
		}
	}
	x += 8;
}
#endif

#define CRTC_INDEX_PORT 0x3D4
#define CRTC_DATA_PORT 0x3D5

#define SET_BASE_VIDEO_ADDRESS_PCJR(base_address) \
    outp(CRTC_INDEX_PORT, (12)); \
    outp(CRTC_DATA_PORT, ((base_address) >> 8) & 0xFF); \
	outp(CRTC_INDEX_PORT, (13)); \
	outp(CRTC_DATA_PORT, (base_address) & 0xFF); \

#define PCJR_MODEL_ID_ADDR 0xFFFEL
#define PCJR_MODEL_ID_SEG  0xF000L
#define PCJR_MODEL_ID_BYTE 0xFD
#define peekb(segm,ofs) (*(uint8_t far*)MK_FP((segm),(ofs)))

static unsigned char Init_TANDY(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	union REGS regs;
	
	screen_width = width;
	screen_height = height;
	
	switch(width)
	{
		case 320:
			set_mode(0x09);
			PLANE_SIZE = 16384*2;
		break;
		// TANDY low-res 160x200 mode
		case 160:
			set_mode(0x08);
			PLANE_SIZE = 16384;
		break;
		// Need to support Tandy Hires 16 colors 640x200 mode
		/*case 640:
			set_mode(0x0A);
			PLANE_SIZE = 16384*2;
		break;*/
	}
	
	// Set it to PCJr compatible if PCJr is detected
	if (peekb(PCJR_MODEL_ID_SEG, PCJR_MODEL_ID_ADDR) == 0xFD)
	{
		SET_BASE_VIDEO_ADDRESS_PCJR(0x18000000L);
		TANDY = (uint8_t far*)0x18000000L;
	}

	memset(TANDY, 0, PLANE_SIZE);
	
	return 0;
}


static void TANDY_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char dontdecompress)
{
    Generic_Load_SIF(file, b, s_width, s_height, dontdecompress);
}


static void TANDY_DrawBMP_static(BITMAP *bmp, short x, short y, unsigned long offset)
{
	/*unsigned off;
	off = pixelOffset(x, y);
	memcpy(TANDY + off, bmp->data[0] + offset, PLANE_SIZE);
	*/

    const void *src = bmp->data[0] + offset;
    size_t count = PLANE_SIZE;
    __asm {
        les di, TANDY
        lds si, src

        mov ax, count
        shr ax, 1
        mov cx, ax
        rep movsw
    }
}

static void TANDY_DrawBMP_compressed(BITMAP *bmp, short x, short y)
{
	unsigned off;
	off = pixelOffset(x, y);
	CAL_LZSAExpand(bmp->data[0], TANDY + off);
}

static void TANDY_Free_bmp(BITMAP *bmp)
{
	if (bmp->data[0])
	{
		free(bmp->data[0]);
		bmp->data[0] = NULL;
	}
}

static void TANDY_stub()
{
}

static void TANDY_Clear()
{
	memset(TANDY, 0, PLANE_SIZE);
}

// TANDY palette is stuck, cannot be reset (except perhaps on some TANDY clone cards)
static void TANDY_LoadPalette(const char *f)
{
}


static void TANDY_Draw_Text(const char* str, unsigned short x, unsigned short y, unsigned short color_index)
{
#if 0
	unsigned char i;
	for(i = 0; str[i]; i++)
		drawChar(str[i], x + (i << 3), y, color_index);
#else
	x = x >> 3;
	y = y >> 3;
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
#endif
}

static void TANDY_Draw_Text_Center(const char* str, unsigned short y, unsigned short color_index)
{
	uint16_t x;
	uint16_t text_width;
	text_width = strlen(str) << 3;
	x = ((screen_width - text_width) >> 1) >> 3;
	y = y >> 3;
	
	VideoInt( _BIOS_SET_CURPOS, 0, 0, (x&0xFF)|((y&0xFF)<<8) );
	puts(str);
}

#define INPUT_STATUS 0x03DA
#define VRETRACE_BIT 3

static void TANDY_Vsync()
{
    unsigned char status;

    // Wait until vertical retrace is finished (bit 3 == 0).
    do {
        status = inp(INPUT_STATUS);
    } while (status & (1 << VRETRACE_BIT));
    
    // Wait until vertical retrace is active (bit 3 == 1).
    do {
        status = inp(INPUT_STATUS);
    } while (!(status & (1 << VRETRACE_BIT)));
}

static void Kill_TANDY()
{
	TANDY_Clear();
	outp(0x3D8, 0x1E); // Disable colorburst
	set_mode(TEXT_MODE);
}



VideoDevice Normal_Tandy = {
	"TANDY video",
	Init_TANDY,
	TANDY_DrawBMP_static,
	TANDY_DrawBMP_compressed,
	TANDY_stub, //Draw Sprite trans
	TANDY_stub, //Draw sprite non trans
	TANDY_Load_SIF,
	TANDY_LoadPalette,
	TANDY_stub, // Set palette
	TANDY_Free_bmp,
	TANDY_Clear,
	TANDY_stub,
	TANDY_Vsync,
	Kill_TANDY,
	TANDY_Draw_Text, // Draw Text
	TANDY_Draw_Text_Center
};

