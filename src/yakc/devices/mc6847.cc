//------------------------------------------------------------------------------
//  mc6847.cc
//------------------------------------------------------------------------------
#include "mc6847.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
mc6847::init(int cpu_khz) {
    // the 6847 is clocked at 3.58 MHz, the CPU clock is lower, thus
    // need to compute tick counter limit in CPU ticks
    const int vdg_khz = 3580;

    // one scanline is 228 3.58 MHz ticks, increase fixed point precision
    h_limit = (228 * cpu_khz * prec) / vdg_khz;
    h_sync_start = (10 * cpu_khz * prec) / vdg_khz;
    h_sync_end = (26 * cpu_khz * prec) / vdg_khz;
    h_count = 0;
    l_count = 0;
    bits = 0;
}

//------------------------------------------------------------------------------
void
mc6847::reset() {
    h_count = 0;
    l_count = 0;
    clear(FSYNC);
}

//------------------------------------------------------------------------------
void
mc6847::step() {
    prev_bits = bits;
    h_count += prec;
    if ((h_count >= h_sync_start) && (h_count < h_sync_end)) {
        // HSYNC on
        bits |= HSYNC;
        if (l_count == l_disp_end) {
            // switch FSYNC on
            bits |= FSYNC;
        }
    }
    else {
        // HSYNC off
        bits &= ~HSYNC;
    }
    if (h_count >= h_limit) {
        h_count -= h_limit;
        l_count++;
        if (l_count >= l_limit) {
            // rewind line counter, FSYNC off
            l_count = 0;
            bits &= ~FSYNC;
        }
        if ((l_count >= l_disp_start) && (l_count < l_disp_end)) {
            decode_line(l_count - l_disp_start);
        }
    }
}

//------------------------------------------------------------------------------
void
mc6847::decode_line(int y) {
    if (y < disp_height) {
        uint32_t* ptr = &(this->rgba8_buffer[y * disp_width]);
        uint8_t r = (y & 7) << 5;
        for (int x = 0; x < disp_width; x++) {
            uint8_t g = (x & 7) << 5;
            *ptr++ = 0xFF000000 | (g<<8) | r;
        }
    }
}

} // namespace YAKC
