#ifndef _MCGA_GRAPH_
#define _MCGA_GRAPH_

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define MISC_OUTPUT         0x03c2    /* VGA misc. output register */
#define SC_INDEX            0x03c4    /* VGA sequence controller */
#define SC_DATA             0x03c5
#define PALETTE_INDEX       0x03c8    /* VGA digital-to-analog converter */
#define PALETTE_DATA        0x03c9
#define CRTC_INDEX          0x03d4    /* VGA CRT controller */

#define MAP_MASK            0x02      /* Sequence controller registers */
#define MEMORY_MODE         0x04

#define H_TOTAL             0x00      /* CRT controller registers */
#define H_DISPLAY_END       0x01
#define H_BLANK_START       0x02
#define H_BLANK_END         0x03
#define H_RETRACE_START     0x04
#define H_RETRACE_END       0x05
#define V_TOTAL             0x06
#define OVERFLOW            0x07
#define MAX_SCAN_LINE       0x09
#define V_RETRACE_START     0x10
#define V_RETRACE_END       0x11
#define V_DISPLAY_END       0x12
#define OFFSET              0x13
#define UNDERLINE_LOCATION  0x14
#define V_BLANK_START       0x15
#define V_BLANK_END         0x16
#define MODE_CONTROL        0x17

#define INPUT_STATUS      	0x03da
#define VRETRACE 			0x08

#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#endif
