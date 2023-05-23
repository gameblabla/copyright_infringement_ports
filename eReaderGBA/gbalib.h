#ifndef GBALIB_H
#define GBALIB_H



#define	SRAM		0x0E000000
#define SRAM_LOC ((uint8_t*)(0x0E000000))

#define	REG_BASE	0x04000000

#define FRAME_SEL_BIT		0x10
#define BG0_ENABLE 0x100
#define BG1_ENABLE 0x200
#define BG2_ENABLE 0x400
#define BG3_ENABLE 0x800

#define BG_PALETTE_MEM ((volatile unsigned short*) 0x05000000)

#define DCNT_MODE0   0x0000
#define DCNT_MODE1   0x0001
#define DCNT_MODE2   0x0002
#define DCNT_MODE3   0x0003
#define DCNT_MODE4   0x0004
#define DCNT_MODE5   0x0004
#define DCNT_WIN1   0x4000
#define DCNT_WINOBJ   0x8000

#define BG_REG_32x32   0
#define BG_4BPP   0
#define BG_8BPP   0x0080

#define MAPPINGMODE_1D 0x0040
#define M4_WIDTH    240     // Width in mode 4
#define	REG_DISPCNT		*((vu16 *)(REG_BASE + 0x00))

#define BG_PRIO_MASK   0x0003
#define BG_PRIO_SHIFT   0
#define BG_PRIO(n)   ((n)<<BG_PRIO_SHIFT)
#define BG_CBB_MASK   0x000C
#define BG_CBB_SHIFT   2
#define BG_CBB(n)   ((n)<<BG_CBB_SHIFT)
#define BG_SBB_MASK   0x1F00
#define BG_SBB_SHIFT   8
#define BG_SBB(n)   ((n)<<BG_SBB_SHIFT)
#define BG_SIZE_MASK   0xC000
#define BG_SIZE_SHIFT   14
#define BG_SIZE(n)   ((n)<<BG_SIZE_SHIFT)
#define BG_BUILD(cbb, sbb, size, bpp, prio, mos, wrap)

#if	defined	( __thumb__ )
#define	SystemCall(Number)	 __asm ("SWI	  "#Number"\n" :::  "r0", "r1", "r2", "r3")
#else
#define	SystemCall(Number)	 __asm ("SWI	  "#Number"	<< 16\n" :::"r0", "r1", "r2", "r3")
#endif

#define _VRAM (uint8_t *)0x6000000

extern uint16_t *fb;
extern uint16_t *pal_register;

extern void Draw_Text(const char* str, uint8_t x, uint8_t y, uint8_t color_index, uint8_t bg);
extern void Draw_Text_Center(const char* str, uint8_t y, uint8_t color_index, uint8_t bg);

extern void Init_GBA(uint8_t m);

#define REG_VCOUNT *(volatile uint16_t*)0x04000006
static inline void vid_vsync()
{
    while(REG_VCOUNT >= 160);   // wait till VDraw
    while(REG_VCOUNT < 160);    // wait till VBlank
}

#define 	REG_DMA3SAD   *(volatile uint32_t*)(REG_BASE+0x00D4)
#define 	REG_DMA3DAD   *(volatile uint32_t*)(REG_BASE+0x00D8)
#define 	REG_DMA3CNT   *(volatile uint32_t*)(REG_BASE+0x00DC)
#define 	DMA_16   0
#define 	DMA_SRC_FIXED   0x01000000
#define 	DMA_ENABLE   0x80000000
#define 	DMA_32   0x04000000
#define 	DMA_AT_NOW   0
#define 	DMA_NOW   (DMA_ENABLE | DMA_AT_NOW)
#define 	DMA_16NOW   (DMA_NOW | DMA_16)
#define 	DMA_32NOW   (DMA_NOW | DMA_32)

typedef struct DMA_REC
{
    const void *src;
    void *dst;
    uint32_t cnt;
} DMA_REC;

#define REG_DMA ((volatile DMA_REC*)0x040000B0)


static inline void DMAFastCopy(void* source, void* dest, unsigned int count, uint32_t mode)
{
	REG_DMA3CNT = 0;
	REG_DMA3SAD = (unsigned int) source;
	REG_DMA3DAD = (unsigned int) dest;
	REG_DMA3CNT = count | mode;
}

//! General DMA full routine
static inline void DMAFastFill( volatile uint32_t src, void *dst, uint32_t count, uint32_t mode)
{
    REG_DMA3CNT = 0; // shut off any previous transfer
	REG_DMA3SAD = (uint32_t)((const void *)(&src));
	REG_DMA3DAD = (uint32_t)(dst);
    REG_DMA3CNT = count | DMA_SRC_FIXED | DMA_32 | DMA_ENABLE;
}


#define CharBaseBlock(n) (((n)*0x800) + 0x6000000)
#define ScreenBaseBlock(n) (((n)*0x800) + 0x6000000)

#define BG_COLOR16 0x00
#define BG_COLOR256 0x80
#define TEXTBG_SIZE_256x256		BG_REG_32x32
#define SCREEN_SHIFT 8
#define WRAPAROUND 0x1
#define MEM_BG_PALETTE ((uint16_t*)(0x05000000))

#define 	REG_KEYINPUT   (*(volatile uint16_t*)0x04000130)
// Definitions for the button masks in the REG_KEYINPUT register
#define BUTTON_A 1<<0
#define BUTTON_B 1<<1
#define BUTTON_SELECT 1<<2
#define BUTTON_START 1<<3
#define BUTTON_RIGHT 1<<4
#define BUTTON_LEFT 1<<5
#define BUTTON_UP 1<<6
#define BUTTON_DOWN 1<<7
#define BUTTON_R 1<<8
#define BUTTON_L 1<<9


static inline uint8_t isButtonAPressed() 
{
    // REG_KEYINPUT is inverted, so "0" means a button is pressed
    return !(REG_KEYINPUT & BUTTON_A);
}

extern uint16_t prevButtons;
extern uint16_t currButtons;

static inline void updateButtons() {
    prevButtons = currButtons;
    currButtons = REG_KEYINPUT;
}

static inline uint8_t wasButtonAPressed() {
    // Returns true if A was not pressed in the previous state
    // and is pressed in the current state
    return (!(prevButtons & BUTTON_A) && (currButtons & BUTTON_A));
}

#endif
