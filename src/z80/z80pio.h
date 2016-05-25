#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80pio
    @brief incomplete Z80 PIO emulation (only what needed for KC85)
    
    FIXME: control mode 0x7 is currently not supported
    http://www.cpcwiki.eu/index.php/Z80_PIO
*/
#include "core/common.h"

namespace yakc {

class z80pio {
public:
    /// channels
    enum channel {
        A = 0,
        B,
        num_channels,
    };

    /// channel state
    struct channel_state_t {
        channel_state_t() :
            interrupt_vector(0),
            interrupt_control(0),
            mode(0),
            inout_select(0),
            mask(0),
            follows(any_follows)
        { };

        ubyte interrupt_vector;
        ubyte interrupt_control;    // D7=enabled, D7=and/or, D5=high/low, D4=mask follows
        ubyte mode;                 // 0=output, 1=input, 2=bidirectional, 3=bit-control
        ubyte inout_select;         // input/output control select (1-bit: in, 0-bit: out)
        ubyte mask;                 // mask in bit-control mode
        ubyte follows;
        enum : ubyte {
            any_follows = 0,        // general control word
            select_follows = 1,     // inout_select expected
            mask_follows = 2,       // interrupr mask expected
        };
    } channel_state[num_channels];

    /// channel data values
    ubyte channel_data[num_channels];
    /// interrupt controller
    z80int int_ctrl;

    /// initialize the pio
    void init();
    /// reset the pio
    void reset();

    /// write control register
    void control(channel c, ubyte m);
    /// write data register
    void write(channel c, ubyte d);
    /// read data register
    ubyte read(channel c) const;
};

//------------------------------------------------------------------------------
inline void
z80pio::init() {
    for (unsigned int i = 0; i < num_channels; i++) {
        channel_state[i] = channel_state_t();
        channel_data[i] = 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80pio::reset() {
    for (unsigned int i = 0; i < num_channels; i++) {
        auto& chn = channel_state[i];
        chn.interrupt_control = 0;
        chn.mode = 0;
        chn.inout_select = 0;
        chn.mask = 0;
        chn.follows = channel_state_t::any_follows;
        channel_data[i] = 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80pio::control(channel c, ubyte m) {
    YAKC_ASSERT((c>=0)&&(c<num_channels));

    auto& chn = channel_state[c];
    if (channel_state_t::select_follows == chn.follows) {
        chn.follows = channel_state_t::any_follows;
        chn.inout_select = m;
    }
    else if (channel_state_t::mask_follows == chn.follows) {
        chn.follows = channel_state_t::any_follows;
        chn.mask = m;
    }
    else {
        if ((m & 0x1) == 0x0) {
            // set interrupt vector
            chn.interrupt_vector = m;
        }
        else if ((m & 0xF) == 0x3) {
            // interrupt control word
            chn.interrupt_control = m;
            if (chn.interrupt_control & (1<<4)) {
                chn.follows = channel_state_t::mask_follows;
            }
        }
        else if ((m & 0xF) == 0xF) {
            // set mode ('bidirectional'=2 and 'control'=3 not supported)
            // FIXME: hmm mode 3 is used by the Z1013
            chn.mode = m>>6;
            YAKC_ASSERT(chn.mode != 2);
            if (3 == chn.mode) {
                chn.follows = channel_state_t::select_follows;
            }
        }
        else {
            // unsupported mode
            YAKC_ASSERT(false);
        }
    }
}

//------------------------------------------------------------------------------
inline void
z80pio::write(channel c, ubyte d) {
    YAKC_ASSERT((c>=0)&&(c<num_channels));
    channel_data[c] = d;
}

//------------------------------------------------------------------------------
inline ubyte
z80pio::read(channel c) const {
    YAKC_ASSERT((c>=0)&&(c<num_channels));
    return channel_data[c];
}

} // namespace yakc
