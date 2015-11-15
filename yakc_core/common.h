#pragma once
//------------------------------------------------------------------------------
/**
    @file yakc/common.h
    @brief general definitions
*/
#ifndef YAKC_MEMSET
#include <string.h>
#define YAKC_MEMSET(ptr,val,size) memset(ptr,val,size);
#endif
#ifndef YAKC_ASSERT
#include <assert.h>
#define YAKC_ASSERT(x) assert(x)
#endif
#ifndef YAKC_MALLOC
#include <stdlib.h>
#define YAKC_MALLOC(s) malloc(s)
#endif
#ifndef YAKC_FREE
#include <stdlib.h>
#define YAKC_FREE(p) free(p)
#endif
#ifndef YAKC_PRINTF
#include <stdio.h>
#define YAKC_PRINTF(...) printf(__VA_ARGS__)
#endif
#ifndef YAKC_RAND
#include <stdlib.h>
#define YAKC_RAND() rand()
#endif

namespace yakc {

typedef unsigned char ubyte;
typedef char byte;
typedef unsigned short uword;
typedef short word;
typedef unsigned short address;

}