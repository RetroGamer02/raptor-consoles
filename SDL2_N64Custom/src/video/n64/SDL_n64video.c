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

/* SDL internals */
#include "../SDL_sysvideo.h"
#include "SDL_version.h"
#include "SDL_syswm.h"
#include "SDL_loadso.h"
#include "SDL_events.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_keyboard_c.h"

#include "SDL_n64video.h"
#include "SDL_n64events_c.h"

static bitdepth_t
PixelFormatToN64FMT(Uint32 format)
{
    /*switch (format) {
        case SDL_PIXELFORMAT_RGBA8888:
            return DEPTH_32_BPP;
        default:
            return DEPTH_16_BPP;
    }*/
   return DEPTH_16_BPP;
}

static void
N64_Destroy(SDL_VideoDevice * device)
{
    if (device->driverdata != NULL) {
        device->driverdata = NULL;
    }
}

static SDL_VideoDevice *
N64_Create()
{
    //fprintf(stderr, "N64_Create\n");
    SDL_VideoDevice *device;
    SDL_VideoData *phdata;

    /* Initialize SDL_VideoDevice structure */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (device == NULL) {
        SDL_OutOfMemory();
        return NULL;
    }

    /* Initialize internal N64 specific data */
    phdata = (SDL_VideoData *) SDL_calloc(1, sizeof(SDL_VideoData));
    if (phdata == NULL) {
        SDL_OutOfMemory();
        SDL_free(device);
        return NULL;
    }

    device->driverdata = phdata;
    phdata->egl_initialized = SDL_FALSE;

    /* Setup amount of available displays */
    device->num_displays = 1;

    /* Set device free function */
    device->free = N64_Destroy;

    /* Setup all functions which we can handle */
    device->VideoInit = N64_VideoInit;
    device->VideoQuit = N64_VideoQuit;
    device->GetDisplayModes = N64_GetDisplayModes;
    device->SetDisplayMode = N64_SetDisplayMode;
    device->CreateSDLWindow = N64_CreateWindow;
    device->CreateSDLWindowFrom = N64_CreateWindowFrom;
    device->SetWindowTitle = N64_SetWindowTitle;
    device->SetWindowIcon = N64_SetWindowIcon;
    device->SetWindowPosition = N64_SetWindowPosition;
    device->SetWindowSize = N64_SetWindowSize;
    device->ShowWindow = N64_ShowWindow;
    device->HideWindow = N64_HideWindow;
    device->RaiseWindow = N64_RaiseWindow;
    device->MaximizeWindow = N64_MaximizeWindow;
    device->MinimizeWindow = N64_MinimizeWindow;
    device->RestoreWindow = N64_RestoreWindow;
    device->DestroyWindow = N64_DestroyWindow;
#if 0
    device->GetWindowWMInfo = N64_GetWindowWMInfo;
    device->GL_LoadLibrary = N64_GL_LoadLibrary;
    device->GL_GetProcAddress = N64_GL_GetProcAddress;
    device->GL_UnloadLibrary = N64_GL_UnloadLibrary;
    device->GL_CreateContext = N64_GL_CreateContext;
    device->GL_MakeCurrent = N64_GL_MakeCurrent;
    device->GL_SetSwapInterval = N64_GL_SetSwapInterval;
    device->GL_GetSwapInterval = N64_GL_GetSwapInterval;
    device->GL_SwapWindow = N64_GL_SwapWindow;
    device->GL_DeleteContext = N64_GL_DeleteContext;
#endif
    device->HasScreenKeyboardSupport = N64_HasScreenKeyboardSupport;
    device->ShowScreenKeyboard = N64_ShowScreenKeyboard;
    device->HideScreenKeyboard = N64_HideScreenKeyboard;
    device->IsScreenKeyboardShown = N64_IsScreenKeyboardShown;

    device->PumpEvents = N64_PumpEvents;

    return device;
}

VideoBootStrap N64_bootstrap = {
    "N64",
    "N64 Libdragon Video Driver",
    N64_Create
};

/*****************************************************************************/
/* SDL Video and Display initialization/handling functions                   */
/*****************************************************************************/
int
N64_VideoInit(_THIS)
{
    /* Initialize Libdragon Joypads (Modern replacement for controller_init) */
    joypad_init();

    SDL_VideoDisplay display;
    SDL_DisplayMode current_mode;
    SDL_zero(current_mode);
    SDL_zero(display);

    current_mode.w = 320;
    current_mode.h = 240;
    current_mode.refresh_rate = 60;
    current_mode.format = SDL_PIXELFORMAT_RGBA5551; // Standard N64 16-bit

    display.desktop_mode = current_mode;
    display.current_mode = current_mode;
    
    SDL_AddDisplayMode(&display, &current_mode);
    SDL_AddVideoDisplay(&display, SDL_FALSE);

    return 1;
}

void
N64_VideoQuit(_THIS)
{
    display_close();
}

void
N64_GetDisplayModes(_THIS, SDL_VideoDisplay * display)
{
}

int
N64_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode)
{
    /*display_close();

    resolution_t ld_resolution = mode->h > 240 ? RESOLUTION_640x480 : RESOLUTION_320x240;
    int pixelformat = PixelFormatToN64FMT(mode->format);

    display_init(ld_resolution, pixelformat, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
*/
    return 0;
}

int
N64_CreateWindow(_THIS, SDL_Window * window)
{
    SDL_WindowData *wdata = (SDL_WindowData *) SDL_calloc(1, sizeof(SDL_WindowData));
    if (!wdata) return SDL_OutOfMemory();
    window->driverdata = wdata;

    resolution_t res = RESOLUTION_320x240; 
    if (window->w > 320) res = RESOLUTION_640x480;

    display_init(res, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
    
    rdpq_init();

    rdpq_mode_filter(FILTER_POINT);
    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_dithering(DITHER_NONE_NONE);
    rdpq_set_scissor(0, 0, window->w, window->h);

    SDL_SetKeyboardFocus(window);
    return 0;
}

int
N64_CreateWindowFrom(_THIS, SDL_Window * window, const void *data)
{
    return SDL_Unsupported();
}

void
N64_SetWindowTitle(_THIS, SDL_Window * window)
{
}
void
N64_SetWindowIcon(_THIS, SDL_Window * window, SDL_Surface * icon)
{
}
void
N64_SetWindowPosition(_THIS, SDL_Window * window)
{
}
void
N64_SetWindowSize(_THIS, SDL_Window * window)
{
}
void
N64_ShowWindow(_THIS, SDL_Window * window)
{
}
void
N64_HideWindow(_THIS, SDL_Window * window)
{
}
void
N64_RaiseWindow(_THIS, SDL_Window * window)
{
}
void
N64_MaximizeWindow(_THIS, SDL_Window * window)
{
}
void
N64_MinimizeWindow(_THIS, SDL_Window * window)
{
}
void
N64_RestoreWindow(_THIS, SDL_Window * window)
{
}
void
N64_DestroyWindow(_THIS, SDL_Window * window)
{
}

/*****************************************************************************/
/* SDL Window Manager function                                               */
/*****************************************************************************/
#if 0
SDL_bool
N64_GetWindowWMInfo(_THIS, SDL_Window * window, struct SDL_SysWMinfo *info)
{
    if (info->version.major <= SDL_MAJOR_VERSION) {
        return SDL_TRUE;
    } else {
        SDL_SetError("Application not compiled with SDL %d.%d",
                     SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
        return SDL_FALSE;
    }

    /* Failed to get window manager information */
    return SDL_FALSE;
}
#endif


/* TO Write Me */
SDL_bool N64_HasScreenKeyboardSupport(_THIS)
{
    return SDL_FALSE;
}
void N64_ShowScreenKeyboard(_THIS, SDL_Window *window)
{
}
void N64_HideScreenKeyboard(_THIS, SDL_Window *window)
{
}
SDL_bool N64_IsScreenKeyboardShown(_THIS, SDL_Window *window)
{
    return SDL_FALSE;
}


#endif /* SDL_VIDEO_DRIVER_N64 */

/* vi: set ts=4 sw=4 expandtab: */
