#include <SDL/SDL.h>
#include "i_video.h"
#include "joyapi.h"

int joy_ack;

bool Up, Down, Left, Right;
bool Start, Back, LeftShoulder, RightShoulder;
bool AButton, BButton, XButton, YButton;

int16_t StickX, StickY, TriggerLeft, TriggerRight;

int MaxJoysticks;
int ControllerIndex;
int JoystickIndex;

int AButtonconvert, BButtonconvert, XButtonconvert, YButtonconvert;
static unsigned int lastTime = 0;

void IPT_CalJoy(void)
{
	AButtonconvert = 0;
	BButtonconvert = 1;
	XButtonconvert = 2;
	YButtonconvert = 3;

	//GetJoyButtonMapping();
}

void  IPT_CloJoy(void)
{

}

void I_HandleJoystickEvent(SDL_Event *sdlevent)
{
	
}

/**
 *  Get the type of a game controller.
 *  This can be called before any controllers are opened.
 */
//extern DECLSPEC SDL_GameControllerType SDLCALL SDL_GameControllerTypeForIndex(int joystick_index);

void GetJoyButtonMapping(void)
{		
	if ((AButtonconvert == 0) && (BButtonconvert == 0) && (XButtonconvert == 0) && (YButtonconvert == 0))
	{
		AButtonconvert = 0;
		BButtonconvert = 1;
		XButtonconvert = 2;
		YButtonconvert = 3;
	}
}

void IPT_CalJoyRumbleLow(void)
{

}

void IPT_CalJoyRumbleMedium(void)
{

}

void IPT_CalJoyRumbleHigh(void)
{

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