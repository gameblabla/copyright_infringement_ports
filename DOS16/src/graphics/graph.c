#include <dos.h>
#include <stdio.h>  /* printf(), puts(), fopen()... */
#include <limits.h> /* ULONG_MAX */
#include <stdlib.h> /* malloc(), free(), rand() */
#include <string.h> /* strcmp() */
#include <time.h>
#include <stdint.h>
#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "vga.h"

/**************************************************************************
 *  set_mode                                                              *
 *     Sets the video mode.                                               *
 **************************************************************************/

void set_mode(uint8_t mode)
{
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  #ifdef _32BITS
  int386(VIDEO_INT, &regs, &regs);
  #else
  int86(VIDEO_INT, &regs, &regs);
  #endif
}
