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
#include <SDL\SDL.h>
#include "SDL2\SDL_opengl.h"

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

// These are (1) the window (or the full screen) that our game is rendered to
// and (2) the renderer that scales the texture (see below) into this window.

static SDL_Surface *screen;

// Window title

static const char *window_title = "";

// These are (1) the 320x200x8 paletted buffer that we draw to (i.e. the one
// that holds I_VideoBuffer), (2) the 320x200x32 RGBA intermediate buffer that
// we blit the former buffer to, (3) the intermediate 320x200 texture that we
// load the RGBA buffer to and that we render into another texture (4) which
// is upscaled by an integer factor UPSCALE using "nearest" scaling and which
// in turn is finally rendered to screen using "linear" scaling.

//static SDL_Surface *screenbuffer = NULL;
//static SDL_Surface *argbbuffer = NULL;
//static SDL_Texture *texture = NULL;
//static SDL_Texture *texture_upscaled = NULL;

static SDL_Rect blit_rect = {
    0,
    0,
    SCREENWIDTH,
    SCREENHEIGHT
};

static uint32_t pixel_format;

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
int window_height = 240;

// Fullscreen mode, 0x0 for SDL_WINDOW_FULLSCREEN_DESKTOP.

int fullscreen_width = 320, fullscreen_height = 240;

// Maximum number of pixels to use for intermediate scale buffer.

static int max_scaling_buffer_pixels = 16000000;

// Run in full screen mode?  (int type for config code)

// int fullscreen = true;
int fullscreen; //Defined in VIDEO_LoadPrefs to read config from setup.ini

// Aspect ratio correction mode

int aspect_ratio_correct; //Defined in VIDEO_LoadPrefs to read config from setup.ini
static int actualheight;

// Force integer scales for resolution-independent rendering

int integer_scaling = false;

// VGA Porch palette change emulation

int vga_porch_flash = false;

// Force software rendering, for systems which lack effective hardware
// acceleration

int force_software_renderer = false; //Testme

// Time to wait for the screen to settle on startup before starting the
// game (ms)

static int startup_delay = 1000;

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

// Does the window currently have focus?

static bool window_focused = true;

// Window resize state.

static bool need_resize = false;
static unsigned int last_resize_time;
#define RESIZE_DELAY 500

// Gamma correction level to use

int usegamma = 0;

// Joystick/gamepad hysteresis
unsigned int joywait = 0;

void VIDEO_LoadPrefs(void)
{
    //Setup Items
    //fullscreen = INI_GetPreferenceLong("Video", "fullscreen", 0);
    aspect_ratio_correct = INI_GetPreferenceLong("Video", "aspect_ratio_correct", 1);
}

static bool MouseShouldBeGrabbed()
{
    return false;
}

void I_SetGrabMouseCallback(grabmouse_callback_t func)
{
    
}

static void SetShowCursor(bool show)
{
    
}

void I_ShutdownGraphics(void)
{
    if (initialized)
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);

        initialized = false;
    }
}


// Adjust window_width / window_height variables to be an an aspect
// ratio consistent with the aspect_ratio_correct variable.
static void AdjustWindowSize(void)
{
    
}

static void HandleWindowEvent() //SDL_WindowEvent *event
{
    
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

    // Main loop
	//while (aptMainLoop())
	//{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();
		//hidKeysHeld returns information about which buttons have are held down in this frame
		u32 kHeld = hidKeysHeld();
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
        if (kDown & KEY_CPAD_LEFT)
        {
            Left = 1;
        }
        if (kUp & KEY_CPAD_LEFT)
        {
            Left = 0;
        }
        if (kDown & KEY_CPAD_RIGHT)
        {
            Right = 1;
        }
        if (kUp & KEY_CPAD_RIGHT)
        {
            Right = 0;
        }
        if (kDown & KEY_CPAD_UP)
        {
            Up = 1;
        }
        if (kUp & KEY_CPAD_UP)
        {
            Up = 0;
        }
        if (kDown & KEY_CPAD_DOWN)
        {
            Down = 1;
        }
        if (kUp & KEY_CPAD_DOWN)
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

        //if (pos.dx != 0000)
        //{
            StickX = pos.dx;
        //}
        //if (pos.dy != 0000)
        //{
            StickY = pos.dy *-1;
        //}

		//Do the keys printing only if keys have changed
		/*if (kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld)
		{
			//Clear console
			consoleClear();

			//These two lines must be rewritten because we cleared the whole console
			printf("\x1b[1;1HPress Start to exit.");
			printf("\x1b[2;1HCirclePad position:");

			printf("\x1b[4;1H"); //Move the cursor to the fourth row because on the third one we'll write the circle pad position

			//Check if some of the keys are down, held or up
			int i;
			for (i = 0; i < 32; i++)
			{
				if (kDown & BIT(i)) printf("%s down\n", keysNames[i]);
				if (kHeld & BIT(i)) printf("%s held\n", keysNames[i]);
				if (kUp & BIT(i)) printf("%s up\n", keysNames[i]);
			}
		}*/

		//Set keys old values for the next frame
		kDownOld = kDown;
		kHeldOld = kHeld;
		kUpOld = kUp;

		//circlePosition pos;

		//Read the CirclePad position
		//hidCircleRead(&pos);

		//Print the CirclePad position
		//if (pos.dx != 0000 || pos.dy != 0000)
        //printf("\x1b[3;1H%04d; %04d", pos.dx, pos.dy);

		// Flush and swap framebuffers
		//gfxFlushBuffers();
		//gfxSwapBuffers();

		//Wait for VBlank
		//gspWaitForVBlank();
	//}
}

static void UpdateGrab(void)
{
    
}

static void LimitTextureSize(int *w_upscale, int *h_upscale)
{
    
}

static void CreateUpscaledTexture(bool force)
{
    
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

    // Blit from the paletted 8-bit screen buffer to the intermediate
    // 32-bit RGBA buffer that we can load into the texture.
    //SDL_LowerBlit(screen, &blit_rect, argbbuffer, &blit_rect);

    SDL_Flip(screen);
}


//
// I_ReadScreen
//
void I_ReadScreen (pixel_t* scr)
{
    //memcpy(scr, I_VideoBuffer, SCREENWIDTH*SCREENHEIGHT*sizeof(*scr));
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
// Set the window title
//

void I_SetWindowTitle(const char *title)
{
    
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

void I_GraphicsCheckCommandLine(void)
{
    
}

// Check if we have been invoked as a screensaver by xscreensaver.

void I_CheckIsScreensaver(void)
{
    
}

static void SetSDLVideoDriver(void)
{

}

// Check the display bounds of the display referred to by 'video_display' and
// set x and y to a location that places the window in the center of that
// display.
static void CenterWindow(int *x, int *y, int w, int h)
{
  
}

void I_GetWindowPosition(int *x, int *y, int w, int h)
{
  
}

static void SetVideoMode(void)
{
  
}

void I_InitGraphics(uint8_t *pal)
{
    //SDL_Event dummy;

    /* Initialize the SDL library */
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Clean up on exit */
    atexit(SDL_Quit);

    SDL_ShowCursor(SDL_DISABLE);

    /*
     * Initialize the display in a 640x480 8-bit palettized mode,
     * requesting a software surface
     */
    if (aspect_ratio_correct == 1)
    {
        screen = SDL_SetVideoMode(window_width, window_height, 8, SDL_HWSURFACE);
    } else {
        screen = SDL_SetVideoMode(window_width, window_height, 8, SDL_HWSURFACE | SDL_FULLSCREEN);
    }
    
    if ( screen == NULL ) {
        fprintf(stderr, "Couldn't set 400x240x8 video mode: %s\n",
                        SDL_GetError());
        //exit(1);
    }

    SDL_FillRect(screen, NULL, 0);
    I_SetPalette(pal);
    SDL_SetPalette(screen, 0, palette, 0, 256);

    /*if (fullscreen && !screensaver_mode)
    {
        SDL_Delay(startup_delay);
    }*/

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
    
}

void I_SetMousePos(int x, int y)
{
    
}

void closewindow(void)
{
    
}
