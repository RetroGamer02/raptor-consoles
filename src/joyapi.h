#include <SDL\SDL.h>
#define MAX_CONTROLLERS 4

extern SDL_Joystick* ControllerHandles[MAX_CONTROLLERS];
//extern SDL_Haptic* RumbleHandles[MAX_CONTROLLERS];

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
