#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::clock
    @brief system clock generator and counters
    
    Can be used to generate the main system clock and counters that can
    trigger callbacks when they reach 0. In the KC85 emulator this is
    used to generate the CPU clock at 1.75MHz(KC85/3) or 1.77MHz (KC85/4),
    the vertical blank and video scan line timings.
*/
#include "yakc/core.h"

namespace YAKC {

class clock {
public:
    /// a timer callback
    typedef void (*cb_timer)(void* userdata);

    /// initialize the clock to a base frequency
    void init(int baseFreqKHz);
    /// return number of cycles for a given time-span in micro-seconds
    int64_t cycles(int micro_seconds) const;

    /// configure a timer
    void config_timer(int index, int hz, cb_timer callback, void* userdata);
    /// advance the timers by a number of cycles
    void update(int num_cycles);

    /// the clock main frequency in KHz
    int base_freq_khz = 0;
    /// max number of timers
    static const int num_timers = 4;
    /// timer state
    struct timer_state {
        int freq_hz = 0;       // timer frequency in Hz
        int count = 0;         // how often the counter went through 0
        int value = 0;         // current counter value
        cb_timer callback = nullptr;
        void* userdata = nullptr;
    } timers[num_timers];
};

} // namespace YAKC
