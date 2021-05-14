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

#include "Mouse.h"

#include "tusb.h"
#include "class/hid/hid_device.h"
#include "class/audio/audio.h"
#include "class/midi/midi.h"
#include "pico/time.h"
#include "pico/binary_info.h"
#include "pico/bootrom.h"
#include "hardware/irq.h"
#include "pico/mutex.h"
#include "hardware/watchdog.h"
#include "pico/unique_id.h"

extern mutex_t __usb_mutex;

// Weak function override to add our descriptor to the TinyUSB list
void __USBInstallMouse() { /* noop */ }

extern int __GetMouseReportID(); // From USB.cpp, because the report ID will change if the keyboard is in use

//================================================================================
//================================================================================
//	Mouse

/* This function is for limiting the input value for x and y
 * axis to -127 <= x/y <= 127 since this is the allowed value
 * range for a USB HID device.
 */
signed char limit_xy(int const xy)
{
  if     (xy < -127) return -127;
  else if(xy >  127) return 127;
  else               return xy;
}

Mouse_::Mouse_(void) : _buttons(0)
{
    /* noop */
}

void Mouse_::begin(void) 
{
}

void Mouse_::end(void) 
{
}

void Mouse_::click(uint8_t b)
{
	_buttons = b;
	move(0,0,0);
        delay(10);
	_buttons = 0;
	move(0,0,0);
        delay(10);
}

void Mouse_::move(int x, int y, signed char wheel)
{
    CoreMutex m(&__usb_mutex);
    tud_task();
    if (tud_hid_ready()) {
        tud_hid_mouse_report(__GetMouseReportID(), _buttons, limit_xy(x), limit_xy(y), wheel, 0);
    }
    tud_task();
}

void Mouse_::buttons(uint8_t b)
{
	if (b != _buttons)
	{
		_buttons = b;
		move(0,0,0);
	}
}

void Mouse_::press(uint8_t b) 
{
	buttons(_buttons | b);
}

void Mouse_::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool Mouse_::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0) 
		return true;
	return false;
}

Mouse_ Mouse;
