#pragma once

#include "rap.h"

#ifdef __PPC__
typedef struct 
{
    little_int32_t link;
    little_int32_t slib;
    little_int32_t x;
    little_int32_t y;
    little_int32_t game;
    little_int32_t level;
}CSPRITE;
#else
typedef struct 
{
    int link;
    int slib;
    int x;
    int y;
    int game;
    int level;
}CSPRITE;
#endif

#ifdef __PPC__
typedef struct 
{
    little_int16_t flats;
    little_int16_t fgame;
}MAZEDATA;
#else
typedef struct 
{
    short flats;
    short fgame;
}MAZEDATA;
#endif

#ifdef __PPC__
typedef struct 
{
    little_int32_t sizerec;
    little_int32_t spriteoff;
    little_int32_t numsprites;
    MAZEDATA map[MAP_SIZE];
}MAZELEVEL;
#else
typedef struct 
{
    int sizerec;
    int spriteoff;
    int numsprites;
    MAZEDATA map[MAP_SIZE];
}MAZELEVEL;
#endif

extern int curplr_diff;

extern MAZELEVEL *mapmem;
extern CSPRITE *csprite;

const char* RAP_InitLoadSave(void);
const char* RAP_SetupFilename(void);

void RAP_ClearPlayer(void);
int RAP_IsSaveFile(PLAYEROBJ *in_plr);
int RAP_FFSaveFile(void);
void RAP_SetPlayerDiff(void);
int RAP_SavePlayer(void);
