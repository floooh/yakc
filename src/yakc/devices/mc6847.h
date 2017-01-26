#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::mc6847
    @brief Motorola MC6847 video display generator emulation
    
    Since the 6847 chip is always clocked at an odd 3.58 MHz the simulation
    is only cycle-correct at main-system-clock cycles (for instance 1 MHz).
    
    https://people.ece.cornell.edu/land/courses/ece4760/ideas/mc6847.pdf
    http://stardot.org.uk/forums/viewtopic.php?t=10025
    
    Timings:

    60 Hz refresh rate
    228 ticks @3.58MHz for full scanline
    262 lines for entire field:
        - 13 lines vertical blank
        - 25 lines top border
        - 192 lines active display area
        - 26 lines bottom border
        - 6 lines vertical retrace
        - 13+25+192+26+6 = 262
*/
#include "yakc/core/core.h"

namespace YAKC {

class mc6847 {
public:
    /// initialize the 6847
    void init(int tick_khz);
    /// perform a reset
    void reset();
    /// step the chip one clock cycle
    void step();

    /// status bits
    enum {
        HSYNC  = (1<<0),    // set if in horizontal sync
        FSYNC  = (1<<1),    // set if in field sync
    };

    /// test if any status bit is set
    bool test(uint8_t mask) const;
    /// check if any status bits have changed state to 'on' this step
    bool on(uint8_t mask) const;
    /// check if any status bits have changed state to 'off' this step
    bool off(uint8_t mask) const;
    /// set a status bit
    void set(uint8_t mask);
    /// clear a status bit
    void clear(uint8_t mask);

    /// decode one scanline
    void decode_line(int y);

    /// fixed-point precision multiplicator for counters
    static const int prec = 8;

    uint8_t prev_bits = 0;
    uint8_t bits = 0;

    static const int l_all = 262;
    static const int l_vblank = 13;     // 13 lines vblank at top of screen
    static const int l_topborder = 25;  // 25 lines top border
    static const int l_disp = 192;      // 192 lines display area
    static const int l_btmborder = 26;  // 26 lines bottom border
    static const int l_vretrace = 6;    // 6 lines vertical retrace
    static const int l_disp_start = l_vblank + l_topborder;
    static const int l_disp_end = l_disp_start + l_disp;
    static const int l_limit = 262;
    static const int l_fsync_start = l_disp_end;    // fsync is active until l_limit
    static_assert(l_all == (l_vblank+l_topborder+l_disp+l_btmborder+l_vretrace), "mc6847 line count");

    int h_count = 0;
    int h_sync_start = 0;
    int h_sync_end = 0;
    int h_limit = 0;
    int l_count = 0;

    static const int disp_width = 256;
    static const int disp_height = 192;
    uint32_t rgba8_buffer[disp_width * disp_height];
};

//------------------------------------------------------------------------------
inline bool
mc6847::test(uint8_t mask) const {
    return 0 != (this->bits & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6847::on(uint8_t mask) const {
    return 0 != ((this->bits & (this->bits ^ this->prev_bits)) & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6847::off(uint8_t mask) const {
    return 0 != ((~this->bits & (this->bits ^ this->prev_bits)) & mask);
}

} // namespace YAKC

    
