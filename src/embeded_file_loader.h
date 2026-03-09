#ifndef __N64__
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "FILE0000_GLB.h"

extern const uint8_t FILE0000_GLB[];
extern const uint8_t FILE0000_GLB_end[];

#include "FILE0001_GLB.h"

extern const uint8_t FILE0001_GLB[];
extern const uint8_t FILE0001_GLB_end[];

typedef struct {
    const uint8_t* buf;    // start
    size_t         size;   // total length
    size_t         pos;    // current read offset
} MemFILE;

size_t memf_read(void* ptr, size_t size, size_t nmemb, MemFILE* mf);
int memf_seek(MemFILE* mf, long offset, int whence);
MemFILE* mfopen_FILE0000(const char* name);
MemFILE* mfopen_FILE0001(const char* name);
void loadViaMemFILE0000();
void loadViaMemFILE0001();
#endif