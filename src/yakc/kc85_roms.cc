//------------------------------------------------------------------------------
//  kc85_roms.cc
//------------------------------------------------------------------------------
#include "kc85_roms.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
kc85_roms::add(rom type, const ubyte* ptr, int size) {
    YAKC_ASSERT((type >= 0) && (type < num_roms));
    YAKC_ASSERT(!this->has(type));
    YAKC_ASSERT((cur_pos + size) <= buf_size);

    memcpy(&this->buffer[cur_pos], ptr, size);
    this->roms[type].pos = this->cur_pos;
    this->roms[type].size = size;
    this->cur_pos += size;
}

//------------------------------------------------------------------------------
bool
kc85_roms::has(rom type) const {
    YAKC_ASSERT((type >= 0) && (type < num_roms));
    return -1 != this->roms[type].pos;
}

//------------------------------------------------------------------------------
const ubyte*
kc85_roms::ptr(rom type) const {
    YAKC_ASSERT(this->has(type));
    return &this->buffer[this->roms[type].pos];
}

//------------------------------------------------------------------------------
int
kc85_roms::size(rom type) const {
    YAKC_ASSERT(this->has(type));
    return this->roms[type].size;
}

} // namespace YAKC
