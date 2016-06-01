//------------------------------------------------------------------------------
//  z1013_video.cc
//------------------------------------------------------------------------------
#include "z1013_video.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z1013_video::init() {
    clear(this->irm, sizeof(this->irm));
}

//------------------------------------------------------------------------------
void
z1013_video::reset() {
    // FIXME
}

//------------------------------------------------------------------------------
void
z1013_video::decode() {
    uint32_t* dst = LinearBuffer;
    for (int y = 0; y < 32; y++) {
        for (int py = 0; py < 8; py++) {
            for (int x = 0; x < 32; x++) {
                ubyte ascii = this->irm[(y<<5) + x];
                ubyte bits = dump_z1013_font[(ascii<<3)|py];
                for (int px = 7; px >=0; px--) {
                    *dst++ = bits & (1<<px) ? 0xFFFFFFFF : 0xFF000000;
                }
            }
        }
    }
}

} // namespace YAKC
