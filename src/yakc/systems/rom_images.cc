//------------------------------------------------------------------------------
//  rom_images.cc
//------------------------------------------------------------------------------
#include "rom_images.h"

namespace YAKC {
uint8_t rom_images::buffer[rom_images::buf_size];
rom_images roms;

//------------------------------------------------------------------------------
void
rom_images::add(rom type, const uint8_t* ptr, int size) {
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
rom_images::has(rom type) const {
    YAKC_ASSERT((type >= 0) && (type < num_roms));
    return -1 != this->roms[type].pos;
}

//------------------------------------------------------------------------------
uint8_t*
rom_images::ptr(rom type) {
    YAKC_ASSERT(this->has(type));
    return &this->buffer[this->roms[type].pos];
}

//------------------------------------------------------------------------------
int
rom_images::size(rom type) const {
    YAKC_ASSERT(this->has(type));
    return this->roms[type].size;
}

} // namespace YAKC
