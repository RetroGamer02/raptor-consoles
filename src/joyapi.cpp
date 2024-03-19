#ifdef __3DS__
#include "SDL/SDL.h"
#else
#include "SDL.h"
#endif
#include "i_video.h"
#include "joyapi.h"

int joy_ack;

bool Up, Down, Left, Right;
bool Start, Back, LeftShoulder, RightShoulder;
bool AButton, BButton, XButton, YButton;

int16_t StickX, StickY, TriggerLeft, TriggerRight;

#ifdef SDL12
SDL_Joystick* ControllerHandles[MAX_CONTROLLERS];
#else
SDL_GameController* ControllerHandles[MAX_CONTROLLERS];
SDL_Haptic* RumbleHandles[MAX_CONTROLLERS] ;
#endif

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
	#ifdef SDL12
	SDL_Init(SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);
	
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
			//continue; //Broken for some reason.
			goto continued;
		}
		continued:
		if (ControllerIndex >= MAX_CONTROLLERS)
		{
			break;
		}
		
		ControllerHandles[ControllerIndex] = SDL_JoystickOpen(JoystickIndex);
	    
		ControllerIndex++;
		GetJoyButtonMapping();
	}
	#else
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
	#endif
}

/***************************************************************************
IPT_CloJoy() - Close Gamecontroller
 ***************************************************************************/
void  
IPT_CloJoy(
	void
)
{
	#ifdef SDL12
	for (ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		{			
			SDL_JoystickClose(ControllerHandles[ControllerIndex]);
		}
	}
	#else
	for (ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		{
			if (RumbleHandles[ControllerIndex])
				SDL_HapticClose(RumbleHandles[ControllerIndex]);
			
			SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
		}
	}
	#endif
}

/***************************************************************************
I_HandleJoystickEvent() - Get current button or axis status
 ***************************************************************************/
void 
I_HandleJoystickEvent(
	SDL_Event *sdlevent
)
{
	#ifdef SDL12
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex] != 0 && SDL_JoystickOpened(ControllerIndex))
		{
			//Hat input handled by native nds input api. See i_video I_GetEvent
			//Up = SDL_JoystickGetHat(ControllerHandles[ControllerIndex], 0); //4
			//Down = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 0); //6
			//Left = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 0); //6
			//Right = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 0); //2
			Start = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 0); //Correct 0
			Back = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 7); //Correct 7
			LeftShoulder = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 5); //Correct 5
			RightShoulder = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 6); //Correct 6
			AButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 1); //Correct 1
			BButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 2); //Correct 2
			XButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 3); //Correct 3
			YButton = SDL_JoystickGetButton(ControllerHandles[ControllerIndex], 4); //Correct 4

			#ifdef __3DS__
			StickX = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 0) / 8000;
			StickY = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 1) / 8000;
			TriggerLeft = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 2) / 8000;
			TriggerRight = SDL_JoystickGetAxis(ControllerHandles[ControllerIndex], 3) / 8000;
			#endif
		}
		
		if (sdlevent->type == SDL_JOYBUTTONUP) 
			joy_ack = 0;
		
		if (sdlevent->type == SDL_JOYBUTTONDOWN) 
			joy_ack = 1;
	}
	#else
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
	#endif
}

/***************************************************************************
GetJoyButtonMapping() - Detect connected Gamecontroller and map buttons for it
 ***************************************************************************/
void 
GetJoyButtonMapping(
	void
)
{
	#ifdef SDL12
		AButtonconvert = 0;
		BButtonconvert = 1;
		XButtonconvert = 2;
		YButtonconvert = 3;
	#else
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
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
	#ifndef SDL12
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x3fff, 0x3fff, 1000);
	}
	#endif
}

/***************************************************************************
IPT_CalJoyRumbleMedium() - Gamecontroller rumbles medium
 ***************************************************************************/
void 
IPT_CalJoyRumbleMedium(
	void
)
{
	#ifndef SDL12
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		    SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x7ffe, 0x7ffe, 1000);
	}
	#endif
}

/***************************************************************************
IPT_CalJoyRumbleHigh() - Gamecontroller rumbles high
 ***************************************************************************/
void 
IPT_CalJoyRumbleHigh(
	void
)
{
	#ifndef SDL12
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0xbffd, 0xbffd, 1000);
	}
	#endif
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