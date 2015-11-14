#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80ctc
    @brief incomplete Z80 CTC emulation (only what's needed for KC85)
*/
#include "yakc_core/common.h"
#include <stdio.h>

namespace yakc {

class z80ctc {
public:
    /// channels
    enum channel {
        CTC0 = 0,
        CTC1,
        CTC2,
        CTC3,
        
        num_channels,
    };

    /// control-word bits
    enum control_bits {
        INTERRUPT = (1<<7),         // 1: interrupt enabled, 0: interrupt disabled
        MODE = (1<<6),              // 1: counter mode, 0: timer mode
        PRESCALER = (1<<5),         // 1: prescaler 256, 0: prescaler 16
        EDGE = (1<<4),              // 1: rising edge, 0: falling edge
        TRIGGER = (1<<3),           // 1: CLK/TRG pulse starts timer, 0: automatic trigger when time constant is loaded
        TIMECONSTANT = (1<<2),      // 1: time constant followes, 0: no time constant follows
        RESET = (1<<1),             // 1: software reset, 0: continued operation
        TYPE = (1<<0),              // 1: control word, 0: vector
    };

    /// channel state
    struct {
        ubyte interrupt_vector = 0;     // interrupt vector
        ubyte control_word = 0;         // last control-word
        
    } channel_state[num_channels];

    /// initialize the ctc
    void init();
    /// reset the ctc
    void reset();
    /// update the CTC for a number of ticks, a tick is equal to a Z80 T-cycle
    void update(int ticks);

    /// write value to channel
    void write(channel c, ubyte v);
};

//------------------------------------------------------------------------------
inline void
z80ctc::init() {
    for (unsigned int i = 0; i < num_channels; i++) {
        channel_state[i].interrupt_vector = 0;
        channel_state[i].control_word = 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80ctc::reset() {
    // FIXME: ???
    for (unsigned int i = 0; i < num_channels; i++) {
        channel_state[i].interrupt_vector = 0;
        channel_state[i].control_word = 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80ctc::write(channel c, ubyte v) {
    printf("CTC%d: %02X\n", c, v);
    if (v & TYPE) {
        printf("  CONTROL WORD:\n");
        if (v & RESET) {
            printf("    RESET\n");
        }
        if (v & TIMECONSTANT) {
            printf("    TIME CONSTANT FOLLOWS\n");
        }
        if (v & TRIGGER) {
            printf("    PULSE STARTS TIMER\n");
        }
        else {
            printf("    AUTOMATIC TRIGGER\n");
        }
        if (v & EDGE) {
            printf("    RISING EDGE\n");
        }
        else {
            printf("    FALLING EDGE\n");
        }
        if (v & PRESCALER) {
            printf("    PRESCALER 256\n");
        }
        else {
            printf("    PRESCALER 16\n");
        }
        if (v & MODE) {
            printf("    COUNTER MODE\n");
        }
        else {
            printf("    TIMER MODE\n");
        }
        if (v & INTERRUPT) {
            printf("    INTERRUPT ENABLED\n");
        }
        else {
            printf("    INTERRUPT DISABLED\n");
        }
    }
    else {
        printf("  VECTOR\n");
    }
}

} // namespace yakc
    