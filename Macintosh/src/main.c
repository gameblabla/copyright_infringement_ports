#include <FixMath.h>
#include <Files.h>
#include <Sound.h>
#include <Dialogs.h>
#include <Retrace.h>
#include <Traps.h>
#include <stdio.h>
#include <Quickdraw.h>
#include <MacMemory.h>
#include <MacWindows.h>
#include <Dialogs.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <string.h>
#include <Sound.h>
#include <stdlib.h>

// mac declares
uint32_t frame_anim = 0;
uint32_t		unused;
#define MS_PER_TICK	(1000/60)

extern const unsigned char frames[196992];
extern const unsigned char titledata[12288];
extern const unsigned char rikdata[21888];
extern const unsigned char riklaughdata[16000];

BitMap offscreenBitmap;
WindowPtr gWindow;
Ptr offscreenMemory;
GrafPort offscreenPort;
short rowBytes;
uint8_t refresh;
Point penLoc;
void* basepointer;

#define		kHelloString		"\pHey, mac!"
#define   kBarkFileName      "\pbag.aif"

Rect destinationRect = {0};
Rect offscreenRect = {0, 0, 342, 512};
Rect screenRect = {0};
// End Mac specific stuff

unsigned char k, game_mode, text_progress;
#define FRAME_CURRENT text_progress
signed char diff;
unsigned char status;
unsigned char i;
unsigned short time_game;

#define KEEPITUP_GAME_STATUS 0
#define STOP_GAME_STATUS 1
#define FUCK_GAME_STATUS 2
#define END_GAME_STATUS 3

const unsigned char status_level1[] =
{ 
	FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS, KEEPITUP_GAME_STATUS, 
	STOP_GAME_STATUS, FUCK_GAME_STATUS,
	END_GAME_STATUS, END_GAME_STATUS 
};

const char ingame_quote[5][5] =
{
	"GO!!\0",
	"STOP\0",
	"FUCK\0",
	"END \0",
};
const unsigned char jump[5] = 
{ 3, 0xFF, 0, 0xFF, 1 };

static char bcd[4];
static char hibcd[4];

void switch_gamemode(unsigned char mode) ;


#define add_score(add) \
	bcd[2] += add; \
	if (bcd[2] > 57) { \
		bcd[1]+= 1; \
		bcd[2] = 48; } \
	if (bcd[1] > 57) { \
		bcd[0]+= 1; \
		bcd[1] = 48; } \

#define decrease_score(dec) \
	bcd[2] -= dec; \
	if (bcd[2] < 48) \
	{ \
		bcd[1]-= 1; \
		bcd[2] = 57; \
	} \
	if (bcd[1] < 48) \
	{ \
		bcd[0]-= 1; \
		bcd[1] = 57; \
	} \


uint32_t bitmapSize;

static inline void BlitScr()
{
	if (refresh)
	{
		CopyBits(&offscreenBitmap, &qd.screenBits, &offscreenRect, &destinationRect, srcCopy, NULL);
	}
}

static inline void COPY_DATA(const uint8_t *src, uint32_t *dest, uint16_t nr_longs)
{
	uint32_t *src_ptr = (uint32_t *)(src);  
	uint32_t *dest_ptr = (uint32_t *)(dest); 
	uint16_t count = (nr_longs);            
	while (count--) {                       
		*dest_ptr++ = *src_ptr++;         
	}
}


extern const unsigned char std_font[];

#define SDL_Delay(ms) Delay(ms/MS_PER_TICK, &unused);


static inline void SetPixelBlack(uint16_t x, uint16_t y) {
	Ptr baseAddr = basepointer;
    // Calculate the byte in memory where the pixel is located.
    uint32_t byte_pix = y * rowBytes + (x >> 3);
    // Calculate the bit mask for the pixel within that byte.
    char mask = 1 << (7 - (x % 8));
    baseAddr[byte_pix] |= mask; // Set the bit to set to black
}

static inline void SetPixelWhite(uint16_t x, uint16_t y) {
	Ptr baseAddr = basepointer;
    // Calculate the byte in memory where the pixel is located.
    uint32_t byte_pix = y * rowBytes + (x >> 3);
    // Calculate the bit mask for the pixel within that byte.
    char mask = 1 << (7 - (x % 8));
	baseAddr[byte_pix] &= ~mask; // Clear the bit to set to white
}


void drawChar(char ch, uint16_t x, uint16_t y)
{
	unsigned char i, j;
	unsigned char *charSprite;
	unsigned short plot_y;
	unsigned char cond;
	
	charSprite = (ch << 3) + std_font; // Updated line
	// Draw charSprite as monochrome 8*8 image using given color
	i = 8;
	while (i--)
	{
		j = 8;
		plot_y = y + i;
		while (j--)
		{
			cond = (charSprite[i] >> j) & 1;
			if(cond)
			{
				SetPixelWhite(x + (7 - j), plot_y);
			}
			
			if(!cond)
			{
				SetPixelBlack(x + (7 - j), y + i);
			}
		}
	}
	x += 8;
}

void Draw_Text(const char* str, unsigned short x, unsigned short y)
{
	unsigned char i;
	for(i = 0; str[i]; i++)
		drawChar(str[i], x + (i << 3), y);
}

void Draw_Text_Center(const char* str, unsigned short y)
{
	unsigned char i;
	uint16_t x;
	uint16_t text_width;
	text_width = strlen(str) << 3;
	x = (512 - text_width) >> 1;
	
	for(i = 0; str[i]; i++)
		drawChar(str[i], x + (i << 3), y);
}

/*
void showError(OSErr error, char* operation) {
    Str255 errorString;
    short itemHit;

    sprintf((char *)errorString + 1, "Error %d during %s", error, operation);
    errorString[0] = strlen((char *)errorString + 1);

    StandardAlert(kAlertStopAlert, errorString, NULL, NULL, &itemHit);
}
*/


uint8_t Init_Mac()
{
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();
	
	screenRect = qd.screenBits.bounds;
    // Paint the screen black
    SetPort(qd.thePort);
	ForeColor(blackColor);
	PaintRect(&screenRect);
	
	rowBytes = 64 & 0x3FFF;
    
    if (screenRect.bottom == 342 && screenRect.right == 512)
    {
		refresh = 0;
		basepointer = qd.screenBits.baseAddr;
		bitmapSize = (screenRect.bottom * screenRect.right) / 8;
		penLoc.h = (screenRect.left + screenRect.right) / 2;
		penLoc.v = (screenRect.top + screenRect.bottom) / 2;
		return 0;
	}
	
	refresh = 1;
	//if (refresh == 1)
	{
		// Calculate the size in bytes of the offscreen bitmap.
		bitmapSize = (offscreenRect.bottom * offscreenRect.right) / 8;

		// Allocate memory for the offscreen bitmap.
		offscreenMemory = NewPtr(bitmapSize);
		if (!offscreenMemory) return 1;

		// Initialize the offscreen bitmap.
		offscreenBitmap.baseAddr = offscreenMemory;
		offscreenBitmap.rowBytes = offscreenRect.right / 8;
		offscreenBitmap.bounds = offscreenRect;	
		
		// Set up the offscreen graphics port.
	 
		OpenPort(&offscreenPort);
		SetPort(&offscreenPort);
		
		offscreenPort.portBits = offscreenBitmap;
		
		// Copy the offscreen bitmap to the center of the screen.
		SetPort(qd.thePort);

		destinationRect.top = (screenRect.bottom - offscreenRect.bottom) / 2;
		destinationRect.left = (screenRect.right - offscreenRect.right) / 2;
		destinationRect.bottom = (screenRect.bottom + offscreenRect.bottom) / 2;
		destinationRect.right = (screenRect.right + offscreenRect.right) / 2;
		
		penLoc.h = (destinationRect.left + destinationRect.right) / 2;
		penLoc.v = (destinationRect.top + destinationRect.bottom) / 2;
		
		basepointer = offscreenBitmap.baseAddr;
	}
	
	return 0;
}

void Init_text()
{
	/* offset the pen's horizontal location by half the string width */
	penLoc.h -= StringWidth (kHelloString) / 2;
	TextFont (0);
	TextFace (0);
	TextSize (12);
}

const uint32_t offset_frames[] =
{
	0,
	20480,
	20480*2,
	20480*3,
	20480*4,
	20480*5,
	20480*6,
	20480*7,
	20480*8,
};

/*
void main(void) {
	Init_Mac();
	memset(basepointer, 0xFF, bitmapSize);
	Init_text();
	
	//SysBeep (1);

	frame_anim = 0;
	
    // Wait for a mouse click to exit.
    while (!Button())
    {
		COPY_DATA(frames + offset_frames[frame_anim], basepointer, 20480/4);
		//memcpy(basepointer, frames + (frame_anim * 20480) , 20480);
		frame_anim++;
		if (frame_anim > 8) frame_anim = 0;
		
		//MoveTo (penLoc.h, penLoc.v);
		//DrawString (kHelloString);
		Draw_Text_Center("HELLO WORLD", 320);
		
		BlitScr();
		SDL_Delay(80);
	}

    // Clean up.
    ClosePort(&offscreenPort);
    DisposePtr(offscreenMemory);

}*/


void DrawHUD()
{
	Draw_Text("SCORE", 0, 320);
	Draw_Text(bcd, 0, 328);
	Draw_Text_Center(ingame_quote[status_level1[status]], 320);	
}

#define ESCAPE_KEY_ROW1 0x00002000 // Escape
#define SPACE_KEY_ROW1 0x00000200 // Space
#define TAB_KEY_ROW1 0x00000100 // TAB
#define RETURN_KEY_ROW1 0x10000000 // Enter

int main ( void )
{
	UInt32 keys[4];
	uint8_t exitflag, check;
	
	check = Init_Mac();
	if (check == 1)
	{
		goto iserror;
	}
	
	memset(basepointer, 0xFF, bitmapSize);
	exitflag = 0;
	//Init_text();
	
	hibcd[0] = 48;
	hibcd[1] = 48;
	hibcd[2] = 48;
	hibcd[3] = '\0';
	
	switch_gamemode(0);

	while(exitflag == 0)
	{
		GetKeys(keys);

		if ((keys[1] & ESCAPE_KEY_ROW1) || (keys[1] & TAB_KEY_ROW1))
		{
			exitflag = 1;
		}
		
		switch(game_mode)
		{
			default:
				if (Button() || (keys[1] & SPACE_KEY_ROW1))
				{
					switch_gamemode(jump[game_mode]);
				}
			break;
			case 1:
				time_game+=1;

				if (FRAME_CURRENT > 0 && status_level1[status] != END_GAME_STATUS)
				{
					if (status_level1[status] == STOP_GAME_STATUS)
					{
						if (!(bcd[2] == 48 && bcd[0] == 48 && bcd[1] == 48)) 
						{
							decrease_score(1);
						}
					}
					else
					{
						add_score(1);
					}
					
					FRAME_CURRENT++;
					if (FRAME_CURRENT > 8) FRAME_CURRENT = 0;
					
					COPY_DATA(frames + offset_frames[FRAME_CURRENT], basepointer, 20480/4);
					Draw_Text(bcd, 0, 328);
				}
				
				if (FRAME_CURRENT == 0)
				{
					if (Button() || (keys[1] & SPACE_KEY_ROW1))
					{
						SysBeep(1);
						//mainsound.Play_Sound(0, special_sound);
						/*if (special_sound == 1) 
						{
							//special_sound = 0;
						}*/
						FRAME_CURRENT = 1;
					}
				}
				
				
				if (time_game > 128)
				{
					if (status_level1[status] == END_GAME_STATUS)
					{
						switch_gamemode(2);
					}
					else
					{
						time_game = 0;
						status++;
						if (status_level1[status] == KEEPITUP_GAME_STATUS)
						{
							//special_sound = 1;
						}
						Draw_Text_Center(ingame_quote[status_level1[status]], 320);	
					}
				}
			break;
			case 3:
				if (Button() || (keys[1] & SPACE_KEY_ROW1))
				{
					SysBeep(2);
					text_progress++;

					if (text_progress > 4)
					{
						switch_gamemode(4);
					}
					else
					{
						
						switch(text_progress)
						{
							case 1:
								COPY_DATA(rikdata + 16000, basepointer, (21888-16000)/4);
								COPY_DATA(riklaughdata, basepointer, 16000/4);
								Draw_Text("You're going to be the perfect victim", 0, 320);
								Draw_Text("for my games!", 0, 328);
								//mainsound.Load_Sound(soundfiles[isSoundDMA][1], 0);
							break;
							case 2:
								COPY_DATA(rikdata , basepointer, (21888)/4);
								Draw_Text("I'm Rikuto, i offer you the chance to", 0, 320);
								Draw_Text("prove your worth in a trial", 0, 328);
								//mainsound.Load_Sound(soundfiles[isSoundDMA][2], 0);
							break;
							case 3:
								COPY_DATA(rikdata + 16000, basepointer, (21888-16000)/4);
								COPY_DATA(riklaughdata, basepointer, 16000/4);
								Draw_Text("Don't be scared, it'll be fun!", 0, 320);
								//mainsound.Load_Sound(soundfiles[isSoundDMA][3], 0);
							break;
							case 4:
								COPY_DATA(rikdata , basepointer, (21888)/4);
								Draw_Text("Your challenge is going to be this:", 0, 320);
								Draw_Text("you'll be on the other end and punish them", 0, 328);
								//mainsound.Load_Sound(soundfiles[isSoundDMA][4], 0);
							break;
						}
						//mainsound.Play_Sound(0, 0);
					}
				}
			break;
		}
		
		// This is too fast otherwise
		BlitScr();
		SDL_Delay(80);
	
	}
	
iserror:
    // Clean up.
    if (refresh)
    {
		ClosePort(&offscreenPort);
		if (offscreenMemory)
		{
			DisposePtr(offscreenMemory);
		}
	}
	
	memset(qd.screenBits.baseAddr, 0xFF, bitmapSize);
}

void switch_gamemode(unsigned char mode)
{
	game_mode = mode;
	text_progress = 0;
	status = 0;
	time_game = 0;
	i = 0;

	memset(basepointer, 0xFF, bitmapSize);
	
	switch(mode)
	{
		case 0:
			COPY_DATA(titledata, basepointer, 12288/4);
			//memcpy(physbase, bmp[0], 1);
		
			Draw_Text_Center("COPYRIGHT INFRINGEMENT", 16);
			Draw_Text_Center("THE REGRESSIVE RIGHT", 16+8);
			Draw_Text_Center("BY GAMEBLABLA 2023", 16+16);
			
			Draw_Text_Center("PRESS SPACE TO PLAY", 280);
			Draw_Text("HISCORE", 0, 320);
			Draw_Text(hibcd, 0, 328);
		break;
		case 1:
			bcd[0] = 48;
			bcd[1] = 48;
			bcd[2] = 48;
			bcd[3] = '\0';
			

			COPY_DATA(frames, basepointer, 20480/4);
			
			DrawHUD();
		break;
		case 2:
			COPY_DATA(rikdata , basepointer, (21888)/4);
		
			Draw_Text("Your SCORE was ", 0, 0);
			Draw_Text(bcd, 0, 16);
			
			//Canonical approach, thanks calcmaniac
			diff = bcd[0] - hibcd[0];
			if (diff == 0)
				diff = bcd[1] - hibcd[1];
			if (diff == 0)
				diff = bcd[2] - hibcd[2];
				
			if (diff > 0)
			{
				hibcd[0] = bcd[0];
				hibcd[1] = bcd[1];
				hibcd[2] = bcd[2];
			}
			
			//Clear_Text();
			
			if (bcd[0] < 48+3)
			{
				Draw_Text("Guess you'll stay there for a while", 0, 320);
			}
			else if (bcd[0] < 48+5)
			{
				Draw_Text("Not bad but you can do better!", 0, 320);
				Draw_Text("Otherwise you can't leave this place!", 0, 328);
			}
			else
			{
				Draw_Text("Congrats! I'll leave you alone.", 0, 320);
				Draw_Text("This won't be the last of me!", 0, 328);
			}
		break;
		case 3:
			COPY_DATA(rikdata , basepointer, (21888)/4);
		
			Draw_Text("Oh my, what do we have here?", 0, 320);
			Draw_Text("You look so innocent...", 0, 328);
			
			//mainsound.Load_Sound(soundfiles[isSoundDMA][0], 0);
			//mainsound.Play_Sound(0, 0);
		break;
		case 4:
			Draw_Text("Your goal is to FUCK Zhongli!", 0, 8);
			Draw_Text("Follow the instructions to get points", 0, 16);
			Draw_Text("Penetrate him by pressing a key", 0, 24);
			Draw_Text("Good luck !", 0, 64);
		break;
	}
	
}

