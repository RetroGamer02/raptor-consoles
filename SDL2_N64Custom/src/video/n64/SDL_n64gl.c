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

#include <stdlib.h>
#include <string.h>

#include "SDL_error.h"
#include "SDL_n64video.h"
#include "SDL_n64gl_c.h"

/*****************************************************************************/
/* SDL OpenGL/OpenGL ES functions (will use libdragon)                       */
/*****************************************************************************/

int
N64_GL_LoadLibrary(_THIS, const char *path)
{
    return 0;
}

void *
N64_GL_GetProcAddress(_THIS, const char *proc)
{
    return NULL;
}

void
N64_GL_UnloadLibrary(_THIS)
{
}

SDL_GLContext
N64_GL_CreateContext(_THIS, SDL_Window * window)
{
    // SDL_WindowData *wdata = (SDL_WindowData *) window->driverdata;
    window->flags |= SDL_WINDOW_FULLSCREEN;

    // _this->gl_data->display = display;
    // _this->gl_data->context = context;
    // _this->gl_data->surface = surface;

    return NULL;
}

int
N64_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
    return 0;
}

int
N64_GL_SetSwapInterval(_THIS, int interval)
{
    return 0;
}

int
N64_GL_GetSwapInterval(_THIS)
{
    return 0;
}

int
N64_GL_SwapWindow(_THIS, SDL_Window * window)
{
    return 0;
}

void
N64_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    return;
}

#endif /* SDL_VIDEO_DRIVER_N64 */

/* vi: set ts=4 sw=4 expandtab: */
