/****************************************************************************
** WARNING: You need a COMPACT memory model to run this.                   **
*****************************************************************************
** Demonstration of playing a single wave file using DMA                   **
**  by Steven H Don                                                        **
**                                                                         **
** For questions, feel free to e-mail me.                                  **
**                                                                         **
**    shd@earthling.net                                                    **
**    http://shd.cjb.net                                                   **
**                                                                         **
****************************************************************************/
//Include files
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <sys/nearptr.h>
#include <sys/types.h>
#include "sound.h"


#define ENABLE_NEAR() __djgpp_nearptr_enable();
#define DISABLE_NEAR() __djgpp_nearptr_disable();

static void PMD_Init_PPS()
{

}



unsigned char NullPCMInit()
{

	return 1;
}

unsigned char NullPCMLoad(const char* file, struct WaveData *vc, unsigned char index)
{	
	
	return 0;
}

void NullPCMUnload(struct WaveData *vc, unsigned char index)
{

}

void NullPCMPlay(struct WaveData *vc, unsigned char index)
{


}

void NullPCMClose()
{
}


SoundDevice NullPCMdevice = {
	"Null PCM driver",
	NullPCMInit,
	NullPCMLoad,
	NullPCMPlay,
	NullPCMUnload,
	NullPCMClose
};
