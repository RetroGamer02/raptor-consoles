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

#ifdef SDL_TIMER_N64

#include <libdragon.h>

static SDL_bool ticks_started = SDL_FALSE;

void
SDL_TicksInit(void)
{
    // timer_init() is now called internally by Libdragon systems, 
    // but calling it again is safe.
    timer_init();
}

void
SDL_TicksQuit(void)
{
    fprintf(stderr, "SDL_TicksQuit\n");
    ticks_started = SDL_FALSE;

    timer_close();
}

Uint64
SDL_GetTicks64(void)
{
    // Convert CPU ticks to milliseconds
    return TICKS_TO_MS(get_ticks());
}

Uint64
SDL_GetPerformanceCounter(void)
{
    return get_ticks();
}

Uint64
SDL_GetPerformanceFrequency(void)
{
    return TICKS_PER_SECOND;
}

void SDL_Delay(Uint32 ms)
{
    // Libdragon's wait_ms uses the internal timer accurately
    wait_ms(ms);
}

#endif /* SDL_TIMER_N64 */

/* vim: ts=4 sw=4
 */
