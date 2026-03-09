/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#ifdef SDL_JOYSTICK_N64

#include <libdragon.h>

#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "../SDL_joystick_c.h"

static struct controller_data keys_pressed;

static int numjoysticks = 0;
static int controllers_connected = 0;

static void N64_JoystickDetect(void)
{
	controller_scan();
    controllers_connected = get_controllers_present();

    numjoysticks = 0;
    if (controllers_connected & CONTROLLER_1_INSERTED)
        ++numjoysticks;
    if (controllers_connected & CONTROLLER_2_INSERTED)
        ++numjoysticks;
    if (controllers_connected & CONTROLLER_3_INSERTED)
        ++numjoysticks;
    if (controllers_connected & CONTROLLER_4_INSERTED)
        ++numjoysticks;
}

/* Function to scan the system for joysticks.
 * Joystick 0 should be the system default joystick.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
static int N64_JoystickInit(void)
{
    controller_init();

	N64_JoystickDetect();
    return numjoysticks;
}

static int N64_JoystickGetCount(void)
{
    return numjoysticks;
}

/* Function to get the device-dependent name of a joystick */
static const char *N64_JoystickGetDeviceName(int device_index)
{
    return "Nintendo 64 Controller";
}

static int N64_JoystickGetDevicePlayerIndex(int device_index)
{
    return -1;
}

static void N64_JoystickSetDevicePlayerIndex(int device_index, int player_index)
{
}

/* Function to perform the mapping from device index to the instance id for this index */
static SDL_JoystickID N64_JoystickGetDeviceInstanceID(int device_index)
{
    return device_index;
}

static void N64_JoystickClose(SDL_Joystick *joystick);

/* Function to open a joystick for use.
The joystick to open is specified by the device index.
This should fill the nbuttons and naxes fields of the joystick structure.
It returns 0, or -1 if there is an error.
*/
static int N64_JoystickOpen(SDL_Joystick *joystick, int device_index)
{
    joystick->instance_id = device_index;
    joystick->nbuttons = 10;
    joystick->naxes = 2;
    joystick->nhats = 1;

    switch (device_index)
    {
    case 0:
        joystick->attached = controllers_connected & CONTROLLER_1_INSERTED;
        break;
    case 1:
        joystick->attached = controllers_connected & CONTROLLER_2_INSERTED;
        break;
    case 2:
        joystick->attached = controllers_connected & CONTROLLER_3_INSERTED;
        break;
    default:
        joystick->attached = controllers_connected & CONTROLLER_4_INSERTED;
        break;
    }

    return 0;
}

Sint16 N64_GetAxisFromController(int pad_id, int axis)
{
	const int max_axis_value = 70;
	const float multiplier = (float)SDL_JOYSTICK_AXIS_MAX / max_axis_value;

	int value = 0;
	if (axis == 0) {
		value = (int)keys_pressed.c[pad_id].x;
	} else {
		value = -(int)keys_pressed.c[pad_id].y;
	}
    if (value > -20 && value < 20) {
        value = 0;
    } else if (value > max_axis_value) {
        value = max_axis_value;
    } else if (value < -max_axis_value) {
        value = -max_axis_value;
    }

	return (Sint16)(multiplier * value);
}

#define BT_PRESSED(button) keys_pressed.c[pad_id].button

Uint8 N64_JoystickGetHat(int pad_id)
{
	Uint8 value = SDL_HAT_CENTERED;
	if (BT_PRESSED(left))
		value |= SDL_HAT_LEFT;
	if (BT_PRESSED(right))
		value |= SDL_HAT_RIGHT;
	if (BT_PRESSED(down))
		value |= SDL_HAT_DOWN;
	if (BT_PRESSED(up))
		value |= SDL_HAT_UP;

	return value;
}

/* Function to update the state of a joystick - called as a device poll.
* This function shouldn't update the joystick structure directly,
* but instead should call SDL_PrivateJoystick*() to deliver events
* and update joystick device state.
*/
static void N64_JoystickUpdate(SDL_Joystick *joystick)
{
	controller_scan();
    keys_pressed = get_keys_pressed();
    
    const int pad_id = joystick->instance_id;

    // x
    SDL_PrivateJoystickAxis(joystick, 0, N64_GetAxisFromController(pad_id, 0));
    // y
    SDL_PrivateJoystickAxis(joystick, 1, N64_GetAxisFromController(pad_id, 1));

    // d-pad
    SDL_PrivateJoystickHat(joystick, 0, N64_JoystickGetHat(pad_id));

    // buttons
    SDL_PrivateJoystickButton(joystick, 0, BT_PRESSED(A));
    SDL_PrivateJoystickButton(joystick, 1, BT_PRESSED(B));
    SDL_PrivateJoystickButton(joystick, 2, BT_PRESSED(Z));
    SDL_PrivateJoystickButton(joystick, 3, BT_PRESSED(start));
    SDL_PrivateJoystickButton(joystick, 4, BT_PRESSED(L));
    SDL_PrivateJoystickButton(joystick, 5, BT_PRESSED(R));
    SDL_PrivateJoystickButton(joystick, 6, BT_PRESSED(C_up));
    SDL_PrivateJoystickButton(joystick, 7, BT_PRESSED(C_down));
    SDL_PrivateJoystickButton(joystick, 8, BT_PRESSED(C_left));
    SDL_PrivateJoystickButton(joystick, 9, BT_PRESSED(C_right));
}

#undef BT_PRESSED

/* Function to close a joystick after use */
static void N64_JoystickClose(SDL_Joystick *joystick)
{
}

/* Function to perform any system-specific joystick related cleanup */
static void N64_JoystickQuit(void)
{
}

static SDL_JoystickGUID N64_JoystickGetDeviceGUID( int device_index )
{
    SDL_JoystickGUID guid;
    /* the GUID is just the first 16 chars of the name for now */
    const char *name = N64_JoystickGetDeviceName( device_index );
    SDL_zero( guid );
    SDL_memcpy( &guid, name, SDL_min( sizeof(guid), SDL_strlen( name ) ) );
    return guid;
}

static int N64_JoystickRumble(SDL_Joystick *joystick, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble)
{
    return SDL_Unsupported();
}

static int N64_JoystickRumbleTriggers(SDL_Joystick *joystick, Uint16 left_rumble, Uint16 right_rumble)
{
    return SDL_Unsupported();
}

static SDL_bool
N64_JoystickGetGamepadMapping(int device_index, SDL_GamepadMapping *out)
{
    return SDL_FALSE;
}

static Uint32 N64_JoystickGetCapabilities(SDL_Joystick *joystick)
{
    return 0;
}

static int N64_JoystickSetLED(SDL_Joystick *joystick, Uint8 red, Uint8 green, Uint8 blue)
{
    return SDL_Unsupported();
}


static int N64_JoystickSendEffect(SDL_Joystick *joystick, const void *data, int size)
{
    return SDL_Unsupported();
}

static int N64_JoystickSetSensorsEnabled(SDL_Joystick *joystick, SDL_bool enabled)
{
    return SDL_Unsupported();
}

SDL_JoystickDriver SDL_N64_JoystickDriver =
{
    N64_JoystickInit,
    N64_JoystickGetCount,
    N64_JoystickDetect,
    N64_JoystickGetDeviceName,
    N64_JoystickGetDevicePlayerIndex,
    N64_JoystickSetDevicePlayerIndex,
    N64_JoystickGetDeviceGUID,
    N64_JoystickGetDeviceInstanceID,
    N64_JoystickOpen,
    N64_JoystickRumble,
    N64_JoystickRumbleTriggers,
    N64_JoystickGetCapabilities,
    N64_JoystickSetLED,
    N64_JoystickSendEffect,
    N64_JoystickSetSensorsEnabled,
    N64_JoystickUpdate,
    N64_JoystickClose,
    N64_JoystickQuit,
    N64_JoystickGetGamepadMapping
};

#endif /* SDL_JOYSTICK_N64 */

/* vi: set ts=4 sw=4 expandtab: */
