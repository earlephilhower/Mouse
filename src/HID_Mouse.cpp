/*
  Mouse.cpp

  Copyright (c) 2015, Arduino LLC
  Original code (pre-library): Copyright (c) 2011, Peter Barrett

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "HID_Mouse.h"
#include <USB.h>

#include "tusb.h"
#include "class/hid/hid_device.h"


//================================================================================
//================================================================================
//	Mouse

/* This function is for limiting the input value for x and y
 * axis to -127 <= x/y <= 127 since this is the allowed value
 * range for a USB HID device.
 */
int HID_Mouse::limit_xy(int const a)
{
    if (_absolute) {
        if (a < -32767) {
            return -32767;
        } else if (a > 32767) {
            return 32767;
        }
    } else {
        if (a < -127) {
            return -127;
        } else if (a > 127) {
            return 127;
        }
    }
    return a;
}

HID_Mouse::HID_Mouse(bool absolute) : _buttons(0), _absolute(absolute)
{
    _running = false;
}

void HID_Mouse::begin(void) 
{
    _running = true;
}

void HID_Mouse::end(void) 
{
    _running = false;
}

void HID_Mouse::click(uint8_t b)
{
    if (!_running) {
        return;
    }
    _buttons = b;
    move(0,0,0);
    delay(10);
    _buttons = 0;
    move(0,0,0);
    delay(10);
}

void HID_Mouse::buttons(uint8_t b)
{
    if (!_running) {
        return;
    }
    if (b != _buttons) {
        _buttons = b;
        move(0,0,0);
    }
}

void HID_Mouse::press(uint8_t b) 
{
    if (!_running) {
        return;
    }
    buttons(_buttons | b);
}

void HID_Mouse::release(uint8_t b)
{
    if (!_running) {
        return;
    }
    buttons(_buttons & ~b);
}

bool HID_Mouse::isPressed(uint8_t b) {
    if ((b & _buttons) > 0) {
        return true;
    } else {
        return false;
    }
}
