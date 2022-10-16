#include <string.h>
#include <ctype.h>
#include <SDL/SDL.h>
#include "common.h"
#include "i_video.h"

int kbd_ack;
int capslock;
int lastscan, lastascii;
int keyboard[256];
void (*kbdhook)(void);

int ASCIINames[] = {
    0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x08, 0x09,
    0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, 0x6F, 0x70, 0x5B, 0x5D, 0x0D, 0x00, 0x61, 0x73,
    0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B, 0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76,
    0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x00, 0x2A, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x38, 0x39, 0x2D, 0x34, 0x35, 0x36, 0x2B, 0x31,
    0x32, 0x33, 0x30, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1B, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x08, 0x09,
    0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49, 0x4F, 0x50, 0x7B, 0x7D, 0x0D, 0x00, 0x41, 0x53,
    0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3A, 0x22, 0x7E, 0x00, 0x7C, 0x5A, 0x58, 0x43, 0x56,
    0x42, 0x4E, 0x4D, 0x3C, 0x3E, 0x3F, 0x00, 0x2A, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x38, 0x39, 0x2D, 0x34, 0x35, 0x36, 0x2B, 0x31,
    0x32, 0x33, 0x30, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

int ScanCodeMap[] = {
    0,
    0,
    0,
    0,
    0x1e,
    0x30,
    0x2e,
    0x20,
    0x12,
    0x21,

    0x22,
    0x23,
    0x17,
    0x24,
    0x25,
    0x26,
    0x32,
    0x31,
    0x18,
    0x19,

    0x10,
    0x13,
    0x1f,
    0x14,
    0x16,
    0x2f,
    0x11,
    0x2d,
    0x15,
    0x2c,

    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,

    0x1c,
    0x01,
    0x0e,
    0x0f,
    0x39,
    0x0c,
    0x0d,
    0x1a,
    0x1b,
    0x2b,

    0,
    0x27,
    0x28,
    0x29,
    0x33,
    0x34,
    0x35,
    0x3a,
    0x3b,
    0x3c,

    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x56,
    0x58,

    0xe0,
    0x46,
    0xe0,
    0x52,
    0x47,
    0x49,
    0x53,
    0x4f,
    0x51,
    0x4d,

    0x4b,
    0x50,
    0x48,
    0x45,
    0xe0,
    0x37,
    0x4a,
    0x4e,
    0x1c,
    0x4f,

    0x50,
    0x51,
    0x4b,
    0x4c,
    0x4d,
    0x47,
    0x48,
    0x49,
    0x52,
    0x53,
};

void I_HandleKeyboardEvent(SDL_Event *sdlevent)
{
    int key = 0;
    if (sdlevent->type != SDL_KEYDOWN && sdlevent->type != SDL_KEYUP)
    {
        if (sdlevent->key.keysym.scancode >= 0 && sdlevent->key.keysym.scancode < 100)
            key = ScanCodeMap[sdlevent->key.keysym.scancode];
    }

    if (!key)
        return;

    if (sdlevent->type == SDL_KEYUP)
    {
        kbd_ack = 0;
        keyboard[key] = 0;
        if (key == 0x3a)
            capslock = 0;
    }
    else
    {
        kbd_ack = 1;
        lastscan = key;
        keyboard[key] = 1;
        if (key && capslock + keyboard[42] + keyboard[54] != 0)
            lastascii = ASCIINames[128 + key];
        else
            lastascii = ASCIINames[key];
        if (key == 0x3a)
            capslock = 1;
    }
}

void KBD_Clear(void)
{
    lastscan = 0;
    memset(keyboard, 0, sizeof(keyboard));
}

void KBD_SetKeyboardHook(void (*hook)(void))
{
    kbdhook = hook;
}

int KBD_Ascii2Scan(int a1)
{
    int i;
    a1 = tolower(a1);
    for (i = 0; i < 100; i++)
    {
        if (ASCIINames[i] == a1)
            return i;
    }
    return 0;
}

void KBD_Wait(int a1)
{
    while (keyboard[a1])
    {
        I_GetEvent();
    }
    lastscan = 0;
    lastascii = 0;
}

int KBD_IsKey(int a1)
{
    if (keyboard[a1])
    {
        KBD_Wait(a1);
        return 1;
    }
    return 0;
}

void KBD_Install(void)
{
    memset(keyboard, 0, sizeof(keyboard));
}

void KBD_End(void)
{

}

