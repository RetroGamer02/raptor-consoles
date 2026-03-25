#ifndef MIPS_H
#define MIPS_H

#ifdef __MIPS__
#include <iostream>
#include <stdio.h>
#include <string.h>
#ifdef __N64__
#include <cstdint>
#include <libdragon.h>
#include <string.h>
#include <SDL.h>
#endif
#endif

#ifdef __N64__
#define PATH_MAX 256
#define printf(...) debugf(__VA_ARGS__)
#endif

//Generic file copy function.
int cp(const char *to, const char *from);

//Init the target system
void sys_init();

#define access checkFile

#pragma pack(push,1)

struct little_int16_t {
    uint8_t b0;
    uint8_t b1;

    inline little_int16_t(int16_t v = 0) { set_value(v); }

    inline int16_t get_value() const {
        return (int16_t)(b0 | (b1 << 8));
    }

    inline void set_value(int16_t v) {
        b0 = v & 0xFF;
        b1 = (v >> 8) & 0xFF;
    }
};

struct little_uint16_t {
    uint8_t b0;
    uint8_t b1;

    inline little_uint16_t(uint16_t v = 0) { set_value(v); }

    inline uint16_t get_value() const {
        return b0 | (b1 << 8);
    }

    inline void set_value(uint16_t v) {
        b0 = v & 0xFF;
        b1 = v >> 8;
    }
};

struct little_int32_t {
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;

    inline little_int32_t(int32_t v = 0) { set_value(v); }

    inline int32_t get_value() const {
        return (int32_t)(
            b0 |
            (b1 << 8) |
            (b2 << 16) |
            (b3 << 24)
        );
    }

    inline void set_value(int32_t v) {
        b0 = v & 0xFF;
        b1 = (v >> 8) & 0xFF;
        b2 = (v >> 16) & 0xFF;
        b3 = (v >> 24) & 0xFF;
    }
};

struct little_uint32_t {
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;

    inline little_uint32_t(uint32_t v = 0) { set_value(v); }

    inline uint32_t get_value() const {
        return
            b0 |
            (b1 << 8) |
            (b2 << 16) |
            (b3 << 24);
    }

    inline void set_value(uint32_t v) {
        b0 = v & 0xFF;
        b1 = (v >> 8) & 0xFF;
        b2 = (v >> 16) & 0xFF;
        b3 = (v >> 24) & 0xFF;
    }
};

#pragma pack(pop)

extern char gExeDir[512];

#endif