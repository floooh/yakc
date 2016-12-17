#pragma once
//------------------------------------------------------------------------------
/**
    class YAKC::rom_images
    @brief storage for dynamically loaded ROM images
*/
#include "yakc/core.h"

namespace YAKC {

class rom_images {
public:
    /// existing ROM blobs
    enum rom {
        hc900 = 0,          // HC-900 CAOS (KC85/2)
        caos22,             // CAOS 2.2
        caos31,             // CAOS 3.1
        caos34,             // CAOS 3.4
        caos42e,            // CAOS 4.2, 4 KByte chunk at E000
        caos42c,            // CAOS 4.2, 8 KByte chunk at C000
        kc85_basic_rom,     // BASIC ROM, KC85/3 and KC85/4
        kc85_basic_mod,     // BASIC+HC-901 CAOS ROM MODULE, KC85/2
        forth,              // KC85 FORTH module
        develop,            // KC85 DEVELOP module
        texor,              // KC85 TEXOR module
        zx48k,              // ZX Spectrum 48K ROM
        zx128k_0,           // ZX Spectrum 128K ROM0
        zx128k_1,           // ZX Spectrum 128K ROM1
        cpc464_os,          // CPC 464 OS ROM
        cpc464_basic,       // CPC 464 BASIC ROM
        cpc6128_os,         // CPC 6128 OS ROM
        cpc6128_basic,      // CPC 6128 BASIC ROM

        num_roms
    };

    /// add a ROM blob
    void add(rom type, const ubyte* ptr, int size);
    /// test if a ROM blob had been added
    bool has(rom type) const;
    /// get the pointer to a rom blob
    ubyte* ptr(rom type);
    /// get the size of a rom blob
    int size(rom type) const;

private:
    struct item {
        item() : pos(-1), size(0) { };
        int pos;
        int size;
    } roms[num_roms];

    static const int buf_size = 1024 * 1024;
    int cur_pos = 0;
    ubyte buffer[buf_size];
};

} // namespace YAKC
