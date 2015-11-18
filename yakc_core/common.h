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

enum class kc85_model {
    kc85_3,
    kc85_4,
    none,
};

inline void
fill_random(void* ptr, int num_bytes) {
    YAKC_ASSERT((num_bytes & 0x03) == 0);
    unsigned int* uptr = (unsigned int*)ptr;
    int num_uints = num_bytes>>2;
    for (int i = 0; i < num_uints; i++) {
        *uptr++ = YAKC_RAND();
    }
}

} // namespace yakc