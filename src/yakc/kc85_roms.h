#pragma once
//------------------------------------------------------------------------------
/**
    class YAKC::kc85_roms
    @brief manage KC85 ROM blobs
*/
#include "yakc/core.h"

namespace YAKC {

class kc85_roms {
public:
    /// existing ROM blobs
    enum rom {
        hc900 = 0,      // HC-900 CAOS (KC85/2)
        caos22,         // CAOS 2.2
        caos31,         // CAOS 3.1
        caos34,         // CAOS 3.4
        caos42e,        // CAOS 4.2, 4 KByte chunk at E000
        caos42c,        // CAOS 4.2, 8 KByte chunk at C000
        basic_rom,      // BASIC ROM, KC85/3 and KC85/4
        basic_mod,      // BASIC+HC-901 CAOS ROM MODULE, KC85/2
        forth,          // FORTH module
        develop,        // DEVELOP module
        texor,          // TEXOR module

        num_roms
    };

    /// add a ROM blob
    void add(rom type, const ubyte* ptr, int size);
    /// test if a ROM blob had been added
    bool has(rom type) const;
    /// get the pointer to a rom blob
    const ubyte* ptr(rom type) const;
    /// get the size of a rom blob
    int size(rom type) const;

private:
    struct item {
        item() : pos(-1), size(0) { };
        int pos;
        int size;
    } roms[num_roms];

    static const int buf_size = 256 * 1024;
    int cur_pos = 0;
    ubyte buffer[buf_size];
};

} // namespace YAKC
