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

#if SDL_VIDEO_DRIVER_N64

#include "SDL_n64events_c.h"

#include "../../events/SDL_events_c.h"

void
N64_PumpEvents(_THIS)
{
    joypad_poll();

    for (int i = 0; i < 4; i++) {
        joypad_port_t port = (joypad_port_t)i;
        if (!joypad_is_connected(port)) continue;

        joypad_buttons_t pressed = joypad_get_buttons_pressed(port);
        joypad_buttons_t released = joypad_get_buttons_released(port);

        // Example Mapping: N64 A Button -> SDL Space or Gamepad A
        if (pressed.a) SDL_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_SPACE);
        if (released.a) SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_SPACE);
        
        // Analog Stick Mapping
        joypad_inputs_t inputs = joypad_get_inputs(port);
        // Map inputs.stick_x and inputs.stick_y (-80 to 80) to SDL Axis events
    }
}

#endif /* SDL_VIDEO_DRIVER_N64 */

/* vi: set ts=4 sw=4 expandtab: */
