#pragma once
//------------------------------------------------------------------------------
/**
    @file core/common.h
    @brief general definitions
*/
#ifndef YAKC_MEMSET
#include <string.h>
#define YAKC_MEMSET(ptr,val,size) memset(ptr,val,size);
#endif
#ifndef YAKC_MEMCPY
#include <string.h>
#define YAKC_MEMCPY(dst,src,num) memcpy(dst,src,num);
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
#include <stdint.h>

namespace yakc {

typedef unsigned char ubyte;
typedef signed char byte;
typedef unsigned short uword;
typedef signed short word;

enum class device {
    kc85_2 = (1<<0),
    kc85_3 = (1<<1),
    kc85_4 = (1<<2),
    z1013_01 = (1<<3),
    z1013_16 = (1<<5),
    z1013_64 = (1<<6),
    none = 0,
    any_kc85 = (kc85_2|kc85_3|kc85_4),
    any_z1013 = (z1013_01|z1013_16|z1013_64)
};

enum class os_rom {
    caos_hc900,
    caos_2_2,
    caos_3_1,
    caos_3_4,
    caos_4_1,
    caos_4_2,
    z1013_mon202,
};

inline void
clear(void* ptr, int num_bytes) {
    YAKC_MEMSET(ptr, 0, num_bytes);
}

inline void
fill_random(void* ptr, int num_bytes) {
    YAKC_ASSERT((num_bytes & 0x03) == 0);

    // RAND_MAX in Visual Studio is (1<<15)
    if (RAND_MAX <= (1<<16)) {
        unsigned short* uptr = (unsigned short*)ptr;
        int num_ushorts = num_bytes >> 1;
        for (int i = 0; i < num_ushorts; i++) {
            unsigned short r = YAKC_RAND();
            *uptr++ = r;
        }
    }
    else {
        unsigned int* uptr = (unsigned int*)ptr;
        int num_uints = num_bytes >> 2;
        for (int i = 0; i < num_uints; i++) {
            unsigned int r = YAKC_RAND();
            *uptr++ = r;
        }
    }
}

} // namespace yakc
