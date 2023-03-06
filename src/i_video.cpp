//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	DOOM graphics stuff for SDL.
//

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <climits>
#include "SDL/SDL.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include "common.h"
#include "i_video.h"
#include "m_misc.h"
#include "ptrapi.h"
#include "input.h"
#include "musapi.h"
#include "prefapi.h"
#include "joyapi.h"

#include <3ds.h>

static SDL_Surface *screen;

static SDL_Rect blit_rect = {
    0,
    0,
    SCREENWIDTH,
    SCREENHEIGHT
};

// palette

static SDL_Color palette[256];
static bool palette_to_set;

// display has been set up?

static bool initialized = false;

// disable mouse?

static bool nomouse = false;
int usemouse = 1;

// Save screenshots in PNG format.

// int png_screenshots = 0;

// SDL video driver name

const char *video_driver = "";

// Window position:

const char *window_position = "center";

// SDL display number on which to run.

int video_display = 0;

// Screen width and height, from configuration file.

int window_width = 320;
int window_height = 200;

int screen_mode; //Defined in VIDEO_LoadPrefs to read config from setup.ini
int render_to_screen;

// Run in full screen mode?  (int type for config code)

// int fullscreen = true;
int fullscreen; //Defined in VIDEO_LoadPrefs to read config from setup.ini

int txt_fullscreen;

// Aspect ratio correction mode

int aspect_ratio_correct; //Defined in VIDEO_LoadPrefs to read config from setup.ini


// Force integer scales for resolution-independent rendering

int integer_scaling = false;

// VGA Porch palette change emulation

int vga_porch_flash = false;

// Force software rendering, for systems which lack effective hardware
// acceleration

int force_software_renderer = false;

// Grab the mouse? (int type for config code). nograbmouse_override allows
// this to be temporarily disabled via the command line.

static int grabmouse = true;
static bool nograbmouse_override = false;

// The screen buffer; this is modified to draw things to the screen

pixel_t *I_VideoBuffer = NULL;

// If true, game is running as a screensaver

bool screensaver_mode = false;

// Flag indicating whether the screen is currently visible:
// when the screen isnt visible, don't render the screen

bool screenvisible = true;

// If true, we display dots at the bottom of the screen to
// indicate FPS.

// static bool display_fps_dots;

// If this is true, the screen is rendered but not blitted to the
// video buffer.

// static boolean noblit;

// Callback function to invoke to determine whether to grab the
// mouse pointer.

static grabmouse_callback_t grabmouse_callback = NULL;

// Gamma correction level to use

int usegamma = 0;

// Joystick/gamepad hysteresis
unsigned int joywait = 0;

void VIDEO_LoadPrefs(void)
{
    //Setup Items
    INI_DeletePreference("Video", "aspect_1to1");
    screen_mode = INI_GetPreferenceLong("Video", "screen_mode", 0);
    if (screen_mode == 0)
        INI_PutPreferenceLong("Video", "screen_mode", 0);
    render_to_screen = INI_GetPreferenceLong("Video", "screen", 1);
}

static bool MouseShouldBeGrabbed()
{
    return false;
}

void I_SetGrabMouseCallback(grabmouse_callback_t func)
{
    grabmouse_callback = func;
}

// Set the variable controlling FPS dots.

// void I_DisplayFPSDots(bool dots_on)
// {
//     display_fps_dots = dots_on;
// }

static void SetShowCursor(bool show)
{
    /*if (!screensaver_mode)
    {
#if 1
        // When the cursor is hidden, grab the input.
        // Relative mode implicitly hides the cursor.
        SDL_SetRelativeMouseMode((SDL_bool)!show);
        SDL_GetRelativeMouseState(NULL, NULL);
#else
        SDL_ShowCursor(show);
#endif
    }*/
}

void I_ShutdownGraphics(void)
{
    if (initialized)
    {
        //SetShowCursor(true);

        SDL_QuitSubSystem(SDL_INIT_VIDEO);

        initialized = false;
    }
}

void I_GetEvent(void)
{
    //Matrix containing the name of each key. Useful for printing when a key is pressed
	char keysNames[32][32] = {
		"KEY_A", "KEY_B", "KEY_SELECT", "KEY_START",
		"KEY_DRIGHT", "KEY_DLEFT", "KEY_DUP", "KEY_DDOWN",
		"KEY_R", "KEY_L", "KEY_X", "KEY_Y",
		"", "", "KEY_ZL", "KEY_ZR",
		"", "", "", "",
		"KEY_TOUCH", "", "", "",
		"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
		"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
	};

    u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0; //In these variables there will be information about keys detected in the previous frame
    
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();
		//hidKeysHeld returns information about which buttons have are held down in this frame
		//u32 kHeld = hidKeysHeld();
		//hidKeysUp returns information about which buttons have been just released
		u32 kUp = hidKeysUp();

		//if (kDown & KEY_START) break; // break in order to return to hbmenu

        if (kDown & KEY_START)
        {
            Start = 1;
        }
        if (kUp & KEY_START)
        {
            Start = 0;
        }
        if (kDown & KEY_SELECT)
        {
            Back = 1;
        }
        if (kUp & KEY_SELECT)
        {
            Back = 0;
        }
        if (kDown & KEY_A)
        {
            AButton = 1;
        }
        if (kUp & KEY_A)
        {
            AButton = 0;
        }
        if (kDown & KEY_B)
        {
            BButton = 1;
        }
        if (kUp & KEY_B)
        {
            BButton = 0;
        }
        if (kDown & KEY_X)
        {
            XButton = 1;
        }
        if (kUp & KEY_X)
        {
            XButton = 0;
        }
        if (kDown & KEY_Y)
        {
            YButton = 1;
        }
        if (kUp & KEY_Y)
        {
            YButton = 0;
        }
        if (kDown & KEY_DLEFT)
        {
            Left = 1;
        }
        if (kUp & KEY_DLEFT)
        {
            Left = 0;
        }
        if (kDown & KEY_DRIGHT)
        {
            Right = 1;
        }
        if (kUp & KEY_DRIGHT)
        {
            Right = 0;
        }
        if (kDown & KEY_DUP)
        {
            Up = 1;
        }
        if (kUp & KEY_DUP)
        {
            Up = 0;
        }
        if (kDown & KEY_DDOWN)
        {
            Down = 1;
        }
        if (kUp & KEY_DDOWN)
        {
            Down = 0;
        }
        if (kDown & KEY_L)
        {
            LeftShoulder = 1;
        }
        if (kUp & KEY_L)
        {
            LeftShoulder = 0;
        }
        if (kDown & KEY_R)
        {
            RightShoulder = 1;
        }
        if (kUp & KEY_R)
        {
            RightShoulder = 0;
        }

        circlePosition pos;

		//Read the CirclePad position
		hidCircleRead(&pos);

        if (pos.dx >= 15 || pos.dx <= -15)
        {
            StickX = pos.dx / 30;
        } else {
            StickX = 0;
        }
        if (pos.dy >= 15 || pos.dy <= -15)
        {
            StickY = pos.dy / 30 *-1;
        } else {
            StickY = 0;
        }

		//Set keys old values for the next frame
		kDownOld = kDown;
		//kHeldOld = kHeld;
		kUpOld = kUp;
}

//
// I_StartTic
//
// void I_StartTic (void)
// {
//     if (!initialized)
//     {
//         return;
//     }
//
//     I_GetEvent();
//
//     if (usemouse && !nomouse && window_focused)
//     {
//         I_ReadMouse();
//     }
//
//     if (joywait < I_GetTime())
//     {
//         I_UpdateJoystick();
//     }
// }


static void UpdateGrab(void)
{
    /*static bool currently_grabbed = false;
    bool grab;

    grab = MouseShouldBeGrabbed();

    if (screensaver_mode)
    {
        // Hide the cursor in screensaver mode

        SetShowCursor(false);
    }
    else if (grab && !currently_grabbed)
    {
        SetShowCursor(false);
    }
    else if (!grab && currently_grabbed)
    {
        SetShowCursor(true);

#if 0
        // When releasing the mouse from grab, warp the mouse cursor to
        // the bottom-right of the screen. This is a minimally distracting
        // place for it to appear - we may only have released the grab
        // because we're at an end of level intermission screen, for
        // example.

        SDL_GetWindowSize(screen, &screen_w, &screen_h);
        SDL_WarpMouseInWindow(screen, screen_w - 16, screen_h - 16);
        SDL_GetRelativeMouseState(NULL, NULL);
#endif
    }

    currently_grabbed = grab;*/

    SetShowCursor(false);
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{

    if (!initialized)
        return;

    //UpdateGrab();

    if (palette_to_set)
    {
        SDL_SetColors(screen, palette, 0, 256);
        palette_to_set = false;
    }

    //SDL_Flip(screen); //If Double Buffering
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

//
// I_SetPalette
//
void I_SetPalette (uint8_t *doompalette, int start /* = 0 */)
{
    int i;

    doompalette += start * 3;

    for (i=start; i<256; ++i)
    {
        palette[i].r = *doompalette++ << 2;
        palette[i].g = *doompalette++ << 2;
        palette[i].b = *doompalette++ << 2;
    }

    palette_to_set = true;
}

void I_GetPalette(uint8_t* pal)
{
    int i;

    for (i=0; i<256; ++i)
    {
        *pal++ = palette[i].r >> 2;
        *pal++ = palette[i].g >> 2;
        *pal++ = palette[i].b >> 2;
    }

}

// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex(int r, int g, int b)
{
    int best, best_diff, diff;
    int i;

    best = 0; best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
        diff = (r - palette[i].r) * (r - palette[i].r)
             + (g - palette[i].g) * (g - palette[i].g)
             + (b - palette[i].b) * (b - palette[i].b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}

//
// Call the SDL function to set the window title, based on
// the title set with I_SetWindowTitle.
//

void I_InitWindowTitle(void)
{

}

// Set the application icon

void I_InitWindowIcon(void)
{

}

static void SetVideoMode(void)
{
    /*
     * Initialize the display in a 320x200 8-bit palettized mode,
     * requesting a hardware surface
     */
    if (render_to_screen == 1)
    {
        if (screen_mode == 1)
        {
            screen = SDL_SetVideoMode(window_width, window_height, 8, SDL_HWSURFACE);
        } else if (screen_mode == 2)
        {
            screen = SDL_SetVideoMode(window_width, window_height, 8, SDL_HWSURFACE | SDL_FULLSCREEN);
        } else if (screen_mode == 3)
        {
            screen = SDL_SetVideoMode(window_width, window_height, 8, SDL_HWSURFACE | SDL_4BY3);
        } else {
            screen = SDL_SetVideoMode(window_width, window_height, 8, SDL_HWSURFACE | SDL_FITHEIGHT);
        }
    } else {
            screen = SDL_SetVideoMode(window_width, window_height, 8, SDL_HWSURFACE | SDL_CONSOLETOP | SDL_BOTTOMSCR);
    }
    
    if ( screen == NULL ) {
        fprintf(stderr, "Couldn't set 320x200x8 video mode: %s\n",
                        SDL_GetError());
    }
}

void I_InitGraphics(uint8_t *pal)
{
    /* Initialize the SDL library */
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Clean up on exit */
    atexit(SDL_Quit);

    SDL_ShowCursor(SDL_DISABLE);

    SetVideoMode();

    SDL_FillRect(screen, NULL, 0);
    I_SetPalette(pal);
    SDL_SetPalette(screen, 0, palette, 0, 256);

    // The actual 320x200 canvas that we draw to. This is the pixel buffer of
    // the 8-bit paletted screen buffer that gets blit on an intermediate
    // 32-bit RGBA screen buffer that gets loaded into a texture that gets
    // finally rendered into our window or full screen in I_FinishUpdate().

    I_VideoBuffer = (pixel_t*)screen->pixels;

    //while (SDL_PollEvent(&dummy));

    initialized = true;
}

// Bind all variables controlling video options into the configuration
// file system.
#if 0
void I_BindVideoVariables(void)
{
    M_BindIntVariable("use_mouse",                 &usemouse);
    M_BindIntVariable("fullscreen",                &fullscreen);
    M_BindIntVariable("video_display",             &video_display);
    M_BindIntVariable("aspect_ratio_correct",      &aspect_ratio_correct);
    M_BindIntVariable("integer_scaling",           &integer_scaling);
    M_BindIntVariable("vga_porch_flash",           &vga_porch_flash);
    M_BindIntVariable("startup_delay",             &startup_delay);
    M_BindIntVariable("fullscreen_width",          &fullscreen_width);
    M_BindIntVariable("fullscreen_height",         &fullscreen_height);
    M_BindIntVariable("force_software_renderer",   &force_software_renderer);
    M_BindIntVariable("max_scaling_buffer_pixels", &max_scaling_buffer_pixels);
    M_BindIntVariable("window_width",              &window_width);
    M_BindIntVariable("window_height",             &window_height);
    M_BindIntVariable("grabmouse",                 &grabmouse);
    M_BindStringVariable("video_driver",           &video_driver);
    M_BindStringVariable("window_position",        &window_position);
    M_BindIntVariable("usegamma",                  &usegamma);
    M_BindIntVariable("png_screenshots",           &png_screenshots);
}
#endif

void I_GetMousePos(int *x, int *y)
{
    /*SDL_Rect viewport;
    float sx, sy;
    SDL_GetMouseState(x, y);
    SDL_RenderGetViewport(renderer, &viewport);
    SDL_RenderGetScale(renderer, &sx, &sy);
    *x = (int)(*x / sx) - viewport.x;
    *y = (int)(((*y / sy - viewport.y) * (float)SCREENHEIGHT) / actualheight);*/
}

void I_SetMousePos(int x, int y)
{
    /*SDL_Rect viewport;
    float sx, sy;
    SDL_RenderGetViewport(renderer, &viewport);
    SDL_RenderGetScale(renderer, &sx, &sy);
    x = (int)((x + viewport.x) * sx);
    y = (int)(((y * actualheight) / (float)SCREENHEIGHT + viewport.y) * sy);
    SDL_WarpMouseInWindow(screen, x, y);*/
}

void closewindow(void)
{
    SDL_FreeSurface(screen);
}
