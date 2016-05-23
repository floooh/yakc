#pragma once
//------------------------------------------------------------------------------
/**
    @class z1013_video
    @brief Z1013 video decoding hardware
*/
#include "core/common.h"

namespace yakc {

class z1013_video {
public:
    /// 1Kbyte video memory
    ubyte irm[0x400];

    /// initialize the video hardware
    void init();
    /// reset the video hardware
    void reset();

    /// decoded linear RGBA8 video buffer
    unsigned int LinearBuffer[256*256];
};

//------------------------------------------------------------------------------
inline void
z1013_video::init() {
    clear(this->irm, sizeof(this->irm));
}

//------------------------------------------------------------------------------
inline void
z1013_video::reset() {
    // FIXME
}

} // namespace yakc
