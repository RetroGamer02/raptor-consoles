<<<<<<< Updated upstream
#include "SDL.h"
=======
#if defined (__3DS__) || defined (__SWITCH__)
#include "SDL2/SDL.h"
#else
#include "SDL.h"
#endif
>>>>>>> Stashed changes
#include "i_video.h"
#include "joyapi.h"

int joy_ack;

bool Up, Down, Left, Right;
bool Start, Back, LeftShoulder, RightShoulder;
bool AButton, BButton, XButton, YButton;

int16_t StickX, StickY, TriggerLeft, TriggerRight;

<<<<<<< Updated upstream
SDL_GameController* ControllerHandles[MAX_CONTROLLERS];
SDL_Haptic* RumbleHandles[MAX_CONTROLLERS] ;
=======
#ifndef SDL12
SDL_GameController* ControllerHandles[MAX_CONTROLLERS];
SDL_Haptic* RumbleHandles[MAX_CONTROLLERS] ;
#endif
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
=======
	#ifdef SDL12
		AButtonconvert = 0;
		BButtonconvert = 1;
		XButtonconvert = 2;
		YButtonconvert = 3;
	#else
>>>>>>> Stashed changes
	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

	MaxJoysticks = SDL_NumJoysticks();
	ControllerIndex = 0;
	AButtonconvert = 0;
	BButtonconvert = 0;
	XButtonconvert = 0;
	YButtonconvert = 0;

	for (JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
	{
		if (!SDL_IsGameController(JoystickIndex))
		{
			continue;
		}
		if (ControllerIndex >= MAX_CONTROLLERS)
		{
			break;
		}
		
		ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
		RumbleHandles[ControllerIndex] = SDL_HapticOpen(JoystickIndex);
		
		if (SDL_HapticRumbleInit(RumbleHandles[ControllerIndex]) != 0)
		{
			SDL_HapticClose(RumbleHandles[ControllerIndex]);
			RumbleHandles[ControllerIndex] = 0;
		}
	    
		ControllerIndex++;
		GetJoyButtonMapping();
	}
<<<<<<< Updated upstream
=======
	#endif
>>>>>>> Stashed changes
}

/***************************************************************************
IPT_CloJoy() - Close Gamecontroller
 ***************************************************************************/
void  
IPT_CloJoy(
	void
)
{
<<<<<<< Updated upstream
=======
	#ifndef SDL12
>>>>>>> Stashed changes
	for (ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		{
			if (RumbleHandles[ControllerIndex])
				SDL_HapticClose(RumbleHandles[ControllerIndex]);
			
			SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
		}
	}
<<<<<<< Updated upstream
=======
	#endif
>>>>>>> Stashed changes
}

/***************************************************************************
I_HandleJoystickEvent() - Get current button or axis status
 ***************************************************************************/
void 
I_HandleJoystickEvent(
	SDL_Event *sdlevent
)
{
<<<<<<< Updated upstream
=======
	#ifndef SDL12
>>>>>>> Stashed changes
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex] != 0 && SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]))
		{
			Up = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP);
			Down = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			Left = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			Right = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
			Start = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_START);
			Back = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_BACK);
			LeftShoulder = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
			RightShoulder = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
			AButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_A);
			BButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_B);
			XButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_X);
			YButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_Y);

			StickX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX) / 8000;
			StickY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY) / 8000;
			TriggerLeft = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 8000;
			TriggerRight = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 8000;
		}
		
		if (sdlevent->type == SDL_CONTROLLERBUTTONUP) 
			joy_ack = 0;
		
		if (sdlevent->type == SDL_CONTROLLERBUTTONDOWN) 
			joy_ack = 1;
	}
<<<<<<< Updated upstream
=======
	#endif
>>>>>>> Stashed changes
}

/***************************************************************************
GetJoyButtonMapping() - Detect connected Gamecontroller and map buttons for it
 ***************************************************************************/
void 
GetJoyButtonMapping(
	void
)
{
<<<<<<< Updated upstream
=======
	#ifdef SDL12
		if ((AButtonconvert == 0) && (BButtonconvert == 0) && (XButtonconvert == 0) && (YButtonconvert == 0))
		{
			AButtonconvert = 0;
			BButtonconvert = 1;
			XButtonconvert = 2;
			YButtonconvert = 3;
		}
	#else
>>>>>>> Stashed changes
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
<<<<<<< Updated upstream
		switch (SDL_GameControllerTypeForIndex(ControllerIndex))
		{
		case SDL_CONTROLLER_TYPE_PS3:
		case SDL_CONTROLLER_TYPE_PS4:
		case SDL_CONTROLLER_TYPE_PS5:
			AButtonconvert = 0;
			BButtonconvert = 1;
			XButtonconvert = 3;
			YButtonconvert = 2;
			break;
		
		case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
		case SDL_CONTROLLER_TYPE_XBOX360:
		case SDL_CONTROLLER_TYPE_XBOXONE:
=======
		#ifdef __XBOX__
>>>>>>> Stashed changes
			AButtonconvert = 0;
			BButtonconvert = 1;
			XButtonconvert = 2;
			YButtonconvert = 3;
<<<<<<< Updated upstream
			break;
		
		default:
			if ((AButtonconvert == 0) && (BButtonconvert == 0) && (XButtonconvert == 0) && (YButtonconvert == 0))
			{
=======
		#else
			switch (SDL_GameControllerTypeForIndex(ControllerIndex))
			{
			case SDL_CONTROLLER_TYPE_PS3:
			case SDL_CONTROLLER_TYPE_PS4:
			case SDL_CONTROLLER_TYPE_PS5:
				AButtonconvert = 0;
				BButtonconvert = 1;
				XButtonconvert = 3;
				YButtonconvert = 2;
				break;
			
			case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
			case SDL_CONTROLLER_TYPE_XBOX360:
			case SDL_CONTROLLER_TYPE_XBOXONE:
>>>>>>> Stashed changes
				AButtonconvert = 0;
				BButtonconvert = 1;
				XButtonconvert = 2;
				YButtonconvert = 3;
<<<<<<< Updated upstream
			}
			break;
		}
=======
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
		#endif
>>>>>>> Stashed changes
	}
	#endif
}

/***************************************************************************
IPT_CalJoyRumbleLow() - Gamecontroller rumbles low
 ***************************************************************************/
void 
IPT_CalJoyRumbleLow(
	void
)
{
<<<<<<< Updated upstream
=======
	#ifndef SDL12
>>>>>>> Stashed changes
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x3fff, 0x3fff, 1000);
	}
<<<<<<< Updated upstream
=======
	#endif
>>>>>>> Stashed changes
}

/***************************************************************************
IPT_CalJoyRumbleMedium() - Gamecontroller rumbles medium
 ***************************************************************************/
void 
IPT_CalJoyRumbleMedium(
	void
)
{
<<<<<<< Updated upstream
=======
	#ifndef SDL12
>>>>>>> Stashed changes
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		    SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x7ffe, 0x7ffe, 1000);
	}
<<<<<<< Updated upstream
=======
	#endif
>>>>>>> Stashed changes
}

/***************************************************************************
IPT_CalJoyRumbleHigh() - Gamecontroller rumbles high
 ***************************************************************************/
void 
IPT_CalJoyRumbleHigh(
	void
)
{
<<<<<<< Updated upstream
=======
	#ifndef SDL12
>>>>>>> Stashed changes
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0xbffd, 0xbffd, 1000);
	}
<<<<<<< Updated upstream
=======
	#endif
>>>>>>> Stashed changes
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