#ifndef GFX_H
#define GFX_H


typedef enum gfx_device
{
	GFX_DEVICE_GDC_TEXT = 0x1,
	GFX_DEVICE_GDC_GFX  = 0x2,
	GFX_DEVICE_GRCG     = 0x4,
	GFX_DEVICE_EGC      = 0x8
} gfx_device_t;

typedef enum gfx_mode
{
	GFX_MODE_NONE,
	GFX_MODE_TEXT,
	GFX_MODE_640x200x8,
	GFX_MODE_640x200x16,
	GFX_MODE_640x400x8,
	GFX_MODE_640x400x16,
	GFX_MODE_640x400x256
} gfx_mode_t;

typedef enum gfx_plane
{
	GFX_PLANE_DISPLAY   = 0,
	GFX_PLANE_RENDERING = 2
} gfx_plane_t;

typedef enum gfx_draw_mode
{
	GFX_DRAW_MODE_SET,
	GFX_DRAW_MODE_AND,
	GFX_DRAW_MODE_OR,
	GFX_DRAW_MODE_XOR
} gfx_draw_mode_t;

#define CLEAR_TEXT_VRAM() memset(((unsigned char far*)0xA0000000L), 0, 16384);

#define CLEAR_TXT() Print_text("\x1B[2J");
#define HIDE_CURSOR() Print_text("\x1B[>1h");
#define TEXT_20LINE() Print_text("\x1B[>3h");

extern void gfx_init();
extern void gfx_end();
extern void gfx_wait_vsync();
extern void gdc_wait_until_ready(unsigned short mode);
extern void gfx_set_plane(gfx_plane_t plane, unsigned char which);
extern unsigned short gfx_get_plane(gfx_plane_t plane);
extern void gfx_fill_palette(const unsigned char* palette, size_t len);

extern void Cursor_SetXY(unsigned char x, unsigned char y);
extern void Print_text(const char *str);

#endif
