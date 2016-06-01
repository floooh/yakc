#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::clock
    @brief system clock generator and counters
    
    Can be used to generate the main system clock and counters that can
    trigger callbacks when they reach 0. In the KC85 emulator this is
    used to generate the CPU clock at 1.75MHz(KC85/3) or 1.77MHz (KC85/4),
    the vertical blank and video scan line timings.
*/
#include "yakc/common.h"

namespace yakc {

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

//------------------------------------------------------------------------------
inline void
clock::init(int khz) {
    YAKC_ASSERT(khz > 0);
    this->base_freq_khz = khz;
    for (auto& t : this->timers) {
        t = timer_state();
    }
}

//------------------------------------------------------------------------------
inline int64_t
clock::cycles(int micro_seconds) const {
    return (int64_t(this->base_freq_khz) * micro_seconds) / 1000;
}

//------------------------------------------------------------------------------
inline void
clock::config_timer(int index, int hz, cb_timer callback, void* userdata) {
    YAKC_ASSERT((index >= 0) && (index < num_timers));
    YAKC_ASSERT(hz > 0);
    auto& t = this->timers[index];
    t.freq_hz = hz;
    t.count = 0;
    t.value = (this->base_freq_khz*1000)/t.freq_hz;
    t.callback = callback;
    t.userdata = userdata;
}

//------------------------------------------------------------------------------
inline void
clock::update(int num_cycles) {
    for (auto& t : this->timers) {
        if (t.callback) {
            t.value -= num_cycles;
            while (t.value <= 0) {
                t.count++;
                t.value += (this->base_freq_khz*1000)/t.freq_hz;
                t.callback(t.userdata);
            }
        }
    }
}

} // namespace yakc
