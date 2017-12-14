//------------------------------------------------------------------------------
//  clock.cc
//------------------------------------------------------------------------------
#include "clock.h"
#include "system_bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
clock::init(int khz) {
    YAKC_ASSERT(khz > 0);
    this->base_freq_khz = khz;
    for (auto& t : this->timers) {
        t = counter();
    }
}

//------------------------------------------------------------------------------
int64_t
clock::cycles(int micro_seconds) const {
    YAKC_ASSERT(this->base_freq_khz > 0);
    return (int64_t(this->base_freq_khz) * micro_seconds) / 1000;
}

//------------------------------------------------------------------------------
void
clock::config_timer_hz(int index, int hz) {
    YAKC_ASSERT((index >= 0) && (index < num_timers));
    YAKC_ASSERT(hz > 0);
    this->timers[index].init((this->base_freq_khz*1000)/hz);
}

//------------------------------------------------------------------------------
void
clock::config_timer_cycles(int index, int cycles) {
    YAKC_ASSERT((index >= 0) && (index < num_timers));
    YAKC_ASSERT(cycles > 0);
    this->timers[index].init(cycles);
}

//------------------------------------------------------------------------------
void
clock::step(system_bus* bus, int num_cycles) {
    for (int i = 0; i < num_timers; i++) {
        auto& t = this->timers[i];
        if (t.period > 0) {
            t.update(num_cycles);
            while (t.step()) {
                if (bus) {
                    bus->timer(i);
                }
            }
        }
    }
}

} // namespace YAKC
