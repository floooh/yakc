#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::clock
    @brief system clock generator and counters
    
    Can be used to generate the main system clock.
*/
#include <stdint.h>

namespace YAKC {

class system_bus;

class clock {
public:
    /// initialize the clock to a base frequency
    void init(int baseFreqKHz);
    /// return number of cycles for a given time-span in micro-seconds
    int64_t cycles(int micro_seconds) const;

    int base_freq_khz = 0;
};

} // namespace YAKC
