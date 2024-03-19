#pragma once

#include "rap.h"

#ifdef __NDS__
struct __attribute__((packed)) csprite_t {
    int link;
    int slib;
    int x;
    int y;
    int game;
    int level;
};
#else
struct csprite_t {
    int link;
    int slib;
    int x;
    int y;
    int game;
    int level;
};
#endif

struct maptile_t {
    short flats;
    short fgame;
};

#ifdef __NDS__
typedef struct __attribute__((packed)) _map_t {
    int sizerec;
    int spriteoff;
    int numsprites;
    maptile_t map[MAP_SIZE];
} map_t;
#else
typedef struct _map_t {
    int sizerec;
    int spriteoff;
    int numsprites;
    maptile_t map[MAP_SIZE];
} map_t;
#endif

extern int curplr_diff;

extern map_t *mapmem;
extern csprite_t *csprite;

const char* RAP_InitLoadSave(void);
const char* RAP_SetupFilename(void);

void RAP_ClearPlayer(void);
int RAP_IsSaveFile(player_t *in_plr);
int RAP_FFSaveFile(void);
void RAP_SetPlayerDiff(void);
int RAP_SavePlayer(void);
