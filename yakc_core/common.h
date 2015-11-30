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
    kc85_2 = (1<<0),
    kc85_3 = (1<<1),
    kc85_4 = (1<<2),
    none = 0,
    any = (kc85_2|kc85_3|kc85_4),
};

enum class kc85_caos {
    caos_hc900,
    caos_2_2,
    caos_3_1,
    caos_3_4,
    caos_4_1,
    caos_4_2,
};

inline void
clear(void* ptr, int num_bytes) {
    memset(ptr, 0, num_bytes);
}

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