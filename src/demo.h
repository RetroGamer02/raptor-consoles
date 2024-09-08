#pragma once

#pragma pack(push, 1)

#define  DEMO_OFF       0
#define  DEMO_RECORD    1
#define  DEMO_PLAYBACK  2
#define  MAX_DEMO       2800

#ifdef __PPC__
typedef struct
{
    char b1;
    char b2;
    char b3;
    char b4;
    little_int16_t px;
    little_int16_t py;
    little_int16_t playerpic;
    little_int16_t fil;
}RECORD;
#else
typedef struct
{
    char b1;
    char b2;
    char b3;
    char b4;
    short px;
    short py;
    short playerpic;
    short fil;
}RECORD;
#endif
#pragma pack(pop)


void DEMO_MakePlayer(int game);
void DEMO_GLBFile(int item);
int DEMO_Play(void);
void DEMO_StartRec(void);
int DEMO_Think(void);
void DEMO_SaveFile(void);
void DEMO_SetFileName(const char *in_name);
void DEMO_LoadFile(void);
void DEMO_DisplayStats(void);
