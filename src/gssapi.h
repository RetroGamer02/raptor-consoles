#pragma once
#include <stdint.h>

#ifdef __PPC__
struct gss1_t {
    little_int16_t format;
    little_int16_t len;
    little_int16_t bank;
    little_int16_t patch;
    uint8_t data[1];
};

struct gss2_t {
    little_int16_t format;
    little_int16_t bank;
    little_int16_t patch;
    little_int16_t note;
    little_int16_t len;
};
#else
struct gss1_t {
    int16_t format;
    int16_t len;
    int16_t bank;
    int16_t patch;
    uint8_t data[1];
};

struct gss2_t {
    int16_t format;
    int16_t bank;
    int16_t patch;
    int16_t note;
    int16_t len;
};
#endif

extern int gsshack;
extern int gssrate;

void GSS_Service(void);
void GSS_Init(int device, int option);
void GSS_DeInit(void);
void GSS_Mix(int16_t *stream, int len);
void GSS_Poll(void);
int GSS_PlayPatch(void *gss, int sep, int pitch, int volume, int priority);
void GSS_StopPatch(int handle);
int GSS_PatchIsPlaying(int handle);
