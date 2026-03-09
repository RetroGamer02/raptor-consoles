#pragma once
#include <stdlib.h>

#ifdef __PPC__
#ifdef __N64__
#include "mips.h"
#else
#include "ppc.h"
#endif
#endif

static inline void EXIT_Error(const char *a1, ...)
{
     exit(0);
}

static inline void EXIT_Clean(void)
{
    exit(0);
}

static inline void EXIT_Install(void (*a1)(int a1))
{

}
