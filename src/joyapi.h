#include <SDL/SDL.h>
#define MAX_CONTROLLERS 4

//#define KEY_F1			SDLK_F1			//Loose a life
//#define KEY_F2			SDLK_F2			//Game over
//#define KEY_E			SDLK_y			//Display energy
//#define KEY_F4			SDLK_RETURN		//Status page
////#define KEY_LEFT		SDLK_left		//Left
////#define KEY_RIGHT		SDLK_right		//Right
////#define KEY_UP			SDLK_up		//Up
////#define KEY_DOWN		SDLK_down		//Down
//#define KEY_JUMP		SDLK_b			//Up
//#define KEY_SPACE		SDLK_a			//Space
//#define KEY_ENTER		SDLK_KP_ENTER	//Enter
//#define KEY_RETURN		SDLK_r			//Return
//#define KEY_ESC			SDLK_ESCAPE		//Quit
//#define KEY_P			SDLK_s			//Toggle pause

extern SDL_Joystick* ControllerHandle;

extern int joy_ack;

extern bool Up, Down, Left, Right;
extern bool Start, Back, LeftShoulder, RightShoulder;
extern bool AButton, BButton, XButton, YButton;

extern int16_t StickX, StickY, TriggerLeft, TriggerRight;

extern int AButtonconvert, BButtonconvert, XButtonconvert, YButtonconvert;

void IPT_CalJoy(void);
void IPT_CloJoy(void);
void IPT_CalJoyRumbleLow(void);
void IPT_CalJoyRumbleMedium(void);
void IPT_CalJoyRumbleHigh(void);

int JOY_IsKey(int a1);
void JOY_Wait(int a1);
int JOY_IsKeyInGameStart(int a1);
int JOY_IsKeyInGameBack(int a1);
int JOY_IsKeyMenu(int a1);

void GetJoyButtonMapping(void);
int JOY_IsScroll(int a1);
