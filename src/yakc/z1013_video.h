#pragma once
//------------------------------------------------------------------------------
/**
    @class z1013_video
    @brief Z1013 video decoding hardware
*/
#include "yakc/core.h"
#include "yakc/roms.h"

namespace YAKC {

class z1013_video {
public:
    /// 1Kbyte video memory
    ubyte irm[0x400];

    /// initialize the video hardware
    void init();
    /// reset the video hardware
    void reset();
    /// decode an entire frame into LinearBuffer
    void decode();

    /// decoded linear RGBA8 video buffer
    uint32_t LinearBuffer[256*256];
};

} // namespace YAKC
