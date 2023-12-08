#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#ifdef DJGPP
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#endif
 
#include "graph.h"
#include "pc98.h"
#include "gdc.h"

static uint8_t palette_pc9821[768];
static uint16_t screen_width, screen_height;

#ifdef DJGPP
// Doing so greatly reduces size
#include <crt0.h>
char **__crt0_glob_function (char *arg) { return 0; }
void   __crt0_load_environment_file (char *progname) { }
void   __crt0_setup_arguments (void) { }
#endif

static int pegc_fb_location = PEGC_FB_LOCATION_LOW;
#ifdef DJGPP
static __dpmi_meminfo	vram_dpmi;			// DPMI descriptor for far-memory location of framebuffer
#endif
static int vram_dpmi_selector;	// Memory region selector handle

extern uint32_t apl_decompress(const void *Source, void *Destination);

#ifdef DJGPP
#define OUTPORT_DOS outportb
#define INPORT_DOS inportb
#else
#define OUTPORT_DOS _outp
#define INPORT_DOS _inp
#endif

#define ENABLE_NEAR() __djgpp_nearptr_enable();
#define DISABLE_NEAR() __djgpp_nearptr_disable();

/* These are used for hardware double buffering using the second plane */
#define PAGE_OFFSET_1 0
#define PAGE_OFFSET_2 PEGC_FB_SIZE/2
#define PAGE_TODRAW PAGE_OFFSET_2
#define PAGE_BUFFER_ID_0 0
#define PAGE_BUFFER_ID_1 1
const unsigned page_to_draw_to[2] = {PAGE_OFFSET_1, PAGE_OFFSET_2};
static uint8_t front_buffer_page = PAGE_BUFFER_ID_0;
static uint8_t back_buffer_page = PAGE_BUFFER_ID_1;

#ifdef EGC

enum grcg_mode_t 
{
	GC_OFF = 0x00,
	GC_TDW = 0x80,
	GC_TCR = 0x80,
	GC_RMW = 0xC0,
};

enum egc_register_t {
	EGC_ACTIVEPLANEREG = 0x04A0,
	EGC_READPLANEREG   = 0x04A2,
	EGC_MODE_ROP_REG   = 0x04A4,
	EGC_FGCOLORREG     = 0x04A6,
	EGC_MASKREG        = 0x04A8,
	EGC_BGCOLORREG     = 0x04AA,
	EGC_ADDRRESSREG    = 0x04AC,
	EGC_BITLENGTHREG   = 0x04AE,
};

// EGC
typedef uint16_t uvram_offset_t;
typedef unsigned int uvram_word_amount_t;
typedef int pixel_t;
typedef unsigned int upixel_t;
typedef uint16_t vram_byte_amount_t;
typedef int screen_x_t;
typedef unsigned int uscreen_x_t;
typedef int vram_y_t;
typedef unsigned int uvram_y_t;
typedef int16_t vram_offset_t;
typedef bool page_t;
#define SEG_PLANE_B 0xA800
#define BYTE_DOTS 8
#define BYTE_MASK (BYTE_DOTS - 1)
#define BYTE_BITS 3
#define RES_X 640
#define RES_Y 400
#define ROW_SIZE (RES_X / BYTE_DOTS)
#define PLANE_SIZE (ROW_SIZE * RES_Y)
#define PLANE_COUNT 4
#define page_access(page)	outportb(0xA6, page)
#define VRAM_OFFSET_SHIFT(x, y) \
	((x >> BYTE_BITS) + ((y << 6) + (y << 4)))


static const int EGC_REGISTER_DOTS = 16;
static const int EGC_REGISTER_MASK = (EGC_REGISTER_DOTS - 1);
static const int EGC_REGISTER_SIZE = (EGC_REGISTER_DOTS / BYTE_DOTS);
static inline vram_byte_amount_t copy_width(const pixel_t w, const upixel_t dst_first) {
    return (((dst_first + w + EGC_REGISTER_MASK) / EGC_REGISTER_DOTS) * EGC_REGISTER_SIZE);
}
void EGCCopy_rect_interpage(screen_x_t left, vram_y_t top, pixel_t w, pixel_t h, page_t src_page) {
    uvram_offset_t vo = (VRAM_OFFSET_SHIFT(left, top) & ~1);
    const upixel_t first = (left & EGC_REGISTER_MASK);
    const vram_byte_amount_t vram_w = copy_width(w, first);
    const bool dst_page = (src_page ^ 1);
    
    
    outportl(EGC_ADDRRESSREG, (first | (first << 4)));
    outportl(EGC_BITLENGTHREG, (w - 1));
    uint16_t ES = SEG_PLANE_B;
    uint16_t AX, BX;

    for (pixel_t y = 0; y < h; y++) {
        for (BX = 0; BX < vram_w; BX += EGC_REGISTER_SIZE) {
            page_access(src_page);
            AX = *((uint16_t *)(ES + vo + BX));
            page_access(dst_page);
            *((uint16_t *)(ES + vo + BX)) = AX;
        }
        vo += ROW_SIZE;
    }
}

void EGCCopy_rect(screen_x_t src_left, uvram_y_t src_top, screen_x_t dst_left, uvram_y_t dst_top, pixel_t w, pixel_t h) 
{
    /*if (w <= 0) {
        return;
    }*/
    uint16_t SI = (VRAM_OFFSET_SHIFT(src_left, src_top) & ~1);
    uint16_t DI = (VRAM_OFFSET_SHIFT(dst_left, dst_top) & ~1);
    const upixel_t src_first = (src_left & EGC_REGISTER_MASK);
    const upixel_t dst_first = (dst_left & EGC_REGISTER_MASK);
    const vram_byte_amount_t vram_w = copy_width(w, dst_first);
    outportl(EGC_ADDRRESSREG, (src_first | (dst_first << 4)));
    outportl(EGC_BITLENGTHREG, (w - 1));
    uint16_t ES = SEG_PLANE_B;
    uint16_t AX, BX;

    if (src_first > dst_first) {
        for (pixel_t y = 0; y < h; y++) {
            AX = *((uint16_t *)(ES + SI));
            SI += EGC_REGISTER_SIZE;

            for (BX = 0; BX < vram_w; BX += EGC_REGISTER_SIZE) {
                AX = *((uint16_t *)(ES + SI + BX));
                *((uint16_t *)(ES + DI + BX)) = AX;
            }
            SI += (ROW_SIZE - EGC_REGISTER_SIZE);
            DI += ROW_SIZE;
        }
    } else {
        for (pixel_t y = 0; y < h; y++) {
            for (BX = 0; BX < vram_w; BX += EGC_REGISTER_SIZE) {
                AX = *((uint16_t *)(ES + SI + BX));
                *((uint16_t *)(ES + DI + BX)) = AX;
            }
            SI += ROW_SIZE;
            DI += ROW_SIZE;
        }
    }
}
#endif


static inline char *removestr(char* myStr)
{
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 1)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

static inline int_fast8_t gfx_DPMI()
{
	// Enable DPMI mapper to framebuffer far location
	if (INPORT_DOS(MEMORY_HOLE_CHECK_ADDR) & 0x04){
		/* Memory hole not available, relocate VRAM address */
		pegc_fb_location = PEGC_FB_LOCATION_HIGH;
	} else {
		/* Memory hole available */
		pegc_fb_location = PEGC_FB_LOCATION_LOW;
	}
	
	#ifdef DJGPP
	if (!__djgpp_nearptr_enable()){
		return -1;
	}
	
	vram_dpmi.address = pegc_fb_location;
	vram_dpmi.size    = PEGC_FB_SIZE;
	
	if (__dpmi_physical_address_mapping(&vram_dpmi) != 0){
		__djgpp_nearptr_disable();
		return -1;
	}
	vram_dpmi_selector = __dpmi_allocate_ldt_descriptors(1);
	if (vram_dpmi_selector < 0){
		__dpmi_free_physical_address_mapping(&vram_dpmi);
		__djgpp_nearptr_disable();
		return -1;
	}
	__dpmi_set_segment_base_address(vram_dpmi_selector, vram_dpmi.address);
	__dpmi_set_segment_limit(vram_dpmi_selector, vram_dpmi.size - 1);
	
	__djgpp_nearptr_disable();
	#endif
	
	return 0;
}

#if 0
void gfx_TextOn(){
	// Text mode on
	OUTPORT_DOS(0x62, GDC_COMMAND_START);
}

void gfx_TextOff(){
	// Text mode off
	OUTPORT_DOS(0x62, GDC_COMMAND_STOP1);
}
#endif


static void Print_text(const char *str)
{
	union REGS regs;
	regs.h.cl = 0x10;
	regs.h.ah = 0;
	while(*str) {
		regs.h.dl = *(str++);
		int86(0xDC, &regs, &regs);
	}
	outp(0x64, 0); // VSync interrupt trigger	
}


#define ClearTextVRAM()  Print_text("\x1B[2J");

static int PC9821_SetVideo(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	// Initialise graphics to a set of configured defaults
	// Text mode off
	//OUTPORT_DOS(0x62, GDC_COMMAND_STOP1);
	
	// Clear text layer
	ClearTextVRAM();
	
	// Print command to disable text cursor
	Print_text("\x1B[>5h");
	
	// Print command to disable system line
	Print_text("\x1B[>1h");

	screen_width = width;
	screen_height = height;
		
	// HSYNC 24 KHz (640x400)
	OUTPORT_DOS(PEGC_SCANFREQ_ADDR, PEGC_SCANFREQ_24);

	// 256 color mode
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x07);
	OUTPORT_DOS(PEGC_MODE_ADDR, PEGC_BPPMODE_256c);
	OUTPORT_DOS(PEGC_MODE_ADDR, PEGC_BPPMODE_DUAL_PAGE);
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x06);
	
	// Enable Packed Pixel mode
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_PIXELMODE_ADDR, PEGC_PIXELMODE_PACKED);
	#endif

	// Set up DPMI mapper for vram framebuffer regionbui
	if (gfx_DPMI() < 0){
		return 1;	
	}
	
	// Enable linear framebuffer at 16MB and 4095MB
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_ON);
	#endif

	// Set screen 0 to be active for drawing and active for display
	OUTPORT_DOS(PEGC_DRAW_SCREEN_SEL_ADDR, 0x00);
	OUTPORT_DOS(PEGC_DISP_SCREEN_SEL_ADDR, 0x00);
	
	// Graphics mode on
	OUTPORT_DOS(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_START);
	
#ifdef EGC
	// EGC
	OUTPORT_DOS(0x7C, GC_TDW);

	OUTPORT_DOS(0x6A, 6 + 1);

	// Enable EGC
	outportw(EGC_ACTIVEPLANEREG, 0xFFF0);
	outportw(EGC_READPLANEREG, 0x00FF);
	outportw(EGC_MASKREG, 0xFFFF);
	outportw(EGC_ADDRRESSREG, 0);
	outportw(EGC_MODE_ROP_REG, 0x29F0);
#endif

	// Set local vram_buffer to empty 
	//memset(vram_buffer, 0, (GFX_ROW_SIZE * GFX_COL_SIZE));
	//gfx_Clear();
	//gfx_Flip();
	
	return 0;
}


static inline void PC9821_Draw_static_bitmap_normal(BITMAP *bmp, short x, short y, unsigned long offset)
{
	movedata(_my_ds(), (unsigned)bmp->data + offset, vram_dpmi_selector, page_to_draw_to[back_buffer_page], (bmp->sprite_width*bmp->sprite_height));
}

static void PC9821_Draw_sprite_normal_trans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	//uint16_t screen_offset = (y<<8)+(y<<6) + x;	
	uint16_t screen_offset = x + (y * screen_width);
	uint16_t bitmap_offset = 0;
	unsigned long sprite_offset = frame*(bmp->sprite_width*bmp->sprite_height);

	for(unsigned short j=0;j<bmp->height;j++)
	{
		for(unsigned short i=0;i<bmp->width;i++,bitmap_offset++)
		{
			uint8_t data = bmp->data[bitmap_offset+sprite_offset];
			if (data)
			{
				_farpokeb(_dos_ds, PEGC_PIXELMODE_ADDR + page_to_draw_to[back_buffer_page] + screen_offset+x+i, PEGC_PIXELMODE_PACKED);
				//doublebuffer[screen_offset+x+i] = data;
			}
			
		}
		screen_offset+=screen_width;
	}
}

static void PC9821_Draw_sprite_normal_notrans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	//uint16_t screen_offset = (y<<8)+(y<<6) + x;	
	uint16_t screen_offset = x + (y * screen_width);
	uint16_t bitmap_offset = 0;
	unsigned long sprite_offset = frame*(bmp->sprite_width*bmp->sprite_height);

	for(unsigned short j=0;j<bmp->height;j++)
	{
		movedata(_my_ds(), (unsigned)bmp->data[bitmap_offset + sprite_offset], vram_dpmi_selector, page_to_draw_to[back_buffer_page] + screen_offset, bmp->width * sizeof(unsigned char));
        //memcpy(&doublebuffer[screen_offset], &bmp->data[bitmap_offset + sprite_offset], bmp->width * sizeof(unsigned char));
        bitmap_offset += bmp->width;
        screen_offset += screen_width;
	}
}


static void PC9821_Set_palette()
{
	unsigned char i;
	i = 255;
	while(i--)
	{
		OUTPORT_DOS(PEGC_PALLETE_SEL_ADDR, i);
		OUTPORT_DOS(PEGC_RED_ADDR, palette_pc9821[(i*3)+0]);
		OUTPORT_DOS(PEGC_GREEN_ADDR, palette_pc9821[(i*3)+1]);
		OUTPORT_DOS(PEGC_BLUE_ADDR, palette_pc9821[(i*3)+2]);
	}
}

static void PC9821_Load_palette(const char *file)
{
	int fd;
	unsigned totalsize;
	_dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, (char*)&palette_pc9821, sizeof(palette_pc9821), &totalsize);
	_dos_close(fd);
}


#pragma pack(push, 1)
typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t sprite_width;
    uint16_t sprite_height;
    uint8_t bytespp;
    uint8_t encoding;
    uint8_t endianess;
    uint32_t filesize;
} BMPHeader;
#pragma pack(pop)

static unsigned char PC9821_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char load_pal)
{
    int fd;
    char str[16];
    unsigned char* temp_mem;
    unsigned totalsize;
	uint32_t filesize = 0;
	BMPHeader header;

    if (_dos_open(file, O_RDONLY, &fd) == 1)
    {
		return 0;
	}
	_dos_read(fd, &header, sizeof(BMPHeader), &totalsize);

	b->width = header.width;
	b->height = header.height;
	b->sprite_width = s_width;
	b->sprite_height = s_height;
	//b->bytespp = header.bytespp;
	//b->encoding = header.encoding;
	filesize = header.filesize;

	// No need to do that
   	//lseek (fd, sizeof(BMPHeader), SEEK_SET);
   	
	if (b->data)
	{
		free(b->data);
		b->data = NULL;
	}
   	
    switch(header.encoding)
    {
        default:
            b->data = malloc(b->width * b->height);
            _dos_read(fd, b->data, (b->width * b->height), &totalsize);
        break;
        case APLIB_COMPRESSION_ID:
            temp_mem = malloc(filesize);
            b->data = malloc(b->width * b->height);
            
            _dos_read(fd, temp_mem, filesize, &totalsize);
            
            apl_decompress(temp_mem,b->data);
            free(temp_mem);
        break;
    }
    
    _dos_close(fd);
    
    if (s_width == 0) b->sprite_width = b->width;
    if (s_height == 0) b->sprite_height = b->height;

    if (load_pal == 1)
    {
        strcpy(str, removestr(basename(file)));
        strcat(str, ".PAL");
        PC9821_Load_palette(str);
    }
    
    return 1;
}

static void PC9821_Wait_for_retrace(void) 
{
	/* Wait until vsync occurs. */
	while (!(inp(GDC_GFX_STATUS) & GDC_STATUS_VSYNC)) {}
	
	/* Now wait until it's over. */
	while (inp(GDC_GFX_STATUS)   & GDC_STATUS_VSYNC) {}
}

static void PC9821_Free_bmp(BITMAP *bmp)
{
	if (bmp->data)
	{
		free(bmp->data);
		bmp->data = NULL;
	}
}

static void PC9821_Clear_Screen()
{
	ClearTextVRAM();
}

static void PC9821_Flip()
{
    uint8_t tmp = front_buffer_page;
    front_buffer_page = back_buffer_page;
    back_buffer_page = tmp;
	outportb(0xA4, front_buffer_page);
}


static void PC9821_Kill()
{
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_OFF);
	#endif

	// 16 Color mode
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x07);
	OUTPORT_DOS(PEGC_MODE_ADDR, PEGC_BPPMODE_16c);
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x06);

	// Graphics mode off
	OUTPORT_DOS(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_STOP1);
	
	#ifdef DJGPP
	// Free DPMI mapping
	__dpmi_free_physical_address_mapping(&vram_dpmi);
	#endif
	
	//  Clear anything we did to the screen
	PC9821_Clear_Screen();
	
	// Text mode on
	OUTPORT_DOS(0x62, GDC_COMMAND_START);

	#ifdef DJGPP
	__djgpp_nearptr_disable();
	#endif
	
	// Print command to enable text cursor
	Print_text("\x1B[>5l");
	// Print command to enable system line
	Print_text("\x1B[>1l");
	
#ifdef EGC
	OUTPORT_DOS(0x6A, 6 + 0); // Disable EGC
	OUTPORT_DOS(0x7C, GC_OFF); // GDC off
#endif
}

#if 0
static void Cursor_SetXY(unsigned char x, unsigned char y) 
{
	union REGS regs;
	regs.h.cl = 0x10;
	regs.h.ah = 3;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0xDC, &regs, &regs);
}
#endif

static void PC9821_Print_Text(const char* text, short x, short y)
{
	// Set Cursor
	union REGS regs;
	regs.h.cl = 0x10;
	regs.h.ah = 3;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0xDC, &regs, &regs);
	
	Print_text(text);
}


VideoDevice Normal_PC9821_PGC = {
	"PC9821 PGC video",
	PC9821_SetVideo,
	PC9821_Draw_static_bitmap_normal,
	PC9821_Draw_sprite_normal_trans,
	PC9821_Draw_sprite_normal_notrans,
	PC9821_Load_SIF,
	PC9821_Load_palette,
	PC9821_Set_palette,
	PC9821_Free_bmp,
	PC9821_Clear_Screen,
	PC9821_Flip,
	PC9821_Wait_for_retrace,
	PC9821_Kill,
	PC9821_Print_Text
};

//#endif
