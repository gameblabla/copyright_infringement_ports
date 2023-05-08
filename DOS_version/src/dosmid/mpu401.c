/*
 * Library to access MPU-401 hardware
 *
 * Copyright (c) 2014, 2015, Mateusz Viste
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <dos.h>
#include <conio.h>  /* inp() and outp() */
#include <stdlib.h> /* NULL */
#include <time.h>

#include "mpu401.h"  /* include self for control */


#define MPU_DATA mpuport
#define MPU_STAT mpuport+1


/* flush everything from the MPU port (if anything) */
void mpu401_flush(int mpuport) {
  while (mpu401_poll(mpuport) != 0) inp(MPU_DATA);
}


/* wait until it's okay for us to write to the MPU */
void mpu401_waitwrite(int mpuport) {
  int buff;
  for (;;) {
    mpu401_flush(mpuport); /* make sure to flush first, in case the MPU */
    buff = inp(MPU_STAT);  /* tries to talk to us */
    if ((buff & 0x40) == 0) break;
  }
}


/* wait until it's okay for us to write to the MPU - but no longer than
 * timeout seconds. returns 0 on success, non-zero otherwise. */
static int mpu401_waitwrite_timeout(int mpuport, int timeout) {
  int buff;
  time_t notafter;
  notafter = time(NULL) + timeout;
  for (;;) {
    buff = inp(MPU_STAT);
    if ((buff & 0x40) == 0) return(0);
    if (time(NULL) >= notafter) return(-1);
  }
}


/* polls the midi interface - returns 0 if nothing is available to be read, non-zero otherwise.
   note that this should be checked as often as possible - whenever UART have some bytes for you, you MUST read them out */
int mpu401_poll(int mpuport) {
  int buff;
  buff = inp(MPU_STAT);
  if ((buff & 0x80) == 0) return(1);
  return(0);
}


void mpu401_waitread(int mpuport) {
  while (mpu401_poll(mpuport) == 0);
}


/* resets the MPU-401. returns 0 on success, non-zero otherwise. */
int mpu401_rst(int mpuport) {
  time_t timeout;
  if (mpu401_waitwrite_timeout(mpuport, 2) != 0) return(-1);  /* wait for the MPU to accept bytes from us */
  outp(MPU_STAT, 0xFF); /* Send MPU-401 RESET Command */
  /* note that some cards do not ACK on 0xFF ! that's why I should wait for a timeout here, and skip waiting if no answer after 1 or 2s */
  /* puts("wait ack"); */
  timeout = time(NULL) + 2; /* timeout is 1-2s */
  while (time(NULL) < timeout) {
    /* wait for the MPU to hand a byte to us (we are waiting for an ACK) */
    if (mpu401_poll(mpuport) != 0) {
      if (inp(MPU_DATA) == 0xFE) break; /* if we got the ACK, continue */
    }
  }
  mpu401_flush(mpuport);
  return(0);
}

void mpu401_uart(int mpuport) {
  mpu401_waitwrite(mpuport); /* Wait for port ready */
  outp(MPU_STAT, 0x3F);      /* Set MPU-401 "Dumb UART" Mode */
  /* I don't read anything back here, because MPU is not supposed to acknowledge the UART command */
}
