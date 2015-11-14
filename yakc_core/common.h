#pragma once
//------------------------------------------------------------------------------
/**
    @file yakc/common.h
    @brief general definitions
*/
#include <assert.h>

#ifndef YAKC_ASSERT
#define YAKC_ASSERT(x) assert(x)
#endif
#ifndef YAKC_MALLOC
#define YAKC_MALLOC(s) malloc(s)
#endif
#ifndef YAKC_FREE
#define YAKC_FREE(p) free(p)
#endif
#ifndef YAKC_PRINTF
#define YAKC_PRINTF(...) printf(__VA_ARGS__)
#endif

namespace yakc {

typedef unsigned char ubyte;
typedef char byte;
typedef unsigned short uword;
typedef short word;
typedef unsigned short address;

}