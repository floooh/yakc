//------------------------------------------------------------------------------
//  z80pio.cc
//------------------------------------------------------------------------------
#include "z80pio.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z80pio::init() {
    for (unsigned int i = 0; i < num_channels; i++) {
        channel_state[i] = channel_state_t();
        channel_data[i] = 0;
    }
}

//------------------------------------------------------------------------------
void
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
void
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
void
z80pio::write(channel c, ubyte d) {
    YAKC_ASSERT((c>=0)&&(c<num_channels));
    channel_data[c] = d;
}

//------------------------------------------------------------------------------
ubyte
z80pio::read(channel c) const {
    YAKC_ASSERT((c>=0)&&(c<num_channels));
    return channel_data[c];
}

} // namespace YAKC
