#include <SDL3/SDL.h>
#include "i_video.h"
#include "joyapi.h"

int joy_ack;

bool Up, Down, Left, Right;
bool Start, Back, LeftShoulder, RightShoulder;
bool AButton, BButton, XButton, YButton;

int16_t StickX, StickY, TriggerLeft, TriggerRight;

SDL_Gamepad* ControllerHandles[MAX_CONTROLLERS];
SDL_Haptic* RumbleHandles[MAX_CONTROLLERS];

int MaxJoysticks;
int ControllerIndex;
int JoystickIndex;

int AButtonconvert, BButtonconvert, XButtonconvert, YButtonconvert;
static unsigned int lastTime = 0;

/***************************************************************************
IPT_CalJoy() - Open Gamecontroller
 ***************************************************************************/
void 
IPT_CalJoy(
	void
)
{
	SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC);

	SDL_GetJoysticks(&MaxJoysticks);
	ControllerIndex = 0;
	AButtonconvert = 0;
	BButtonconvert = 0;
	XButtonconvert = 0;
	YButtonconvert = 0;

	for (JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
	{
		if (!SDL_IsGamepad(JoystickIndex))
		{
			continue;
		}
		if (ControllerIndex >= MAX_CONTROLLERS)
		{
			break;
		}
		
		ControllerHandles[ControllerIndex] = SDL_OpenGamepad(JoystickIndex);
		RumbleHandles[ControllerIndex] = SDL_OpenHaptic(JoystickIndex);
		
		if (SDL_InitHapticRumble(RumbleHandles[ControllerIndex]) != 0)
		{
			SDL_CloseHaptic(RumbleHandles[ControllerIndex]);
			RumbleHandles[ControllerIndex] = 0;
		}
	    
		ControllerIndex++;
		GetJoyButtonMapping();
	}
}

/***************************************************************************
IPT_CloJoy() - Close Gamecontroller
 ***************************************************************************/
void  
IPT_CloJoy(
	void
)
{
	for (ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		{
			if (RumbleHandles[ControllerIndex])
				SDL_CloseHaptic(RumbleHandles[ControllerIndex]);
			
			SDL_CloseGamepad(ControllerHandles[ControllerIndex]);
		}
	}
}

/***************************************************************************
I_HandleJoystickEvent() - Get current button or axis status
 ***************************************************************************/
void 
I_HandleJoystickEvent(
	SDL_Event *sdlevent
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex] != 0 && SDL_GamepadConnected(ControllerHandles[ControllerIndex]))
		{
			Up = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_DPAD_UP);
			Down = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_DPAD_DOWN);
			Left = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_DPAD_LEFT);
			Right = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
			Start = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_START);
			Back = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_BACK);
			LeftShoulder = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
			RightShoulder = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
			AButton = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_SOUTH);
			BButton = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_EAST);
			XButton = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_WEST);
			YButton = SDL_GetGamepadButton(ControllerHandles[ControllerIndex], SDL_GAMEPAD_BUTTON_NORTH);

			StickX = SDL_GetGamepadAxis(ControllerHandles[ControllerIndex], SDL_GAMEPAD_AXIS_LEFTX) / 8000;
			StickY = SDL_GetGamepadAxis(ControllerHandles[ControllerIndex], SDL_GAMEPAD_AXIS_LEFTY) / 8000;
			TriggerLeft = SDL_GetGamepadAxis(ControllerHandles[ControllerIndex], SDL_GAMEPAD_AXIS_LEFT_TRIGGER) / 8000;
			TriggerRight = SDL_GetGamepadAxis(ControllerHandles[ControllerIndex], SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) / 8000;
		}
		
		if (sdlevent->type == SDL_EVENT_GAMEPAD_BUTTON_UP) 
			joy_ack = 0;
		
		if (sdlevent->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) 
			joy_ack = 1;
	}
}

/***************************************************************************
GetJoyButtonMapping() - Detect connected Gamecontroller and map buttons for it
 ***************************************************************************/
void 
GetJoyButtonMapping(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		switch (SDL_GetGamepadTypeForID(ControllerIndex))
		{
		case SDL_GAMEPAD_TYPE_PS3:
		case SDL_GAMEPAD_TYPE_PS4:
		case SDL_GAMEPAD_TYPE_PS5:
			AButtonconvert = 0;
			BButtonconvert = 1;
			XButtonconvert = 3;
			YButtonconvert = 2;
			break;
		
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
		case SDL_GAMEPAD_TYPE_XBOX360:
		case SDL_GAMEPAD_TYPE_XBOXONE:
			AButtonconvert = 0;
			BButtonconvert = 1;
			XButtonconvert = 2;
			YButtonconvert = 3;
			break;
		
		default:
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
}

/***************************************************************************
IPT_CalJoyRumbleLow() - Gamecontroller rumbles low
 ***************************************************************************/
void 
IPT_CalJoyRumbleLow(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_RumbleGamepad(ControllerHandles[ControllerIndex], 0x3fff, 0x3fff, 1000);
	}
}

/***************************************************************************
IPT_CalJoyRumbleMedium() - Gamecontroller rumbles medium
 ***************************************************************************/
void 
IPT_CalJoyRumbleMedium(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		    SDL_RumbleGamepad(ControllerHandles[ControllerIndex], 0x7ffe, 0x7ffe, 1000);
	}
}

/***************************************************************************
IPT_CalJoyRumbleHigh() - Gamecontroller rumbles high
 ***************************************************************************/
void 
IPT_CalJoyRumbleHigh(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_RumbleGamepad(ControllerHandles[ControllerIndex], 0xbffd, 0xbffd, 1000);
	}
}

/***************************************************************************
JOY_Wait() - Waits for button to be released
 ***************************************************************************/
void 
JOY_Wait(
	int button
)
{
	while (StickX || StickY || Up || Down || Left || Right || Start || Back || LeftShoulder || RightShoulder || AButton || BButton || XButton || YButton)
	{
		I_GetEvent();
	}
}

/***************************************************************************
JOY_IsKey() - Tests to see if button is down if so waits for release
 ***************************************************************************/
int 
JOY_IsKey(
	int button
)
{
	if (StickX || StickY || Up || Down || Left || Right || Start || Back || LeftShoulder || RightShoulder || AButton || BButton || XButton || YButton)
	{
		JOY_Wait(button);
		
		return 1;
	}
    
	return 0;
}

/***************************************************************************
JOY_IsKeyInGameStart() - Tests to see if button is down if so waits for release
 ***************************************************************************/
int 
JOY_IsKeyInGameStart(
	int button
)
{
	if (Start)
	{
		JOY_Wait(button);
		
		return 1;
	}
	
	return 0;
}

/***************************************************************************
JOY_IsKeyInGameBack() - Tests to see if button is down if so waits for release
 ***************************************************************************/
int 
JOY_IsKeyInGameBack(
	int button
)
{

	if (Back)
	{
		JOY_Wait(button);
		
		return 1;
	}
	
	return 0;
}

/***************************************************************************
JOY_IsKeyMenu() - Tests to see if button is down if so waits for release
 ***************************************************************************/
int 
JOY_IsKeyMenu(
	int button
)
{
	if (RightShoulder || Back || BButton)
	{
		JOY_Wait(button);
		
		return 1;
	}
	
	return 0;
}

/***************************************************************************
JOY_IsScroll() - Scroll cursor in menu
 ***************************************************************************/
int 
JOY_IsScroll(
	int scrollflag
)
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