#include <SDL\SDL.h>
#include "i_video.h"
#include "joyapi.h"
#include "3ds.h"

int joy_ack;

bool Up, Down, Left, Right;
bool Start, Back, LeftShoulder, RightShoulder;
bool AButton, BButton, XButton, YButton;

int16_t StickX, StickY, TriggerLeft, TriggerRight;

SDL_Joystick* ControllerHandle;

SDL_Event joyevent;

int MaxJoysticks;
int ControllerIndex;
int JoystickIndex;

int AButtonconvert, BButtonconvert, XButtonconvert, YButtonconvert;
static unsigned int lastTime = 0;

void IPT_CalJoy(void)
{
	SDL_Init(SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);

	AButtonconvert = 0;
	BButtonconvert = 0;
	XButtonconvert = 0;
	YButtonconvert = 0;

   //printf("%i joysticks were found.\n\n", SDL_NumJoysticks() );
    printf("The names of the joysticks are:\n");
		
    for(int i=0; i < SDL_NumJoysticks(); i++ ) 
    {
        printf("    %s\n at %d", SDL_JoystickName(i), i);
    }

	ControllerHandle = SDL_JoystickOpen(0);

	//GetJoyButtonMapping();
}

void  IPT_CloJoy(void)
{
	if (ControllerHandle)
	{
		SDL_JoystickClose(ControllerHandle);
	}
}

void I_HandleJoystickEvent(SDL_Event *sdlevent)
{
		//if (ControllerHandle != 0)
		//{
			SDL_N3DSKeyBind(KEY_START, SDLK_RETURN);
			SDL_N3DSKeyBind(KEY_SELECT, SDLK_ESCAPE);

			SDL_N3DSKeyBind(KEY_A, SDLK_a);
			SDL_N3DSKeyBind(KEY_B, SDLK_b);
			SDL_N3DSKeyBind(KEY_X, SDLK_x);
			SDL_N3DSKeyBind(KEY_Y, SDLK_y);

			SDL_N3DSKeyBind(KEY_ZL, SDLK_LSHIFT);
			SDL_N3DSKeyBind(KEY_ZR, SDLK_RSHIFT);

			SDL_N3DSKeyBind(KEY_CPAD_UP|KEY_CSTICK_UP, SDLK_UP);
			SDL_N3DSKeyBind(KEY_CPAD_DOWN|KEY_CSTICK_DOWN, SDLK_DOWN);
			SDL_N3DSKeyBind(KEY_CPAD_LEFT|KEY_CSTICK_LEFT, SDLK_LEFT);
			SDL_N3DSKeyBind(KEY_CPAD_RIGHT|KEY_CSTICK_RIGHT, SDLK_RIGHT);

			Up = SDL_JoystickGetHat(ControllerHandle, KEY_CPAD_UP);
			Down = SDL_JoystickGetHat(ControllerHandle, KEY_CPAD_DOWN);
			Left = SDL_JoystickGetHat(ControllerHandle, KEY_CPAD_LEFT);
			Right = SDL_JoystickGetHat(ControllerHandle, KEY_CPAD_RIGHT);
			Start = SDL_JoystickGetButton(ControllerHandle, KEY_START);
			Back = SDL_JoystickGetButton(ControllerHandle, KEY_SELECT);
			LeftShoulder = SDL_JoystickGetButton(ControllerHandle, KEY_ZL);
			RightShoulder = SDL_JoystickGetButton(ControllerHandle, KEY_ZR);
			AButton = SDL_JoystickGetButton(ControllerHandle, KEY_A);
			BButton = SDL_JoystickGetButton(ControllerHandle, KEY_B);
			XButton = SDL_JoystickGetButton(ControllerHandle, KEY_X);
			YButton = SDL_JoystickGetButton(ControllerHandle, KEY_Y);

			//StickX = SDL_JoystickGetAxis(ControllerHandle, 1) / 8000;
			//StickY = SDL_JoystickGetAxis(ControllerHandle, 2) / 8000;
			//TriggerLeft = SDL_JoystickGetAxis(ControllerHandle, 3) / 8000;
			//TriggerRight = SDL_JoystickGetAxis(ControllerHandle, 4) / 8000;
	    //}

		/*if (sdlevent->SDL_JoyButtonEvent == SDL_JOYBUTTONUP) 
			joy_ack = 0;
		if (sdlevent->type == SDL_JOYBUTTONDOWN)
		{
			joy_ack = 1;
			printf("Pressed button");
		}*/

		/*while(SDL_PollEvent(&joyevent))
    {  
        switch(joyevent.type)
        {  
            case SDL_JOYBUTTONDOWN:
			printf("Button Pressed?");			
            break;

			default:
			printf("Default");
			break;
        }
    }*/
}

/*void SDL_N3DSKeyBind(unsigned int hidkey, SDLKey key)
{
	SDL_N3DSKeyBind(KEY_A, SDLK_a);
	SDL_N3DSKeyBind(KEY_B, SDLK_b);
	SDL_N3DSKeyBind(KEY_X, SDLK_x);
	SDL_N3DSKeyBind(KEY_Y, SDLK_y);

	SDL_N3DSKeyBind(KEY_ZL, SDLK_LSHIFT);
	SDL_N3DSKeyBind(KEY_ZR, SDLK_RSHIFT);

	SDL_N3DSKeyBind(KEY_CPAD_UP|KEY_CSTICK_UP, SDLK_UP);
	SDL_N3DSKeyBind(KEY_CPAD_DOWN|KEY_CSTICK_DOWN, SDLK_DOWN);
	SDL_N3DSKeyBind(KEY_CPAD_LEFT|KEY_CSTICK_LEFT, SDLK_LEFT);
	SDL_N3DSKeyBind(KEY_CPAD_RIGHT|KEY_CSTICK_RIGHT, SDLK_RIGHT);
}*/

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