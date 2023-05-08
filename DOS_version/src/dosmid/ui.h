/*
 * User interface routines of DOSMid.
 * Copyright (C) Mateusz Viste
 */

#ifndef ui_h_sentinel
#define ui_h_sentinel

enum fileformats {
  FORMAT_UNKNOWN,
  FORMAT_MIDI,
  FORMAT_RMID,
  FORMAT_MUS
};

struct trackinfodata {
  unsigned long tempo;
  unsigned long totlen; /* total length, in seconds */
  unsigned long elapsedsec; /* time elapsed (in seconds) */
  unsigned short notestates[128]; /* here I record the state of every note on every channel, to turn all notes OFF in case of program termination */
  unsigned short channelsusage;  /* a bit field indicating what channels are used */
  unsigned short miditimeunitdiv;
  unsigned char chanprogs[16];
  int titlescount;
  enum fileformats fileformat;
  int midiformat;
  char filename[11];
};

#endif

