
#include <stdio.h>
#include <conio.h> /* this is where Open Watcom hides the outp() etc. functions */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <dos.h>
//#include "snd.h"
#include "generic.h"
#include "graph.h"
#include "keyboard.h"
#include "sound.h"
#include "music.h"

BITMAP bmp;
short y = 0, x = 0;

VideoDevice mainvideo;
MusicDevice mainmusic;
SoundDevice mainsound;

struct WaveData *vc[2];


#define VGA 1

int res;

int main(int argc,char **argv) 
{
	int done = 0;
	Init_Machine();
	

	mainvideo = Normal_MCGA;
	
	mainvideo.SetVideoMode(320, 200, 1, argc, argv);
	mainvideo.LoadBMP("RIK_NO.SIF", &bmp, 320, 200, 1);
	mainvideo.LoadPalette("RIK_NO.PAL");
	mainvideo.SetPalette();

	mainvideo.DrawBMP_static(&bmp, 0, 0, 0);
	mainvideo.Print_text("THIS IS A TEST", 0, 0, 255);
	
	Keyboard_Init();
	
	while (!done) 
	{
		
		if (keystate[KEY_ESC] == ISPRESSED)
		{
			done = 1;
		}
		
		if (keystate[KEY_SPACE] == ISPRESSED)
		{
			//mainmusic.Stop_Music();
			keystate[KEY_SPACE] = 0;
		}
		
		//mainsound.Sound_Update();
		mainvideo.FlipVideo();
		mainvideo.Vsyncwait();
	}
	mainvideo.FreeBMP(&bmp);
	/*mainsound.Sound_Unload(vc[0], 0);
	mainsound.Sound_Close();*/
	mainvideo.FreeVideo();
	Keyboard_Clean();
	Close_Machine();
	

	return 0;
}

