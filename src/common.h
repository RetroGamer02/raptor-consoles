#pragma once
#include <stdlib.h>

#ifdef __ARM__
#include "arm.h"
#elif __XBOX__
#include "xbox.h"
#elif __PPC__
#include "ppc.h"
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
