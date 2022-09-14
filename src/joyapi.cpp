#include <SDL\SDL.h>
#include "i_video.h"
#include "joyapi.h"

int joy_ack;

bool Up, Down, Left, Right;
bool Start, Back, LeftShoulder, RightShoulder;
bool AButton, BButton, XButton, YButton;

int16_t StickX, StickY, TriggerLeft, TriggerRight;

SDL_Joystick* ControllerHandles[MAX_CONTROLLERS];
//SDL_Haptic* RumbleHandles[MAX_CONTROLLERS] ;

int MaxJoysticks;
int ControllerIndex;
int JoystickIndex;

int AButtonconvert, BButtonconvert, XButtonconvert, YButtonconvert;
static unsigned int lastTime = 0;

void IPT_CalJoy(void)
{
	SDL_Init(SDL_INIT_JOYSTICK);

	MaxJoysticks = SDL_NumJoysticks();
	ControllerIndex = 0;
	AButtonconvert = 0;
	BButtonconvert = 0;
	XButtonconvert = 0;
	YButtonconvert = 0;

	for (JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
	{
		if (!SDL_JoystickOpened(JoystickIndex))
		{
			continue;
		}
		if (ControllerIndex >= MAX_CONTROLLERS)
		{
			break;
		}
		ControllerHandles[ControllerIndex] = SDL_JoystickOpen(JoystickIndex);

	    ControllerIndex++;
		GetJoyButtonMapping();
	}
}

void  IPT_CloJoy(void)
{
	for (ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		{
			SDL_JoystickClose(ControllerHandles[ControllerIndex]);
		}
	}
}

void I_HandleJoystickEvent(SDL_Event *sdlevent)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex] != 0)
		{
			Up = SDL_JoystickGetHat(ControllerHandles[ControllerIndex], SDL_HAT_UP);
			Down = SDL_JoystickGetHat(ControllerHandles[ControllerIndex], SDL_HAT_DOWN);
			Left = SDL_JoystickGetHat(ControllerHandles[ControllerIndex], SDL_HAT_LEFT);
			Right = SDL_JoystickGetHat(ControllerHandles[ControllerIndex], SDL_HAT_RIGHT);
			Start = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_RETURN);
			Back = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_ESCAPE);
			LeftShoulder = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_LSHIFT);
			RightShoulder = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_RSHIFT);
			AButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_a);
			BButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_b);
			XButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_x);
			YButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], SDLK_y);

			StickX = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 1) / 8000;
			StickY = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 2) / 8000;
			TriggerLeft = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 3) / 8000;
			TriggerRight = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 4) / 8000;
	    }
		if (sdlevent->type == SDL_JOYBUTTONUP) 
			joy_ack = 0;
		if (sdlevent->type == SDL_JOYBUTTONDOWN) 
			joy_ack = 1;
	}
}

/*typedef enum
{
    SDL_CONTROLLER_TYPE_UNKNOWN = 0,
    SDL_CONTROLLER_TYPE_XBOX360,
    SDL_CONTROLLER_TYPE_XBOXONE,
    SDL_CONTROLLER_TYPE_PS3,
    SDL_CONTROLLER_TYPE_PS4,
    SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO,
    SDL_CONTROLLER_TYPE_VIRTUAL,
    SDL_CONTROLLER_TYPE_PS5
} SDL_GameControllerType;*/

/**
 *  Start a rumble effect
 *  Each call to this function cancels any previous rumble effect, and calling it with 0 intensity stops any rumbling.
 *
 *  \param gamecontroller The controller to vibrate
 *  \param low_frequency_rumble The intensity of the low frequency (left) rumble motor, from 0 to 0xFFFF
 *  \param high_frequency_rumble The intensity of the high frequency (right) rumble motor, from 0 to 0xFFFF
 *  \param duration_ms The duration of the rumble effect, in milliseconds
 *
 *  \return 0, or -1 if rumble isn't supported on this controller
 */
//extern DECLSPEC int SDLCALL SDL_GameControllerRumble(SDL_GameController *gamecontroller, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble, Uint32 duration_ms);

/**
 *  Get the type of a game controller.
 *  This can be called before any controllers are opened.
 */
//extern DECLSPEC SDL_GameControllerType SDLCALL SDL_GameControllerTypeForIndex(int joystick_index);

void GetJoyButtonMapping(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		
			if ((AButtonconvert == 0) && (BButtonconvert == 0) && (XButtonconvert == 0) && (YButtonconvert == 0))
			{
				AButtonconvert = 0;
				BButtonconvert = 1;
				XButtonconvert = 2;
				YButtonconvert = 3;
			}
			break;
		
	}
}

void IPT_CalJoyRumbleLow(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		//if (ControllerHandles[ControllerIndex])
            //SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x3fff, 0x3fff, 1000);
    }
}

void IPT_CalJoyRumbleMedium(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		//if (ControllerHandles[ControllerIndex])
		    //SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x7ffe, 0x7ffe, 1000);
	}
}

void IPT_CalJoyRumbleHigh(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		//if (ControllerHandles[ControllerIndex])
		    //SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0xbffd, 0xbffd, 1000);
    }
}

void JOY_Wait(int a1)
{
	while (StickX || StickY || Up || Down || Left || Right || Start || Back || LeftShoulder || RightShoulder || AButton || BButton || XButton || YButton)
	{
		I_GetEvent();
	}
}

int JOY_IsKey(int a1)
{
	if (StickX || StickY || Up || Down || Left || Right || Start || Back || LeftShoulder || RightShoulder || AButton || BButton || XButton || YButton)
	{
		JOY_Wait(a1);
		return 1;
	}
    return 0;
}

int JOY_IsKeyInGameStart(int a1)
{
	if (Start)
	{
		JOY_Wait(a1);
		return 1;
	}
	return 0;
}

int JOY_IsKeyInGameBack(int a1)
{

	if (Back)
	{
		JOY_Wait(a1);
		return 1;
	}
	return 0;
}

int JOY_IsKeyMenu(int a1)
{
	if (RightShoulder || Back || BButton)
	{
		JOY_Wait(a1);
		return 1;
	}
	return 0;
}

int JOY_IsScroll(int a1)
{
	unsigned int currentTime;
	currentTime = SDL_GetTicks();
	
	if (currentTime > lastTime + 200)
	{
		lastTime = currentTime;
		return 1;
	}
	return 0;
}