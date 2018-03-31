//------------------------------------------------------------------------------
//  yakc/core.cc
//------------------------------------------------------------------------------
#include "core.h"

namespace YAKC {

ext_funcs func;

//------------------------------------------------------------------------------
void
clear(void* ptr, int num_bytes) {
    memset(ptr, 0, num_bytes);
}

//------------------------------------------------------------------------------
static uint32_t xorshift_state = 0x6D98302B;
static uint32_t xorshift32() {
    uint32_t x = xorshift_state;
    x ^= x<<13; x ^= x>>17; x ^= x<<5;
    xorshift_state = x;
    return x;
}

//------------------------------------------------------------------------------
void
fill_random(void* ptr, int num_bytes) {
    YAKC_ASSERT((num_bytes & 0x03) == 0);
    uint32_t* uptr = (uint32_t*)ptr;
    for (int i = 0; i < (num_bytes/4); i++) {
        *uptr++ = xorshift32();
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
    if (strcmp(str, "acorn_atom")==0) return system::acorn_atom;
    if (strcmp(str, "c64_pal")==0) return system::c64_pal;
    if (strcmp(str, "c64_ntsc")==0) return system::c64_ntsc;
    if (strcmp(str, "any_kc85")==0) return system::any_kc85;
    if (strcmp(str, "any_z1013")==0) return system::any_z1013;
    if (strcmp(str, "any_z9001")==0) return system::any_z9001;
    if (strcmp(str, "any_zx")==0) return system::any_zx;
    if (strcmp(str, "any_cpc")==0) return system::any_cpc;
    if (strcmp(str, "any_c64")==0) return system::any_c64;
    if (strcmp(str, "any")==0) return system::any;
    return system::none;
}

//------------------------------------------------------------------------------
const char*
string_from_system(system sys) {
    switch (sys) {
        case system::kc85_2:            return "kc85_2";
        case system::kc85_3:            return "kc85_3";
        case system::kc85_4:            return "kc85_4";
        case system::z1013_01:          return "z1013_01";
        case system::z1013_16:          return "z1013_16";
        case system::z1013_64:          return "z1013_64";
        case system::z9001:             return "z9001";
        case system::kc87:              return "kc87";
        case system::zxspectrum48k:     return "zxspectrum48k";
        case system::zxspectrum128k:    return "zxspectrum128k";
        case system::cpc464:            return "cpc464";
        case system::cpc6128:           return "cpc6128";
        case system::kccompact:         return "kccompact";
        case system::acorn_atom:        return "acorn_atom";
        case system::c64_pal:           return "c64_pal";
        case system::c64_ntsc:          return "c64_ntsc";
        default:                        return "none";
    }
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
    return os_rom::none;
}

extern os_rom os_from_string(const char* str);

} // namespace YAKC
