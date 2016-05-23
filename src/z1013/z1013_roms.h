#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z1013_roms
    @brief manage z1013 ROM data blobs
*/
#include "core/common.h"

namespace yakc {

class z1013_roms {
public:
    /// existing ROM blobs
    enum rom {
        mon202 = 0,
        font,

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

    static const int buf_size = 32 * 1024;
    int cur_pos = 0;
    ubyte buffer[buf_size];
};

//------------------------------------------------------------------------------
inline void
z1013_roms::add(rom type, const ubyte* ptr, int size) {
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
z1013_roms::has(rom type) const {
    YAKC_ASSERT((type >= 0) && (type < num_roms));
    return -1 != this->roms[type].pos;
}

//------------------------------------------------------------------------------
inline const ubyte*
z1013_roms::ptr(rom type) const {
    YAKC_ASSERT(this->has(type));
    return &this->buffer[this->roms[type].pos];
}

//------------------------------------------------------------------------------
inline int
z1013_roms::size(rom type) const {
    YAKC_ASSERT(this->has(type));
    return this->roms[type].size;
}

} // namespace yakc
