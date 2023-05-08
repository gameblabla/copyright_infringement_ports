/*
 * Modified DOSMIDI so it could work as a library for games.
*/


/*
 * DOSMID - a low-requirement MIDI and MUS player for DOS
 *
 * Copyright (C) 2014-2018, Mateusz Viste
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
#include <stdio.h>  /* printf(), puts(), fopen()... */
#include <limits.h> /* ULONG_MAX */
#include <stdlib.h> /* malloc(), free(), rand() */
#include <string.h> /* strcmp() */
#include <time.h>

#include "mem.h"
#include "midi.h"
#include "mus.h"
#include "outdev.h"
#include "rs232.h"
#include "syx.h"
#include "timer.h"
#include "ui.h"

#include "generic.h"

#define MAXTRACKS 64
#define EVENTSCACHESIZE 64 /* *must* be a power of 2 !!! */
#define EVENTSCACHEMASK 63 /* used by the circular events buffer */

#define ACTION_NONE 0
#define ACTION_NEXT 1
#define ACTION_PREV 2
#define ACTION_ERR_SOFT 3
#define ACTION_ERR_HARD 4
#define ACTION_EXIT 64

struct clioptions {
  int memmode;          /* type of memory to use: MEM_XMS or MEM_MALLOC */
  int xmsdelay;
  unsigned short devport;
  unsigned short port_mpu;
  unsigned short port_awe;
  unsigned short port_opl;
  unsigned short port_sb;
  int dontstop;
  int random;       /* randomize playlist order */
  enum outdev_types device;
  int devicesubtype;
  char *devname;    /* the human name of the out device (MPU, AWE..) */
  char *midifile;   /* MIDI filename to play */
  char *syxrst;     /* syx file to use for MIDI resets */
  int delay;        /* additional delay to apply before playing a file */
  char *sbnk;       /* optional sound bank to use (IBK file or so) */
};

struct clioptions params;
unsigned char action = ACTION_NONE;
struct trackinfodata *trackinfo;
struct midi_event_t *eventscache;


long trackpos;
unsigned long midiplaybackstart;
struct midi_event_t *curevent;
unsigned char *sysexbuff;
  unsigned long nexteventtime;

static int hexchar2int(char c) {
  if ((c >= '0') && (c <= '9')) return(c - '0');
  if ((c >= 'a') && (c <= 'f')) return(10 + c - 'a');
  if ((c >= 'A') && (c <= 'F')) return(10 + c - 'A');
  return(-1);
}


/* converts a hex string to unsigned int. stops at first null terminator or
 * space. returns zero on error. */
static unsigned int hexstr2uint(char *hexstr) {
  unsigned int v = 0;
  while ((*hexstr != 0) && (*hexstr != ' ')) {
    int c;
    c = hexchar2int(*hexstr);
    if (c < 0) return(0);
    v <<= 4;
    v |= c;
    hexstr++;
  }
  return(v);
}


/* high resolution sleeping routine, waits n microseconds */
static void udelay(unsigned long us) {
  unsigned long t1, t2;
  timer_read(&t1);
  for (;;) {
    timer_read(&t2);
    if (t2 < t1) { /* detect timer wraparound */
      break;
    } else if (t2 - t1 >= us) {
      break;
    }
  }
}


static char *devtoname(enum outdev_types device, int devicesubtype) {
  switch (device) {
    case DEV_NONE:   return("NONE");
    case DEV_MPU401: return("MPU");
    case DEV_AWE:    return("AWE");
    case DEV_OPL:    return("OPL");
    case DEV_OPL2:   return("OPL2");
    case DEV_OPL3:   return("OPL3");
    case DEV_RS232:
      if (devicesubtype == 1) return("COM1");
      if (devicesubtype == 2) return("COM2");
      if (devicesubtype == 3) return("COM3");
      if (devicesubtype == 4) return("COM4");
      return("COM");
    case DEV_SBMIDI: return("SB");
    default:         return("UNK");
  }
}


/* analyzes a 16 bytes file header and guess the file format */
static enum fileformats header2fileformat(unsigned char *hdr) {
  /* Classic MIDI */
  if ((hdr[0] == 'M') && (hdr[1] == 'T') && (hdr[2] == 'h') && (hdr[3] == 'd')) {
    return(FORMAT_MIDI);
  }
  /* RMID inside a RIFF container */
  if ((hdr[0] == 'R') && (hdr[1] == 'I') && (hdr[2] == 'F') && (hdr[3] == 'F')
   && (hdr[8] == 'R') && (hdr[9] == 'M') && (hdr[10] == 'I') && (hdr[11] == 'D')) {
    return(FORMAT_RMID);
  }
  /* MUS (as used in Doom, from Id Software) */
  if ((hdr[0] == 'M') && (hdr[1] == 'U') && (hdr[2] == 'S') && (hdr[3] == 0x1A)) {
    return(FORMAT_MUS);
  }
  /* else I don't know */
  return(FORMAT_UNKNOWN);
}

/* interpret a single config argument, returns NULL on succes, or a pointer to
 * an error string otherwise */
static char *feedarg(char *arg, struct clioptions *params, int fileallowed) {
  if (strucmp(arg, "/noxms") == 0) {
    params->memmode = MEM_MALLOC;
  } else if (strucmp(arg, "/xmsdelay") == 0) {
    params->xmsdelay = 1;
  } else if (strucmp(arg, "/dontstop") == 0) {
    params->dontstop = 1;
  } else if (strucmp(arg, "/random") == 0) {
    params->random = 1;
  } else if (strucmp(arg, "/nosound") == 0) {
    params->device = DEV_NONE;
    params->devport = 0;
#ifdef SBAWE
  } else if (strucmp(arg, "/awe") == 0) {
    params->device = DEV_AWE;
    params->devport = params->port_awe;
    /* if AWE port not found in BLASTER, use the default 0x620 */
    if (params->devport == 0) params->devport = 0x620;
  } else if (stringstartswith(arg, "/awe=") == 0) {
    params->device = DEV_AWE;
    params->devport = hexstr2uint(arg + 5);
    if (params->devport < 1) return("Invalid AWE port provided. Example: /awe=620");
#endif
  } else if (strucmp(arg, "/mpu") == 0) {
    params->device = DEV_MPU401;
    params->devport = params->port_mpu;
    /* if MPU port not found in BLASTER, use the default 0x330 */
    if (params->devport == 0) params->devport = 0x330;
#ifdef OPL
  } else if (strucmp(arg, "/opl") == 0) {
    params->device = DEV_OPL;
    params->devport = 0x388;
  } else if (stringstartswith(arg, "/opl=") == 0) {
    params->device = DEV_OPL;
    params->devport = hexstr2uint(arg + 5);
    if (params->devport < 1) return("Invalid OPL port provided. Example: /opl=388");
#endif
  } else if (stringstartswith(arg, "/sbnk=") == 0) {
    if (params->sbnk != NULL) free(params->sbnk); /* drop last sbnk if already present, so a CLI sbnk would take precedence over a config-file sbnk */
    params->sbnk = strdup(arg + 6);
  } else if (stringstartswith(arg, "/mpu=") == 0) {
    params->device = DEV_MPU401;
    params->devport = hexstr2uint(arg + 5);
    if (params->devport < 1) return("Invalid MPU port provided. Example: /mpu=330");
  } else if (stringstartswith(arg, "/com=") == 0) {
    params->device = DEV_RS232;
    params->devport = hexstr2uint(arg + 5);
    if (params->devport < 10) return("Invalid COM port provided. Example: /com=3f8");
  } else if (stringstartswith(arg, "/com") == 0) { /* must be compared AFTER "/com=" */
    params->device = DEV_RS232;
    params->devicesubtype = arg[4] - '0';
    if ((params->devicesubtype < 1) || (params->devicesubtype > 4)) return("Invalid COM port provided. Example: /com1");
    params->devport = rs232_getport(params->devicesubtype);
    if (params->devport < 1) return("Failed to autodetect the I/O address of this COM port. Try using the /com=XXX option.");
  } else if (strucmp(arg, "/sbmidi") == 0) {
    params->device = DEV_SBMIDI;
    params->devport = params->port_sb;
    /* if SB port not found in BLASTER, use the default 0x220 */
    if (params->devport == 0) params->devport = 0x220;
  } else if (stringstartswith(arg, "/sbmidi=") == 0) {
    params->device = DEV_SBMIDI;
    params->devport = hexstr2uint(arg + 8);
    if (params->devport < 1) return("Invalid SBMIDI port provided. Example: /sbmidi=220");
  } else if (stringstartswith(arg, "/syx=") == 0) {
    params->syxrst = strdup(arg + 5);
  } else if (stringstartswith(arg, "/delay=") == 0) {
    params->delay = atoi(arg + 7);
    if ((params->delay < 1) || (params->delay > 9000)) {
      return("Invalid delay value: must be in the range 1..9000");
    }
  } else if ((strucmp(arg, "/?") == 0) || (strucmp(arg, "/h") == 0) || (strucmp(arg, "/help") == 0)) {
    return("");
  } 
  else if ((fileallowed != 0) && (arg[0] != '/') && (params->midifile == NULL)) 
  {
	params->midifile = arg;
  } 
  else 
  {
    return("Unknown option.");
  }
  return(NULL);
}

/* parse command line params and fills the params struct accordingly. returns
   NULL on sucess, or a pointer to an error string otherwise. */
static char *parseargv(int argc, char **argv, struct clioptions *params) {
  int i;
  /* set default memory mode */
  params->memmode = MEM_XMS;
  /* if no params at all, don't waste time */
  if (argc == 0) return("");
  /* now read params */
  for (i = 1; i < argc; i++) {
    char *r;
    r = feedarg(argv[i], params, 1);
    if (r != NULL) return(r);
  }

  /* all good */
  return(NULL);
}

/* check the event cache for a given event. to reset the cache, issue a single
 * call with trackpos < 0. */
static struct midi_event_t *getnexteventfromcache(struct midi_event_t *eventscache, long trackpos, int xmsdelay) {
  static unsigned int itemsincache = 0;
  static unsigned int curcachepos = 0;
  struct midi_event_t *res = NULL;
  long nextevent;
  /* if trackpos < 0 then this is only about flushing cache */
  if (trackpos < 0) {
    memset(eventscache, 0, sizeof(*eventscache));
    itemsincache = 0;
    curcachepos = 0;
    return(NULL);
  }
  /* if we have available cache */
  if (itemsincache > 0) {
      curcachepos++;
      curcachepos &= EVENTSCACHEMASK;
      itemsincache--;
      res = &eventscache[curcachepos];
      /* if we have some free time, refill the cache proactively */
      if (res->deltatime > 0) {
        int nextslot, pullres;
        /* sleep 2ms after a MIDI OUT write, and before accessing XMS.
           This is especially important for SoundBlaster "AWE" cards with the
           AWEUTIL TSR midi emulation enabled, without this AWEUTIL crashes. */
        if (xmsdelay != 0) udelay(2000);
        nextslot = curcachepos + itemsincache;
        nextevent = eventscache[nextslot & EVENTSCACHEMASK].next;
        while ((itemsincache < EVENTSCACHESIZE - 1) && (nextevent >= 0)) {
          nextslot++;
          nextslot &= EVENTSCACHEMASK;
          pullres = mem_pull(nextevent, &eventscache[nextslot], sizeof(struct midi_event_t));
          if (pullres != 0) {
            return(NULL);
          }
          nextevent = eventscache[nextslot].next;
          itemsincache++;
        }
      }
    } else { /* need to refill the cache NOW */
      int refillcount, pullres;
      /* sleep 2ms after a MIDI OUT write, and before accessing XMS.
         this is especially important for SoundBlaster "AWE" cards with the
         AWEUTIL TSR midi emulation enabled, without this AWEUTIL crashes. */
      if (xmsdelay != 0) udelay(2000);
      nextevent = trackpos;
      curcachepos = 0;
      for (refillcount = 0; refillcount < EVENTSCACHESIZE; refillcount++) {
        pullres = mem_pull(nextevent, &eventscache[refillcount], sizeof(struct midi_event_t));
        if (pullres != 0) {
          return(NULL);
        }
        nextevent = eventscache[refillcount].next;
        itemsincache++;
        if (nextevent < 0) break;
      }
      itemsincache--;
      res = eventscache;
  }
  return(res);
}


/* reads the BLASTER variable for best guessing of current hardware and port.
 * If nothing found, fallbacks to MPU and 0x330 */
static void preload_outdev(struct clioptions *params) {
  char *blaster;

  params->port_mpu = 0;
  params->port_awe = 0;
  params->port_opl = 0;
  params->port_sb  = 0;

  /* check if a blaster variable is present */
  blaster = getenv("BLASTER");
  /* if so, read it looking for 'P' and 'E' parameters */
  if (blaster != NULL) {
    char *blasterptr[16];
    int blastercount = 0;

    /* read the variable in a first pass to collect all starting points */
    if (*blaster != 0) {
      blasterptr[blastercount++] = blaster++;
    }
    for (;;) {
      if (*blaster == ' ') {
        blasterptr[blastercount++] = ++blaster;
      } else if ((*blaster == 0) || (blastercount >= 16)) {
        break;
      } else {
        blaster++;
      }
    }

    while (blastercount-- > 0) {
      unsigned short p;
      unsigned short *portptr;
      blaster = blasterptr[blastercount];
      /* have we found an interesting param? */
      if ((*blaster != 'P') && (*blaster != 'E') && (*blaster != 'A')) continue;
      if (*blaster == 'E') {
        portptr = &(params->port_awe);
      } else if (*blaster == 'P') {
        portptr = &(params->port_mpu);
      } else {
        portptr = &(params->port_sb);
      }
      /* read the param value into a variable */
      p = hexstr2uint(blaster + 1);
      /* if what we have read looks sane, keep it */
      if (p > 0) *portptr = p;
    }
  }

  /* look at what we got, and choose in order of preference */

  /* set NONE, just so we have anything set */
  params->device = DEV_NONE;
  params->devport = 0;

  /* use OPL on port 0x388, if OPL output is compiled in */
#ifdef OPL
  params->device = DEV_OPL;
  params->devport = 0x388;
#endif

  /* never try using SBMIDI: it's unlikely anything's connected to it anyway */

  /* is there an MPU? if so, take it */
  if (params->port_mpu > 0) {
    params->device = DEV_MPU401;
    params->devport = params->port_mpu;
  }

  /* AWE is the most desirable, if present (and compiled in) */
#ifdef SBAWE
  if (params->port_awe > 0) { /* AWE is the most desirable, if present */
    params->device = DEV_AWE;
    params->devport = params->port_awe;
  }
#endif
}


static unsigned char loadfile_midi(FILE *fd, struct clioptions *params, struct trackinfodata *trackinfo, long *trackpos) {
  struct midi_chunkmap_t *chunkmap;
  unsigned int miditracks;
//GB
  unsigned int i;
  long newtrack;

  *trackpos = -1;

  if (fd == NULL) return(ACTION_ERR_HARD);

  chunkmap = malloc(sizeof(struct midi_chunkmap_t) * MAXTRACKS);
  if (chunkmap == NULL) {
    return(ACTION_ERR_HARD);
  }

  if (midi_readhdr(fd, &(trackinfo->midiformat), &miditracks, &(trackinfo->miditimeunitdiv), chunkmap, MAXTRACKS) != 0) {
    free(chunkmap);
    return(ACTION_ERR_SOFT);
  }

  if ((trackinfo->midiformat != 0) && (trackinfo->midiformat != 1)) {
    free(chunkmap);
    return(ACTION_ERR_SOFT);
  }

  if (miditracks > MAXTRACKS) {
    free(chunkmap);
    return(ACTION_ERR_SOFT);
  }

  for (i = 0; i < miditracks; i++) 
  {
    unsigned long tracklen;
    /* is it really a track we got here? */
    if (strcmp(chunkmap[i].id, "MTrk") != 0) {
      free(chunkmap);
      return(ACTION_ERR_SOFT);
    }

    fseek(fd, chunkmap[i].offset, SEEK_SET);
	newtrack = midi_track2events(fd, "", 1, NULL, 0, NULL, 0, &(trackinfo->channelsusage), NULL, &tracklen);

    /* look for error conditions */
    if (newtrack == MIDI_OUTOFMEM) {
      free(chunkmap);
      return(ACTION_ERR_SOFT);
    } else if (newtrack == MIDI_TRACKERROR) {
      free(chunkmap);
      return(ACTION_ERR_SOFT);
    }

    /* merge the track now */
    if (newtrack >= 0) {
      *trackpos = midi_mergetrack(*trackpos, newtrack, &(trackinfo->totlen), trackinfo->miditimeunitdiv);
    }
  }
  /* free memory */
  free(chunkmap);

  return(ACTION_NONE);
}


static unsigned char loadfile(struct clioptions *params, struct trackinfodata *trackinfo, long *trackpos) {
  FILE *fd;
  unsigned char hdr[16];
  unsigned char res;

  /* flush all MIDI events from memory for new events to have where to load */
  mem_clear();

  /* (try to) open the music file */
  fd = fopen(params->midifile, "rb");
  if (fd == NULL) {
    //ui_puterrmsg(params->midifile, "Error: Failed to open the file");
    return(ACTION_ERR_SOFT);
  }

  /* read first few bytes of the file to detect its format, and rewind */
  if (fread(hdr, 1, 16, fd) != 16) {
    fclose(fd);
    return(ACTION_ERR_SOFT);
  }
  rewind(fd);

  /* analyze the header to guess the format of the file */
  trackinfo->fileformat = header2fileformat(hdr);

  /* load file if format recognized */
  switch (trackinfo->fileformat) {
    case FORMAT_MIDI:
    case FORMAT_RMID:
      res = loadfile_midi(fd, params, trackinfo, trackpos);
      break;
    case FORMAT_MUS:
      *trackpos = mus_load(fd, &(trackinfo->totlen), &(trackinfo->miditimeunitdiv), &(trackinfo->channelsusage));
      if (*trackpos == MUS_OUTOFMEM) { /* detect out of memory */
        res = ACTION_ERR_SOFT;
      } else if (*trackpos < 0) { /* detect any other problems */
        res = ACTION_ERR_SOFT;
      } else { /* all right, now we're talking */
        res = ACTION_NONE;
      }
      break;
    default:
      res = ACTION_ERR_SOFT;
      //ui_puterrmsg(params->midifile, "Error: Unknown file format");
      break;
  }
  fclose(fd);

  return(res);
}


/*static void pauseplay(unsigned long *starttime, unsigned long *nexteventtime, struct trackinfodata *trackinfo) {
  unsigned long beforepause, afterpause, deltaremainder;
  int i;

  timer_read(&beforepause);
  deltaremainder = *nexteventtime - beforepause;

  for (i = 0; i < 128; i++) {
    if (trackinfo->notestates[i] != 0) {
      int c;
      for (c = 0; c < 16; c++) {
        if (trackinfo->notestates[i] & (1 << c)) {
          dev_noteoff(c, i);
        }
      }
    }
  }
  // Resume
  timer_read(&afterpause);
  *nexteventtime = afterpause + deltaremainder;
  *starttime += (afterpause - beforepause);
}*/


void _interrupt PlayMusic()
{
	const unsigned char volume = 100; /* volume is static because it needs to be retained between songs */
	unsigned char i;
	unsigned char exitaction;

	while (1) 
	{
		curevent = getnexteventfromcache(eventscache, trackpos, params.xmsdelay);
		if (curevent == NULL) 
		{
			exitaction = ACTION_ERR_HARD;
			break;
		}

		dev_tick();

		if (curevent->deltatime > 0)
		{
			nexteventtime += (curevent->deltatime * trackinfo->tempo / trackinfo->miditimeunitdiv);
			while (exitaction == ACTION_NONE) {
				unsigned long t;
				timer_read(&t);
				if (t >= nexteventtime) break;
				if (nexteventtime - t > ULONG_MAX / 2) break;
			}
			if (exitaction != ACTION_NONE) break;
		}

		switch (curevent->type) 
		{
		  case EVENT_NOTEON:
				dev_noteon(curevent->data.note.chan, curevent->data.note.note, (volume * curevent->data.note.velocity) / 100);
				trackinfo->notestates[curevent->data.note.note] |= (1 << curevent->data.note.chan);
			break;
		  case EVENT_NOTEOFF:
				dev_noteoff(curevent->data.note.chan, curevent->data.note.note);
				trackinfo->notestates[curevent->data.note.note] &= (0xFFFF ^ (1 << curevent->data.note.chan));
			break;
		  case EVENT_TEMPO:
				trackinfo->tempo = curevent->data.tempoval;
			break;
		  case EVENT_PROGCHAN:
				trackinfo->chanprogs[curevent->data.prog.chan] = curevent->data.prog.prog;
				dev_setprog(curevent->data.prog.chan, curevent->data.prog.prog);
			break;
		  case EVENT_PITCH:
				dev_pitchwheel(curevent->data.pitch.chan, curevent->data.pitch.wheel);
			break;
		  case EVENT_CONTROL:
				dev_controller(curevent->data.control.chan, curevent->data.control.id, curevent->data.control.val);
			break;
		  case EVENT_CHANPRESSURE:
				dev_chanpressure(curevent->data.chanpressure.chan, curevent->data.chanpressure.pressure);
			break;
		  case EVENT_KEYPRESSURE:
				dev_keypressure(curevent->data.keypressure.chan, curevent->data.keypressure.note, curevent->data.keypressure.pressure);
			break;
		  case EVENT_SYSEX:
			{
				unsigned short sysexlen;
				mem_pull(curevent->data.sysex.sysexptr, &sysexlen, 2);
				i = sysexlen;
				if ((i & 1) != 0) i++;
				sysexbuff = malloc(i);
				if (sysexbuff != NULL) {
				  mem_pull(curevent->data.sysex.sysexptr, sysexbuff, i + 2);
				  dev_sysex(sysexbuff[2] & 0x0F, sysexbuff + 2, sysexlen);
				  free(sysexbuff);
				}
				break;
			}
			default:
			break;
		}

		if (trackpos < 0) break;
		trackpos = curevent->next;
  }


}

/* plays a file. returns 0 on success, non-zero if the program must exit */
static unsigned char playfile() 
{
	const unsigned char volume = 100; /* volume is static because it needs to be retained between songs */
	unsigned char i , exitaction;

	/* init trackinfo & cache data */
	getnexteventfromcache(eventscache, -1, 0);

	/* reset the timer, to make sure it doesn't wrap around during playback */
	timer_reset();
	timer_read(&nexteventtime); /* save current time, to schedule when the song shall start */

	/* load piano to all channels (even real MIDI synths do not always reset
	* those properly) - this could just as well happen during dev_clear(), but
	* there are users that happen to use DOSMid to init their MPU hardware,
	* and resetting patches *after* the midi file played would break that
	* usage for them */
	for (i = 0; i < 16; i++) dev_setprog(i, 0);

	/* if a SYX init file is provided, feed it to the MIDI synth now */
	if (params.syxrst != NULL) 
	{
		int syxlen;
		FILE *syxfd;
		/* open the syx file */
		syxfd = fopen(params.syxrst, "rb");
		if (syxfd == NULL) 
		{
			puts("Error: Failed to open the SYX file");
			return(ACTION_ERR_HARD);
		}
		/* alloc a temporary buffer to hold sysex messages */
		sysexbuff = malloc(8192);
		if (sysexbuff == NULL) 
		{
			fclose(syxfd);
			puts("Error: Out of memory");
			return(ACTION_ERR_HARD);
		}
		/* read SYSEX messages until EOF */
		for (;;) 
		{
			syxlen = syx_fetchnext(syxfd, sysexbuff, 8192);
			if (syxlen == 0) break; /* EOF */
			if (syxlen < 0) { /* error condition */
				fclose(syxfd); /* close the syx file */
				free(sysexbuff);
				puts("Error: Failed to process the SYX file");
				return(ACTION_ERR_HARD);
			}
			/* send the SYSEX message to the MIDI device, and wait a short moment
			* between messages so the device has time to digest them */
			dev_sysex(sysexbuff[0] & 0x0F, sysexbuff, syxlen);
			udelay(40000);	/* 40ms should be enough time for the MPU interface   */
						/* note that MT32 rev00 are *very* sensitive to this! */
			if (sysexbuff[0] == 0x7F) {  /* the 'all parameters reset' sysex takes */
				udelay(250000lu);          /* a very long time to be processed on    */
			}                            /* MT32 rev00 gears.                      */
		}
		
		fclose(syxfd); /* close the syx file */
		free(sysexbuff);
  }

  nexteventtime += params.delay * 1000; /* add the extra custom delay */
  exitaction = loadfile(&params, trackinfo, &trackpos);
  if (exitaction != ACTION_NONE) return(exitaction);
  /* draw the gui with track's data */
  for (;;) 
  {
	timer_read(&midiplaybackstart); /* save start time so we can compute elapsed time later */
    if (midiplaybackstart >= nexteventtime) break; /* wait until the scheduled start time is met */
    if (midiplaybackstart + 10000000lu < nexteventtime) break; /* do not freeze on timer wraparound */
  }
  nexteventtime = midiplaybackstart;

  return(exitaction);
}


unsigned char LoadMusic(const unsigned char* filename, int argc, char **argv)
{
	char *errstr;
	/* make sure to init all CLI params to empty values */
	memset(&params, 0, sizeof(params));
	/* preload the mpu port to be used (might be forced later via **argv) */
	preload_outdev(&params);
	errstr = parseargv(argc, argv, &params);
	if (errstr != NULL) 
	{
		return(1);
	}
	strcpy(params.midifile, filename);
	params.devname = devtoname(params.device, params.devicesubtype);
	
	/* allocate the work memory */
	if (mem_init(params.memmode) == 0) 
	{
		if (params.memmode == MEM_XMS) 
		{
		puts("ERROR: Memory init failed! No XMS maybe? Try /noxms.");
		} 	
		else 
		{
			puts("ERROR: Memory init failed!");
		}
		return(1);
	}

	/* allocate memory segments to be used later */
	trackinfo = malloc(sizeof(struct trackinfodata));
	eventscache = malloc(sizeof(struct midi_event_t) * EVENTSCACHESIZE);
	if ((trackinfo == NULL) || (eventscache == NULL)) 
	{
		puts("ERROR: Out of memory! Free some conventional memory.");
		mem_close();
		if (trackinfo != NULL) free(trackinfo);
		if (eventscache != NULL) free(eventscache);
		return(1);
	}
	/* init random numbers */
	srand((unsigned int)time(NULL));

	/* initialize the high resolution timer */
	timer_init();
	
	errstr = dev_init(params.device, params.devport, params.sbnk);
	if (errstr != NULL) 
	{
		return 1;
	}
	/* refresh outdev and its name (might have been changed due to OPL autodetection) */
	params.device = dev_getcurdev();
	params.devname = devtoname(params.device, params.devicesubtype);

	playfile();
}

/*void PlayMusic()
{
	//playfile(&params, trackinfo, eventscache);
	Sync_Music();
}*/

void Exit_DOSMID()
{
	dev_close();
	/* unload XMS memory */
	mem_close();
	/* free allocated heap memory */
	free(trackinfo);
	free(eventscache);
	/* free the allocated strings, if any */
	if (params.sbnk != NULL) free(params.sbnk);
	if (params.syxrst != NULL) free(params.syxrst);
}

void Clear_Mus()
{
	unsigned char c, i;
	for (i = 0; i < 128; i++) 
	{
		if (trackinfo->notestates[i] != 0) 
		{
			int c;
			for (c = 0; c < 16; c++) 
			{
				if (trackinfo->notestates[i] & (1 << c)) 
				{
					dev_noteoff(c, i);
				}
			}
		}
	}
	
	dev_clear();
}

