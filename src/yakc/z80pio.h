#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z80pio
    @brief incomplete Z80 PIO emulation (only what needed for KC85)
    
    FIXME: control mode 0x7 is currently not supported
    http://www.cpcwiki.eu/index.php/Z80_PIO
*/
#include "yakc/z80int.h"

namespace YAKC {

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

} // namespace YAKC
