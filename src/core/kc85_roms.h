#pragma once
//------------------------------------------------------------------------------
/**
    class yakc::kc85_roms
    @brief manage KC85 ROM blobs
*/
#include "core/common.h"

namespace yakc {

class kc85_roms {
public:
    /// existing ROM blobs
    enum rom {
        hc900 = 0,      // HC-900 CAOS (KC85/2)
        caos22,         // CAOS 2.2
        caos31,         // CAOS 3.1
        caos34,         // CAOS 3.4
        caos41e,        // CAOS 4.1, 4 KByte chunk at E000
        caos41c,        // CAOS 4.1, 8 KByte chunk at C000
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
    struct {
        int pos = -1;
        int size = 0;
    } roms[num_roms];

    static const int buf_size = 256 * 1024;
    int cur_pos = 0;
    ubyte buffer[buf_size];
};

//------------------------------------------------------------------------------
inline void
kc85_roms::add(rom type, const ubyte* ptr, int size) {
    YAKC_ASSERT((type >= 0) && (type < num_roms));
    YAKC_ASSERT(!this->has(type));
    YAKC_ASSERT((cur_pos + size) <= buf_size);

    YAKC_MEMCPY(&this->buffer[cur_pos], ptr, size);
    this->roms[type].pos = this->cur_pos;
    this->roms[type].size = size;
    this->cur_pos += size;
}

//------------------------------------------------------------------------------
inline bool
kc85_roms::has(rom type) const {
    YAKC_ASSERT((type >= 0) && (type < num_roms));
    return -1 != this->roms[type].pos;
}

//------------------------------------------------------------------------------
inline const ubyte*
kc85_roms::ptr(rom type) const {
    YAKC_ASSERT(this->has(type));
    return &this->buffer[this->roms[type].pos];
}

//------------------------------------------------------------------------------
inline int
kc85_roms::size(rom type) const {
    YAKC_ASSERT(this->has(type));
    return this->roms[type].size;
}

} // namespace yakc
