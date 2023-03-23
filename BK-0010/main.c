#include <stddef.h>
#include <stdint.h>

extern const unsigned char BAKURA_FACE[5524];
extern const unsigned char BKFRAME1_CUT[1718];
extern const unsigned char BKFRAME3_CUT[1718];
extern const unsigned char BKFRAME4_CUT[1744];
extern const unsigned char BKFRAME5_CUT[1704];
extern const unsigned char font[1024];

#define SIZE 16384
#define DEST_ADDRESS ((volatile void*)040000)
#define SCREEN_MODE ((volatile unsigned char*)000040)
#define BEEPER_LINE ((volatile unsigned char*)177716)

static uint8_t *dest = (uint8_t *)DEST_ADDRESS;

#define DECOMPRESS_ZX0(a,b) \
    asm("mov %0, r1" :: "r" (a)); \
    asm("mov %0, r2" :: "r" (b)); \
	asm("jsr pc,_dzx0"); \

#define DECOMPRESS_ZX1(a,b) \
    asm("mov %0, r1" :: "r" (a)); \
    asm("mov %0, r2" :: "r" (b)); \
	asm("jsr pc,_dzx1"); \
	
#define DECOMPRESS_LZSA3(a,b) \
    asm("mov %0, r1" :: "r" (a)); \
    asm("mov %0, r2" :: "r" (b)); \
	asm("jsr pc,_Unpacklzsa3"); \
	
#define CLEAR_SCREEN() \
	asm("CLR R2"); \
	asm("MOV $040000,R1"); \
	asm("MOV $020000,R0"); \
	asm("1: MOV R2,(R1)+"); \
	asm("SOB R0,1b"); \

// Clear text bakura
#define CLEAR_SCREEN_PARTIAL() \
	asm("CLR R2"); \
	asm("MOV $070000,R1"); \
	asm("MOV $002000,R0"); \
	asm("1: MOV R2,(R1)+"); \
	asm("SOB R0,1b"); \

#define INIT_COVOX() \
	asm("MOV #176,@$0177714"); \
	

#if 0
void clear_screen()
{
	uint16_t i;
	uint16_t *dest = (uint16_t *)DEST_ADDRESS;
    for (i = 0; i < SIZE/2; i++) {
        dest[i] = 0xFFFF;
    }
}

static void copy_screen(uint8_t* src, uint16_t size)
{
	uint8_t *dest = (uint8_t *)DEST_ADDRESS;
    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}
#endif


const char ingame_quote[5][9] =
{
	"  GO ON ",
	"  STOP  ",
	"FUCK HIM",
	"FINISH  ",
};

unsigned char game_mode, text_progress, status;
unsigned short time_game;
unsigned char bcd[3];
unsigned char FRAME_CURRENT;

#define SCREEN_RAM_ADDRESS ((volatile uint8_t*)040000)
/*
#define PUT_CHARACTER(character, x, y) \
    temp = (y << 6) + x; \
    \
    SCREEN_RAM_ADDRESS[temp] = font[character]; \
    SCREEN_RAM_ADDRESS[temp+64] = font[character+128]; \
    SCREEN_RAM_ADDRESS[temp+128] = font[character+256]; \
    SCREEN_RAM_ADDRESS[temp+192] = font[character+384]; \
    SCREEN_RAM_ADDRESS[temp+256] = font[character+512]; \
	SCREEN_RAM_ADDRESS[temp+320] = font[character+640]; \
    SCREEN_RAM_ADDRESS[temp+384] = font[character+768]; \
    SCREEN_RAM_ADDRESS[temp+448] = font[character+896]; \

*/

static inline void PUT_CHARACTER(char character, uint8_t x,  uint8_t y)
{
    unsigned short temp = (y << 6) + x;
    SCREEN_RAM_ADDRESS[temp] = font[character];
    SCREEN_RAM_ADDRESS[temp+64] = font[character+128];
    SCREEN_RAM_ADDRESS[temp+128] = font[character+256];
    SCREEN_RAM_ADDRESS[temp+192] = font[character+384];
    SCREEN_RAM_ADDRESS[temp+256] = font[character+512];
	SCREEN_RAM_ADDRESS[temp+320] = font[character+640];
    SCREEN_RAM_ADDRESS[temp+384] = font[character+768];
    SCREEN_RAM_ADDRESS[temp+448] = font[character+896];
}

static void put_text(const char *text, uint8_t x, uint8_t y)
{
	unsigned short temp = (y << 6) + x;
    while (*text) {
		dest[temp] = font[*text];
		dest[temp+64] = font[*text+128];
		dest[temp+128] = font[*text+256];
		dest[temp+192] = font[*text+384];
		dest[temp+256] = font[*text+512];
		dest[temp+320] = font[*text+640];
		dest[temp+384] = font[*text+768];
		dest[temp+448] = font[*text+896];
        temp += 1;
        text++;
    }
} 

/*
 * 32 : Space
 * 12 : Enter
 * 26 : Cursor Up
 * 86 : Cursor Left
 * 25 : Cursor Right
 * 27 : Cursor Down
*/
#if 0
static inline uint16_t read_char()
{
    uint16_t result;
    asm("emt 6");
    asm volatile(
        "mov r0, %0\n" // Move the result from R0 register to result variable
        : "=r"(result) // Output
        :
        : "r0"         // Clobbered register
    );
    return result;
}
#endif




const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };


#define Read_keyboard_any() ((*((uint16_t *)0177716) & 0x0040) ? 1 : 0)
/*
static inline uint8_t Read_keyboard_any()
{
    uint16_t *memory = (uint16_t *)0177716;
    return (*memory & 0x0040) ? 1 : 0;
}*/

const unsigned char* frames_offset[]=
{
	BKFRAME1_CUT,
	BKFRAME3_CUT,
	BKFRAME4_CUT,
	BKFRAME5_CUT
};

#define KEEPITUP_GAME_STATUS 0
#define STOP_GAME_STATUS 1
#define FUCK_GAME_STATUS 2
#define END_GAME_STATUS 3

#define GENERATE_BEEP() *((uint16_t *)0177716) |= 0x0040;
#define STOP_BEEP() *((uint16_t *)0177716) &= ~0x0040;

const unsigned char status_level1[] =
{ 
	FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS,
	END_GAME_STATUS, END_GAME_STATUS 
};


#define add_score(add) \
	bcd[2] += add; \
	if (bcd[2] > 9) { \
		bcd[1]+= 1; \
		bcd[2] = 0; } \
	if (bcd[1] > 9) { \
		bcd[0]+= 1; \
		bcd[1] = 0; } \

#define decrease_score(dec) \
	bcd[2] -= dec; \
	if (bcd[2] < 0) \
	{ \
		bcd[1]-= 1; \
		bcd[2] = 9; \
	} \
	if (bcd[1] < 0) \
	{ \
		bcd[0]-= 1; \
		bcd[1] = 9; \
	} \


void switch_gamemode(unsigned char mode) ;

void main()
{
	switch_gamemode(0);
	for(;;)
	{
		switch(game_mode)
		{
			default:
				if (Read_keyboard_any() == 0)
				{
					GENERATE_BEEP();
					switch_gamemode(jump[game_mode]);
				}
			break;
			case 1:
				time_game+=1;

				if (status_level1[status] != END_GAME_STATUS)
				{
					if (FRAME_CURRENT > 0)
					{
						if (status_level1[status] == STOP_GAME_STATUS)
						{
							if (!(bcd[2] == 0 && bcd[0] == 0 && bcd[1] == 0)) 
							{
								decrease_score(1);
							}
						}
						else
						{
							add_score(1);
						}
						
						FRAME_CURRENT++;
						if (FRAME_CURRENT > 3) FRAME_CURRENT = 0;
						
						DECOMPRESS_ZX1(frames_offset[FRAME_CURRENT], DEST_ADDRESS + 3840);
						
						PUT_CHARACTER(bcd[0], 222, 160);
						PUT_CHARACTER(bcd[1], 223, 160);
						PUT_CHARACTER(bcd[2], 224, 160);
						time_game+=1230/2;
						STOP_BEEP();
					}
					
					if (FRAME_CURRENT == 0)
					{
						if (Read_keyboard_any() == 0)
						{
							GENERATE_BEEP();
							FRAME_CURRENT = 1;
						}
					}
				}
				
				if (time_game > 123*100)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						time_game = 0;
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						put_text(ingame_quote[status_level1[status]], 221, 32);
					}
				}
			break;
			case 3:
				time_game++;
				if (Read_keyboard_any() == 0 && time_game > 1000)
				{
					GENERATE_BEEP();
					text_progress++;

					if (text_progress > 3)
					{
						switch_gamemode(4);
					}
					else
					{
						CLEAR_SCREEN_PARTIAL();
						switch(text_progress)
						{
							case 1:
								put_text("You're going to be the perfect victim for my games!", 4, 192);
							break;
							case 2:
								put_text("I'm Rikuto, i offer you the chance to prove", 4, 192);
								put_text("your worth in a trial.", 4, 208);
							break;
							case 3:
								put_text("Your challenge is this:", 4, 192);
								put_text("Teach them a valuable lesson !", 4, 208);
							break;
						}
					}
					
					
					STOP_BEEP();

					time_game = 0;
				}
			break;
		}
	
	}
}




void switch_gamemode(unsigned char mode) 
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	FRAME_CURRENT = 0;
	CLEAR_SCREEN();
	
	STOP_BEEP();

	
	switch(mode)
	{
		case 0:
			put_text("COPYRIGHT INFRINGEMENT", 211, 48);
			put_text("PUSH KEY TO START", 213, 192);
		break;
		case 1:
			bcd[0] = 0;
			bcd[1] = 0;
			bcd[2] = 0;
			DECOMPRESS_ZX1(BKFRAME1_CUT, DEST_ADDRESS + 3840);
			put_text("SCORE", 222, 148);

			put_text(ingame_quote[status_level1[status]], 221, 32);
		break;
		case 2:
			DECOMPRESS_ZX1(BAKURA_FACE, DEST_ADDRESS);
			if (bcd[0] < 1)
			{
				put_text("Guess you'll stay there.", 1, 192);
			}
			else if (bcd[1] < 3)
			{
				put_text("Not bad but you can do better", 1, 192);
			}
			else
			{
				put_text("Congrats! I'll leave you alone for now.",  1, 192);
			}
		break;
		case 3:
			DECOMPRESS_ZX1(BAKURA_FACE, DEST_ADDRESS);
			put_text("Oh my,what do we have here? You look so innocent.", 4, 192);
		break;
		case 4:
			put_text("You must fuck Zhongli to get points", 4, 32);
			put_text("Hold key to fuck and release when STOP appears.", 4, 64);
			put_text("PRESS KEY TO START", 4, 128);
		break;
	}
	
}

