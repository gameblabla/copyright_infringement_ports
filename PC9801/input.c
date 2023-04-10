/* input.c, Combined joystick and keyboar user input routines for the pc98Launcher.
 Copyright (C) 2020  John Snowdon
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <dos.h>
#include <i86.h>
#include "input.h"
#include "keys.h"

uint8_t inputs[18] = {0};

static union REGS out, in;

#define input_func_bool(var) { var = 1; } else { var = 0; }

#define input_update_bool(var, cur_sensed) \
	if(cur_sensed) input_func_bool(var)

static void keygroup_sense(char group_id) {
	in.h.ah = 0x04;
	in.h.al = group_id;
	int86(0x18, &in, &out);
}

void Get_Input() 
{
	
	keygroup_sense(0);
	input_update_bool(inputs[ESC_KEY], (out.h.ah & K0_ESC));
	input_update_bool(inputs[KEY1_GAME], (out.h.ah & K0_1));
	input_update_bool(inputs[KEY2_GAME], (out.h.ah & K0_2));
	input_update_bool(inputs[KEY3_GAME], (out.h.ah & K0_3));
	
	keygroup_sense(1);
	input_update_bool(inputs[KEY0_GAME], (out.h.ah & K1_0));
	
	keygroup_sense(2);
	input_update_bool(inputs[KEY_W], (out.h.ah & K2_W));
	

	keygroup_sense(3);
	input_update_bool(inputs[CONFIRM_KEY], (out.h.ah & K3_RETURN));
	input_update_bool(inputs[KEY_S], (out.h.ah & K3_S));
	input_update_bool(inputs[KEY_A], (out.h.ah & K3_A));
	input_update_bool(inputs[KEY_D], (out.h.ah & K3_D));


	keygroup_sense(6);
	input_update_bool(inputs[SPACE_KEY], (out.h.ah & K6_SPACE));
	
	keygroup_sense(7);
	input_update_bool(inputs[DOWN_KEY], (out.h.ah & K7_ARROW_DOWN));
	input_update_bool(inputs[UP_KEY], (out.h.ah & K7_ARROW_UP));
	input_update_bool(inputs[LEFT_KEY], (out.h.ah & K7_ARROW_LEFT));
	input_update_bool(inputs[RIGHT_KEY], (out.h.ah & K7_ARROW_RIGT));
	
	keygroup_sense(9);
	input_update_bool(inputs[NUMPAD0_KEY], (out.h.ah & K9_NUM_0));
	input_update_bool(inputs[NUMPAD1_KEY], (out.h.ah & K9_NUM_1));
	input_update_bool(inputs[NUMPAD2_KEY], (out.h.ah & K9_NUM_2));
}
