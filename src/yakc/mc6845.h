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
    void write(uint8_t val);
    /// read selected register
    uint8_t read() const;
    /// read special type 1 (UM6845R) status register
    uint8_t read_status() const;

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
    uint8_t regs[NUM_REGS] = { };

    /// current memory address (14 bits)
    uint16_t ma = 0;
    /// current row address (5 bits)
    uint16_t ra = 0;
    /// test if any status bit is set
    bool test(uint8_t mask) const;
    /// check if any status bits have changed state to 'on' this step
    bool on(uint8_t mask) const;
    /// check if any status bits have changed state to 'off' this step
    bool off(uint8_t mask) const;

    /// horizontal counter
    int h_count;
    /// hsync counter
    int hsync_count;
    /// character row counter
    int row_count;
    /// vsync counter
    int vsync_count;
    /// scanline counter
    int scanline_count;
    /// vtotal_adjust additional scanline counter
    int adjust_scanline_count;

    /// set a status bit
    void set(uint8_t mask);
    /// clear a status bit
    void clear(uint8_t mask);

    int type = int(type::MC6845);
    int reg_sel = 0;                // currently selected register
    uint8_t prev_bits = 0;
    uint8_t bits = 0;
    uint16_t ma_row_start = 0;      // memory address at row start
};

//------------------------------------------------------------------------------
inline bool
mc6845::test(uint8_t mask) const {
    return 0 != (this->bits & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6845::on(uint8_t mask) const {
    return 0 != ((this->bits & (this->bits ^ this->prev_bits)) & mask);
}

//------------------------------------------------------------------------------
inline bool
mc6845::off(uint8_t mask) const {
    return 0 != ((~this->bits & (this->bits ^ this->prev_bits)) & mask);
}

//------------------------------------------------------------------------------
inline void
mc6845::set(uint8_t mask) {
    this->bits |= mask;
}

//------------------------------------------------------------------------------
inline void
mc6845::clear(uint8_t mask) {
    this->bits &= ~mask;
}

} // namespace YAKC
