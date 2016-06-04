#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z80ctc
    @brief incomplete Z80 CTC emulation (only what's needed for KC85)
    
    CTC channels 0 and 1 seem to be triggered per video scanline,
    and channels 2 and 3 once per vertical refresh @50Hz 
    (see here: https://github.com/mamedev/mame/blob/dfa148ff8022e9f1a544c8603dd0e8c4aa469c1e/src/mame/machine/kc.cpp#L710)
*/
#include "yakc/core.h"
#include "yakc/z80.h"
#include "yakc/z80int.h"

namespace YAKC {

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
        ubyte interrupt_vector = 0;
        ctc_cb zcto_callback = nullptr;
        void* zcto_userdata = nullptr;
        ctc_cb write_callback = nullptr;
        void* write_userdata = nullptr;
        z80int int_ctrl;
    } channels[num_channels];

    /// initialize the ctc
    void init();
    /// initialize the downstream daisy chain
    void init_daisychain(z80int* downstream);

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

    /// connect a write callback to channel0 (on mode or constant written)
    void connect_write0(ctc_cb cb, void* userdata);
    /// connect a write callback to channel1
    void connect_write1(ctc_cb cb, void* userdata);
    /// connect a write callback to channel2
    void connect_write2(ctc_cb cb, void* userdata);
    /// connect a write callback to channel3
    void connect_write3(ctc_cb cb, void* userdata);

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

} // namespace YAKC
    