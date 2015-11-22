#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80ctc
    @brief incomplete Z80 CTC emulation (only what's needed for KC85)
    
    CTC channels 0 and 1 seem to be triggered per video scanline,
    and channels 2 and 3 once per vertical refresh @50Hz 
    (see here: https://github.com/mamedev/mame/blob/dfa148ff8022e9f1a544c8603dd0e8c4aa469c1e/src/mame/machine/kc.cpp#L710)
*/
#include "yakc_core/common.h"
#include "yakc_core/z80int.h"

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

    /// CTC channel mode bits
    enum mode_bits {
        INTERRUPT = (1<<7),         // 1: interrupt enabled, 0: interrupt disabled
        INTERRUPT_ENABLED = INTERRUPT,
        INTERRUPT_DISABLED = 0,

        MODE = (1<<6),              // 1: counter mode, 0: timer mode
        MODE_COUNTER = MODE,
        MODE_TIMER = 0,

        PRESCALER = (1<<5),         // 1: prescaler 256, 0: prescaler 16
        PRESCALER_256 = PRESCALER,
        PRESCALER_16 = 0,

        EDGE = (1<<4),              // 1: rising edge, 0: falling edge
        EDGE_RISING = EDGE,
        EDGE_FALLING = 0,

        TRIGGER = (1<<3),           // 1: CLK/TRG pulse starts timer, 0: automatic trigger when time constant is loaded
        TRIGGER_PULSE = TRIGGER,
        TRIGGER_AUTOMATIC = 0,

        CONSTANT_FOLLOWS = (1<<2),          // 1: time constant followes, 0: no time constant follows

        RESET = (1<<1),             // 1: software reset, 0: continued operation

        CONTROL = (1<<0),           // 1: control word, 0: vector
        CONTROL_WORD = CONTROL,
        CONTROL_VECTOR = 0,
    };

    /// callback for ZC/TO0..3 lines
    typedef void (*ctc_cb)(void* userdata);

    /// channel state
    struct channel_state {
        ubyte mode = RESET;             // current mode bits
        ubyte constant = 0;             // the time constant
        int down_counter = 0;           // current down-counter value
        bool waiting_for_trigger = false;
        ctc_cb zcto_callback = nullptr;
        void* zcto_userdata = nullptr;
        ubyte interrupt_vector = 0;
    } channels[num_channels];

    /// interrupt controller
    z80int int_ctrl;

    /// initialize the ctc
    void init();
    /// reset the ctc
    void reset();
    /// update the CTC for a number of ticks, a tick is equal to a Z80 T-cycle
    void update_timers(int ticks);

    /// set callback for ZC/TO0 line (zero-count/time-out)
    void connect_zcto0(ctc_cb cb, void* userdata);
    /// set callback for ZC/TO1 line
    void connect_zcto1(ctc_cb cb, void* userdata);
    /// set callback for ZC/TO2 line
    void connect_zcto2(ctc_cb cb, void* userdata);

    /// trigger line for CTC0
    static void ctrg0(void* self);
    /// trigger line for CTC1
    static void ctrg1(void* self);
    /// trigger line for CTC2
    static void ctrg2(void* self);
    /// trigger line for CTC3
    static void ctrg3(void* self);

    /// write value to channel
    void write(channel c, ubyte v);
    /// read value from channel
    ubyte read(channel c);

private:
    /// get the counter/timer cycle count (prescaler * constant)
    int down_counter_init(const channel_state& chn) const;
    /// execute actions when down_counter reaches zero
    void down_counter_callback(channel_state& c);
    /// external trigger, called from trg0..trg3
    void update_counter(channel_state& chn);
};

//------------------------------------------------------------------------------
inline void
z80ctc::init() {
    for (auto& chn : channels) {
        chn = channel_state();
    }
}

//------------------------------------------------------------------------------
inline void
z80ctc::reset() {
    // don't clear callbacks on reset
    for (auto& chn : channels) {
        chn.mode = RESET;
        chn.constant = 0;
        chn.down_counter = 0;
        chn.waiting_for_trigger = false;
    }
}

//------------------------------------------------------------------------------
inline void
z80ctc::write(channel c, ubyte v) {
    YAKC_ASSERT((c >= 0) && (c<num_channels));

    channel_state& chn = channels[c];
    if (chn.mode & CONSTANT_FOLLOWS) {
        // time constant value following a control word
        chn.constant = v;
        chn.down_counter = down_counter_init(chn);
        chn.mode &= ~(CONSTANT_FOLLOWS|RESET);
        if ((chn.mode & MODE) == MODE_TIMER) {
            chn.waiting_for_trigger = (chn.mode & TRIGGER) == TRIGGER_PULSE;
        }
    }
    else if ((v & CONTROL) == CONTROL_WORD) {
        // a control word
        chn.mode = v;
    }
    else {
        // an interrupt vector for the whole CTC must be written
        // to channel 0, interrupt vectors for following channels
        // are computed from the base vector plus 2 byte per channel
        if (CTC0 == c) {
            for (int i = 0; i < num_channels; i++) {
                channels[i].interrupt_vector = (v & 0xF8) + 2*i;
            }
        }
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80ctc::read(channel c) {
YAKC_ASSERT(false); // not sure when CTC channels are read, so assert when it happens
    YAKC_ASSERT((c >= 0) && (c<num_channels));
    const channel_state& chn = channels[c];
    int val = chn.down_counter;
    if ((chn.mode & MODE) == MODE_TIMER) {
        val /= ((chn.mode & PRESCALER) == PRESCALER_256) ? 256 : 16;
    }
    return (ubyte) val;
}

//------------------------------------------------------------------------------
inline int
z80ctc::down_counter_init(const channel_state& chn) const {
    int c = (0 == chn.constant) ? 0x100 : chn.constant;
    if ((chn.mode & MODE) == MODE_TIMER) {
        c *= ((chn.mode & PRESCALER) == PRESCALER_256) ? 256 : 16;
    }
    return c;
}

//------------------------------------------------------------------------------
inline void
z80ctc::update_timers(int ticks) {
    for (int i = 0; i < num_channels; i++) {
        channel_state& chn = channels[i];
        if (!(chn.mode & RESET)) {
            if (((chn.mode & MODE) == MODE_TIMER) && !chn.waiting_for_trigger) {
                chn.down_counter -= ticks;
                while (chn.down_counter <= 0) {
                    down_counter_callback(chn);
                    chn.down_counter += down_counter_init(chn);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
inline void
z80ctc::update_counter(channel_state& chn) {
    if (!(chn.mode & RESET)) {
        if ((chn.mode & MODE) == MODE_COUNTER) {
            if (--chn.down_counter == 0) {
                down_counter_callback(chn);
            }
        }
        chn.waiting_for_trigger = false;
    }
}

//------------------------------------------------------------------------------
inline void
z80ctc::down_counter_callback(channel_state& chn) {
    if ((chn.mode & INTERRUPT) == INTERRUPT_ENABLED) {
        this->int_ctrl.request_interrupt(chn.interrupt_vector);
    }
    if (chn.zcto_callback) {
        chn.zcto_callback(chn.zcto_userdata);
    }
    chn.down_counter = down_counter_init(chn);
}

//------------------------------------------------------------------------------
inline void
z80ctc::connect_zcto0(ctc_cb cb, void* userdata) {
    channels[CTC0].zcto_callback = cb;
    channels[CTC0].zcto_userdata = userdata;
}

//------------------------------------------------------------------------------
inline void
z80ctc::connect_zcto1(ctc_cb cb, void* userdata) {
    channels[CTC1].zcto_callback = cb;
    channels[CTC1].zcto_userdata = userdata;
}

//------------------------------------------------------------------------------
inline void
z80ctc::connect_zcto2(ctc_cb cb, void* userdata) {
    channels[CTC2].zcto_callback = cb;
    channels[CTC2].zcto_userdata = userdata;
}

//------------------------------------------------------------------------------
inline void
z80ctc::ctrg0(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(self->channels[CTC0]);
}

//------------------------------------------------------------------------------
inline void
z80ctc::ctrg1(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(self->channels[CTC1]);
}

//------------------------------------------------------------------------------
inline void
z80ctc::ctrg2(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(self->channels[CTC2]);
}

//------------------------------------------------------------------------------
inline void
z80ctc::ctrg3(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(self->channels[CTC3]);
}

} // namespace yakc
    