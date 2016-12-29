#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::mc6845
    @brief Motorola MC6845 cathode-ray-tube-controller
    
    http://www.cpcwiki.eu/index.php/CRTC    
    http://www.cpcwiki.eu/imgs/d/da/Mc6845.motorola.pdf
    
    NOTE: light pen functionality is not implemented
*/
#include "yakc/core.h"

namespace YAKC {

class mc6845 {
public:
    /// subtypes with slightly different behaviour
    enum class type {
        UM6845 = 0, // CPC 'type 0'
        UM6845R,    // CPC 'type 1'
        MC6845,     // CPC 'type 2'

        NUM,
    };

    /// initialize the MC6845 instance
    void init(type t);
    /// reset the MC6845
    void reset();
    /// step the chip one clock cycle
    void step();
    /// select a register (0..17) for reading/writing
    void select(int r);
    /// write selected register
    void write(ubyte val);
    /// read selected register
    ubyte read() const;

    /// status bits
    enum {
        HSYNC  = (1<<0),
        VSYNC  = (1<<1),
        DISPEN = (1<<2),
        CURSOR = (1<<3),

        // internal
        DISPEN_H = (1<<4),
        DISPEN_V = (1<<5),
    };

    /// register indices
    enum {
        H_TOTAL = 0,
        H_DISPLAYED,
        H_SYNC_POS,
        H_SYNC_WIDTH,
        V_TOTAL,
        V_TOTAL_ADJUST,
        V_DISPLAYED,
        V_SYNC_POS,
        INTERLACE_MODE,
        MAX_SCANLINE_ADDR,
        CURSOR_START,
        CURSOR_END,
        START_ADDR_HI,
        START_ADDR_LO,
        CURSOR_HI,
        CURSOR_LO,
        LIGHTPEN_HI,
        LIGHTPEN_LO,

        NUM_REGS,
    };
    static_assert(NUM_REGS == 18, "mc6845 num registers");
    /// the register file
    ubyte regs[NUM_REGS] = { };

    /// current memory address (14 bits)
    uword ma = 0;
    /// current row address (5 bits)
    uword ra = 0;
    /// test if any status bit is set
    bool test(ubyte mask) const;
    /// check if any status bits have changed state to 'on' this step
    bool on(ubyte mask) const;
    /// check if any status bits have changed state to 'off' this step
    bool off(ubyte mask) const;

    /// horizontal counter
    int h_count = 0;
    /// hsync counter
    int hsync_count = 0;
    /// character row counter
    int row_count = 0;
    /// vsync counter
    int vsync_count = 0;
    /// scanline counter
    int scanline_count = 0;
    /// vtotal_adjust additional scanline counter
    int adjust_scanline_count = 0;

    /// step the horizontal counter
    void step_h_count();
    /// step the scanline counter
    void step_scanline_count();
    /// step the row counter
    void step_row_count();
    /// set a status bit
    void set(ubyte mask);
    /// clear a status bit
    void clear(ubyte mask);

    int type = int(type::MC6845);
    int reg_sel = 0;                // currently selected register
    ubyte set_bits = 0;
    ubyte cleared_bits = 0;
    ubyte bits = 0;
    uword ma_row_start = 0;         // memory address at row start
};

//------------------------------------------------------------------------------
inline bool
mc6845::test(ubyte mask) const {
    return 0 != (this->bits & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6845::on(ubyte mask) const {
    return 0 != (this->set_bits & mask);
//    return 0 != ((this->bits & (this->bits ^ this->prev_bits)) & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6845::off(ubyte mask) const {
    return 0 != (this->cleared_bits & mask);
//    return 0 != ((~this->bits & (this->bits ^ this->prev_bits)) & mask);
}

//------------------------------------------------------------------------------
inline void
mc6845::set(ubyte mask) {
    this->set_bits |= ~this->bits & mask;
    this->bits |= mask;
}

//------------------------------------------------------------------------------
inline void
mc6845::clear(ubyte mask) {
    this->cleared_bits |= this->bits & mask;
    this->bits &= ~mask;
}

} // namespace YAKC
