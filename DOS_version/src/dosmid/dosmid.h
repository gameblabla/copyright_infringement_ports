/*
 * User interface routines of DOSMid.
 * Copyright (C) Mateusz Viste
 */

#ifndef dosmidi_h_sentinel
#define dosmidi_h_sentinel

extern unsigned char LoadMusic(const unsigned char* filename, int argc, char **argv);
extern void _interrupt PlayMusic();
extern void Exit_DOSMID();

#endif

