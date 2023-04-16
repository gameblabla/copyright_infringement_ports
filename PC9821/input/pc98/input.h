/* input.h, User input routines for the pc98Launcher.
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
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define CONFIRM_KEY 18
#define UP_KEY 1
#define LEFT_KEY 2
#define RIGHT_KEY 3
#define DOWN_KEY 4
#define SPACE_KEY 5
#define ESC_KEY 6

#define KEY0_GAME 7
#define KEY1_GAME 8
#define KEY2_GAME 9
#define KEY3_GAME 10

#define NUMPAD0_KEY 11
#define NUMPAD1_KEY 12
#define NUMPAD2_KEY 13

#define KEY_W 14
#define KEY_S 15
#define KEY_A 16
#define KEY_D 17

// Function prototypes
extern uint8_t inputs[19];
extern void Get_Input();

#endif
