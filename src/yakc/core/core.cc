//------------------------------------------------------------------------------
//  yakc/core.cc
//------------------------------------------------------------------------------
#include "core.h"
#include <string.h>
#include <stdlib.h>

namespace YAKC {

ext_funcs func;

//------------------------------------------------------------------------------
void
clear(void* ptr, int num_bytes) {
    memset(ptr, 0, num_bytes);
}

//------------------------------------------------------------------------------
void
fill_random(void* ptr, int num_bytes) {
    YAKC_ASSERT((num_bytes & 0x03) == 0);

    // RAND_MAX in Visual Studio is (1<<15)
    if (RAND_MAX <= (1<<16)) {
        unsigned short* uptr = (unsigned short*)ptr;
        int num_ushorts = num_bytes >> 1;
        for (int i = 0; i < num_ushorts; i++) {
            unsigned short r = rand();
            *uptr++ = r;
        }
    }
    else {
        unsigned int* uptr = (unsigned int*)ptr;
        int num_uints = num_bytes >> 2;
        for (int i = 0; i < num_uints; i++) {
            unsigned int r = rand();
            *uptr++ = r;
        }
    }
}

} // namespace YAKC
