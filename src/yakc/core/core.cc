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

//------------------------------------------------------------------------------
system
system_from_string(const char* str) {
    if (strcmp(str, "kc85_2")==0) return system::kc85_2;
    if (strcmp(str, "kc85_3")==0) return system::kc85_3;
    if (strcmp(str, "kc85_4")==0) return system::kc85_4;
    if (strcmp(str, "z1013_01")==0) return system::z1013_01;
    if (strcmp(str, "z1013_16")==0) return system::z1013_16;
    if (strcmp(str, "z1013_64")==0) return system::z1013_64;
    if (strcmp(str, "z9001")==0) return system::z9001;
    if (strcmp(str, "kc87")==0) return system::kc87;
    if (strcmp(str, "zxspectrum48k")==0) return system::zxspectrum48k;
    if (strcmp(str, "zxspectrum128k")==0) return system::zxspectrum128k;
    if (strcmp(str, "cpc464")==0) return system::cpc464;
    if (strcmp(str, "cpc6128")==0) return system::cpc6128;
    if (strcmp(str, "kccompact")==0) return system::kccompact;
    if (strcmp(str, "bbcmicro_b")==0) return system::bbcmicro_b;
    if (strcmp(str, "acorn_atom")==0) return system::acorn_atom;
    return system::none;
}

//------------------------------------------------------------------------------
os_rom
os_from_string(const char* str) {
    if (strcmp(str, "caos_hc900")==0) return os_rom::caos_hc900;
    if (strcmp(str, "caos_2_2")==0) return os_rom::caos_2_2;
    if (strcmp(str, "caos_3_1")==0) return os_rom::caos_3_1;
    if (strcmp(str, "caos_3_4")==0) return os_rom::caos_3_4;
    if (strcmp(str, "caos_4_2")==0) return os_rom::caos_4_2;
    if (strcmp(str, "z1013_mon202")==0) return os_rom::z1013_mon202;
    if (strcmp(str, "z1013_mon_a2")==0) return os_rom::z1013_mon_a2;
    if (strcmp(str, "z9001_os_1_2")==0) return os_rom::z9001_os_1_2;
    if (strcmp(str, "kc87_os_2")==0) return os_rom::kc87_os_2;
    if (strcmp(str, "amstrad_zx48k")==0) return os_rom::amstrad_zx48k;
    if (strcmp(str, "amstrad_zx128k")==0) return os_rom::amstrad_zx128k;
    if (strcmp(str, "cpc464_os")==0) return os_rom::cpc464_os;
    if (strcmp(str, "cpc464_basic")==0) return os_rom::cpc464_basic;
    if (strcmp(str, "cpc6128")==0) return os_rom::cpc6128;
    if (strcmp(str, "cpc6128_basic")==0) return os_rom::cpc6128_basic;
    if (strcmp(str, "kcc_os")==0) return os_rom::kcc_os;
    if (strcmp(str, "kcc_basic")==0) return os_rom::kcc_basic;
    if (strcmp(str, "bbcmicro_b_os")==0) return os_rom::bbcmicro_b_os;
    if (strcmp(str, "bbcmicro_b_basic")==0) return os_rom::bbcmicro_b_basic;
    return os_rom::none;
}

extern os_rom os_from_string(const char* str);


} // namespace YAKC
