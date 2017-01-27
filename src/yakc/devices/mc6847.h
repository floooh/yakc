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
    /// video memory read callback
    typedef uint8_t(*read_func)(uint16_t addr);

    /// initialize the 6847
    void init(read_func vidmem_reader_func, int tick_khz);
    /// perform a reset
    void reset();
    /// step the chip one clock cycle
    void step();

    /// pin status bits
    enum {
        HSYNC   = (1<<0),   // out: set if in horizontal sync
        FSYNC   = (1<<1),   // out: set if in field sync
        A_G     = (1<<2),   // in: the A/G pin (graphics)
        A_S     = (1<<3),   // in: the A/S pin (semigraphics)
        INT_EXT = (1<<4),   // in: the INT/EXT pin (internal/external character rom)
        GM0     = (1<<5),   // in: the GM0 pin (graphics mode, bit 0)
        GM1     = (1<<6),   // in: the GM1 pin (graphics mode, bit 1)
        GM2     = (1<<7),   // in: the GM2 pin (graphics mode, bit 2)
        CSS     = (1<<8),   // in: the CSS pin (color set selection)
        INV     = (1<<9),   // in: the INV pin
    };

    /// test if any status bit is set
    bool test(uint16_t mask) const;
    /// check if any status bits have changed state to 'on' this step
    bool on(uint16_t mask) const;
    /// check if any status bits have changed state to 'off' this step
    bool off(uint16_t mask) const;
    /// set a status bit
    void set(uint16_t mask);
    /// clear a status bit
    void clear(uint16_t mask);
    /// set or clear the A_G bit
    void ag(bool b);
    /// set or clear the A_S bit
    void as(bool b);
    /// set or clear the GM0 bit
    void gm0(bool b);
    /// set or clear the GM1 bit
    void gm1(bool b);
    /// set or clear the GM2 bit
    void gm2(bool b);
    /// set or clear the INV bit
    void inv(bool b);
    /// set or clear the INT_EXT bit
    void int_ext(bool b);

    /// decode one scanline (generic)
    void decode_line(int y);
    /// decode on alpha-numeric mode scanline
    void decode_line_alnum(uint32_t* ptr, int y);

    /// fixed-point precision multiplicator for counters
    static const int prec = 8;

    read_func read_addr_func = nullptr;
    uint16_t prev_bits = 0;
    uint16_t bits = 0;

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
mc6847::test(uint16_t mask) const {
    return 0 != (this->bits & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6847::on(uint16_t mask) const {
    return 0 != ((this->bits & (this->bits ^ this->prev_bits)) & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6847::off(uint16_t mask) const {
    return 0 != ((~this->bits & (this->bits ^ this->prev_bits)) & mask);
}

//------------------------------------------------------------------------------
inline void
mc6847::ag(bool b) {
    if (b) bits |= A_G;
    else   bits &= ~A_G;
}

//------------------------------------------------------------------------------
inline void
mc6847::as(bool b) {
    if (b) bits |= A_S;
    else   bits &= ~A_S;
}

//------------------------------------------------------------------------------
inline void
mc6847::gm0(bool b) {
    if (b) bits |= GM0;
    else   bits &= ~GM0;
}

//------------------------------------------------------------------------------
inline void
mc6847::gm1(bool b) {
    if (b) bits |= GM1;
    else   bits &= ~GM1;
}

//------------------------------------------------------------------------------
inline void
mc6847::gm2(bool b) {
    if (b) bits |= GM2;
    else   bits &= ~GM2;
}

//------------------------------------------------------------------------------
inline void
mc6847::inv(bool b) {
    if (b) bits |= INV;
    else   bits &= ~INV;
}

//------------------------------------------------------------------------------
inline void
mc6847::int_ext(bool b) {
    if (b) bits |= INT_EXT;
    else   bits &= ~INT_EXT;
}

} // namespace YAKC

    
