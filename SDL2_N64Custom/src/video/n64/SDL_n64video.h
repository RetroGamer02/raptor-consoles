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

#ifndef SDL_n64video_h_
#define SDL_n64video_h_

#include "../../SDL_internal.h"
#include "../SDL_sysvideo.h"

#include <libdragon.h>

typedef struct SDL_VideoData
{
    SDL_bool egl_initialized;
    uint32_t egl_refcount;
} SDL_VideoData;


typedef struct SDL_DisplayData
{
} SDL_DisplayData;


typedef struct SDL_WindowData
{
} SDL_WindowData;

/****************************************************************************/
/* SDL_VideoDevice functions declaration                                    */
/****************************************************************************/

/* Display and window functions */
int N64_VideoInit(_THIS);
void N64_VideoQuit(_THIS);
void N64_GetDisplayModes(_THIS, SDL_VideoDisplay * display);
int N64_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode);
int N64_CreateWindow(_THIS, SDL_Window * window);
int N64_CreateWindowFrom(_THIS, SDL_Window * window, const void *data);
void N64_SetWindowTitle(_THIS, SDL_Window * window);
void N64_SetWindowIcon(_THIS, SDL_Window * window, SDL_Surface * icon);
void N64_SetWindowPosition(_THIS, SDL_Window * window);
void N64_SetWindowSize(_THIS, SDL_Window * window);
void N64_ShowWindow(_THIS, SDL_Window * window);
void N64_HideWindow(_THIS, SDL_Window * window);
void N64_RaiseWindow(_THIS, SDL_Window * window);
void N64_MaximizeWindow(_THIS, SDL_Window * window);
void N64_MinimizeWindow(_THIS, SDL_Window * window);
void N64_RestoreWindow(_THIS, SDL_Window * window);
void N64_DestroyWindow(_THIS, SDL_Window * window);

/* Window manager function */
SDL_bool N64_GetWindowWMInfo(_THIS, SDL_Window * window,
                             struct SDL_SysWMinfo *info);

/* N64 on screen keyboard */
SDL_bool N64_HasScreenKeyboardSupport(_THIS);
void N64_ShowScreenKeyboard(_THIS, SDL_Window *window);
void N64_HideScreenKeyboard(_THIS, SDL_Window *window);
SDL_bool N64_IsScreenKeyboardShown(_THIS, SDL_Window *window);

#endif /* SDL_n64video_h_ */

/* vi: set ts=4 sw=4 expandtab: */
