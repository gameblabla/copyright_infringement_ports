#include <Files.h>
#include <Sound.h>
#include <Dialogs.h>
#include <stdio.h>
#include <Quickdraw.h>
#include <MacWindows.h>
#include <Dialogs.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <string.h>
#include <Types.h>
#include <Timer.h>
#include <OSUtils.h>
#include <Gestalt.h>
#include <Processes.h>
#include "audio.h"

BitMap offscreenBitmap;
WindowPtr gWindow;
Rect offscreenRect;
Rect screenRect;
Ptr offscreenMemory;
Rect destinationRect = {0};
GrafPort offscreenPort;
short rowBytes;
uint8_t refresh;
char* basepointer;

uint32_t frame_anim;
uint32_t frame_pointer;
uint32_t		unused;
#define MS_PER_TICK	(1000/60)


extern const unsigned char frames[196992];
extern const unsigned char titledata[12288];
extern const unsigned char rikdata[21888];
extern const unsigned char riklaughdata[16000];

void BlitScr()
{
	if (refresh)
	{
		CopyBits(&offscreenBitmap, &qd.screenBits, &offscreenRect, &destinationRect, srcCopy, NULL);
	}
}

void SDL_Delay(uint32_t ms)
{
	Delay(ms/MS_PER_TICK, &unused);
}

void SetPixel(short x, short y, Boolean white) {
    // Get a pointer to the offscreen bitmap's base address.
    Ptr baseAddr = offscreenBitmap.baseAddr;

    // Calculate the byte in memory where the pixel is located.
    long byte = y * rowBytes + (x >> 3);

    // Calculate the bit mask for the pixel within that byte.
    char mask = 1 << (7 - (x % 8));

    // Set or clear the bit depending on the desired color.
    baseAddr[byte] |= mask; // Set the bit to set to black
    
    if (white) {
        baseAddr[byte] &= ~mask; // Clear the bit to set to white
    }
}

extern void vbi_handler();


void main(void) {
	int a, i;
	long bitmapSize;

    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();
    
    // Paint the screen black
    screenRect = qd.screenBits.bounds;
    SetPort(qd.thePort);
    ForeColor(blackColor);
    PaintRect(&screenRect);
    
    refresh = 0;
    if (screenRect.bottom == 342 && screenRect.right == 512) {
		refresh = 1;
		basepointer = qd.screenBits.baseAddr;
		rowBytes = 64 & 0x3FFF;
	}

	if (refresh == 0)
	{
		// Define the dimensions of the offscreen bitmap.
		offscreenRect = (Rect){0, 0, 342, 512};

		// Calculate the size in bytes of the offscreen bitmap.
		bitmapSize = (offscreenRect.bottom * offscreenRect.right) / 8;

		// Allocate memory for the offscreen bitmap.
		offscreenMemory = NewPtr(bitmapSize);
		if (offscreenMemory == NULL) {
			// Handle memory allocation error.
			return;
		}

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
		
		basepointer = offscreenBitmap.baseAddr;
		
		rowBytes = offscreenBitmap.rowBytes & 0x3FFF;
		
		SetPixel(20,20, 1);
		RefreshScreen();
	}
	

	frame_anim = 0;
	
    // Wait for a mouse click to exit.
    while (!Button())
    {
		memcpy(basepointer, frames + (frame_anim * bitmapSize) , bitmapSize);
		frame_anim++;
		if (frame_anim > 8) frame_anim = 0;
		
		BlitScr();
		SDL_Delay(100);
	}

    // Clean up.
    ClosePort(&offscreenPort);
    if (offscreenMemory != NULL) {
        DisposePtr(offscreenMemory);
    }
}
