/****************************************************************************
** Demonstration of playing a single wave file through DMA      		   **
**  by Steven H Don                                                        **
**                                                                         **
** For questions, feel free to e-mail me.                                  **
**                                                                         **
**    shd@earthling.net                                                    **
**    http://shd.cjb.net                                                   **
**                                                                         **
****************************************************************************/
//Include files
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include <math.h>
#include <mem.h>
#include <io.h>

#include <stdlib.h>
#include <string.h>
#include <io.h>      // Include for DOS I/O functions
#include <fcntl.h>   // Include for file access modes
#include <sys/stat.h>// Include for file permission modes
#include "generic.h"
#include "sound.h"

#define DSP_WRITE_PORT 0xC

#define AUTO_INIT                   1
#define FAIL                        0
#define MASTER_VOLUME            0x22
#define MIC_VOLUME               0x0A
#define MIXER_ADDR                0x4
#define MIXER_DATA                0x5
#define MONO                        0
#define PIC_END_OF_INT           0x20
#define PIC_MASK                 0x21
#define PIC_MODE                 0x20
#define SUCCESS                     1
#define SINGLE_CYCLE                0
#define STEREO                      1
#define VOICE_VOLUME             0x04

#define DSP_BLOCK_SIZE            0x0048
#define DSP_DATA_AVAIL               0xE
#define DSP_HALT_SINGLE_CYCLE_DMA 0x00D0
#define DSP_READ_PORT                0xA
#define DSP_READY                   0xAA
#define DSP_RESET                    0x6
#define DSP_TIME_CONSTANT         0x0040
#define DSP_WRITE_PORT               0xC
#define DSP_VERSION                    0xE1

#define TRUE 1
#define FALSE 0

static char                          Key;
static unsigned int              CBuffer; //Clear Buffer indicator
static unsigned char          *DMABuffer; //Pointer to protected mode DMA Buffer
static unsigned int                  DMA, DMA16; //The DMA channel
static unsigned int                  IRQ; //The IRQ level
static unsigned int IOBasePort = 0x220;
static unsigned char DSP_Ver = 0;
static unsigned char soundblaster_init = 0;

static uint8_t soundblaster_cond;

#define BUFFER_SIZE 32768
#define BLOCK_SIZE 8192

#if defined(__386__)
uint16_t sel_dma_internal[2];

uint16_t dpmi_alloc(unsigned int par, uint16_t *sel);
void dpmi_free(uint16_t sel);

#pragma aux dpmi_alloc = \
		"mov eax, 0x100" \
		"int 0x31" \
		"mov [edi], dx" \
		value[ax] parm[ebx][edi];

#pragma aux dpmi_free = \
		"mov eax, 0x101" \
		"int 0x31" \
		parm[dx];

#endif

static char GetBlasterEnv(unsigned int *DMAChan8Bit, unsigned int *DMAChan16Bit, unsigned int *IRQNumber)
{
  char  Buffer[5],
    DMAChannelNotFound = TRUE,
       *EnvString,
    IOPortNotFound     = TRUE,
    IRQNotFound        = TRUE,
    SaveChar;

  int   digit,
    i,
    multiplier;


  EnvString = getenv("BLASTER");

  if (EnvString == NULL)
    return(FAIL);

  do
  {
    switch(*EnvString)
    {
      case 'A':  // I/O base port address found
      case 'a':
    EnvString++;
    for (i = 0; i < 3; i++)  // Grab the digits
    {
      Buffer[i] = *EnvString;
      EnvString++;
    }

    // The string is in HEX, convert it to decimal
    multiplier = 1;
    IOBasePort = 0;
    for (i -= 1; i >= 0; i--)
    {
      // Convert to HEX
      if (Buffer[i] >= '0' && Buffer[i] <= '9')
        digit = Buffer[i] - '0';
      else if (Buffer[i] >= 'A' && Buffer[i] <= 'F')
        digit = Buffer[i] - 'A' + 10;
      else if (Buffer[i] >= 'a' && Buffer[i] <= 'f')
        digit = Buffer[i] - 'a' + 10;

      IOBasePort = IOBasePort + digit * multiplier;
      multiplier *= 16;
    }

    IOPortNotFound = FALSE;
      break;


      case 'D': // 8-bit DMA channel
      case 'd':
      case 'H': // 16-bit DMA channel
      case 'h':
    SaveChar = *EnvString;
    EnvString++;
    Buffer[0] = *EnvString;
    EnvString++;

    if (*EnvString >= '0' && *EnvString <= '9')
    {
      Buffer[1] = *EnvString; // DMA Channel No. is 2 digits
      Buffer[2] = 0;
      EnvString++;
    }
    else
      Buffer[1] = 0;       // DMA Channel No. is 1 digit

    if (SaveChar == 'D' || SaveChar == 'd')
      *DMAChan8Bit  = atoi(Buffer);  // 8-Bit DMA channel
    else
      *DMAChan16Bit = atoi(Buffer);  // 16-bit DMA channel

    DMAChannelNotFound = FALSE;
      break;

      case 'I':  // IRQ number
      case 'i':
    EnvString++;
    Buffer[0] = *EnvString;
    EnvString++;

    if (*EnvString >= '0' && *EnvString <= '9')
    {
      Buffer[1] = *EnvString; // IRQ No. is 2 digits
      Buffer[2] = 0;
      EnvString++;
    }
    else
      Buffer[1] = 0;       // IRQ No. is 1 digit

    *IRQNumber  = atoi(Buffer);
    IRQNotFound = FALSE;
      break;

      default:
    EnvString++;
      break;
    }

  } while (*EnvString != NULL);

  if (DMAChannelNotFound || IOPortNotFound || IRQNotFound)
    return(FAIL);

  return(SUCCESS);
}

/****************************************************************************
** Checks to see if a Sound Blaster exists at a given address, returns     **
** true if Sound Blaster found, false if not.                              **
****************************************************************************/
static int ResetDSP(unsigned int Test)
{
  unsigned long i;

  outp(IOBasePort + DSP_RESET, (int) 1);
  delay(10); // wait 10 mS
  outp(IOBasePort + DSP_RESET, (int) 0);
  delay(10); // wait 10 mS
  // Wait until data is available
  while ((inp(IOBasePort + DSP_DATA_AVAIL) & 0x80) == 0);

  if (inp(IOBasePort + DSP_READ_PORT) == DSP_READY)
  {
    outp(IOBasePort + DSP_WRITE_PORT, DSP_VERSION);
    while ((inp(IOBasePort + DSP_DATA_AVAIL) & 0x80) == 0);
    DSP_Ver = inp(IOBasePort + DSP_READ_PORT);
    inp(IOBasePort + DSP_READ_PORT);
    return(1);
  }

  return(0);
}

/****************************************************************************
** Send a byte to the DSP (Digital Signal Processor) on the Sound Blaster  **
****************************************************************************/
static void WriteDSP(unsigned char Value)
{
	//Wait for the DSP to be ready to accept data
	// Thanks Geri for fix
	// "If the sb freezes, the system will not freeze"
	unsigned short hibaw=512;
	while ((inp(IOBasePort + DSP_WRITE_PORT) & 0x80) != 0)
    {
		hibaw--;
		if(!hibaw) break;
    }
	//Send byte
	outp (IOBasePort + DSP_WRITE_PORT, Value);
}

/****************************************************************************
** Starts playback of the buffer. The DMA controller is programmed with    **
** a block length of 32K - the entire buffer. The DSP is instructed to     **
** play blocks of 8K and then generate an interrupt (which allows the      **
** program to clear the parts that have already been played)               **
****************************************************************************/
static void StartPlayBack ()
{
	unsigned int  Page, OffSet;
	long LinearAddress;
  
	WriteDSP (0xD1);                 //DSP-command D1h - Enable speaker, required
                                   //on older SB's
	WriteDSP (0x40);                 //DSP-command 40h - Set sample frequency
	// 165 : 11025hz
	// 132 : 8000hz
	WriteDSP(132);          //Write time constant

	//Convert pointer to linear address
#if defined(__386__)
	LinearAddress = (DMABuffer);
#else
	LinearAddress = FP_SEG (DMABuffer);
	LinearAddress = (LinearAddress << 4) + FP_OFF (DMABuffer);
#endif
	Page = LinearAddress >> 16;      //Calculate page
	OffSet = LinearAddress & 0xFFFF; //Calculate offset in the page

	outp (0x0A, 4 | DMA);        //Mask DMA channel
	outp (0x0C, 0);              //Clear byte pointer
  
	// 0x58 block mode
	// 0x48 single cycle
	if (DSP_Ver < 4)  outp (0x0B, 0x48 | DMA);		// Set mode single cycle mode
	else outp (0x0B, 0x58 | DMA);					// Set mode auto init mode
 
  /*
      The mode consists of the following:
      0x58 + x = binary 01 00 10 xx
                        |  |  |  |
                        |  |  |  +- DMA channel
                        |  |  +---- Read operation (the DSP reads from memory)
                        |  +------- Single cycle mode
                        +---------- Block mode
  */

	outp (DMA << 1, OffSet & 0xFF); //Write the offset to the DMA controller
	outp (DMA << 1, OffSet >> 8);

	if (DMA == 0) outp (0x87, Page);
	if (DMA == 1) outp (0x83, Page);
	if (DMA == 3) outp (0x82, Page);

	outp ((DMA << 1) + 1, (BUFFER_SIZE - 1) & 0x00FF); //Set the block length to (BUFFER_SIZE-1) = 32 Kbyte
	outp ((DMA << 1) + 1, (BUFFER_SIZE - 1) >> 8);

	outp (0x0A, DMA);            //Unmask DMA channel

	if (DSP_Ver < 4)
	{
		WriteDSP (0x14);                 		//DSP-command 14h - Start auto-init playback
		WriteDSP ((BUFFER_SIZE - 1) & 0x00FF); 	//Set the block length to 0x1FFF = 8 Kbyte
		WriteDSP ((BUFFER_SIZE - 1) >> 8);
	}
	else
	{
		WriteDSP (0x48);                 			//DSP-command 48h - Set block length
		WriteDSP ((BLOCK_SIZE - 1) & 0x00FF);		//Set the block length to 0x1FFF = 8 Kbyte
		WriteDSP ((BLOCK_SIZE - 1) >> 8);
		WriteDSP (0x1C);							//DSP-command 1Ch - Start auto-init playback
	}
}

/****************************************************************************
** Clears an 8K part of the DMA buffer                                     **
****************************************************************************/
static void ClearBuffer (unsigned int Buffer)
{
	char *Address;

	//Fill an 8K block in the DMA buffer with 128's - silence
	Address = (char *)MK_FP (FP_SEG (DMABuffer), FP_OFF (DMABuffer) + (Buffer << 13));
	memset (Address, 128, BLOCK_SIZE);
}

/****************************************************************************
** Mixes a sample with the contents of the DMA buffer                      **
****************************************************************************/
static void MixVoice (uint8_t index)
{
  unsigned int Counter, beforeOffSet, OffSet, DMAPointer;

  //Read DMA pointer from DMA controller
  DMAPointer = inp (1 + (DMA << 1));
  DMAPointer = DMAPointer + (inp (1 + (DMA << 1)) << 8);

  /*
    DMAPointer contains amount that remains to be played.
    This is convert to the offset of the current sample
  */

	DMAPointer = (BUFFER_SIZE-1) - DMAPointer;

	OffSet = DMAPointer;
 
	for (Counter = 0; Counter <= PCM_SoundLength[index]; Counter++) 
	{
		//Mix byte
		DMABuffer [OffSet++] += PCM_SoundData[index][Counter];
		OffSet &= (BUFFER_SIZE-1); //Move on to next byte and keep it in the 32K range
	}

}

/****************************************************************************
** Loads a wave file into memory. This procedure treats any file as a      **
** standard 11025Hz, 8bit, mono .WAV file. It doesn't perform any error    **
** checking.                                                               **
****************************************************************************/
static unsigned char LoadVoice (const char *FileName, unsigned char index)
{
    int WAVFile;
    unsigned ii;
    unsigned fileLength;
    unsigned int OffSet;
    // If it can't be opened...
	_dos_open(FileName, _O_RDONLY | _O_BINARY, &WAVFile);
    if (WAVFile == -1) {
        // ..display error message
        // printf ("Unable to open wave file\n");
        return 1;
    }

    // Return length of file for sound length minus 48 bytes for .WAV header
    fileLength = lseek(WAVFile, 0L, SEEK_END);
    PCM_SoundLength[index] = fileLength - 48;
    lseek(WAVFile, 0L, SEEK_SET);

	/*if (PCM_SoundData[index])
	{
		free(PCM_SoundData[index]);
		PCM_SoundData[index] = NULL;
	}*/

	PCM_SoundData[index] = (unsigned char *)malloc(PCM_SoundLength[index] + 2); //Assign memory
    if (!PCM_SoundData[index]) return 1;
    

    lseek(WAVFile, 46L, SEEK_SET);                                  // Skip the header

    // Load the sample data
    _dos_read(WAVFile, PCM_SoundData[index], PCM_SoundLength[index], &ii);

    _dos_close(WAVFile); // Close the file
    
  
    // Convert voice, to prevent overflow
	for (OffSet = 0; OffSet <= PCM_SoundLength[index]; OffSet++) {
		PCM_SoundData[index] [OffSet] >>= 2;
		PCM_SoundData[index] [OffSet] -= 32;
	}
    
	return 0;
}


/****************************************************************************
** IRQ service routine - this is called when the DSP has finished playing  **
** a block                                                                 **
****************************************************************************/

static void interrupt ServiceIRQ ()
{
	//Relieve DSP
	inp (IOBasePort + 0xE);

	//Acknowledge cascade interrupt for IRQ 2, 10 and 11
	if (soundblaster_cond) outp (0xA0, 0x20);
	//Increase pointer to clear buffer and keep it in the range 0..3
	CBuffer++;
	CBuffer &= 3;
	//Clear buffer
	ClearBuffer (CBuffer);

#if defined(__386__)
	//Acknowledge hardware interrupt
	outp (0x20, 0x20);
#endif
}

void (__interrupt __far *old_sb_irq)(); // Pointer to old int09 handler

/****************************************************************************
** This procedure allocates 32K of memory to the DMA buffer and makes sure **
** that no page boundary is crossed                                        **
****************************************************************************/
static void AssignBuffer ()
{
	unsigned int Page1, Page2;  //Words
	uint16_t seg;
	uint32_t addr;
	char *TempBuf; //Temporary pointer
	long LinearAddress;

#if defined(__386__)
	seg = (unsigned char *)dpmi_alloc(BUFFER_SIZE/16, &sel_dma_internal[0]); //Assign memory
	addr = (uint32_t)seg << 4;
	TempBuf = (void*)addr;
#else
	//Assign 32K of memory
	TempBuf = (char *)malloc(BUFFER_SIZE);
#endif

	//Calculate linear address
#if defined(__386__)
	LinearAddress = TempBuf;
#else
	LinearAddress = FP_SEG (TempBuf);
	LinearAddress = (LinearAddress << 4) + FP_OFF (TempBuf);
#endif

	//Calculate page at start of buffer
	Page1 = LinearAddress >> 16;

	//Calculate page at end of buffer}
	Page2 = (LinearAddress + (BUFFER_SIZE-1)) >> 16;

	//Check to see if a page boundary is crossed
	if (Page1 != Page2) {
#if defined(__386__)
		seg = (unsigned char *)dpmi_alloc(BUFFER_SIZE/16, &sel_dma_internal[1]); //Assign memory
		addr = (uint32_t)seg << 4;
		DMABuffer = (void*)addr;
		dpmi_free(sel_dma_internal[0]);
#else
		//If so, assign another part of memory to the buffer
		if (!DMABuffer) DMABuffer = (char *)malloc(BUFFER_SIZE);
		free (TempBuf);
#endif
	} else //otherwise, use the part we've already allocated
		DMABuffer = TempBuf;

	memset (DMABuffer, 128, (BUFFER_SIZE-1));
}

static unsigned char SBDMA_Init()
{
	unsigned char Temp;

	// Check for Sound Blaster, address: ports 220, 230, 240, 250, 260 or 280
	for (Temp = 1; Temp < 9; Temp++)
	{
		if (Temp != 7)
		if (ResetDSP (0x200 + (Temp << 4))) {
			break;
		}
	}
	if (Temp == 9) {
		//printf ("No Sound Blaster found\n");
		return 1;
	}

	// Detecting SB environment
	if (GetBlasterEnv(&DMA, &DMA16, &IRQ) == FAIL)
	{
		// Setting to default values
		DMA = 1;
		IRQ = 7;
	}

	//Assign memory to the DMA Buffer
	AssignBuffer ();
	
	soundblaster_cond = (IRQ == 2 || IRQ == 10 || IRQ == 11) ? 1 : 0;

	//Save old/set new IRQ vector and Enable IRQ
	if (soundblaster_cond)
	{
		switch(IRQ)
		{
			case 2:
			old_sb_irq = _dos_getvect(0x71);
			_dos_setvect (0x71,  ServiceIRQ);
			outp (0xA1, inp (0xA1) & 253);
			break;
			case 10:
			old_sb_irq = _dos_getvect(0x72);
			_dos_setvect (0x72,  ServiceIRQ);
			outp (0xA1, inp (0xA1) & 251);
			break;
			case 11:
			old_sb_irq = _dos_getvect(0x73);
			_dos_setvect (0x73,  ServiceIRQ);
			outp (0xA1, inp (0xA1) & 247);
			break;
		}
		outp (0x21, inp (0x21) & 251);
	}
	else
	{
		old_sb_irq = _dos_getvect(8 + IRQ);
		_dos_setvect (8 + IRQ,  ServiceIRQ);
		outp (0x21, inp (0x21) & !(1 << IRQ));
	}

	//Set clear buffer to last buffer
	CBuffer = 3;
	
	// Mute micro but turn the volume up for the rest
	outp(IOBasePort + MIXER_ADDR, (int) MIC_VOLUME);
	outp(IOBasePort + MIXER_DATA, (int) 0x00);

	outp(IOBasePort + MIXER_ADDR, (int) VOICE_VOLUME);
	outp(IOBasePort + MIXER_DATA, (int) 0xFF);

	outp(IOBasePort + MIXER_ADDR, (int) MASTER_VOLUME);
	outp(IOBasePort + MIXER_DATA, (int) 0xFF);
	
	soundblaster_init = 1;
	
	return 0;
}

static unsigned char SBDMA_Load(const char* WaveFile, unsigned char index)
{
	return LoadVoice (WaveFile, index);
}

static void SBDMA_Play(unsigned char index)
{
	if (soundblaster_init == 0) return;

	StartPlayBack ();
	CBuffer = 3;
	ClearBuffer (CBuffer);
	memset (DMABuffer, 128, (BUFFER_SIZE-1));
	MixVoice (index);
}

static void SBDMA_Unload( unsigned char index)
{
	if (PCM_SoundData[index])
	{
		free(PCM_SoundData[index]);
		PCM_SoundData[index] = NULL;
		PCM_SoundLength[index] = 0;
	}
}

static void SBDMA_Update()
{

}

static void SBDMA_Close()
{
	if (soundblaster_init == 0) return;
	
	//Stops DMA-transfer
	WriteDSP (0xD0);
	// Only for SB2+
	if (DSP_Ver > 3)
	{
		WriteDSP (0xDA);
	}

	//Free the memory allocated to the sound buffer
	if (DMABuffer)
	{
		free (DMABuffer);
		DMABuffer = NULL;
	}

	//Free interrupt vectors used to service IRQs
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) _dos_setvect (0x71, old_sb_irq);
		if (IRQ == 10) _dos_setvect (0x72, old_sb_irq);
		if (IRQ == 11) _dos_setvect (0x73, old_sb_irq);
	} else _dos_setvect (8 + IRQ, old_sb_irq);

	//Mask IRQs
	if (IRQ == 2 || IRQ == 10 || IRQ == 11) {
		if (IRQ == 2) outp (0xA1, inp (0xA1) | 2);
		if (IRQ == 10) outp (0xA1, inp (0xA1) | 4);
		if (IRQ == 11) outp (0xA1, inp (0xA1) | 8);
		outp (0x21, inp (0x21) | 4);
	} else outp (0x21, inp (0x21) | (1 << IRQ));
	
#if defined(__386__)
	if (sel_dma_internal[0]) dpmi_free(sel_dma_internal[0]);
	if (sel_dma_internal[1]) dpmi_free(sel_dma_internal[1]);
#endif
}


SoundDevice SBDMA_device = {
	"SBDMA driver",
	SBDMA_Init,
	SBDMA_Load,
	SBDMA_Play,
	SBDMA_Unload,
	SBDMA_Update,
	SBDMA_Close
};
