#include "SDL/SDL.h"
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

/***************************************************************************
IPT_CalJoy() - Open Gamecontroller
 ***************************************************************************/
void 
IPT_CalJoy(
	void
)
{
	AButtonconvert = 0;
	BButtonconvert = 1;
	XButtonconvert = 2;
	YButtonconvert = 3;

	//GetJoyButtonMapping();
}

/***************************************************************************
IPT_CloJoy() - Close Gamecontroller
 ***************************************************************************/
void  
IPT_CloJoy(
	void
)
{
	
}

/***************************************************************************
I_HandleJoystickEvent() - Get current button or axis status
 ***************************************************************************/
void 
I_HandleJoystickEvent(
	SDL_Event *sdlevent
)
{
	
}

/***************************************************************************
GetJoyButtonMapping() - Detect connected Gamecontroller and map buttons for it
 ***************************************************************************/
void 
GetJoyButtonMapping(
	void
)
{
	if ((AButtonconvert == 0) && (BButtonconvert == 0) && (XButtonconvert == 0) && (YButtonconvert == 0))
	{
		AButtonconvert = 0;
		BButtonconvert = 1;
		XButtonconvert = 2;
		YButtonconvert = 3;
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
	
}

/***************************************************************************
IPT_CalJoyRumbleMedium() - Gamecontroller rumbles medium
 ***************************************************************************/
void 
IPT_CalJoyRumbleMedium(
	void
)
{
	
}

/***************************************************************************
IPT_CalJoyRumbleHigh() - Gamecontroller rumbles high
 ***************************************************************************/
void 
IPT_CalJoyRumbleHigh(
	void
)
{
	
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