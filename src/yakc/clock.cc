//------------------------------------------------------------------------------
//  clock.cc
//------------------------------------------------------------------------------
#include "clock.h"
#include "z80bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
clock::init(int khz) {
    YAKC_ASSERT(khz > 0);
    this->base_freq_khz = khz;
    for (auto& t : this->timers) {
        t = timer_state();
    }
}

//------------------------------------------------------------------------------
int64_t
clock::cycles(int micro_seconds) const {
    return (int64_t(this->base_freq_khz) * micro_seconds) / 1000;
}

//------------------------------------------------------------------------------
void
clock::config_timer_hz(int index, int hz) {
    YAKC_ASSERT((index >= 0) && (index < num_timers));
    YAKC_ASSERT(hz > 0);
    auto& t = this->timers[index];
    t.interval = (this->base_freq_khz*1000)/hz;
    t.count = 0;
    t.value = t.interval;
}

//------------------------------------------------------------------------------
void
clock::config_timer_cycles(int index, int cycles) {
    YAKC_ASSERT((index >= 0) && (index < num_timers));
    YAKC_ASSERT(cycles > 0);
    auto& t = this->timers[index];
    t.interval = cycles;
    t.count = 0;
    t.value = t.interval;
}

//------------------------------------------------------------------------------
void
clock::update(z80bus* bus, int num_cycles) {
    for (int i = 0; i < num_timers; i++) {
        auto& t = this->timers[i];
        if (t.interval > 0) {
            t.value -= num_cycles;
            while (t.value <= 0) {
                t.count++;
                t.value += t.interval;
                if (bus) {
                    bus->timer(i);
                }
            }
        }
    }
}

} // namespace YAKC
