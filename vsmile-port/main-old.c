#include "Resource.h"

#define PPU_BG1_SCROLL_X ((volatile unsigned*)0x2810)
#define PPU_BG1_SCROLL_Y ((volatile unsigned*)0x2811)
#define PPU_BG1_ATTR ((volatile unsigned*)0x2812)
#define PPU_BG1_CTRL ((volatile unsigned*)0x2813)
#define PPU_BG1_TILE_ADDR ((volatile unsigned*)0x2814)
#define PPU_BG1_ATTR_ADDR ((volatile unsigned*)0x2815)
#define PPU_BG1_SEGMENT_ADDR ((volatile unsigned*)0x2820)

#define PPU_BG2_SCROLL_X ((volatile unsigned*)0x2816)
#define PPU_BG2_SCROLL_Y ((volatile unsigned*)0x2817)
#define PPU_BG2_ATTR ((volatile unsigned*)0x2818)
#define PPU_BG2_CTRL ((volatile unsigned*)0x2819)

#define PPU_BG2_TILE_ADDR ((volatile unsigned*)0x281A)
#define PPU_BG2_ATTR_ADDR ((volatile unsigned*)0x281B)

#define PPU_BG2_SEGMENT_ADDR ((volatile unsigned*)0x2821)

#define PPU_FADE_CTRL ((volatile unsigned*)0x2830)
#define PPU_SPRITE_CTRL ((volatile unsigned*)0x2842)

#define PPU_COLOR ((volatile unsigned*)0x2B00)

#define PPU_INT_CTRL ((volatile unsigned*)0x2862)
#define PPU_INT_CLEAR ((volatile unsigned*)0x2863)

#define SYSTEM_CTRL ((volatile unsigned*)0x3D20)
#define INT_CTRL ((volatile unsigned*)0x3D21)
#define INT_CLEAR ((volatile unsigned*)0x3D22)
#define WATCHDOG_CLEAR ((volatile unsigned*)0x3D24)

#define IO_MODE ((volatile unsigned*)0x3D00)
#define PORTC_DATA ((volatile unsigned*)0x3D0B)
#define PORTC_DIR ((volatile unsigned*)0x3D0D)
#define PORTC_ATTR ((volatile unsigned*)0x3D0E)
#define PORTC_SPECIAL ((volatile unsigned*)0x3D0F)

#define UART_CONTROL ((volatile unsigned*)0x3D30)
#define UART_STATUS ((volatile unsigned*)0x3D31)
#define UART_BAUDRATE_LOW ((volatile unsigned*)0x3D33)
#define UART_BAUDRATE_HIGH ((volatile unsigned*)0x3D34)
#define UART_TXBUF ((volatile unsigned*)0x3D35)
#define UART_RXBUF ((volatile unsigned*)0x3D36)

static int irq3_rx_count = 0;
static int irq3_tx_count = 0;
static int irq5_lo_count = 0;
static int irq5_hi_count = 0;

static int colors_val = 0;
static int buttons_val = 0;
static int joyx_val = 0;
static int joyy_val = 0;
static int last_rx = 0;
static int idle_counter = 0;

static int bitmap_tilemap[240];
static int attr_map[240/2];

static const char *hextable = "0123456789abcdef";

static int text_tilemap[2048];

static int controller_reset_counter = 0;

static int frame_picture = 0, display_frame = 0, delay = 0;
static int wait_input = 0;
static int status = 0, text_progress = 0;

#define make_color(r, g, b) ((r << 10) | (g << 5) | (b << 0))

int time_game = 0;
int global_score = 0;
int game_mode = 0;

const unsigned char status_level1[] =
{ 2, 3, 0, 2, 3, 0, 2, 3, 0, 4 };

const unsigned char text_minigame_1[12][40] = 
{
	"Your goal is to FUCK Zhongli.",
	"",
	"You must follow on the on-screen",
	"instructions to get more points.",
	"",
	"Penetrate him by holding OK.",
	"Pull out by releasing OK",
	"Good luck !",
	"",
	"",
	"(Press OK to play)",
};

const unsigned char gameover_mj1[10][34] = 
{
	"Zhongli dumped you because you",
	"don't know how to fuck a MAN.",
	"All men learned of this and they",
	"are now mocking you for this.",
	"Girls went extinct because",
	"ISIS killed them all.",
	"You're now fucking worthless",
	"",
	"",
	"GAME OVER"
};

const unsigned char gameover_mj1_alternate[5][42] = 
{
	"Zhongli seemed to be only midly satisfed.",
	"He then promptly dumped you.",
	"You are now really sad.",
	"You will never see his beautiful nose.",
	"GAME OVER"
};


void print_dec(int y, int x, unsigned int value)
{
	int digits[5];
	int i, j, len = 0;

	do {
		digits[len++] = value % 10;
	} while(value /= 10);

	for (i = 4; i >= len; i--) {
		text_tilemap[64*y + x + i] = ' ';
	}
	for (i = len - 1, j = 0; i >= 0; i--, j++) {
		text_tilemap[64*y + x + i] = hextable[digits[j]];
	}
}

void print_hex(int y, int x, int value)
{
	int i;
	for(i = 3; i >= 0; i--, value >>= 4){
		text_tilemap[64*y + x + i] = hextable[value & 0x0f];
	}
}

void print_hex2(int y, int x, int value)
{
	int i;
	for(i = 1; i >= 0; i--, value >>= 4){
		text_tilemap[64*y + x + i] = hextable[value & 0x0f];
	}
}

void print_string(int y, int x, const char *str) {
	int i = 0;
	while(str[i]){
		text_tilemap[64*y + (x++)] = str[i++];
	}
}

void clear_text_tilemap(int tile[], int siz){
	int i;
	// 0x00 is transparent
	// 0x20 is black
	for (i = 0; i < siz; i++)
	{
		tile[i] = 0x0;
	}
}

void clear_bitmap(){
	int i;
	for (i = 0; i < sizeof(bitmap_tilemap); i++)
	{
		bitmap_tilemap[i] = 0x0;
	}
	for (i = 0; i < sizeof(attr_map); i++)
	{
		attr_map[i] = 0x0;
	}
}

void IRQ0(void) __attribute__ ((ISR));
void IRQ3(void) __attribute__ ((ISR));
void IRQ5(void) __attribute__ ((ISR));

void IRQ0(void)  {
	*PPU_INT_CLEAR = 1;
	
	/*delay++;
	if (delay > 5)
	{
		frame_picture++;
		if (frame_picture > 7) frame_picture = 0;
		
		delay = 0;
	}*/
	
	wait_input++;
	
	display_frame = 1;
}

void IRQ3(void)  {
	*INT_CLEAR = 0x0100;
	if(*UART_STATUS & 1){
		int data = *UART_RXBUF;
		*UART_STATUS = 1;
		last_rx = data;
		irq3_rx_count++;
		switch(data & 0xf0){
			case 0x90:
				colors_val = data;
				break;
			case 0xa0:
				buttons_val = data;
				break;
			case 0xc0:
				joyx_val = data;
				break;
			case 0x80:
				joyy_val = data;
				break;
			case 0x50:
				if(data == 0x55) {
					idle_counter++;
					controller_reset_counter++;
				}
				break;
		}

	}
	else if(*UART_STATUS & 2) {
		*UART_STATUS = 2;
		irq3_tx_count++;
		//transmit done, do more here later
	}
}

inline void enable_cts() {
	*PORTC_DATA |= 0x0100;
}

inline void disable_cts() {
	*PORTC_DATA &= ~0x0100;
}

inline int read_rts(){
	return *PORTC_DATA & 0x0400;
}

void IRQ5(void) {
	int data = read_rts();
	*INT_CLEAR = 0x0200;
	if(data) {
		irq5_hi_count++;
		disable_cts();
	}
	else {
		irq5_lo_count++;
		enable_cts();
	}
}

void Draw_Sprite(unsigned long line_addr, unsigned long end_addr, unsigned long height, unsigned long y)
{
	unsigned short i;
    for(i = y; i < y+height; i += 2) {
        bitmap_tilemap[i] = (unsigned char) line_addr ;
        attr_map[i/2] = (line_addr >> 16UL);
        line_addr += 320L;
        bitmap_tilemap[(i+1)] = (unsigned char) line_addr;
        attr_map[i/2] |= (line_addr >> 16UL) << 8;
        line_addr += 320L;
    }
}

void Draw_Picture(unsigned long line_addr)
{
	unsigned short i;
    for(i = 0; i < 240; i += 2) {
        bitmap_tilemap[i] = (unsigned char) line_addr;
        attr_map[i/2] = (line_addr >> 16UL);
        line_addr += 320L;
        bitmap_tilemap[i+1] = (unsigned char) line_addr;
        attr_map[i/2] |= (line_addr >> 16UL) << 8;
        line_addr += 320L;
    }
}

void Draw_Picture_8bpp(unsigned long line_addr)
{
	unsigned short i;
    for(i = 0; i < 240; i += 2) {
        bitmap_tilemap[i] = (unsigned char) line_addr;
        attr_map[i/2] = (line_addr >> 16UL);
        line_addr += 160L;
        bitmap_tilemap[i+1] = (unsigned char) line_addr;
        attr_map[i/2] |= (line_addr >> 16UL) << 8;
        line_addr += 160L;
    }
}


void Init_Controller()
{
	// Disable interrupts
    __asm__("irq off"); 
	
	// Configure port C
	*IO_MODE = 0x09;
	*PORTC_DATA = 0x8180;
	*PORTC_DIR = 0xcbc0;
	*PORTC_ATTR = 0xcbc0;
	*PORTC_SPECIAL = 0x6000;

	// Configure UART
	*UART_BAUDRATE_LOW = 0xA0;
	*UART_BAUDRATE_HIGH = 0xFE;
	*UART_CONTROL = 0xc3;
	*UART_STATUS = 3;
	
	// Enable interrupts
	*INT_CTRL |= 0x0200; // External interrupt enable (IRQ5)
	*INT_CTRL |= 0x0100; // UART interrupt enable (IRQ3)
	__asm__("irq on");

	// Send start command to controller
	enable_cts();
	*UART_TXBUF = 0x71;
	while (*UART_STATUS & 0x40); // Wait synchronously for tx to finish
	disable_cts();	
}

#define HICOLOR_MODE 0x89
#define EIGHT_BPP	 0x09
#define TEXT_BPP	 0x0a

void Init_BG(unsigned short bg_number, unsigned short colormode)
{
	switch(bg_number)
	{
		// Hicolor
		case 0:
		*PPU_BG1_SCROLL_X = 0;
		*PPU_BG1_SCROLL_Y = 0;
		
		if (colormode == EIGHT_BPP)
		{
			*PPU_BG1_ATTR = 0x03;
		}
		else
		{
			*PPU_BG1_ATTR = 0;
		}
		/*
		 * HICOLOR = 0x89
		 * 8BPP = 0x09
		 * TEXT LAYER = 0x0a
		 */
		*PPU_BG1_CTRL = colormode;
		*PPU_BG1_SEGMENT_ADDR = 0;

		
		if (colormode == TEXT_BPP)
		{
			*PPU_BG1_TILE_ADDR = (int) text_tilemap;
			*PPU_BG1_SEGMENT_ADDR = RES_FONT_BIN_SA >> 6;	
			
			PPU_COLOR[0] = make_color(0, 0, 0);
			PPU_COLOR[1] = make_color(31, 31, 31);
		}
		else
		{
			*PPU_BG1_TILE_ADDR = (int) bitmap_tilemap;
			*PPU_BG1_ATTR_ADDR = (int) attr_map;		
		}
		break;
		case 1:
		*PPU_BG2_SCROLL_X = 0;
		*PPU_BG2_SCROLL_Y = 0;
		if (colormode == EIGHT_BPP)
		{
			*PPU_BG2_ATTR = 0x03;
		}
		else
		{
			*PPU_BG2_ATTR = 0;
		}
		/*
		 * HICOLOR = 0x89
		 * 8BPP = 0x09
		 * TEXT LAYER = 0x0a
		 */
		*PPU_BG2_CTRL = colormode;
		*PPU_BG2_SEGMENT_ADDR = 0;
		
		if (colormode == TEXT_BPP)
		{
			*PPU_BG2_TILE_ADDR = (int) text_tilemap;
			*PPU_BG2_SEGMENT_ADDR = RES_FONT_BIN_SA >> 6;	
			
			PPU_COLOR[0] = make_color(0, 0, 0);
			PPU_COLOR[1] = make_color(31, 31, 31);
		}
		else if (colormode != 0)
		{
			*PPU_BG2_TILE_ADDR = (int) bitmap_tilemap;
			*PPU_BG2_ATTR_ADDR = (int) attr_map;		
		}
		break;
	}
	
    *PPU_SPRITE_CTRL = 0;
    *PPU_INT_CTRL = 1;
}


void Upload_Palette(unsigned long line_addr, unsigned long end)
{
    unsigned short i;
    unsigned short* ptr;
    
    for(i = 0; i < (end - line_addr); i += 1) {
		ptr = (unsigned short*)line_addr + i;
		PPU_COLOR[i] = *ptr;
    }
}

void draw_multiline_text(int x, int y, const unsigned char *text)
{
    const int line_height = 1; // height of each line in pixels
    const int max_lines = 20;  // maximum number of lines to draw
    
    int num_lines = 0;         // number of lines drawn so far
    int line_start = 0;        // index of start of current line
    int i = 0;                 // current character index
    
    while (num_lines < max_lines && text[i] != '\0') {
        if (text[i] == '\n') { // end of line
            print_string(y + num_lines * line_height, x,  (const char *)(text + line_start));
            num_lines++;
            line_start = i + 1; // start of next line
        }
        i++;
    }
    // draw last line (if any)
    if (line_start < i) {
        print_string(x, y + num_lines * line_height, (const char *)(text + line_start));
    }
}

void switch_gamemode(unsigned char mode)
{
	int i;
	game_mode = mode;
	status = 0;
	time_game = 0;
	wait_input = 0;
	clear_text_tilemap(text_tilemap, sizeof(text_tilemap));
	
    
	switch(mode)
	{
		/* Titlescreen */
		case 0:
			Init_BG(0, HICOLOR_MODE);
			Init_BG(1, 0);
			Draw_Picture(RES_VSMILE_BIN_SA);
		break;
		
		/* In-game (gameplay) */
		case 1:
			Init_BG(0, HICOLOR_MODE);
			// Needs hardware testing before
			//Init_BG(1, TEXT_BPP);
			Init_BG(1, 0);
			global_score = 0;
			frame_picture = 0;
			status = 0;
		break;
		
		case 2:
			if (global_score < 500)
			{
				Init_BG(0, EIGHT_BPP);
				Init_BG(1, TEXT_BPP);
				Draw_Picture_8bpp(RES_IMAGE_8BIT_BIN_SA);
				Upload_Palette(RES_PALETTE_8BIT_BIN_SA, RES_PALETTE_8BIT_BIN_EA);
				for(i=0;i<10;i++)
				{
					print_string(2 + i, 2, gameover_mj1[i]);
				}
				print_string(13, 2, "Your Score:");
				print_dec(13, 13, global_score);
			}
			else if (global_score < 1000)
			{
				Init_BG(0, EIGHT_BPP);
				Init_BG(1, TEXT_BPP);
				Draw_Picture_8bpp(RES_IMAGE_8BIT_BIN_SA);
				Upload_Palette(RES_PALETTE_8BIT_BIN_SA, RES_PALETTE_8BIT_BIN_EA);
				for(i=0;i<5;i++)
				{
					print_string(2 + i, 2, gameover_mj1_alternate[i]);
				}
				print_string(13, 2, "Your Score:");
				print_dec(13, 13, global_score);
			}
			else
			{
				Init_BG(0, HICOLOR_MODE);
				Init_BG(1, 0);
				Draw_Picture(RES_TEXT6_BIN_SA);
			}

		break;

		/* Introduction */
		case 3:
			Init_BG(0, HICOLOR_MODE);
			Init_BG(1, 0);
			Draw_Picture(RES_TEXT1_BIN_SA);
			text_progress = 0;
			
		break;
		// Instructions
		case 4:
			Init_BG(0, EIGHT_BPP);
			Init_BG(1, TEXT_BPP);
			clear_text_tilemap(text_tilemap, sizeof(text_tilemap));
			clear_text_tilemap(bitmap_tilemap, sizeof(bitmap_tilemap));
		break;
	}
	
	// For text
	PPU_COLOR[0] = make_color(0, 0, 0);
	PPU_COLOR[1] = make_color(31, 31, 31);
}

int Button_Pressed()
{
	if (buttons_val == 0xa1 || colors_val != 0x90)
	{
		return 1;
	}
	return 0;
}


void picture()
{
	switch(frame_picture)
	{
		case 0:
			Draw_Picture(RES_PORN1_BIN_SA);
		break;	
		case 1:
			Draw_Picture(RES_PORN2_BIN_SA);
		break;	
		case 2:
			Draw_Picture(RES_PORN3_BIN_SA);
		break;	
		case 3:
			Draw_Picture(RES_PORN4_BIN_SA);
		break;	
		case 4:
			Draw_Picture(RES_PORN5_BIN_SA);
		break;	
		case 5:
			Draw_Picture(RES_PORN6_BIN_SA);
		break;	
		case 6:
			Draw_Picture(RES_PORN7_BIN_SA);
		break;	
		case 7:
			Draw_Picture(RES_PORN8_BIN_SA);
		break;	
		case 8:
			Draw_Picture(RES_PORN9_BIN_SA);
		break;	
	}
					
	switch(status_level1[status])
	{
		case 2: // STOP
			Draw_Sprite(RES_INST2_BIN_SA, RES_INST2_BIN_EA, 16, 240-16);
		break;
		case 3: // FUCK
		case 0:
			Draw_Sprite(RES_INST1_BIN_SA, RES_INST1_BIN_EA, 16, 240-16);
		break;
		case 4: // FINISH
			Draw_Sprite(RES_INST3_BIN_SA, RES_INST3_BIN_EA, 16, 240-16);
		break;
	}	
}


#define add_score(sc) global_score += sc;
#define decrease_score() if (global_score > 2) global_score-=1;

void ingame_fucking()
{
	picture();
	// Text layer in hicolor needs hardware testing
	//print_dec(0, 0, global_score);

	time_game+=1;
	delay ++;
	
	if (status_level1[status] != 4)
	{
		if (frame_picture != 0)
		{
			if (status_level1[status] == 0 || status_level1[status] == 3)
			{
				add_score(1);
			}
			else if (status_level1[status] == 2)
			{
				decrease_score();
			}
			
			if (delay > 4)
			{
				delay = 0;
				frame_picture++;
				if (frame_picture > 7) frame_picture = 0;
			}
		}
		
		if (frame_picture == 0 && Button_Pressed())
		{
			frame_picture = 1;
		}

	}
	
	if (time_game > 123*2)
	{
		if (status_level1[status] == 4)
		{
			switch_gamemode(2);
		}
		time_game = 0;
		status++;
	}


}

int main(){
    int i = 0;
    unsigned long counter = 0;
    *SYSTEM_CTRL = 0;
    *INT_CTRL = 0;
    *WATCHDOG_CLEAR = 0x55aa;
    // Configure PPU
    
    switch_gamemode(0);
    
    //Draw_Picture(RES_PORN1_BIN_SA);

	//Upload_Palette(RES_PALETTE_8BIT_BIN_SA, RES_PALETTE_8BIT_BIN_EA);
	//Draw_Picture(RES_PORN1_BIN_SA );

	Init_Controller();

    
    for(;;)
    {
		

		if (display_frame == 1)
		{
			switch(game_mode)
			{
				// TITLESCREEN
				case 0:
					if (Button_Pressed() && wait_input >= 20)
					{
						switch_gamemode(3);
					}
				break;
				// Ingame
				case 1:
					ingame_fucking();
				break;
				
				// Results
				case 2:
					if (Button_Pressed() && wait_input >= 30)
					{
						text_progress++;
						wait_input = 0;
						switch_gamemode(0);
					}
				break;
				
				case 3:
					if (Button_Pressed() && wait_input >= 20)
					{
						text_progress++;
						wait_input = 0;
						if (text_progress > 4)
						{
							switch_gamemode(4);
						}
					}
				
					switch(text_progress)
					{
						case 0:
							Draw_Picture(RES_TEXT1_BIN_SA);
						break;	
						case 1:
							Draw_Picture(RES_TEXT2_BIN_SA);
						break;	
						case 2:
							Draw_Picture(RES_TEXT3_BIN_SA);
						break;	
						case 3:
							Draw_Picture(RES_TEXT4_BIN_SA);
						break;	
						case 4:
							Draw_Picture(RES_TEXT5_BIN_SA);
						break;	
					}
				break;
				case 4:
					for(i=0;i<12;i++)
					{
						print_string(2 + i, 2, text_minigame_1[i]);
					}
					if (Button_Pressed() && wait_input >= 30)
					{
						text_progress++;
						wait_input = 0;
						switch_gamemode(1);
					}
				break;
			}

    		display_frame = 0;
		}
		
		if (controller_reset_counter > 59)
		{
			Init_Controller();
			controller_reset_counter = 0;
		}
    	
	/*	if (idle_counter > 59)
		{
			
		}
		
    	if (joyx_val == 0xc7)
    	{
    		Draw_Picture(RES_IMAGE_HICOLOR_BIN_SA);
    	}
    	else if (joyx_val == 0xcf)
    	{
			
		}
		
    	if (joyx_val == 0x87)
    	{
    		Draw_Picture(RES_IMAGE_HICOLOR_BIN_SA);
    	}
    	else if (joyx_val == 0x8f)
    	{
			
		}
		
		if (buttons_val == 0xa1)
		{
			
		}*/
    }
    
    return 0;
}
