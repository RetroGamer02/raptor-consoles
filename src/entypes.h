#pragma once
#include <SDL3/SDL_endian.h>

#define LE_USHORT(x) SDL_Swap16LE(x)
#define LE_SHORT(x) (signed short) SDL_Swap16LE(x)
#define LE_ULONG(x) SDL_Swap32LE(x)
#define LE_LONG(x) (signed int) SDL_Swap32LE(x)