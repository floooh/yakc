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
#include "yakc/core/counter.h"

namespace YAKC {

class system_bus;

class clock {
public:
    /// initialize the clock to a base frequency
    void init(int baseFreqKHz);
    /// return number of cycles for a given time-span in micro-seconds
    int64_t cycles(int micro_seconds) const;

    /// configure a timer with a Hz frequency
    void config_timer_hz(int index, int hz);
    /// configure a timer with a cycle count (Z80 T-states)
    void config_timer_cycles(int index, int cycles);
    /// advance the timers by a number of cycles
    void step(system_bus* bus, int num_cycles);

    int base_freq_khz = 0;
    static const int num_timers = 4;
    counter timers[num_timers];
};

} // namespace YAKC
