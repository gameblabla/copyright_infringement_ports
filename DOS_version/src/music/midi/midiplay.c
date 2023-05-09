/****************************************************************************
** Background MIDI unit                                                    **
**  by Steven H Don                                                        **
*****************************************************************************
** The file FM.DAT is necessary to play through Sound Blaster/compatible   **
*****************************************************************************
** Unit for playing MIDI files in the background through either an FM      **
** sound card, such as the Sound Blaster (any version) or a General MIDI   **
** device, such as the Roland cards or a Gravis UltraSound using MegaEM.   **
** It does not implement all of the MIDI effects and asynchronous files    **
** are not supported. It works with most files I have, however. As you can **
** see from the size of the file, as well as the complexity of the code,   **
** MIDI is no easy stuff. This file should be useful though.               **
**                                                                         **
** For questions, feel free to e-mail me.                                  **
**                                                                         **
**    shd@earthling.net                                                    **
**    http://shd.cjb.net                                                   **
**                                                                         **
****************************************************************************/
/*
  Functions available to the calling program are:
    LoadMIDI   : loads the MIDI file into memory.
                   Expects a filename including .MID extension
                   Returns 1 if successful
    UnloadMIDI : unloads the MIDI file and fress the memory.
    PlayMIDI   : starts MIDI playback
    StopMIDI   : stops MIDI playback
    SetGM      : sets the playback device to General MIDI.
    SetFM      : sets the playback device to the FM synthesizer
    SetVol     : sets the playback volume, from 0..255
    Playing    : returns 0 when the MIDI file has ended
*/
#include <dos.h>
#include <fcntl.h>
#include <stdint.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <i86.h>
#include <conio.h>
#ifdef __DJGPP
  //Extra for protected mode interrupt handler in DJGPP
  #include <dpmi.h>
  #include <pc.h>
#endif

//Used to distinguish between General MIDI devices and FM synthesizers
#define None 0
#define FM 1
#define GM 2
//Used to activate tremolo and vibrato amplification
const AM = 1;
const VIB = 0;

  //The MIDI File's header
  struct FileHeaderType {
    int32_t MTHd; // 6468544Dh = "MTHd"
    int32_t HeaderSize;
    uint16_t FileFormat, NumberOfTracks, TicksPerQNote;
  };

  //The header of a track
  struct TrackHeaderType {
    int32_t MTrk; //6B72544Dh = "MTrk"
    int32_t TrackSize;
  };

//Variables for reading the MIDI file
  //How many tracks are there in the file
  unsigned char NumberOfTracks;
  //This is used to determine whether a track needs attention
  int32_t WaitingFor [64];
  //Stores the actual MIDI data
  char *TrackData [64];
  //Stores the byte length of each track
  uint32_t TrackSize [64];
  //Which byte is to be read next
  uint32_t NextBytePtr [64];
  //Stores the last MIDI command sent, this is necessary for "running" mode
  unsigned char LastCommand [64];

  //This stores a pointer to the original timer handler
  #ifdef __DJGPP
    //DJGPP requires some doing
    void pm_new8h(void);
    static _go32_dpmi_seginfo pm_old_handler, pm_new_handler;
  #else
    void (__interrupt __far *BIOSTimerHandler)();
  #endif

  //This is used for counting the clock ticks
  long TicksPerQNote;
  long ClockTicks, TickCounter, MIDICounter;

  //Variable for amplifying AM, VIB and percussion mode.
  char ByteBD;

//Variables and constants necessary for playing the MIDI file through an FM
//based sound card such as Sound Blaster

//Addresses of the operators used to form voice data
const OpAdr [9] = {0, 1, 2, 8, 9, 10, 16, 17, 18};
//F numbers - to form a specific note
const uint16_t FNr [128] = {86,91,96,102,108,114,121,128,136,144,
                                       153,162,172,182,192,204,108,114,121,
                                       128,136,144,153,162,172,182,192,204,
                                       216,229,242,257,136,144,153,162,172,
                                       182,192,204,216,229,242,257,272,288,
                                       306,324,172,182,192,204,216,229,242,
                                       257,272,288,306,324,343,363,385,408,
                                       216,229,242,257,272,288,306,324,343,
                                       363,385,408,432,458,485,514,272,288,
                                       306,324,343,363,385,408,432,458,485,
                                       514,544,577,611,647,343,363,385,408,
                                       432,458,485,514,544,577,611,647,686,
                                       726,770,816,432,458,485,514,544,577,
                                       611,647,686,726,770,816,864,916,970,1023};
/*
  Some operators are reserved for percussion. They are at the end of the
  SB's operators which means they're in the middle of the SB Pro's. The main
  program doesn't take this into account so this is used to convert from
  virtual voice number to real voice number
*/
const RealVoice [15] = {0, 1, 2, 3, 4, 5, 9, 10,
                        11, 12, 13, 14, 15, 16, 17};

char InUse [15];
long Activated [15];
unsigned char MIDILink [15], NoteNumber [15], NoteVelocity [15];
unsigned char NoteOffData [18][2];
//This stores which instrument is currently in use on a MIDI channel
unsigned char Instrument [16];
//This stores the FM instrument definitions
unsigned char M2 [128], M4 [128], M6 [128], M8 [128], ME [128], MC [128],
              C2 [128], C4 [128], C6 [128], C8 [128], CE [128];

//This indicates whether the file should be played through a General MIDI
//device or through an FM synthesizer, such as the Sound Blaster
unsigned char Device;
//This indicates whether we have a Sound Blaster (6 voices) or an SB Pro or
//better (15 voices) - the 5 drums are always available
unsigned char Voices;
//This stores the Base address of the Sound Blaster and GM device
unsigned short int FMBase, GMBase;
//The master volume. Normal volume is 128.
long MasterVolume;

//This procedure compensates a given volume for the master volume
unsigned char DoVolume (unsigned char Before)
{
  long After;

  After = Before;
  After *= MasterVolume;
  After >>= 7;
  if (After > 127) After = 127;
  return (unsigned char)After;
}

//This procedure changes the speed of the timer to adjust the tempo of a song
//NewSpeed gives the amount of microseconds per quarter note
void ChangeSpeed (long NewSpeed)
{
  long Divisor;

  Divisor = (TicksPerQNote << 20) / NewSpeed;


  //Set the appropriate values for the timer interrupt
  TickCounter = 0x1234DD / Divisor;
  outp (0x43, 0x34);
  outp (0x40, TickCounter & 0xFF);
  outp (0x40, (TickCounter >> 8) & 0xFF);
}

//Writes a value to a specified index register on the FM card
void WriteFM (unsigned char Chip, unsigned char Register, unsigned char Value)
{
  unsigned int Counter;
  unsigned short Address;

  Address = FMBase + 2;
  if (Chip == 0) Address = FMBase;
  
  //Select register
  outp (Address, Register);
  //Wait for card to accept value
  for (Counter = 0; Counter < 25; Counter++) inp (Address);
  //Send value
  outp (Address + 1, Value);
  //Wait for card to accept value
  for (Counter = 0; Counter < 100; Counter++) inp (Address);
}

//Sets a channel on the FM synthesizer to a specific instrument
void SetInstr (unsigned char Voice, unsigned char I, unsigned char Volume)
{
  unsigned int Chip, Value;
  Chip = 0;
  if (Voice > 8) {
    Chip = 1;
    Voice -= 9;
  }
  
  //Correction for volume
  Value = 63 - (M4 [I] & 63);
  Value = Value * Volume / 127;
  
  Value = 0;
  if (Value < 64) Value = 63 - Value;
  
  Value = (M4 [I] & 0xC0) | Value;
  //Set up voice modulator
  WriteFM (Chip, 0x20 + OpAdr [Voice], M2 [I]);
  WriteFM (Chip, 0x40 + OpAdr [Voice], Value);
  WriteFM (Chip, 0x60 + OpAdr [Voice], M6 [I]);
  WriteFM (Chip, 0x80 + OpAdr [Voice], M8 [I]);
  WriteFM (Chip, 0xE0 + OpAdr [Voice], ME [I]);
  //The "or 3 shl 4" is enables the voice on the OPL3
  WriteFM (Chip, 0xC0 + OpAdr [Voice], MC [I] + (3 << 4));

  //Correction for volume
  Value = 63 - (C4 [I] & 63);
  Value = Value * Volume / 127;
  if (Value > 63) Value = 0; else Value = 63 - Value;
  Value = (C4 [I] & 0xC0) | Value;
  //Set up voice carrier
  WriteFM (Chip, 0x23 + OpAdr [Voice], C2 [I]);
  WriteFM (Chip, 0x43 + OpAdr [Voice], Value);
  WriteFM (Chip, 0x63 + OpAdr [Voice], C6 [I]);
  WriteFM (Chip, 0x83 + OpAdr [Voice], C8 [I]);
  WriteFM (Chip, 0xE3 + OpAdr [Voice], CE [I]);

}

//Sets up a drum channel, in much the same way as a normal voice
void SetDrum (unsigned char Operator, unsigned char O2, unsigned char O4,
              unsigned char O6, unsigned char O8, unsigned char OE,
              unsigned char OC)
{
  WriteFM (0, 0x20 + Operator, O2);
  WriteFM (0, 0x40 + Operator, O4);
  WriteFM (0, 0x60 + Operator, O6);
  WriteFM (0, 0x80 + Operator, O8);
  WriteFM (0, 0xE0 + Operator, OE);
  WriteFM (0, 0xC0 + Operator, OC);
}

//Enables a note on the FM synthesizer
void EnableNote (unsigned char Voice, unsigned char Number)
{
  unsigned char Chip, Note, Block;

  //Calculate which part of the OPL3 chip should receive the data
  Chip = 0;
  
  //Calculate appropriate data
  if (Voice > 8) {
    Chip = 1;
    Voice -= 9;
  }
  //Calculate appropriate data for FM synthesizer
  Note = Number;
  Block = Number >> 4;
  //Store data to disable the note when necessary
  NoteOffData [Voice][0] = (FNr[Note] & 0xFF);
  NoteOffData [Voice][1] = (FNr[Note] >> 8) + (Block << 2);
  //Write data to FM synthesizer
  WriteFM (Chip, 0xA0+Voice, (FNr[Note] & 0xFF));
  WriteFM (Chip, 0xB0+Voice, (FNr[Note] >> 8) + (Block << 2) + 32);
}

//Disables a note on the FM synthesizer
void DisableNote (unsigned char Voice)
{
  unsigned char Chip;
  Chip = 0;
  //Calculate which part of the OPL3 chip should receive the data
  if (Voice > 8) {
    Chip = 1;
    Voice -= 9;
  } 
  //Write data to FM synthesizer
  WriteFM (Chip, 0xA0+Voice, NoteOffData [Voice][0]);
  WriteFM (Chip, 0xB0+Voice, NoteOffData [Voice][1]);
}

//Cuts a note on the FM synthesizer immediately
void CutNote (unsigned char Voice)
{
  unsigned char Chip;
  Chip = 0;
  //Calculate which part of the OPL3 chip should receive the data}
  if (Voice > 8) Chip = 1;
  
  //Set decay rate to fast - to avoid "plink" sound
  WriteFM (Chip, 0x80 + OpAdr [Voice % 9], 0xF);
  WriteFM (Chip, 0x83 + OpAdr [Voice % 9], 0xF);
  //Disable the note
  DisableNote (Voice);
}

//Processes a "NoteOff" event for the FM synthesizer
void NoteOff (unsigned char MIDIChannel, unsigned char Number,
              unsigned char Velocity)
{
  unsigned char FoundChannel, FMChannel;

  //Assume the note can't be found
  FoundChannel = 255;
  //Scan for note on FM channels
  for (FMChannel = 0; FMChannel <= Voices; FMChannel++) {
    if (InUse[FMChannel] != 0) {
      //Is this the correct channel?
      if (MIDILink [FMChannel] == MIDIChannel
      && NoteNumber [FMChannel] == Number) {
        //If the correct channel has been found then report that
        FoundChannel = FMChannel;
        break;
      }
    }
  }
  if (FoundChannel != 255) {
    //Disable the note
    DisableNote (RealVoice [FoundChannel]);
    //Store appropriate information
    InUse [FoundChannel] = 0; //InUse flag
  }
  //Done to prevent "Parameter 'Velocity' never used
  Velocity++;
}

//Processes a "NoteOn" event for the FM synthesizer
void NoteOn (unsigned char MIDIChannel, unsigned char Number,
             unsigned char Velocity)
{
  unsigned char FreeChannel, FMChannel;
  int32_t oldestIndex;
  uint32_t oldestTimestamp;


  //Velocity of zero means note off
  if (Velocity == 0) {
    NoteOff (MIDIChannel, Number, Velocity);
    return;
  }
  
  //Assume no free channel
  FreeChannel = 255;
  oldestIndex = -1;
  oldestTimestamp = 0x7FFFFFFF;
  //Scan for free channel
  for (FMChannel = 0; FMChannel <= Voices; FMChannel++) {
    if (InUse[FMChannel] == 0) {
      FreeChannel = FMChannel;
      break;
    } else {
      if (Activated[FMChannel] < oldestTimestamp) {
        oldestTimestamp = Activated[FMChannel];
        oldestIndex = FMChannel;
      }
    }
  }

  if (FreeChannel == 255) {
    FreeChannel = oldestIndex;
    CutNote(RealVoice[FreeChannel]);
  }

  //Change the instrument settings for the FM channel chosen
  SetInstr (RealVoice [FreeChannel], Instrument [MIDIChannel], Velocity);
  //Start playing the note
  EnableNote (RealVoice [FreeChannel], Number);
  //Store appropriate information
  InUse [FreeChannel] = 1; //InUse flag
  Activated [FreeChannel] = MIDICounter; //Activation time
  MIDILink [FreeChannel] = MIDIChannel; //Link FM channel to MIDI channel
  NoteNumber [FreeChannel] = Number; //Note number (which note is being played)
  NoteVelocity [FreeChannel] = Velocity; //Velocity (=volume)
}

//Plays a drum note
void DrumOn(unsigned char MIDIChannel, unsigned char Number, unsigned char Velocity) {
    // If velocity is 0, note is turned off, this is ignored
    if (Velocity == 0) return;
    
    // Convert velocity to "level" needed by SB and reduce the volume slightly
    //Velocity = (int)(Velocity << 3) / 10;
    //Velocity = 63 - (Velocity >> 1);
    Velocity = 63 - (((Velocity - (Velocity >> 3)) >> 1));

    switch (Number) {
        case 35: case 36: case 41: case 43: // Bass drum
            SetDrum(16, 0, 13, 248, 102, 0, 48);
            SetDrum(19, 0, Velocity, 246, 87, 0, 16);
            WriteFM(0, 0xBD, ByteBD | 16);
            WriteFM(0, 0xBD, ByteBD);
            break;
        
        case 38: case 40: // Snare drum
            SetDrum(20, 0, Velocity, 240, 7, 2, 16);
            WriteFM(0, 0xBD, ByteBD | 8);
            WriteFM(0, 0xBD, ByteBD);
            break;
        
        default: // Other drum types
            switch (Number) {
                case 37: case 39: case 42: case 44: case 46: case 56:
                case 62: case 69: case 70: case 71: case 72: case 78: // HiHat
                    SetDrum(17, 0, Velocity, 240, 6, 0, 16);
                    WriteFM(0, 0xBD, ByteBD | 1);
                    break;
                
                case 45: case 47: case 48: case 50: case 60: case 61:
                case 63: case 64: case 65: case 66: case 67: case 68:
                case 73: case 74: case 75: case 76: case 77: // TomTom
                    SetDrum(18, 2, Velocity, 240, 6, 0, 16);
                    WriteFM(0, 0xBD, ByteBD | 4);
                    break;
                
                case 49: case 51: case 52: case 53: case 54: case 55:
                case 57: case 58: case 59: case 79: case 80: case 81: // Cymbal
                    SetDrum(21, 4, Velocity, 240, 6, 0, 16);
                    WriteFM(0, 0xBD, ByteBD | 2);
                    break;
            }
            WriteFM(0, 0xBD, ByteBD); // Deactivate in the default case
            break;
    }
    
    // Done to avoid "Parameter 'MIDIChannel' never used
    MIDIChannel++;
}

//Disables a drum note, well, it actually does nothing since drum notes
//do not need to be disabled
void DrumOff (unsigned char MIDIChannel, unsigned char Number,
              unsigned char Velocity)
{
  MIDIChannel++; Number++; Velocity++;
}

//Sends a GM command to the GM device
void SendGM (unsigned char c)
{
  while ((inp (GMBase + 1) & 0x40) != 0);
  outp (GMBase, c);
}

//This function reads a byte from a specific track
unsigned char ReadByte (unsigned char TrackNumber)
{
  if (WaitingFor [TrackNumber] < 0xFFFFFF)
    return TrackData [TrackNumber][NextBytePtr [TrackNumber]++];
    
  return 0;
}

//This function reads a Variable Length Encoded (VLE) number from the track
long GetVLE (unsigned char TrackNumber)
{
  unsigned char ByteRead;
  long Result;

  //Assume zero
  Result = 0;
  do {
    //Read first byte
    ByteRead = ReadByte (TrackNumber);
    //Store 7bit part
    Result = (Result << 7) + (ByteRead & 0x7F);
  } while ((ByteRead & 0x80) != 0);
  return Result;
}

//This void stores the time for the next event
void GetDeltaTime (unsigned char TrackNumber)
{
  WaitingFor [TrackNumber] += GetVLE (TrackNumber);
}

//This void handles the MIDI events
void DoEvent (unsigned char TrackNumber)
{
  unsigned char MIDIByte, MIDICommand, MIDIChannel, MetaEvent;
  long DataLength;
  unsigned char n1, n2;
  long Counter;
  long Data;

  //Get the MIDI event command from the track
  MIDIByte = ReadByte (TrackNumber);
  //If this is not a command, we are in "running" mode and the last
  //command issued on the track is assumed
  if ((MIDIByte & 0x80) == 0) {
    MIDIByte = LastCommand [TrackNumber];
    NextBytePtr [TrackNumber]--;
  }
  //Store the command for running mode
  LastCommand [TrackNumber] = MIDIByte;
  /*
    META-EVENTS
    ===========
    Special commands controlling timing etc.
  */
  if (MIDIByte == 0xFF) {
    MetaEvent  = ReadByte (TrackNumber);
    DataLength = GetVLE (TrackNumber);
	switch (MetaEvent) {
	  case 0x2F: // End of track
		WaitingFor[TrackNumber] = 0xFFFFFF;
		break;

	  case 0x51: // Tempo change
		Data = ReadByte(TrackNumber);
		Data = (Data << 8) + ReadByte(TrackNumber);
		Data = (Data << 8) + ReadByte(TrackNumber);
		ChangeSpeed(Data);
		break;

	  default: // Others (text events, track sequence numbers etc. - ignore
		for (Counter = 1; Counter <= DataLength; Counter++) {
		  ReadByte(TrackNumber);
		}
		break;
	}
  }
  /*
    CHANNEL COMMANDS
    ================
    Upper nibble contains command, lower contains channel
  */
  MIDIChannel = MIDIByte & 0xF;
  MIDICommand = MIDIByte >> 4;
	switch (MIDICommand) {
	  case 8: // Note off
		n1 = ReadByte(TrackNumber);
		n2 = DoVolume(ReadByte(TrackNumber));

		if (Device == FM) { // FM - Sound Blaster or AdLib
		  if (MIDIChannel == 9 || MIDIChannel == 15) {
			DrumOff(MIDIChannel, n1, n2);
		  } else {
			NoteOff(MIDIChannel, n1, n2);
		  }
		} else { // GM - General MIDI device
		  SendGM(MIDIByte);
		  SendGM(n1);
		  SendGM(n2);
		}
		break;

	  case 9: // Note on
		n1 = ReadByte(TrackNumber);
		n2 = DoVolume(ReadByte(TrackNumber));

		if (Device == FM) {
		  if (MIDIChannel == 9 || MIDIChannel == 15) {
			DrumOn(MIDIChannel, n1, n2);
		  } else {
			NoteOn(MIDIChannel, n1, n2);
		  }
		} else {
		  SendGM(MIDIByte);
		  SendGM(n1);
		  SendGM(n2);
		}
		break;

	  case 0xA: // Key Aftertouch - only supported for GM device
		n1 = ReadByte(TrackNumber);
		n2 = DoVolume(ReadByte(TrackNumber));

		if (Device == GM) {
		  SendGM(MIDIByte);
		  SendGM(n1);
		  SendGM(n2);
		}
		break;

	  case 0xB: // Control change - only supported for GM device
		n1 = ReadByte(TrackNumber);
		n2 = ReadByte(TrackNumber);

		if (Device == GM) {
		  SendGM(MIDIByte);
		  SendGM(n1);
		  SendGM(n2);
		}
		break;

	  case 0xC: // Patch change - this changes the instrument on a channel
		n1 = ReadByte(TrackNumber);

		if (Device == FM) {
		  Instrument[MIDIChannel] = n1;
		} else {
		  SendGM(MIDIByte);
		  SendGM(n1);
		}
		break;

	  case 0xD: // Channel aftertouch - only supported on GM device
		n1 = ReadByte(TrackNumber);

		if (Device == GM) {
		  SendGM(MIDIByte);
		  SendGM(n1);
		}
		break;

	  case 0xE: // Pitch wheel change - only supported on GM device
		n1 = ReadByte(TrackNumber);
		n2 = ReadByte(TrackNumber);

		if (Device == GM) {
		  SendGM(MIDIByte);
		  SendGM(n1);
		  SendGM(n2);
		}
		break;

	  case 0xF: // SYSTEM COMMANDS
		switch (MIDIChannel) {
		  case 0: // System Exclusive
			do {
			  ReadByte(TrackNumber);
			} while (ReadByte(TrackNumber) != 0x7F);
			break;

		  case 2: // Song Position Pointer
			ReadByte(TrackNumber);
			ReadByte(TrackNumber);
			break;

		  case 3: // Song Select
			ReadByte(TrackNumber);
			break;
		}
		break;
	}

}

//Returns TRUE if the MIDI file is still playing. FALSE if it has stopped
/*
unsigned char Playing ()
{
  unsigned char CurrentTrack;
  unsigned char Result;

  //Assume it has stopped
  Result = 0;
  //Check for at least one track still playing
  for (CurrentTrack = 1; CurrentTrack <= NumberOfTracks; CurrentTrack++)
    if (WaitingFor [CurrentTrack] < 0xFFFFFF) Result = 1;
  return Result;
}
*/

//This is the new timer interrupt handler and routines to install it
#ifdef __DJGPP
  void TimerHandler ()
#else
  #ifdef __cplusplus
    void interrupt TimerHandler (...)
  #else
    void interrupt TimerHandler ()
  #endif
#endif
{
	unsigned char CurrentTrack;

	//Increase MIDI counter
	MIDICounter += 1;
	//Check all the channels for MIDI events
	for (CurrentTrack = 1; CurrentTrack <= NumberOfTracks; CurrentTrack++)
	{
		//If it is time to handle an event, do so
		if (NextBytePtr [CurrentTrack] < TrackSize [CurrentTrack])
		while (MIDICounter >= WaitingFor [CurrentTrack]) {
		  //Call the event handler
		  DoEvent (CurrentTrack);
		  //Store the time for the next event
		  GetDeltaTime (CurrentTrack);
		}
	}

	//Check whether we need to call the original timer handler
	ClockTicks += TickCounter;
	//Do so if required
	if (ClockTicks > 65535)
	{
		ClockTicks -= 65536;
		(*BIOSTimerHandler)();
	} 
#if defined(__386__)
	//Acknowledge hardware interrupt
	outp (0x20, 0x20);
#endif
}

//Installs the MIDI timer handler
void InstallTimer ()
{
  //Assume 18.2 times a second
  TickCounter = 0;
  //Assume tempo 120 according to MIDI spec
  ChangeSpeed (TicksPerQNote * 25000 / 3);
  //Install new timer handler
  #ifdef __DJGPP
    _go32_dpmi_chain_protected_mode_interrupt_vector(8, &pm_new_handler);
  #else
    _dos_setvect (8, TimerHandler);
  #endif
}


//This converts a 32bit number from little-endian (Motorola) to big-endian
//(Intel) format
long L2B32 (long L)
{
  long B;
  unsigned char T;

  B = 0;
  for (T = 0; T <= 3; T++) {
    B = (B << 8) + (L & 0xFF);
    L >>= 8;
  }
  return B;
}

//This converts a 16bit number from little-endian (Motorola) to big-endian
//(Intel) format
unsigned int L2B16 (unsigned int L)
{
  return ((L & 0xFF) << 8) + (L >> 8);
}

/*
#define L2B32(L) (((L & 0xFF) << 24) | (((L >> 8) & 0xFF) << 16) | (((L >> 16) & 0xFF) << 8) | ((L >> 24) & 0xFF))
#define L2B16(L) (((L & 0xFF) << 8) | (L >> 8))
*/

//This loads the MIDI file into memory
unsigned char LoadMIDI(const char* FileName) {
  int MIDIFile;
  struct FileHeaderType MIDIHeader;
  struct TrackHeaderType TrackHeader;
  unsigned char CurrentTrack;
  unsigned char Result;
  unsigned bytesRead;

  Result = 0;
  _dos_open(FileName, _O_RDONLY | _O_BINARY, &MIDIFile);
  if (MIDIFile == -1) return 0;

  _dos_read(MIDIFile, &MIDIHeader, 14, &bytesRead);
  if (MIDIHeader.MTHd == 0x6468544D) {
    if (L2B32(MIDIHeader.HeaderSize) == 6) {
      MIDIHeader.FileFormat = L2B16(MIDIHeader.FileFormat);
      if (MIDIHeader.FileFormat != 2) {
        TicksPerQNote = L2B16(MIDIHeader.TicksPerQNote);
        NumberOfTracks = L2B16(MIDIHeader.NumberOfTracks);
        if (MIDIHeader.FileFormat == 0) NumberOfTracks = 1;

        for (CurrentTrack = 1; CurrentTrack <= NumberOfTracks; CurrentTrack++) {
          _dos_read(MIDIFile, &TrackHeader, 8, &bytesRead);
          if (TrackHeader.MTrk != 0x6B72544D) return 0;
          TrackHeader.TrackSize = L2B32(TrackHeader.TrackSize);

          TrackSize[CurrentTrack] = TrackHeader.TrackSize;
          TrackData[CurrentTrack] = (char*)malloc(TrackSize[CurrentTrack]);
          _dos_read(MIDIFile, (char*)TrackData[CurrentTrack], TrackSize[CurrentTrack], &bytesRead);
        }
        Result = 1;
      }
    }
  }

  _dos_close(MIDIFile);
  return Result;
}

//This resets the drums
void EnableDrums ()
{
  //Enable waveform select
  WriteFM (0, 1, 0x20);
  //Enable percussion mode, amplify AM & VIB
  ByteBD = (AM << 7) | (VIB << 6) | 0x20;
  WriteFM (0, 0xBD, ByteBD);
  //Set drum frequencies
  WriteFM (0, 0xA6, 400 & 0xFF);
  WriteFM (0, 0xB6, (400 >> 8) + (2 << 2));
  WriteFM (0, 0xA7, 500 & 0xFF);
  WriteFM (0, 0xB7, (500 >> 8) + (2 << 2));
  WriteFM (0, 0xA8, 650 & 0xFF);
  WriteFM (0, 0xB8, (650 >> 8) + (2 << 2));
}

//Guess!!
void StopMIDI ()
{
  unsigned char CurrentChannel;

  //RestoreTimer ();
  //Return to 18.2 times a second}
  outp (0x43, 0x34);
  outp (0x40, 0);
  outp (0x40, 0);
  
  //Install old timer handler
  #ifdef __DJGPP
    _go32_dpmi_set_protected_mode_interrupt_vector(8, &pm_old_handler);
  #else
    _dos_setvect (8, BIOSTimerHandler);
  #endif
  
  //Send "All notes off" to each channel
  if (Device == FM) {
    for (CurrentChannel = 0; CurrentChannel <= 17; CurrentChannel++)
      if (InUse[CurrentChannel] != 0) DisableNote (CurrentChannel);
  } else for (CurrentChannel = 0; CurrentChannel <= 15; CurrentChannel++) {
    SendGM (0xB0 | CurrentChannel);
    SendGM (123);
    SendGM (0);
  }
}

//This starts playing the MIDI file}
void PlayMIDI ()
{
  unsigned char CurrentTrack;

  //MIDI might already be playing, so stop it first
  StopMIDI ();
  //Clear read pointers for every track
  for (CurrentTrack = 1; CurrentTrack <= NumberOfTracks; CurrentTrack++) {
    NextBytePtr [CurrentTrack] = 0;
    WaitingFor [CurrentTrack] = 0;
    LastCommand [CurrentTrack] = 0xFF;
    GetDeltaTime (CurrentTrack);
  }
  MIDICounter = 0;
  EnableDrums ();
  InstallTimer ();
}

//This unloads the MIDI file from memory
void UnloadMIDI ()
{
  unsigned char CurrentTrack;

  StopMIDI ();
  for (CurrentTrack = 1; CurrentTrack <= NumberOfTracks; CurrentTrack++) {
    if (TrackSize [CurrentTrack]) {
        free (TrackData [CurrentTrack]);
      TrackSize [CurrentTrack] = 0;
    }
  }
}

//Set the playback volume
void SetVol (unsigned char NewVol)
{
  MasterVolume = NewVol;
}

//Check for the existence of an OPL2/3 chip
unsigned char TestOPL (unsigned short Test)
{
  unsigned char A, B;
  unsigned char Result;

  //Assume no OPL was found
  Result = 0;

  //Find it
  outp (Test, 0); delay (1); outp (Test+1, 0); delay (1);
  outp (Test, 4); delay (1); outp (Test+1, 0x60); delay (1);
  outp (Test, 4); delay (1); outp (Test+1, 0x60); delay (1);
  A = inp (Test);
  outp (Test, 2); delay (1); outp (Test+1, 0xFF); delay (1);
  outp (Test, 4); delay (1); outp (Test+1, 0x21); delay (1);
  B = inp (Test);
  outp (Test, 4); delay (1); outp (Test+1, 0x60); delay (1);
  outp (Test, 4); delay (1); outp (Test+1, 0x60); delay (1);

  if ((A & 0xE0) == 0 && (B & 0xE0) == 0xC0)
    //This might be an OPL2
    Result = 2;
  else
    //There's nothin here, so stop looking
    return 0;

  //Check for OPL3
  if ((inp (Test) & 0x06) == 0) Result = 3;

  return Result;
}

//This function returns -1 if a GM device is detected at the specified port
unsigned char TestGM (unsigned short Base)
{
  delay (10);
  if ((inp (Base + 1) & 0x40) == 0) {
    outp (Base, 0xF8);
    delay (10);
    if ((inp (Base + 1) & 0x40) == 0) return -1;
  }
  return 0;
}



//Initialize FM driver
char SetFM ()
{
  int FMFile;
  unsigned bytesRead;
  unsigned char Temp;

  //Assume a standard SB or AdLib: 6 melodic voices, 5 percussion voices
  Voices = 5;
  //Check for FM card
  if (TestOPL (0x388) > 0) FMBase = 0x388;
  //Check for OPL3 at 0x220 and 0x240
  Temp = TestOPL (0x240);
  if (Temp == 2) FMBase = 0x240;
  if (Temp == 3) { FMBase = 0x240; Voices = 14; }
  Temp = TestOPL (0x220);
  if (Temp == 2) FMBase = 0x220;
  if (Temp == 3) { FMBase = 0x220; Voices = 14; }
  if (FMBase != 0) {
    //Enable OPL3 if present
    if (Voices != 5) {
      WriteFM (1, 5, 1);
      WriteFM (1, 4, 0);
    }

	  _dos_open("FM.DAT", _O_RDONLY | _O_BINARY, &FMFile);
	  _dos_read(FMFile, &M2[0], 128, &bytesRead);
	  _dos_read(FMFile, &M4[0], 128, &bytesRead);
	  _dos_read(FMFile, &M6[0], 128, &bytesRead);
	  _dos_read(FMFile, &M8[0], 128, &bytesRead);
	  _dos_read(FMFile, &ME[0], 128, &bytesRead);
	  _dos_read(FMFile, &MC[0], 128, &bytesRead);
	  _dos_read(FMFile, &C2[0], 128, &bytesRead);
	  _dos_read(FMFile, &C4[0], 128, &bytesRead);
	  _dos_read(FMFile, &C6[0], 128, &bytesRead);
	  _dos_read(FMFile, &C8[0], 128, &bytesRead);
	  _dos_read(FMFile, &CE[0], 128, &bytesRead);
	  _dos_close(FMFile);
    
    Device = FM;
  }
  return Device == FM;
}

//Initialize GM driver
char SetGM ()
{
  //Try detecting a GM device
  GMBase = 0;
  if (TestGM (0x300)) GMBase = 0x300;
  if (TestGM (0x330)) GMBase = 0x330;
  //If it is detected, use it
  if (GMBase) Device = GM;
  return Device == GM;
}

void InitMIDI ()
{
  //No device found yet
  Device = None;
  //Start at normal volume
  SetVol (128);

  //Save old timer handler
  #ifdef __DJGPP
    _go32_dpmi_get_protected_mode_interrupt_vector(8, &pm_old_handler);
    pm_new_handler.pm_offset = (int) TimerHandler;
    pm_new_handler.pm_selector = _go32_my_cs();
  #else
    BIOSTimerHandler = _dos_getvect (8);
  #endif
}
