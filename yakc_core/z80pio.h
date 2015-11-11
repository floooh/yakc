#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80pio
    @brief hold the Z80 PIO state
    
    FIXME: control mode 0x7 is currently not supported
    http://www.cpcwiki.eu/index.php/Z80_PIO
*/
#include "yakc_core/common.h"

namespace yakc {

class z80pio {
public:
    /// channels
    enum channel {
        A = 0,
        B = 1,
        num_channels = 2
    };

    /// channel state
    struct {
        ubyte interrupt_vector = 0;
        bool interrupt_enabled = false;
        ubyte mode = 0;     // 0=out, 1=in, 2=bi, 3=control
    } channel_state[num_channels];

    /// channel data values
    ubyte channel_data[num_channels]={0};

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
    for (int i = 0; i < num_channels; i++) {
        channel_state[i].interrupt_vector = 0;
        channel_state[i].interrupt_enabled = false;
        channel_state[i].mode = 0;
        channel_data[i] = 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80pio::reset() {
    for (int i = 0; i < num_channels; i++) {
        channel_state[i].interrupt_enabled = false;
        channel_state[i].mode = 0;
        channel_data[i] = 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80pio::control(channel c, ubyte m) {
    YAKC_ASSERT((c>=0)&&(c<num_channels));

    if ((m & 0x1) == 0x0) {
        // set interrupt vector
        channel_state[c].interrupt_vector = m;
    }
    else if ((m & 0xF) == 0x3) {
        // interrupt enable/disable
        channel_state[c].interrupt_enabled = m & 0x80;
    }
    else if ((m & 0xF) == 0xF) {
        // set mode ('bidirectional'=2 and 'control'=3 not supported)
        channel_state[c].mode = m>>6;
        YAKC_ASSERT(channel_state[c].mode < 2);
    }
    else {
        // unsupported mode
        YAKC_ASSERT(false);
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
